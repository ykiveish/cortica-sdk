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