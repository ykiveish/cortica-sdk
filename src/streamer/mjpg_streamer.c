/*******************************************************************************
#                                                                              #
#      MJPG-streamer allows to stream JPG frames from an input-plugin          #
#      to several output plugins                                               #
#                                                                              #
#      Copyright (C) 2007 Tom Stöveken                                         #
#                                                                              #
# This program is free software; you can redistribute it and/or modify         #
# it under the terms of the GNU General Public License as published by         #
# the Free Software Foundation; version 2 of the License.                      #
#                                                                              #
# This program is distributed in the hope that it will be useful,              #
# but WITHOUT ANY WARRANTY; without even the implied warranty of               #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                #
# GNU General Public License for more details.                                 #
#                                                                              #
# You should have received a copy of the GNU General Public License            #
# along with this program; if not, write to the Free Software                  #
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA    #
#                                                                              #
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/videodev.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <getopt.h>
#include <pthread.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <syslog.h>

#include "utils.h"
#include "mjpg_streamer.h"

/* globals */
static globals global;

int split_parameters(char *parameter_string, int *argc, char **argv)
{
    int count = 1;
    argv[0] = NULL; // the plugin may set it to 'INPUT_PLUGIN_NAME'
    if(parameter_string != NULL && strlen(parameter_string) != 0) {
        char *arg = NULL, *saveptr = NULL, *token = NULL;

        arg = strdup(parameter_string);

        if(strchr(arg, ' ') != NULL) {
            token = strtok_r(arg, " ", &saveptr);
            if(token != NULL) {
                argv[count] = strdup(token);
                count++;
                while((token = strtok_r(NULL, " ", &saveptr)) != NULL) {
                    argv[count] = strdup(token);
                    count++;
                    if(count >= MAX_PLUGIN_ARGUMENTS) {
                        IPRINT("ERROR: too many arguments to input plugin\n");
                        return 0;
                    }
                }
            }
        }
    }
    *argc = count;
    return 1;
}

int streamer_start (mjpg_context_t *ctx)
{
    // char *input[1]  = { "./libinput_file.so -f /tmp/cortica/output -r" };
    // char *output[1] = { "./liboutput_http.so -w ./mjpg-streamer-www" };
    
    char input[1][128];
    char output[1][128];
    
    int i;
    size_t tmp = 0;
    global.outcnt = 1;
    global.incnt  = 1;
    
    sprintf (input[0], "./libinput_file.so -f %s", ctx->image_source);
    
    if (strlen(ctx->website_path) == 0) {
        sprintf (output[0], "./liboutput_http.so -p %d", ctx->port);
    } else {
        sprintf (output[0], "./liboutput_http.so -p %d -w %s", ctx->port, ctx->website_path);
    }
    
    printf (" --------------------------------> %s\n", output);
    
    openlog("MJPG-streamer ", LOG_PID | LOG_CONS, LOG_USER);
    syslog(LOG_INFO, "starting application");

    /* ignore SIGPIPE (send by OS if transmitting to closed TCP sockets) */
    signal(SIGPIPE, SIG_IGN);

    /*
     * messages like the following will only be visible on your terminal
     * if not running in daemon mode
     */
#ifdef SVN_REV
    LOG("MJPG Streamer Version: svn rev: %s\n", SVN_REV);
#else
    LOG("MJPG Streamer Version.: %s\n", SOURCE_VERSION);
#endif
    
    /* open input plugin */
    for(i = 0; i < global.incnt; i++) {
        /* this mutex and the conditional variable are used to synchronize access to the global picture buffer */
        if(pthread_mutex_init(&global.in[i].db, NULL) != 0) {
            LOG("could not initialize mutex variable\n");
            closelog();
            exit(EXIT_FAILURE);
        }
        if(pthread_cond_init(&global.in[i].db_update, NULL) != 0) {
            LOG("could not initialize condition variable\n");
            closelog();
            exit(EXIT_FAILURE);
        }

        tmp = (size_t)(strchr(input[i], ' ') - input[i]);
        global.in[i].stop      = 0;
        global.in[i].buf       = NULL;
        global.in[i].size      = 0;
        global.in[i].plugin = (tmp > 0) ? strndup(input[i], tmp) : strdup(input[i]);
        global.in[i].handle = dlopen(global.in[i].plugin, RTLD_LAZY);
        if(!global.in[i].handle) {
            LOG("ERROR: could not find input plugin\n");
            LOG("       Perhaps you want to adjust the search path with:\n");
            LOG("       # export LD_LIBRARY_PATH=/path/to/plugin/folder\n");
            LOG("       dlopen: %s\n", dlerror());
            closelog();
            exit(EXIT_FAILURE);
        }
        global.in[i].init = dlsym(global.in[i].handle, "input_init");
        if(global.in[i].init == NULL) {
            LOG("%s\n", dlerror());
            exit(EXIT_FAILURE);
        }
        global.in[i].stop = dlsym(global.in[i].handle, "input_stop");
        if(global.in[i].stop == NULL) {
            LOG("%s\n", dlerror());
            exit(EXIT_FAILURE);
        }
        global.in[i].run = dlsym(global.in[i].handle, "input_run");
        if(global.in[i].run == NULL) {
            LOG("%s\n", dlerror());
            exit(EXIT_FAILURE);
        }
        /* try to find optional command */
        global.in[i].cmd = dlsym(global.in[i].handle, "input_cmd");

        global.in[i].param.parameters = strchr(input[i], ' ');
        split_parameters(global.in[i].param.parameters, &global.in[i].param.argc, global.in[i].param.argv);
        global.in[i].param.global = &global;
        global.in[i].param.id = i;

        if(global.in[i].init(&global.in[i].param, i)) {
            LOG("input_init() return value signals to exit\n");
            closelog();
            exit(0);
        }
    }

    /* open output plugin */
    for(i = 0; i < global.outcnt; i++) {
        tmp = (size_t)(strchr(output[i], ' ') - output[i]);
        global.out[i].plugin = (tmp > 0) ? strndup(output[i], tmp) : strdup(output[i]);
        global.out[i].handle = dlopen(global.out[i].plugin, RTLD_LAZY);
        if(!global.out[i].handle) {
            LOG("ERROR: could not find output plugin %s\n", global.out[i].plugin);
            LOG("       Perhaps you want to adjust the search path with:\n");
            LOG("       # export LD_LIBRARY_PATH=/path/to/plugin/folder\n");
            LOG("       dlopen: %s\n", dlerror());
            closelog();
            exit(EXIT_FAILURE);
        }
        global.out[i].init = dlsym(global.out[i].handle, "output_init");
        if(global.out[i].init == NULL) {
            LOG("%s\n", dlerror());
            exit(EXIT_FAILURE);
        }
        global.out[i].stop = dlsym(global.out[i].handle, "output_stop");
        if(global.out[i].stop == NULL) {
            LOG("%s\n", dlerror());
            exit(EXIT_FAILURE);
        }
        global.out[i].run = dlsym(global.out[i].handle, "output_run");
        if(global.out[i].run == NULL) {
            LOG("%s\n", dlerror());
            exit(EXIT_FAILURE);
        }

        /* try to find optional command */
        global.out[i].cmd = dlsym(global.out[i].handle, "output_cmd");

        global.out[i].param.parameters = strchr(output[i], ' ');
        split_parameters(global.out[i].param.parameters, &global.out[i].param.argc, global.out[i].param.argv);

        global.out[i].param.global = &global;
        global.out[i].param.id = i;
        if(global.out[i].init(&global.out[i].param, i)) {
            LOG("output_init() return value signals to exit\n");
            closelog();
            exit(0);
        }
    }

    /* start to read the input, push pictures into global buffer */
    DBG("starting %d input plugin\n", global.incnt);
    for(i = 0; i < global.incnt; i++) {
        syslog(LOG_INFO, "starting input plugin %s", global.in[i].plugin);
        if(global.in[i].run(i)) {
            LOG("can not run input plugin %d: %s\n", i, global.in[i].plugin);
            closelog();
            return 1;
        }
    }

    DBG("starting %d output plugin(s)\n", global.outcnt);
    for(i = 0; i < global.outcnt; i++) {
        syslog(LOG_INFO, "starting output plugin: %s (ID: %02d)", global.out[i].plugin, global.out[i].param.id);
        global.out[i].run(global.out[i].param.id);
    }
    
    printf ("Waiting for closing ...\n");
    // Wait for signal.
    pthread_mutex_lock(&(ctx->sync.mutex));
    pthread_cond_wait(&(ctx->sync.cond), &(ctx->sync.mutex));
    pthread_mutex_unlock(&(ctx->sync.mutex));
    printf ("Shutting down streamer ...\n");
    
    /* signal "stop" to threads */
    LOG("setting signal to stop\n");
    global.stop = 1;
    usleep(1000 * 1000);

    /* clean up threads */
    LOG("force cancellation of threads and cleanup resources\n");
    for(i = 0; i < global.incnt; i++) {
        global.in[i].stop(i);
    }

    for(i = 0; i < global.outcnt; i++) {
        global.out[i].stop(global.out[i].param.id);
        pthread_cond_destroy(&global.in[i].db_update);
        pthread_mutex_destroy(&global.in[i].db);
    }
    usleep(1000 * 1000);

    /* close handles of input plugins */
    for(i = 0; i < global.outcnt; i++) {
        dlclose(global.in[i].handle);
    }

    for(i = 0; i < global.outcnt; i++) {
        dlclose(global.out[i].handle);
    }
    DBG("all plugin handles closed\n");

    LOG("done\n");

    closelog();

    return 0;
}
