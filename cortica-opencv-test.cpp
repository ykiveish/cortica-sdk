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

using namespace cv;
using namespace std;

void serializeOpencv(Mat input, stringstream& ss);
void sig_handler (int signo);

bool amWorking = true;

int
main (int argc, char ** argv) {
	Mat 		frame;
	vector<int> compression_params;

	VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;
    
    Cortica* api = Cortica::GetApi (CORTICAPI_PROVIDER_CLOUD);
    api->initializeDB ();

    umask(0);
    struct stat st = {0};
    if (stat ("/tmp/cortica", &st) == -1) {
        mkdir ("/tmp/cortica", 0777);
    }

    signal (SIGINT, sig_handler);

    while (amWorking) {
    	// 1. Capture frame.
	    cap >> frame;
	    compression_params.push_back(CV_IMWRITE_JPEG_QUALITY); //specify the compression technique
	    compression_params.push_back(98); //specify the compression quality

	    // 2. Save JPEG image.
	    bool bSuccess = imwrite("/tmp/cortica/cortica-image.jpg", frame, compression_params); //write the image to file
	    if (!bSuccess) {
	      cout << "ERROR : Failed to save the image" << endl;
	    }

	    // 3. Read image to string container.
	    ifstream ifs ("/tmp/cortica/cortica-image.jpg");
	    string content ((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));

	    // 4. Set Cortica data container.
		RGBImage data;
		data.ImageStream = content;

		// 5. Ask for sync matching.
		vector<Tag> tags = api->matchingSync (&data);

		std::cout << "------------- RESULT -------------" << std::endl;
		for (vector<Tag>::iterator item = tags.begin(); item != tags.end(); ++item) {
			std::cout << (*item).Name << std::endl;
		}
		std::cout << "------------- ------ -------------" << std::endl;
    }
    
	std::cout << "Exit 'cortica-test'" << std::endl;
	return 0;
}

void
serializeOpencv(Mat input, stringstream& ss) {
    // We will need to also serialize the width, height, type and size of the matrix
    int width = input.cols;
    int height = input.rows;
    int type = input.type();
    size_t size = input.total() * input.elemSize();

    // Initialize a stringstream and write the data
    ss.write((char*)(&width), sizeof(int));
    ss.write((char*)(&height), sizeof(int));
    ss.write((char*)(&type), sizeof(int));
    ss.write((char*)(&size), sizeof(size_t));

    // Write the whole image data
    ss.write((char*)input.data, size);
}

void
sig_handler (int signo) {
	if (signo == SIGINT) {
		amWorking = false;
	}
}

/*
	std::cout << "image64base: " 
			<< static_cast<void *>(&image64base) 
			<< std::endl;
*/
