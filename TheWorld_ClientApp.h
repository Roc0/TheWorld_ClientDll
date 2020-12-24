#pragma once


#include "TheWorld_ClientDll.h"
#include "client_lib/event.h"
#include <boost/algorithm/string.hpp>
#include <boost/thread/thread.hpp>

#include <string>
#include <queue>

class __declspec(dllexport) TheWorld_ClientApp : public KBEngine::EventHandle
{
public:
	TheWorld_ClientApp(void);
	~TheWorld_ClientApp(void);

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
	//typedef void (*pOnEventFunction)(const KBEngine::EventData* lpEventData);
	virtual void kbengine_MessagePump(TheWorld_ClientApp *clientApp);
	
	virtual void client_onEvent(const KBEngine::EventData* lpEventData) = 0;

	virtual void setDoSleepInMainLoop(bool b) { m_bDoSleepInMainLoop = b; };
	virtual bool getDoSleepInMainLoop(void) { return m_bDoSleepInMainLoop; };
	virtual void setLoginStatus(int i) { m_iLogin = i; };
	virtual int getLoginStatus(void) { return m_iLogin; };
	virtual void setAccountName(std::string s) { m_sAccountName = s; };
	virtual std::string getAccountName(void) { return m_sAccountName; };

private:
	std::queue< std::tr1::shared_ptr<const KBEngine::EventData> > events_;
	boost::mutex m_KbeEventsMutex;
	bool m_bDoSleepInMainLoop;
	std::string m_sAccountName;
	int m_iLogin;
#define LOGIN_NOT_DONE				0
#define LOGIN_STARTED				1
#define LOGIN_DONE					2
};

