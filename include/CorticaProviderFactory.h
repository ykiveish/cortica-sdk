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