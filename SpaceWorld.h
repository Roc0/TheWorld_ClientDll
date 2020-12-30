#pragma once

//#include "TheWorld_ClientDll.h"
#include <string>
#include <map>
#include <memory>

class Space {
public:
	__declspec(dllexport) Space(int spaceID, const char *resPath);
	void dumpStatus(int idx, bool minidump);
	const char * getResPath() { return m_resPath.c_str(); }

private:
	int m_spaceID;
	std::string m_resPath;
};


typedef std::map<int, std::shared_ptr<Space> > SPACES;


class SpaceWorld
{
public:
	__declspec(dllexport) SpaceWorld();
	virtual ~SpaceWorld();

	void setPlayerSpaceId(int playerSpaceId) { m_playerSpaceId = playerSpaceId; };
	int getPlayerSpaceId(void) { return m_playerSpaceId; };
	void addSpace(int spaceID, const char* resPath);
	__declspec(dllexport) Space* findSpace(int spaceID = -1);
	__declspec(dllexport) void dumpStatus(bool minidump);

private:
	int m_playerSpaceId;
	SPACES m_Spaces;
};

