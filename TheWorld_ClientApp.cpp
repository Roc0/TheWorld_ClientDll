#include "TheWorld_ClientApp.h"

#include <boost/algorithm/string.hpp>
#include <boost/thread/thread.hpp>

HINSTANCE g_hKBEngineDll = NULL;

// Default Account Name
std::string g_defaultAccountName("6529868038458114048");
//------

TheWorld_ClientApp::TheWorld_ClientApp() :
	events_()
{
	m_bDoSleepInMainLoop = true;
	m_iLogin = LOGIN_NOT_DONE;

	m_pSpaceWorld = new SpaceWorld;
	m_pPlayerEntity = m_pTargetEntity = m_pMouseTarget = NULL;
}

TheWorld_ClientApp::~TheWorld_ClientApp()
{
}

bool TheWorld_ClientApp::kbengine_Init(void)
{
#ifdef _DEBUG
	std::string kbenginedll_name = "TheWorld_ClientDll_d.dll";
#else
	std::string kbenginedll_name = "TheWorld_ClientDll.dll";
#endif

	g_hKBEngineDll = LoadLibrary(kbenginedll_name.c_str());
	if (g_hKBEngineDll == NULL)
	{
		std::string s = "kbengine_Init - Load " + kbenginedll_name + " is failed!";
		char str[256];	strcpy(str, s.c_str());
		kbengine_PrintMessage(str);
		return 0;
	}

	if (!kbe_init())
	{
		kbengine_PrintMessage("kbengine_init() is failed!");
		return 0;
	}


	kbe_registerEventHandle(this);

	return true;
}

void TheWorld_ClientApp::kbengine_Destroy(void)
{
	kbe_deregisterEventHandle(this);
	kbe_destroy();
	FreeLibrary(g_hKBEngineDll);
}

void TheWorld_ClientApp::kbengine_Sleep(UINT32 ui)
{
	kbe_sleep(ui);
}

void TheWorld_ClientApp::kbengine_Shutdown(void)
{
	kbe_shutdown();
}

UINT32 TheWorld_ClientApp::kbengine_PlayerID(void)
{
	return (UINT32)kbe_playerID();
}

UINT64 TheWorld_ClientApp::kbengine_GenUUID64(void)
{
	return KBEngine::genUUID64();
}

void TheWorld_ClientApp::kbengine_onEvent(const KBEngine::EventData* lpEventData)
{
	KBEngine::EventData* peventdata = KBEngine::copyKBEngineEvent(lpEventData);

	if (peventdata)
	{
		boost::mutex::scoped_lock lock(m_KbeEventsMutex);
		events_.push(std::tr1::shared_ptr<const KBEngine::EventData>(peventdata));
		m_bDoSleepInMainLoop = false;
	}
}

bool TheWorld_ClientApp::kbengine_Login(const char* accountname, const char* passwd, const char* datas, KBEngine::uint32 datasize, const char* ip, KBEngine::uint32 port)
{
	std::string accountName;
	if (accountname == NULL)
	{
		m_sAccountName = kbe_getLastAccountName();
		if (m_sAccountName.size() == 0)
		{
			/*KBEngine::uint64 uuid = kbe_genUUID64();
			std::stringstream ss;
			ss << uuid;
			g_accountName = ss.str();
			//g_accountName = KBEngine::StringConv::val2str(KBEngine::genUUID64());*/
			m_sAccountName = g_defaultAccountName;
		}
		accountName = m_sAccountName;
	}
	else
		accountName = accountname;

	char str[256];
	sprintf(str, "KBE Login - AccountName: [%s], Password: [%s], Client Name: [%s]\n", accountName.c_str(), passwd, datas);
	kbengine_PrintMessage(str);
	if (kbe_login(accountName.c_str(), passwd, datas, datasize, ip, port))
	{
		m_iLogin = LOGIN_STARTED;
		return true;
	}
	else
		return false;
}

bool TheWorld_ClientApp::kbengine_Logout()
{
	kbengine_PrintMessage("KBE Logout\n");
	kbengine_Reset();
	m_iLogin = LOGIN_NOT_DONE;

	return true;
}

bool TheWorld_ClientApp::kbengine_CreateAvatar(std::string avatarName)
{
	char str[256];
	sprintf(str, "KBE Create Avatar - Name: [%s]\n", avatarName.c_str());
	kbengine_PrintMessage(str);

	kbe_lock();
	sprintf(str, "[1, \"kbeconsole_%s\"]", avatarName.c_str());
	kbe_callEntityMethod(kbe_playerID(), "reqCreateAvatar", str);
	kbe_unlock();

	return true;
}

bool TheWorld_ClientApp::kbengine_RemoveAvatar(std::string avatarName)
{
	char str[256];
	sprintf(str, "KBE Remove Avatar - Name: [%s]\n", avatarName.c_str());
	kbengine_PrintMessage(str);

	kbe_lock();
	kbe_callEntityMethod(kbe_playerID(), "reqRemoveAvatar", avatarName.c_str());
	kbe_unlock();

	return true;
}

bool TheWorld_ClientApp::kbengine_SelectAvatarGame(KBEngine::DBID avatarDBID)
{
	char str[256];
	sprintf(str, "KBE Select Avatar Game - DBID: [%d]\n", (int)avatarDBID);
	kbengine_PrintMessage(str);

	kbe_lock();
	kbe_callEntityMethod(kbe_playerID(), "selectAvatarGame", KBEngine::StringConv::val2str(avatarDBID).c_str());
	kbe_unlock();

	return true;
}

void TheWorld_ClientApp::kbengine_Reset(void)
{
	kbe_reset();
}

void TheWorld_ClientApp::kbengine_PrintMessage(char* message, bool interline, bool console, KBEMessageType t)
{
	if (message[strlen(message) - 1] != '\n')
		kbe_printMessage("Next message without trailing New Line", t);
	kbe_printMessage(message, t);
	if (console)
	{
		printf(message);
		if (interline)
			printf("\n");
	}
}

void TheWorld_ClientApp::kbengine_MessagePump(TheWorld_ClientApp* clientApp)
{
	while (true)
	{
		m_KbeEventsMutex.lock();

		if (events_.empty())
		{
			m_KbeEventsMutex.unlock();
			break;
		}

		std::tr1::shared_ptr<const KBEngine::EventData> pEventData = events_.front();
		events_.pop();
		m_KbeEventsMutex.unlock();

		KBEngine::EventID id = pEventData->id;

		if (id == CLIENT_EVENT_SCRIPT)
		{
			kbe_lock();
		}

		clientApp->client_onEvent(pEventData.get());

		if (id == CLIENT_EVENT_SCRIPT)
		{
			kbe_unlock();
		}

		m_bDoSleepInMainLoop = true;
	}
}

void TheWorld_ClientApp::kbengine_UpdateVolatile(void)
{
	KBEntity* pPlayer = getPlayerEntity();
	if (pPlayer != NULL)
	{
		KBEngine::ENTITY_ID eid = (getTargetEntity() == NULL ? -1 : getTargetEntity()->id());
		float x = 0, y = 0, z = 0, yaw = 0, pitch = 0, roll = 0;
		pPlayer->getDesideredPosition(x, y, z);
		pPlayer->getDesideredDirection(yaw, pitch, roll);

		kbe_updateVolatile(eid, x, y, z, yaw, pitch, roll);

		pPlayer->setPosition(x, y, z);
		pPlayer->setDirection(yaw, pitch, roll);
	}
}
