#include "CorticaProviderFactory.h"
#include "CorticaProviderCloud.h"
#include <iostream>

CorticaProviderFactory::CorticaProviderFactory () {
	std::cout << "CorticaProviderFactory::CorticaProviderFactory" << std::endl;
	Register(CORTICAPI_PROVIDER_CLOUD, &CorticaProviderCloud::Create);
}

void
CorticaProviderFactory::Register (CORTICAPI_PROVIDER provider, 
				CreateProviderFn pfnCreate) {
	m_FactoryMap[provider] = pfnCreate;
}

ICorticaProvider*
CorticaProviderFactory::CreateProvider (CORTICAPI_PROVIDER provider) {
	FactroyMap::iterator it = m_FactoryMap.find (provider);
	if (it != m_FactoryMap.end()) {
		return it->second();
	}

	return NULL;
}