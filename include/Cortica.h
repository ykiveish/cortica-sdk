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

#pragma once

#include "CORTICAPI.h"
#include "ICorticaProvider.h"
#include "CorticaProviderFactory.h"
#include "json/json.h"

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sstream>
#include <fstream>
#include <pthread.h>
#include <linux/videodev.h>
#include "v4l2uvc.h"

#define TMP_PATH          "/tmp/cortica"
#define PROCESSING_PATH   "/tmp/cortica/processing"
#define OUTPUT_PATH       "/tmp/cortica/output"

using namespace std;

class CORTICAPI_EXPORT Cortica : public CORTICAPI {
public:
	Cortica ();
	~Cortica ();
	CORTICAPI_RESULT initializeDB ();
	vector<Tag> matchingSync (RGBImage * data);
	CORTICAPI_RESULT matchingAsync (RGBImage * data);
	CORTICAPI_RESULT setMatchingCallback (onMatchingCallback);
    
    CORTICAPI_RESULT initCalssificationCamera (string device);
    vector<Tag> getClassificationTag ();
    CORTICAPI_RESULT closeClassificationCamera ();

	CORTICAPI_RESULT SetProvider (CORTICAPI_PROVIDER provider);

	static Cortica* GetApi (CORTICAPI_PROVIDER provider) {
		static Cortica instance;
		instance.SetProvider (provider);
		return &instance;
	}

    pthread_mutex_t v4l2uvcMutex;
    bool            v4l2uvcWorking;
    string          v4l2uvcVideoDevice;
    
private:
	ICorticaProvider* m_provider;
    
    pthread_t m_v4l2uvcThread;
};
