#pragma once

#include "CORTICAPI.h"
#include "ICorticaProvider.h"
#include "CorticaProviderFactory.h"
#include "json/json.h"

using namespace std;

class CORTICAPI_EXPORT Cortica : public CORTICAPI {
public:
	Cortica ();
	~Cortica ();
	CORTICAPI_RESULT initializeDB ();
	vector<Tag> matchingSync (RGBImage * data);
	CORTICAPI_RESULT matchingAsync (RGBImage * data);
	CORTICAPI_RESULT setMatchingCallback (onMatchingCallback);

	CORTICAPI_RESULT SetProvider (CORTICAPI_PROVIDER provider);

	static Cortica* GetApi (CORTICAPI_PROVIDER provider) {
		static Cortica instance;
		instance.SetProvider (provider);
		return &instance;
	}

private:
	ICorticaProvider* m_provider;
};
