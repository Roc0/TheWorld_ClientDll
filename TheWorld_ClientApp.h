#pragma once


#include "TheWorld_ClientDll.h"
#include "client_lib/event.h"
#include <boost/algorithm/string.hpp>
#include <boost/thread/thread.hpp>
#include "SpaceWorld.h"
#include "Entity.h"
#include "PlayerEntity.h"
#include "OtherEntity.h"
#include "Avatar.h"

#include <string>
#include <queue>

typedef std::map<KBEngine::ENTITY_ID, std::tr1::shared_ptr<KBEntity> > ENTITIES;
typedef std::map<KBEngine::DBID, std::tr1::shared_ptr<KBAvatar> > AVATARS;

class __declspec(dllexport) TheWorld_ClientApp : public KBEngine::EventHandle
{
public:
	TheWorld_ClientApp(void);
	~TheWorld_ClientApp(void);

	// *** KBEngine interaction ***
	virtual bool kbengine_Init(void);
	virtual void kbengine_Destroy(void);
	virtual bool kbengine_Login(const char* accountname, const char* passwd, const char* datas = NULL, KBEngine::uint32 datasize = 0, const char* ip = NULL, KBEngine::uint32 port = 0);
	virtual bool kbengine_Logout(void);
	virtual void kbengine_Reset(void);
	virtual bool kbengine_CreateAvatar(std::string avatarName);
	virtual bool kbengine_RemoveAvatar(std::string avatarName);
	virtual bool kbengine_SelectAvatarGame(KBEngine::DBID avatarDBID);
	virtual void kbengine_onEvent(const KBEngine::EventData* lpEventData);
	virtual void kbengine_Sleep(UINT32 ui);
	virtual void kbengine_Shutdown(void);
	virtual UINT32 kbengine_PlayerID(void);
	virtual UINT64 kbengine_GenUUID64(void);
	virtual void kbengine_PrintMessage(char* message, bool interline = false, bool console = true, KBEMessageType t = Print);
	virtual void kbengine_MessagePump(TheWorld_ClientApp *clientApp);
	virtual void kbengine_UpdateVolatile(void);
	// *** KBEngine interaction ***

	//virtual void setMain(TheWorld_ClientApp* pMain) { m_pMain = pMain; };

	virtual void client_onEvent(const KBEngine::EventData* lpEventData);

	virtual void clearEntities(void) { m_Entities.clear(); };
	virtual int eraseEntity(KBEngine::ENTITY_ID eid) { return m_Entities.erase(eid); };
	virtual KBEntity* getEntity(KBEngine::ENTITY_ID eid, bool& bPlayer)
	{
		bPlayer = false; 
		if (kbengine_PlayerID() == eid)
			bPlayer = true;
		ENTITIES::iterator iter = m_Entities.find(eid);
		if (iter == m_Entities.end())
			return NULL;
		else
			return iter->second.get();
	}
	virtual KBEntity* createEntity(KBEngine::ENTITY_ID eid, bool& bPlayer)
	{
		KBEntity* pEntity = NULL;
		bPlayer = false;
		if (kbengine_PlayerID() == eid)
		{
			bPlayer = true;
			pEntity = new PlayerEntity(eid, getSpaceWorld());
			//setPlayerEntity(pEntity);
		}
		else
			pEntity = new OtherEntity(eid, getSpaceWorld());
		m_Entities[eid].reset(pEntity);
		return pEntity;
	}

	virtual void clearAvatars(void) { m_Avatars.clear(); };
	virtual int eraseAvatar(KBEngine::DBID dbid) { return m_Avatars.erase(dbid); };
	virtual KBAvatar* getAvatar(KBEngine::DBID dbid)
	{
		AVATARS::iterator iter = m_Avatars.find(dbid);
		if (iter == m_Avatars.end())
			return NULL;
		else
			return iter->second.get();
	}

	// config AppMode
	enum _AppMode
	{
		InitialMenu,
		WorldMode
	};
	virtual void setAppMode(enum TheWorld_ClientApp::_AppMode r, bool bForce = false)
	{
		if (m_appMode != r || bForce)
		{
			m_appMode = r;
			m_bInitAppModeRequired = true;
		}
	};
	virtual enum TheWorld_ClientApp::_AppMode getAppMode(void) { return m_appMode; };
	virtual bool getInitAppModeRequired(void) { return m_bInitAppModeRequired; };
	virtual void setInitAppModeRequired(bool b) { m_bInitAppModeRequired = b; };
	virtual bool getReinitAppModeRequired(void) { return m_bReinitAppModeRequired; };
	virtual void setReinitAppModeRequired(bool b) { m_bReinitAppModeRequired = b; };
	// config AppMode

	virtual void setShutdownRequired(bool b) { m_bShutDown = b; };
	virtual bool getShutdownRequired(void) { return m_bShutDown; };

	bool playerEnterSpace(KBEngine::SPACE_ID spaceID)
	{
		m_bReinitAppModeRequired = true;
		getSpaceWorld()->setPlayerSpaceId(spaceID);
		return true;
	};

	void playerLeaveSpace(KBEngine::SPACE_ID spaceID)
	{
		if (getSpaceWorld()->getPlayerSpaceId() != spaceID)
			return;
		getSpaceWorld()->setPlayerSpaceId(-1);
		m_bReinitAppModeRequired = true;
	};

	virtual void setDoSleepInMainLoop(bool b) { m_bDoSleepInMainLoop = b; };
	virtual bool getDoSleepInMainLoop(void) { return m_bDoSleepInMainLoop; };
	virtual void setLoginStatus(int i) { m_iLogin = i; };
	virtual int getLoginStatus(void) { return m_iLogin; };
	virtual void setAccountName(std::string s) { m_sAccountName = s; };
	virtual std::string getAccountName(void) { return m_sAccountName; };
	virtual SpaceWorld* getSpaceWorld(void) { return m_pSpaceWorld; };
	virtual ENTITIES& getEntities(void) { return m_Entities; };
	virtual AVATARS& getAvatars(void) { return m_Avatars; };
	virtual void setPlayerEntity(KBEntity* pPlayer) { m_pPlayerEntity = pPlayer; }
	virtual KBEntity* getPlayerEntity(void) { return m_pPlayerEntity; }
	virtual void setTargetEntity(KBEngine::ENTITY_ID eid) { bool bPlayer; m_pTargetEntity = getEntity(eid, bPlayer); }
	virtual void setTargetEntity(KBEntity* pTarget) { m_pTargetEntity = pTarget; }
	virtual KBEntity* getTargetEntity(void) { return m_pTargetEntity; }
	virtual void setMouseTarget(KBEntity* pTarget) { m_pMouseTarget = pTarget; }
	virtual KBEntity* getMouseTarget(void) { return m_pMouseTarget; }

private:
	//TheWorld_ClientApp* m_pMain;
	bool m_bShutDown;
	enum TheWorld_ClientApp::_AppMode m_appMode;
	bool m_bInitAppModeRequired;
	bool m_bReinitAppModeRequired;

	std::queue< std::tr1::shared_ptr<const KBEngine::EventData> > events_;
	boost::mutex m_KbeEventsMutex;
	bool m_bDoSleepInMainLoop;
	std::string m_sAccountName;
	int m_iLogin;
#define LOGIN_NOT_DONE				0
#define LOGIN_STARTED				1
#define LOGIN_DONE					2

	SpaceWorld* m_pSpaceWorld;
	ENTITIES m_Entities;
	KBEntity* m_pPlayerEntity;
	KBEntity* m_pTargetEntity;
	KBEntity* m_pMouseTarget;
	AVATARS m_Avatars;
};

