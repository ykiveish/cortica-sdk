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

using namespace std;

void * 
v4l2uvcThreadHandler (void * args) {
    Cortica * obj       = (Cortica *) args;
    char *videodevice   = (char *)(obj->v4l2uvcVideoDevice).c_str();
    int format          = V4L2_PIX_FMT_MJPEG;
    int width           = 640;
    int height          = 480;
    int grabmethod      = 1;
    int brightness = 0, contrast = 0, saturation = 0, gain = 0;
    struct vdIn *videoIn;
    FILE *file;
    
    videoIn = (struct vdIn *) calloc (1, sizeof (struct vdIn));
    if (init_videoIn (videoIn, (char *) videodevice, width, height, format, grabmethod) < 0) {
        obj->v4l2uvcWorking = false;
    }
    
    v4l2ResetControl (videoIn, V4L2_CID_BRIGHTNESS);
    v4l2ResetControl (videoIn, V4L2_CID_CONTRAST);
    v4l2ResetControl (videoIn, V4L2_CID_SATURATION);
    v4l2ResetControl (videoIn, V4L2_CID_GAIN);
    
    while (obj->v4l2uvcWorking) {
    	// 1. Capture frame.
        if (uvcGrab (videoIn) < 0) {
            obj->v4l2uvcWorking = false;
        } else {
            // 2. Save JPEG image.
            pthread_mutex_lock (&(obj->v4l2uvcMutex));
            file = fopen (PROCESSING_PATH"/cortica-image.jpg", "wb");
            if (file != NULL) {
                fwrite (videoIn->tmpbuffer, videoIn->buf.bytesused + DHT_SIZE, 1, file);
                fclose (file);
            }
            pthread_mutex_unlock (&(obj->v4l2uvcMutex));
        }
    }
    
    close_v4l2 (videoIn);
    free (videoIn);
    std::cout  	<< "Cleaning v4l2 objects ..." << std::endl;
}

Cortica::Cortica () {
}

Cortica::~Cortica () {
	if (m_provider) {
		m_provider->Free();
	}
}

CORTICAPI_RESULT
Cortica::initializeDB () {
	m_provider->Initialize ();
	return CORTICAPI_SUCCESS;
}

vector<Tag>
Cortica::matchingSync (RGBImage * data) {
	vector<Tag> tags;
	Json::Value root;
	Json::Reader reader;

    std::string rawData;
    if (data->ImageURL != "") {
        rawData = m_provider->SendImage (data->ImageURL);
    } else if (data->ImageStream != "") {
        rawData = m_provider->SendEmbeddedImage (data->ImageStream);
    } else {
        return tags;
    }
    
    if (rawData != "") {
    	if (!reader.parse(rawData, root)) {
			std::cout  	<< "Failed to parse configuration\n"
						<< reader.getFormattedErrorMessages();
		} else {
			const Json::Value jsonTags = root["matches"];
			for ( int index = 0; index < jsonTags.size(); ++index ) {
				Tag tag;
				tag.Name = jsonTags[index].get("tag", 0).asString();
				tags.push_back (tag);
			}
		}
    }
	
	return tags;
}

CORTICAPI_RESULT
Cortica::matchingAsync (RGBImage * data) {
	return CORTICAPI_SUCCESS;
}

CORTICAPI_RESULT
Cortica::SetProvider (CORTICAPI_PROVIDER provider) {
	m_provider = CorticaProviderFactory::Get()->CreateProvider (provider);
	return CORTICAPI_SUCCESS;
}

CORTICAPI_RESULT
Cortica::setMatchingCallback (onMatchingCallback) {
	return CORTICAPI_SUCCESS;
}

CORTICAPI_RESULT
Cortica::initCalssificationCamera (string device) {
    v4l2uvcWorking      = true;
    v4l2uvcVideoDevice  = device;
    
    umask(0);
    struct stat st = {0};
    if (stat (TMP_PATH, &st) == -1) {
        mkdir (TMP_PATH         ,0777);
        mkdir (PROCESSING_PATH  ,0777);
        mkdir (OUTPUT_PATH      ,0777);
    }
    
    int error = pthread_create(&m_v4l2uvcThread, NULL, v4l2uvcThreadHandler, this);
    if (error) {
        v4l2uvcWorking = false;
    }
    
    pthread_mutex_init (&v4l2uvcMutex, NULL);
}

vector<Tag>
Cortica::getClassificationTag () {
    vector<Tag> tags;
    
    if (v4l2uvcWorking == true) {
        // 1. Read image to string container.
        pthread_mutex_lock (&v4l2uvcMutex);
        ifstream ifs (PROCESSING_PATH"/cortica-image.jpg");
        string content ((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));
        pthread_mutex_unlock (&v4l2uvcMutex);

        // 2. Set Cortica data container.
        RGBImage data;
        data.ImageStream = content;

        // 3. Ask for sync matching.
        tags = matchingSync (&data);
        
        // 4. Write image to OUTPUT for presenting
        ofstream ofs;
        ofs.open (OUTPUT_PATH"/cortica-image.jpg");
        ofs << content;
        ofs.close();
    }
    
    return tags;
}

CORTICAPI_RESULT
Cortica::closeClassificationCamera () {
    v4l2uvcWorking = false;
    
    usleep (1000000);
    pthread_cancel (m_v4l2uvcThread);
    pthread_mutex_destroy (&v4l2uvcMutex);
}
