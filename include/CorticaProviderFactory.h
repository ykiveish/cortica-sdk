#pragma once

#include "ICorticaProvider.h"
#include <CORTICAPITypes.h>
#include <map>
#include <iostream>

typedef ICorticaProvider* (*CreateProviderFn)(void);

class CorticaProviderFactory {
public:
	~CorticaProviderFactory () {
		std::cout << "~CorticaProviderFactory" << std::endl;
		m_FactoryMap.clear();
	}

	void Register (CORTICAPI_PROVIDER provider, CreateProviderFn pfnCreate);
	ICorticaProvider* CreateProvider (CORTICAPI_PROVIDER provider);

	static CorticaProviderFactory* Get () {
		static CorticaProviderFactory instance;
		return &instance;
	}
private:
	CorticaProviderFactory ();
	CorticaProviderFactory (const CorticaProviderFactory&) = delete;
	CorticaProviderFactory &operator= (const CorticaProviderFactory&) {
		return *this;
	}

	typedef std::map<CORTICAPI_PROVIDER, CreateProviderFn> FactroyMap;
	FactroyMap m_FactoryMap;
};