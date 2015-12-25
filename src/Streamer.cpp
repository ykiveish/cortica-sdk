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

#include "Streamer.h"

using namespace std;

void * 
streamerThreadHandler (void * args) {
    Streamer * obj = (Streamer *) args;
    
    streamer_start (&(obj->mjpegCtx)); 

    cout << "Exit - streamerThreadHandler" << endl;
}

Streamer::Streamer (string imagepath, string websitepath, int port) {
    imagePath   = imagepath;
    websitePath = websitepath;
    
    mjpegCtx.image_source   = (char *) imagePath.c_str();
    mjpegCtx.website_path   = (char *) websitePath.c_str();
    mjpegCtx.port           = port;
    
    m_streamerActive = false;
}

void
Streamer::Start () {
    pthread_cond_init  (&(mjpegCtx.sync.cond), NULL);
    pthread_mutex_init (&(mjpegCtx.sync.mutex), NULL);
    
    int error = pthread_create(&m_mjpegThread, NULL, streamerThreadHandler, this);
    if (error) {
        pthread_cond_destroy  (&(mjpegCtx.sync.cond));
        pthread_mutex_destroy (&(mjpegCtx.sync.mutex));
    }
    
    m_streamerActive = true;
}

void
Streamer::End () {
    if (m_streamerActive == true) {
        pthread_mutex_lock(&(mjpegCtx.sync.mutex));
        pthread_cond_signal(&(mjpegCtx.sync.cond));
        pthread_mutex_unlock(&(mjpegCtx.sync.mutex));

        usleep (2 * 1000 * 1000);
        
        pthread_cond_destroy  (&(mjpegCtx.sync.cond));
        pthread_mutex_destroy (&(mjpegCtx.sync.mutex));
    }
}
