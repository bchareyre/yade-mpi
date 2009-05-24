/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  with help from Bronek Kozicki                                         *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "DynLibManager.hpp"


#include<fstream>
#include<boost/filesystem/operations.hpp>
#include<boost/filesystem/convenience.hpp>
#include<string.h>

#include "ClassFactory.hpp"


//using namespace std;
using namespace boost;

CREATE_LOGGER(DynLibManager);


DynLibManager::DynLibManager ()
{
	autoUnload = true;
}


DynLibManager::~DynLibManager ()
{
	if(autoUnload) unloadAll();
}


bool DynLibManager::load (const string& fullLibName, const string& libName )
{
	if (libName.empty() || fullLibName.empty()){
		LOG_ERROR("Empty filename for library `"<<libName<<"'.");
		return false;
	}
#ifdef WIN32
	if (isLoaded(libName)) return true;
	HINSTANCE handle = LoadLibraryA(fullLibName.c_str());
#else
	void * handle = dlopen(fullLibName.data(), RTLD_NOW);
#endif
	if (!handle) return !error();
	handles[libName] = handle;
	return true;
}


bool DynLibManager::unload (const string libName)
{
	if (isLoaded(libName))
	#ifdef WIN32
		return FreeLibrary(handles[libName]);
	#else
		return closeLib(libName);
	#endif		
	else
		return false;
}


bool DynLibManager::unloadAll ()
{
	#ifdef WIN32
		std::map<const string, HINSTANCE>::iterator ith  = handles.begin();
		std::map<const string, HINSTANCE>::iterator ithEnd  = handles.end();
	#else
		std::map<const string, void *>::iterator ith  = handles.begin();
		std::map<const string, void *>::iterator ithEnd  = handles.end();
	#endif

	for( ; ith!=ithEnd ; ++ith)
		if ((*ith).first.length()!=0)
			unload((*ith).first);
	return false;
}


bool DynLibManager::isLoaded (const string libName)
{

	#ifdef WIN32
		std::map<const string, HINSTANCE>::iterator ith = handles.find(libName);	
	#else
		std::map<const string, void *>::iterator ith = handles.find(libName);	
	#endif

	return (ith!= handles.end() && (*ith).second!=NULL);

}


void DynLibManager::setAutoUnload ( bool enabled )
{
	autoUnload = enabled;
}


bool DynLibManager::closeLib(const string libName)
{
	#ifdef WIN32
		FreeLibrary(handles[libName]);
		return !error();
	#else	
		dlclose(handles[libName]);
		return !error();
	#endif

}

std::string DynLibManager::lastError()
{
        return lastError_;
}

bool DynLibManager::error() 
{
	#ifdef WIN32
		char* lpMsgBuf;
		const DWORD lastError = GetLastError();
	
		FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, lastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR) &lpMsgBuf, 0, NULL);
	
		std::string errMsg(lpMsgBuf);
		LocalFree(lpMsgBuf); // mind the FORMAT_MESSAGE_ALLOCATE_BUFFER !
	
		if (lastError != ERROR_SUCCESS)
		{
			lastError_ = errMsg;
			return true;
		}

		return false;
	#else
 		char * error = dlerror();
		if (error != NULL)  
		{
			lastError_ = error;
		}
		return (error!=NULL);
	#endif
}


string DynLibManager::libNameToSystemName(const string& name)
{
	string systemName; 
	#ifdef WIN32
		systemName = name + ".dll";
	#else
		systemName = "lib" + name + ".so";
	#endif 
	return systemName;
}


string DynLibManager::systemNameToLibName(const string& name)
{
	string libName;
	if(name.length()<=3){ // this arbitrary value may disappear once the logic below is dumped...
		// LOG_WARN("Filename `"<<name<<"' too short, returning empty string (cross thumbs).");
		return "[Garbage plugin file `"+name+"']";
	}

	#ifdef WIN32
		libName = name.substr(0,name.size()-4);
	#else
		libName = name.substr(3,name.size()-3);
	#endif 

	return libName;
}

