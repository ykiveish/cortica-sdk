/*
* Author: Kiveisha Yevgeniy
* Copyright (c) 2015 Intel Corporation.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
* LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
* OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
* WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "Cortica.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <pthread.h>

#include <linux/videodev.h>
#include "v4l2uvc.h"

using namespace std;

void sig_handler (int signo);

pthread_mutex_t gMutex;
bool amWorking = true;

void * 
threadHandler (void * args) {
    char *videodevice = "/dev/video0";
    int format = V4L2_PIX_FMT_MJPEG;
    int width = 640;
    int height = 480;
    int grabmethod = 1;
    int brightness = 0, contrast = 0, saturation = 0, gain = 0;
    struct vdIn *videoIn;
    FILE *file;
    
    videoIn = (struct vdIn *) calloc (1, sizeof (struct vdIn));
    if (init_videoIn (videoIn, (char *) videodevice, width, height, format, grabmethod) < 0) {
        exit (1);
    }
    
    v4l2ResetControl (videoIn, V4L2_CID_BRIGHTNESS);
    v4l2ResetControl (videoIn, V4L2_CID_CONTRAST);
    v4l2ResetControl (videoIn, V4L2_CID_SATURATION);
    v4l2ResetControl (videoIn, V4L2_CID_GAIN);
    
    while (amWorking) {
    	// 1. Capture frame.
        if (uvcGrab (videoIn) < 0) {
            amWorking = false;
        } else {
            // 2. Save JPEG image.
            pthread_mutex_lock (&gMutex);
            file = fopen ("/tmp/cortica/processing/cortica-image.jpg", "wb");
            if (file != NULL) {
                fwrite (videoIn->tmpbuffer, videoIn->buf.bytesused + DHT_SIZE, 1, file);
                fclose (file);
            }
            pthread_mutex_unlock (&gMutex);
        }
    }
    
    close_v4l2 (videoIn);
    free (videoIn);
}

int
main (int argc, char ** argv) {    
    pthread_t thread;

    Cortica* api = Cortica::GetApi (CORTICAPI_PROVIDER_CLOUD);
    api->initializeDB ();

    umask(0);
    struct stat st = {0};
    if (stat ("/tmp/cortica", &st) == -1) {
        mkdir ("/tmp/cortica", 0777);
        mkdir ("/tmp/cortica/processing", 0777);
        mkdir ("/tmp/cortica/output", 0777);
    }
    
    int error = pthread_create(&thread, NULL, threadHandler, NULL);
    if (error) {
        amWorking = false;
    }
    
    pthread_mutex_init (&gMutex, NULL);

    signal (SIGINT, sig_handler);
    while (amWorking) {            
        // 3. Read image to string container.
        pthread_mutex_lock (&gMutex);
        ifstream ifs ("/tmp/cortica/processing/cortica-image.jpg");
        string content ((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));
        pthread_mutex_unlock (&gMutex);

        // 4. Set Cortica data container.
        RGBImage data;
        data.ImageStream = content;

        // 5. Ask for sync matching.
        vector<Tag> tags = api->matchingSync (&data);
        for (vector<Tag>::iterator item = tags.begin(); item != tags.end(); ++item) {
            std::cout << (*item).Name << std::endl << std::endl;
        }
        
        ofstream ofs;
        ofs.open ("/tmp/cortica/output/cortica-image.jpg");
        ofs << content;
        ofs.close();
    }
    
    pthread_cancel (thread);
    pthread_mutex_destroy (&gMutex);
	std::cout << "Exit 'cortica-test'" << std::endl;
	return 0;
}

void
sig_handler (int signo) {
	if (signo == SIGINT) {
		amWorking = false;
	}
}
