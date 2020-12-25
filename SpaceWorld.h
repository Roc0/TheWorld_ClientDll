#pragma once

#include "TheWorld_ClientDll.h"

class __declspec(dllexport) Space {
public:
	Space(KBEngine::SPACE_ID spaceID, const std::string& resPath);
	void dumpStatus(int idx, bool minidump);
	std::string getResPath() { return m_resPath; }

protected:
	KBEngine::SPACE_ID m_spaceID;
	std::string m_resPath;
};


typedef std::map<KBEngine::SPACE_ID, std::tr1::shared_ptr<Space> > SPACES;


class __declspec(dllexport) SpaceWorld
{
public:
	SpaceWorld();
	virtual ~SpaceWorld();

	void setPlayerSpaceId(KBEngine::SPACE_ID playerSpaceId) { m_playerSpaceId = playerSpaceId; };
	KBEngine::SPACE_ID getPlayerSpaceId(void) { return m_playerSpaceId; };
	void addSpace(KBEngine::SPACE_ID spaceID, const std::string& resPath);
	Space* findSpace(KBEngine::SPACE_ID spaceID = -1);
	void dumpStatus(bool minidump);

protected:
	KBEngine::SPACE_ID m_playerSpaceId;
	SPACES m_Spaces;
};

