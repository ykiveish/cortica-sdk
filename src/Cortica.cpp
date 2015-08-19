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

	std::string rawData = m_provider->SendImage (data->ImageURL);

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
