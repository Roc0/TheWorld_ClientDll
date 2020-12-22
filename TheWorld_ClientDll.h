#pragma once

//#ifndef __KBENGINE_DLL__
//#define __KBENGINE_DLL__

#pragma warning(disable : 4217)
#pragma warning(disable : 4049)

#include "targetver.h"
#include "client_lib/clientobjectbase.h"

#ifdef WIN32_LEAN_AND_MEAN
#else
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#endif

// Windows Header Files:
#include <windows.h>

#include "stdio.h"
#include "stdlib.h"
#include "winsock.h"
#include "common/common.h"
#include "Python.h"

#ifdef KBE_DLL_API
#else
#define KBE_DLL_API  extern "C" _declspec(dllimport)
#endif

namespace KBEngine {
	class EventHandle;
}

inline char* wchar2char(const wchar_t* ts)
{
	int len = int((wcslen(ts) + 1) * 4);
	char* ccattr = (char *)malloc(len);
	memset(ccattr, 0, len);
	wcstombs(ccattr, ts, len);
	return ccattr;
};

inline wchar_t* char2wchar(const char* cs)
{
	int len = int((strlen(cs) + 1) * 4);
	wchar_t* ccattr = (wchar_t *)malloc(len);
	memset(ccattr, 0, len);
	mbstowcs(ccattr, cs, len);
	return ccattr;
};

/**
*/
KBE_DLL_API bool kbe_init();
KBE_DLL_API bool kbe_destroy();

/**
*/
KBE_DLL_API KBEngine::uint64 kbe_genUUID64();

/**
*/
KBE_DLL_API void kbe_sleep(KBEngine::uint32 ms);

/**
*/
KBE_DLL_API KBEngine::uint32 kbe_getSystemTime();

/**
*/
KBE_DLL_API bool kbe_login(const char* accountName, const char* passwd, const char* datas = NULL, KBEngine::uint32 datasize = 0,
	const char* ip = NULL, KBEngine::uint32 port = 0);

KBE_DLL_API void kbe_reset(void);

/**
*/
KBE_DLL_API void kbe_update();

/**
*/
KBE_DLL_API void kbe_lock();
KBE_DLL_API void kbe_unlock();

/**
*/
KBE_DLL_API void kbe_inProcess(bool v);

/**
*/
KBE_DLL_API const char* kbe_getLastAccountName();

/**
*/
KBE_DLL_API KBEngine::ENTITY_ID kbe_playerID();

/**
*/
KBE_DLL_API KBEngine::DBID kbe_playerDBID();

/**
*/
KBE_DLL_API void kbe_updateVolatile(KBEngine::ENTITY_ID eid, float x, float y, float z, float yaw, float pitch, float roll);

/**
*/
KBE_DLL_API bool kbe_registerEventHandle(KBEngine::EventHandle* pHandle);
KBE_DLL_API bool kbe_deregisterEventHandle(KBEngine::EventHandle* pHandle);

/**
*/
KBE_DLL_API void kbe_callEntityMethod(KBEngine::ENTITY_ID entityID, const char *method,
	const char* strargs);


/**
*/
KBE_DLL_API void kbe_fireEvent(const char *eventID, PyObject *args);


/**
*/
KBE_DLL_API const char* kbe_getPyUserResPath();

/**
*/
KBE_DLL_API void kbe_shutdown(void);

enum KBEMessageType
{
	Print,
	Debug,
	warning,
	Info,
	Error,
	Critical
};

KBE_DLL_API void kbe_printMessage(char *message, KBEMessageType m = Print);

KBE_DLL_API KBEngine::client::Entity* kbe_findEntity(KBEngine::ENTITY_ID eid);

KBE_DLL_API bool kbe_isOnGround(KBEngine::ENTITY_ID eid);

//#endif // __KBENGINE_DLL__