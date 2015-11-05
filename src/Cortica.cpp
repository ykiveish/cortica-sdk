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
