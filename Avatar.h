#pragma once

//#include "TheWorld_ClientDll.h"
#include <string>

class KBAvatar
{
public:
	KBAvatar(__int64 avatarDBID, const char* avatarName);
	~KBAvatar();

	__int64 getAvatarID(void) { return m_avatarDBID; }
	const char* getAvatarName(void) { return m_avatarName.c_str(); }
	
	__declspec(dllexport) void dumpStatus(int idx, bool minidump);

protected:
	__int64 m_avatarDBID;
	std::string m_avatarName;
};

