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

#include "CorticaProviderCloud.h"
#include <iostream>
#include <sstream>
#include <stdlib.h>

size_t CurlDataArrivedCB (void *ptr, size_t size, size_t count, void *stream) {
	((std::string*)stream)->append((char*)ptr, 0, size*count);
	return size*count;
}

void
CorticaProviderCloud::Initialize () {
	std::cout << "CorticaProviderCloud::Initialize" << std::endl;

	curl_global_init(CURL_GLOBAL_ALL);
	m_HTTPRequest = curl_easy_init();
	m_POSTData = (char *)malloc(MAX_SIZE_DATA_PACKAGE);
}

std::string
CorticaProviderCloud::SendImage (std::string url) {
	CURLcode 			response;
	std::stringstream 	request;
	std::string 		resData;

	request << 	getenv("CORTICA_PATH") << 
				"MatchImage?data={" << 
				"'tid':'1'," <<
				"'user_id':'yevgeniy'," <<
				"'image_url':'" << url << "'}";
	std::cout << request.str() << std::endl;

	if (m_HTTPRequest) {
		curl_easy_setopt (m_HTTPRequest, CURLOPT_URL, request.str().c_str());
		curl_easy_setopt (m_HTTPRequest, CURLOPT_WRITEFUNCTION, CurlDataArrivedCB);
		curl_easy_setopt (m_HTTPRequest, CURLOPT_WRITEDATA, &resData);

		response = curl_easy_perform (m_HTTPRequest);

		if(response != CURLE_OK) {
			std::cout << "CURL ERROR :: " << 
			curl_easy_strerror(response) << std::endl;
			return NULL;
		}
	}

	return resData;
}

std::string
CorticaProviderCloud::SendEmbeddedImage (const char* image64base) {
	return NULL;
}

void
CorticaProviderCloud::Free () {
	std::cout << "CorticaProviderCloud::Free" << std::endl;

	curl_easy_cleanup (m_HTTPRequest);
	curl_global_cleanup ();
	free (m_POSTData);
	delete this;
}
