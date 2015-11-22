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

#include <linux/videodev.h>
#include "v4l2uvc.h"
#include "Streamer.h"

using namespace std;

void sig_handler (int signo);
bool amWorking = true;

int
main (int argc, char ** argv) {    
    pthread_t thread;

    Cortica* api = Cortica::GetApi (CORTICAPI_PROVIDER_CLOUD);
    api->initializeDB ();
    api->initCalssificationCamera ("/dev/video0");
    
    Streamer* stream = new Streamer ();
    stream->start ();

    signal (SIGINT, sig_handler);
    while (amWorking) {
        vector<Tag> tags = api->getClassificationTag ();
        for (vector<Tag>::iterator item = tags.begin(); item != tags.end(); ++item) {
            std::cout << (*item).Name << std::endl << std::endl;
        }
    }
    
    stream->end ();
    api->closeClassificationCamera ();
	std::cout << "Exit 'cortica-test'" << std::endl;
	return 0;
}

void
sig_handler (int signo) {
	if (signo == SIGINT) {
		amWorking = false;
	}
}
