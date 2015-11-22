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
#include <opencv2/opencv.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <pthread.h>
#include "Streamer.h"

using namespace cv;
using namespace std;

void sig_handler (int signo);

pthread_mutex_t gMutex;
bool amWorking = true;

void * 
threadHandler (void * args) {
    Mat	frame;
    vector<int> compression_params;

    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        amWorking = false;
    
    Streamer* stream = new Streamer ();
    stream->start ();
    
    while (amWorking) {
        // 1. Capture frame.
        cap >> frame;
        
        compression_params.push_back(CV_IMWRITE_JPEG_QUALITY); //specify the compression technique
        compression_params.push_back(98); //specify the compression quality

        // 2. Save JPEG image.
        pthread_mutex_lock (&gMutex);
        bool bSuccess = imwrite("/tmp/cortica/processing/cortica-image.jpg", frame, compression_params); //write the image to file
        if (!bSuccess) {
            cout << "ERROR : Failed to save the image" << endl;
        }
        pthread_mutex_unlock (&gMutex);
    }
    
    stream->end ();
}

int
main (int argc, char ** argv) {
    pthread_t thread;
    vector<int> compression_params;
    long counter = 1;
    
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
        Mat frame = imread("/tmp/cortica/processing/cortica-image.jpg", CV_LOAD_IMAGE_UNCHANGED);
        pthread_mutex_unlock (&gMutex);

	    // 4. Set Cortica data container.
		RGBImage data;
		data.ImageStream = content;

		// 5. Ask for sync matching.
		vector<Tag> tags = api->matchingSync (&data);

        int row = 50;
		for (vector<Tag>::iterator item = tags.begin(); item != tags.end(); ++item) {
			// std::cout << (*item).Name << std::endl;
            putText(frame, (*item).Name, Point(50, row), CV_FONT_HERSHEY_PLAIN, 2, Scalar(255,0,0), 3, 8);
            row += 50;
		}
        
        ostringstream number;
        number << counter;
        
        putText(frame, number.str(), Point(570, 50), CV_FONT_HERSHEY_PLAIN, 2, Scalar(0,255,0), 3, 8);
        counter++;        
        
        compression_params.push_back(CV_IMWRITE_JPEG_QUALITY); //specify the compression technique
        compression_params.push_back(98); //specify the compression quality
        
        bool bSuccess = imwrite("/tmp/cortica/output/cortica-image.jpg", frame, compression_params); //write the image to file
	    if (!bSuccess) {
	      cout << "ERROR : Failed to save the image" << endl;
	    }
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
