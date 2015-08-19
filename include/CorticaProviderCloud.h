#pragma once

#include <string>
#include "ICorticaProvider.h"
#include <curl/curl.h>

#define MAX_SIZE_DATA_PACKAGE 512

class CorticaProviderCloud : public ICorticaProvider {
public:
	void Initialize ();
	std::string SendImage (std::string url);
	std::string SendEmbeddedImage (const char* image64base);
	void Free ();
	static ICorticaProvider* Create () {
		return new CorticaProviderCloud ();
	}

private:
	CURL* m_HTTPRequest;
	char* m_POSTData;
};