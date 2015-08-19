#pragma once

#include <string>

class ICorticaProvider {
public:
	virtual void Initialize () = 0;
	virtual std::string SendImage (std::string url) = 0;
	virtual std::string SendEmbeddedImage (const char* image64base) = 0;
	virtual void Free () = 0;
};