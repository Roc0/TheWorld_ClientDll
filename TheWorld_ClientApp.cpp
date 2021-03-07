#include "TheWorld_ClientApp.h"

#include <json/json.h>
#include <boost/algorithm/string.hpp>
#include <boost/thread/thread.hpp>

//HINSTANCE g_hKBEngineDll = NULL;

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

	m_bInitAppModeRequired = true;
	m_bReinitAppModeRequired = false;

	setShutdownRequired(false);

	//m_pMain = NULL;
}

TheWorld_ClientApp::~TheWorld_ClientApp()
{
}

bool TheWorld_ClientApp::kbengine_Init(void)
{
/*
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
*/
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
	//FreeLibrary(g_hKBEngineDll);
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
	//sprintf(str, "[1, \"kbeconsole_%s\"]", avatarName.c_str());
	sprintf(str, "[1, \"%s\"]", avatarName.c_str());
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

void TheWorld_ClientApp::kbengine_Relive(void)
{
	kbe_fireEvent("relive", NULL);
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
		pPlayer->getForClientPosition(x, y, z);
		pPlayer->getForClientDirection(yaw, pitch, roll);

		kbe_updateVolatile(eid, x, y, z, yaw, pitch, roll);

		//pPlayer->setForClientPosition(x, y, z);
		//pPlayer->setForClientDirection(yaw, pitch, roll);
	}
}

void TheWorld_ClientApp::client_onEvent(const KBEngine::EventData* lpEventData)
{
	char str[256];

	switch (lpEventData->id)
	{
	case CLIENT_EVENT_ENTERWORLD:
	{
		const KBEngine::EventData_EnterWorld* pEventData_EnterWorld = static_cast<const KBEngine::EventData_EnterWorld*>(lpEventData);
		KBEngine::ENTITY_ID eid = pEventData_EnterWorld->entityID;

		bool bPlayer = false;
		KBEntity* pEntity = getEntity(eid, bPlayer);

		if (!pEntity)
		{
			if (isDebugEnabled())
			{
				sprintf(str, "KBE Event received ==> CLIENT_EVENT_ENTERWORLD - %s - EntityID: %d (NOT IN LIST), SpaceID: %d, Yaw/Pitch/Roll: %f/%f/%f, X/Y/Z: %f/%f/%f, Speed: %f, IsOnGround: %d, Res: %s\n", bPlayer ? "PLAYER" : "OTHER",
					(int)pEventData_EnterWorld->entityID, (int)pEventData_EnterWorld->spaceID,
					pEventData_EnterWorld->yaw, pEventData_EnterWorld->pitch, pEventData_EnterWorld->roll,
					pEventData_EnterWorld->x, pEventData_EnterWorld->y, pEventData_EnterWorld->z,
					pEventData_EnterWorld->speed, pEventData_EnterWorld->isOnGround, pEventData_EnterWorld->res.c_str());
				kbengine_PrintMessage(str, true);
			}
			break;
		}

		if (bPlayer)
			setPlayerEntity(pEntity);

		pEntity->setKnowByServerPosition(pEventData_EnterWorld->x, pEventData_EnterWorld->y, pEventData_EnterWorld->z);
		//if (bPlayer)
		//	pEntity->setNewPlayerPosition(pEventData_EnterWorld->x, pEventData_EnterWorld->y, pEventData_EnterWorld->z);
		pEntity->setKnowByServerDirection(pEventData_EnterWorld->yaw, pEventData_EnterWorld->pitch, pEventData_EnterWorld->roll);
		//if (bPlayer)
		//	pEntity->setNewPlayerDirection(pEventData_EnterWorld->yaw, pEventData_EnterWorld->pitch, pEventData_EnterWorld->roll);
		pEntity->setMoveSpeed(pEventData_EnterWorld->speed);
		pEntity->setSpaceID(pEventData_EnterWorld->spaceID);
		pEntity->setIsOnGround(pEventData_EnterWorld->isOnGround);
		pEntity->setRes(pEventData_EnterWorld->res.c_str());

		pEntity->setIsInWorld(true);

		if (isDebugEnabled())
		{
			sprintf(str, "KBE Event received ==> CLIENT_EVENT_ENTERWORLD - %s - EntityID: %d, SpaceID: %d, Yaw/Pitch/Roll: %f/%f/%f, X/Y/Z: %f/%f/%f, Speed: %f, IsOnGround: %d, Res: %s\n", bPlayer ? "PLAYER" : "OTHER",
				(int)pEventData_EnterWorld->entityID, (int)pEventData_EnterWorld->spaceID,
				pEventData_EnterWorld->yaw, pEventData_EnterWorld->pitch, pEventData_EnterWorld->roll,
				pEventData_EnterWorld->x, pEventData_EnterWorld->y, pEventData_EnterWorld->z,
				pEventData_EnterWorld->speed, pEventData_EnterWorld->isOnGround, pEventData_EnterWorld->res.c_str());
			kbengine_PrintMessage(str, true);
		}

		//if (bPlayer)
		//	setAppMode(TheWorld_ClientApp_GL::GraphicMode);
	}
	break;

	case CLIENT_EVENT_LEAVEWORLD:
	{
		KBEngine::ENTITY_ID eid = static_cast<const KBEngine::EventData_LeaveWorld*>(lpEventData)->entityID;

		bool bPlayer = false;
		if (kbengine_PlayerID() == eid)
		{
			bPlayer = true;
			setPlayerEntity(NULL);
		}

		if (getTargetEntity() && getTargetEntity()->id() == eid)
			setTargetEntity(NULL);

		if (getMouseTarget() && getMouseTarget()->id() == eid)
			setMouseTarget(NULL);

		eraseEntity(eid);

		if (isDebugEnabled())
		{
			sprintf(str, "KBE Event received ==> CLIENT_EVENT_LEAVEWORLD - %s - EntityID %d\n", bPlayer ? "PLAYER" : "OTHER", (int)eid);
			kbengine_PrintMessage(str);
		}
	}
	break;

	case CLIENT_EVENT_ENTERSPACE:
	{
		const KBEngine::EventData_EnterSpace* pEventData_EnterSpace = static_cast<const KBEngine::EventData_EnterSpace*>(lpEventData);
		KBEngine::ENTITY_ID eid = pEventData_EnterSpace->entityID;

		bool bPlayer = false;
		KBEntity* pEntity = getEntity(eid, bPlayer);
		if (pEntity == NULL)
		{
			if (isDebugEnabled())
			{
				sprintf(str, "KBE Event received ==> CLIENT_EVENT_ENTERSPACE - %s - EntityID: %d (NOT IN LIST), SpaceID: %d, Yaw/Pitch/Roll: %f/%f/%f, X/Y/Z: %f/%f/%f, Speed: %f, IsOnGround: %d, Res: %s\n", bPlayer ? "PLAYER" : "OTHER",
					(int)pEventData_EnterSpace->entityID, (int)pEventData_EnterSpace->spaceID,
					pEventData_EnterSpace->yaw, pEventData_EnterSpace->pitch, pEventData_EnterSpace->roll,
					pEventData_EnterSpace->x, pEventData_EnterSpace->y, pEventData_EnterSpace->z,
					pEventData_EnterSpace->speed, pEventData_EnterSpace->isOnGround, pEventData_EnterSpace->res.c_str());
				kbengine_PrintMessage(str, true);
			}
			break;
		}

		pEntity->setKnowByServerPosition(pEventData_EnterSpace->x, pEventData_EnterSpace->y, pEventData_EnterSpace->z);
		//if (bPlayer)
		//	pEntity->setNewPlayerPosition(pEventData_EnterSpace->x, pEventData_EnterSpace->y, pEventData_EnterSpace->z);
		pEntity->setKnowByServerDirection(pEventData_EnterSpace->yaw, pEventData_EnterSpace->pitch, pEventData_EnterSpace->roll);
		//if (bPlayer)
		//	pEntity->setNewPlayerDirection(pEventData_EnterSpace->yaw, pEventData_EnterSpace->pitch, pEventData_EnterSpace->roll);
		pEntity->setMoveSpeed(pEventData_EnterSpace->speed);
		pEntity->setSpaceID(pEventData_EnterSpace->spaceID);
		pEntity->setIsOnGround(pEventData_EnterSpace->isOnGround);
		pEntity->setRes(pEventData_EnterSpace->res.c_str());

		pEntity->setIsInWorld(true);

		if (isDebugEnabled())
		{
			sprintf(str, "KBE Event received ==> CLIENT_EVENT_ENTERSPACE - %s - EntityID: %d, SpaceID: %d, Yaw/Pitch/Roll: %f/%f/%f, X/Y/Z: %f/%f/%f, Speed: %f, IsOnGround: %d, Res: %s\n", bPlayer ? "PLAYER" : "OTHER",
				(int)pEventData_EnterSpace->entityID, (int)pEventData_EnterSpace->spaceID,
				pEventData_EnterSpace->yaw, pEventData_EnterSpace->pitch, pEventData_EnterSpace->roll,
				pEventData_EnterSpace->x, pEventData_EnterSpace->y, pEventData_EnterSpace->z,
				pEventData_EnterSpace->speed, pEventData_EnterSpace->isOnGround, pEventData_EnterSpace->res.c_str());
			kbengine_PrintMessage(str, true);
		}

		if (bPlayer)
		{
			setAppMode(TheWorld_ClientApp::WorldMode);
			playerEnterSpace(pEventData_EnterSpace->spaceID);
		}
	}
	break;

	case CLIENT_EVENT_LEAVESPACE:
	{
		KBEngine::ENTITY_ID eid = static_cast<const KBEngine::EventData_LeaveSpace*>(lpEventData)->entityID;
		KBEngine::SPACE_ID spaceId = static_cast<const KBEngine::EventData_LeaveSpace*>(lpEventData)->spaceID;

		bool bPlayer = false;
		if (kbengine_PlayerID() == eid)
		{
			bPlayer = true;
			//m_pPlayer = NULL;
		}

		if (getTargetEntity() && getTargetEntity()->id() == eid)
			setTargetEntity(NULL);

		if (getMouseTarget() && getMouseTarget()->id() == eid)
			setMouseTarget(NULL);

		eraseEntity(eid);

		if (isDebugEnabled())
		{
			sprintf(str, "KBE Event received ==> CLIENT_EVENT_LEAVESPACE - %s - EntityID %d, SpaceID: %d\n", bPlayer ? "PLAYER" : "OTHER", (int)eid, (int)spaceId);
			kbengine_PrintMessage(str);
		}

		if (bPlayer)
		{
			playerLeaveSpace(spaceId);
		}
	}
	break;

	case CLIENT_EVENT_CREATEDENTITY:
	{
		const KBEngine::EventData_CreatedEntity* pEventData_createEntity = static_cast<const KBEngine::EventData_CreatedEntity*>(lpEventData);
		KBEngine::ENTITY_ID eid = pEventData_createEntity->entityID;

		bool bPlayer = false;
		KBEntity* pEntity = createEntity(eid, bPlayer);

		pEntity->setModelScale(pEventData_createEntity->modelScale);

		if (isDebugEnabled())
		{
			sprintf(str, "KBE Event received ==> CLIENT_EVENT_CREATEDENTITY - %s - EntityID: %d, ModelScale: %f\n", bPlayer ? "PLAYER" : "OTHER", (int)pEventData_createEntity->entityID, (float)pEventData_createEntity->modelScale);
			kbengine_PrintMessage(str);
		}
	}
	break;

	case CLIENT_EVENT_LOGIN_SUCCESS:
	{
		setLoginStatus(LOGIN_DONE);
		if (isDebugEnabled())
		{
			strcpy(str, "KBE Event received ==> CLIENT_EVENT_LOGIN_SUCCESS\n");
			kbengine_PrintMessage(str);
		}
		onLoginSuccess();
	}
	break;

	case CLIENT_EVENT_LOGIN_FAILED:
	{
		setLoginStatus(LOGIN_NOT_DONE);
		const KBEngine::EventData_LoginFailed* info = static_cast<const KBEngine::EventData_LoginFailed*>(lpEventData);

		if (isDebugEnabled())
		{
			if (info->failedcode == 20)
			{
				sprintf(str, "KBE Event received ==> CLIENT_EVENT_LOGIN_FAILED - Server is starting, please wait!\n");
			}
			else
			{
				sprintf(str, "KBE Event received ==> CLIENT_EVENT_LOGIN_FAILED - Login is failed (code=%u)!\n", info->failedcode);
			}
			kbengine_PrintMessage(str);
		}

		onLoginFailed(info->failedcode);
	}
	break;

	case CLIENT_EVENT_LOGIN_BASEAPP_SUCCESS:
	{
		if (isDebugEnabled())
		{
			strcpy(str, "KBE Event received ==> CLIENT_EVENT_LOGIN_BASEAPP_SUCCESS\n");
			kbengine_PrintMessage(str);
		}
	}
	break;

	case CLIENT_EVENT_LOGIN_BASEAPP_FAILED:
	{
		const KBEngine::EventData_LoginBaseappFailed* info = static_cast<const KBEngine::EventData_LoginBaseappFailed*>(lpEventData);
		if (isDebugEnabled())
		{
			sprintf(str, "KBE Event received ==> CLIENT_EVENT_LOGIN_BASEAPP_FAILED (code=%u)!\n", info->failedcode);
			kbengine_PrintMessage(str);
		}
	}
	break;

	case CLIENT_EVENT_SCRIPT:
	{
		const KBEngine::EventData_Script* peventdata = static_cast<const KBEngine::EventData_Script*>(lpEventData);

		Json::Reader reader;
		Json::Value root;

		if (!reader.parse(peventdata->datas.c_str(), root))
		{
			assert(false);
		}

		if (peventdata->name == "update_avatars")
		{
			if (isDebugEnabled())
			{
				strcpy(str, "KBE Event received ==> CLIENT_EVENT_SCRIPT, peventdata->name: ");
				strcat(str, peventdata->name.c_str());
				strcat(str, "\n");
				kbengine_PrintMessage(str);
			}

			setInitAppModeRequired(true);

			clearAvatars();

			Json::Value::Members mem = root.getMemberNames();
			for (auto iter = mem.begin(); iter != mem.end(); iter++)
			{
				Json::Value& val = root[*iter];
				std::string name = val[1].asString();
				KBEngine::DBID avatarDBID = val[Json::Value::UInt(0)].asUInt();
				KBAvatar* pAvatar = new KBAvatar(avatarDBID, name.c_str());
				AVATARS& avatars = getAvatars();
				avatars[avatarDBID].reset(pAvatar);
				if (isDebugEnabled())
				{
					sprintf(str, "\t\tAvatar DBID: [%ld] , Avatar Name: [%s]\n", (long)avatarDBID, name.c_str());
					kbengine_PrintMessage(str, true);
				}
			}

			onUpdateAvatars();
		}
		else
		{
			KBEngine::ENTITY_ID eid = root[Json::Value::UInt(0)].asInt();

			bool bPlayer = false;
			KBEntity* pEntity = getEntity(eid, bPlayer);;

			if (peventdata->name == "set_name")
			{
				if (pEntity == NULL)
				{
					if (isDebugEnabled())
					{
						sprintf(str, "KBE Event received ==> CLIENT_EVENT_SCRIPT (ENTITY NOT FOUND), EntityID: %d, peventdata->name: %s\n", (int)eid, peventdata->name.c_str());
						kbengine_PrintMessage(str);
					}
					break;
				}

				std::string name = root[1].asString();
				pEntity->setName(name.c_str());

				if (isDebugEnabled())
				{
					sprintf(str, "KBE Event received ==> CLIENT_EVENT_SCRIPT, EntityID: %d, peventdata->name: %s, name: %s\n", (int)eid, peventdata->name.c_str(), name.c_str());
					kbengine_PrintMessage(str);
				}
			}
			else if (peventdata->name == "set_class_name")
			{
				if (pEntity == NULL)
				{
					if (isDebugEnabled())
					{
						sprintf(str, "KBE Event received ==> CLIENT_EVENT_SCRIPT (ENTITY NOT FOUND), EntityID: %d, peventdata->name: %s\n", (int)eid, peventdata->name.c_str());
						kbengine_PrintMessage(str);
					}
					break;
				}

				std::string className = root[1].asString();
				pEntity->setClassName(className);

				if (isDebugEnabled())
				{
					sprintf(str, "KBE Event received ==> CLIENT_EVENT_SCRIPT, EntityID: %d, peventdata->name: %s, name: %s\n", (int)eid, peventdata->name.c_str(), className.c_str());
					kbengine_PrintMessage(str);
				}
			}
			else if (peventdata->name == "set_modelScale")
			{
				if (pEntity == NULL)
				{
					if (isDebugEnabled())
					{
						sprintf(str, "KBE Event received ==> CLIENT_EVENT_SCRIPT (ENTITY NOT FOUND), EntityID: %d, peventdata->name: %s\n", (int)eid, peventdata->name.c_str());
						kbengine_PrintMessage(str);
					}
					break;
				}

				uint32_t scale = root[1].asUInt();
				pEntity->setModelScale(scale / (float)100.0);

				if (isDebugEnabled())
				{
					sprintf(str, "KBE Event received ==> CLIENT_EVENT_SCRIPT, EntityID: %d, peventdata->name: %s, modelScale: %d\n", (int)eid, peventdata->name.c_str(), (int)scale);
					kbengine_PrintMessage(str);
				}
			}
			else if (peventdata->name == "set_modelID")
			{
				if (pEntity == NULL)
				{
					if (isDebugEnabled())
					{
						sprintf(str, "KBE Event received ==> CLIENT_EVENT_SCRIPT (ENTITY NOT FOUND), EntityID: %d, peventdata->name: %s\n", (int)eid, peventdata->name.c_str());
						kbengine_PrintMessage(str);
					}
					break;
				}

				uint32_t modelID = root[1].asUInt();
				pEntity->setModelID(modelID);

				if (isDebugEnabled())
				{
					sprintf(str, "KBE Event received ==> CLIENT_EVENT_SCRIPT, EntityID: %d, peventdata->name: %s, modelID: %d\n", (int)eid, peventdata->name.c_str(), (int)modelID);
					kbengine_PrintMessage(str);
				}
			}
			else if (peventdata->name == "set_state")
			{
				if (pEntity == NULL)
				{
					if (isDebugEnabled())
					{
						sprintf(str, "KBE Event received ==> CLIENT_EVENT_SCRIPT (ENTITY NOT FOUND), EntityID: %d, peventdata->name: %s\n", (int)eid, peventdata->name.c_str());
						kbengine_PrintMessage(str);
					}
					break;
				}

				int32_t state = root[1].asInt();
				pEntity->setState(state);

				if (isDebugEnabled())
				{
					sprintf(str, "KBE Event received ==> CLIENT_EVENT_SCRIPT, EntityID: %d, peventdata->name: %s, state: %d\n", (int)eid, peventdata->name.c_str(), (int)state);
					kbengine_PrintMessage(str);
				}
			}
			else if (peventdata->name == "set_HP_Max")
			{
				if (pEntity == NULL)
				{
					if (isDebugEnabled())
					{
						sprintf(str, "KBE Event received ==> CLIENT_EVENT_SCRIPT (ENTITY NOT FOUND), EntityID: %d, peventdata->name: %s\n", (int)eid, peventdata->name.c_str());
						kbengine_PrintMessage(str);
					}
					break;
				}

				int32_t v = root[1].asInt();
				pEntity->setHPMax(v);

				if (isDebugEnabled())
				{
					sprintf(str, "KBE Event received ==> CLIENT_EVENT_SCRIPT, EntityID: %d, peventdata->name: %s, HPMax: %d\n", (int)eid, peventdata->name.c_str(), (int)v);
					kbengine_PrintMessage(str);
				}
			}
			else if (peventdata->name == "set_MP_Max")
			{
				if (pEntity == NULL)
				{
					if (isDebugEnabled())
					{
						sprintf(str, "KBE Event received ==> CLIENT_EVENT_SCRIPT (ENTITY NOT FOUND), EntityID: %d, peventdata->name: %s\n", (int)eid, peventdata->name.c_str());
						kbengine_PrintMessage(str);
					}
					break;
				}

				int32_t v = root[1].asInt();
				pEntity->setMPMax(v);

				if (isDebugEnabled())
				{
					sprintf(str, "KBE Event received ==> CLIENT_EVENT_SCRIPT, EntityID: %d, peventdata->name: %s, MPMax: %d\n", (int)eid, peventdata->name.c_str(), (int)v);
					kbengine_PrintMessage(str);
				}
			}
			else if (peventdata->name == "recvDamage")
			{
				KBEngine::ENTITY_ID attackerID = root[1].asInt();
				uint32_t skillID = root[2].asUInt();
				uint32_t damageType = root[3].asUInt();
				uint32_t damage = root[4].asUInt();
				uint32_t HP = root[5].asUInt();

				bool bPlayer = false;
				KBEntity* attacker = getEntity(attackerID, bPlayer);
				KBEntity* receiver = pEntity;
				KBEngine::ENTITY_ID eidAttacker = 0;

				if (attacker)
				{
					attacker->attack(receiver, skillID, damageType, damage);
				}

				if (receiver)
				{
					receiver->recvDamage(attacker, skillID, damageType, damage);
					receiver->setHP(HP);
				}

				if (isDebugEnabled())
				{
					sprintf(str, "KBE Event received ==> CLIENT_EVENT_SCRIPT, EntityID: %d, peventdata->name: %s, EID Attacker: %d, skillId: %d, damageType: %d, damage: %d, HP: %d\n", (int)eid, peventdata->name.c_str(), (int)eidAttacker, (int)skillID, (int)damageType, (int)damage, (int)HP);
					kbengine_PrintMessage(str);
				}
			}
		}
		// TODO
	}
	break;

	case CLIENT_EVENT_POSITION_CHANGED:
	{
		const KBEngine::EventData_PositionChanged* pEventData = static_cast<const KBEngine::EventData_PositionChanged*>(lpEventData);
		KBEngine::ENTITY_ID eid = pEventData->entityID;

		bool bPlayer = false;
		KBEntity* pEntity = getEntity(eid, bPlayer);
		if (!pEntity)
		{
			if (isDebugEnabled())
			{
				sprintf(str, "KBE Event received ==> CLIENT_EVENT_POSITION_CHANGED - %s - EntityID %d (NOT IN LIST), X/Y/Z: %f/%f/%f\n", bPlayer ? "PLAYER" : "OTHER", (int)eid, pEventData->x, pEventData->y, pEventData->z);
				kbengine_PrintMessage(str);
			}
			break;
		}

		pEntity->setKnowByServerPosition(pEventData->x, pEventData->y, pEventData->z);
		//if (bPlayer)
		//	pEntity->setNewPlayerPosition(pEventData->x, pEventData->y, pEventData->z);

		if (isDebugEnabled())
		{
			sprintf(str, "KBE Event received ==> CLIENT_EVENT_POSITION_CHANGED - %s - EntityID %d, X/Y/Z: %f/%f/%f\n", bPlayer ? "PLAYER" : "OTHER", (int)eid, pEventData->x, pEventData->y, pEventData->z);
			kbengine_PrintMessage(str);
		}
	}
	break;

	case CLIENT_EVENT_DIRECTION_CHANGED:
	{
		const KBEngine::EventData_DirectionChanged* pEventData = static_cast<const KBEngine::EventData_DirectionChanged*>(lpEventData);
		KBEngine::ENTITY_ID eid = pEventData->entityID;

		bool bPlayer = false;
		KBEntity* pEntity = getEntity(eid, bPlayer);
		if (!pEntity)
		{
			if (isDebugEnabled())
			{
				sprintf(str, "KBE Event received ==> CLIENT_EVENT_DIRECTION_CHANGED - %s - EntityID %d (NOT IN LIST), Yaw/Pitch/Roll: %f/%f/%f\n", bPlayer ? "PLAYER" : "OTHER", (int)eid, pEventData->yaw, pEventData->pitch, pEventData->roll);
				kbengine_PrintMessage(str);
			}
			break;
		}

		pEntity->setKnowByServerDirection(pEventData->yaw, pEventData->pitch, pEventData->roll);
		//if (bPlayer)
		//	pEntity->setNewPlayerDirection(pEventData->yaw, pEventData->pitch, pEventData->roll);

		if (isDebugEnabled())
		{
			sprintf(str, "KBE Event received ==> CLIENT_EVENT_DIRECTION_CHANGED - %s - EntityID %d, Yaw/Pitch/Roll: %f/%f/%f\n", bPlayer ? "PLAYER" : "OTHER", (int)eid, pEventData->yaw, pEventData->pitch, pEventData->roll);
			kbengine_PrintMessage(str);
		}
	}
	break;

	case CLIENT_EVENT_MOVESPEED_CHANGED:
	{
		const KBEngine::EventData_MoveSpeedChanged* pEventData = static_cast<const KBEngine::EventData_MoveSpeedChanged*>(lpEventData);
		KBEngine::ENTITY_ID eid = pEventData->entityID;

		bool bPlayer = false;
		KBEntity* pEntity = getEntity(eid, bPlayer);
		if (!pEntity)
		{
			if (isDebugEnabled())
			{
				sprintf(str, "KBE Event received ==> CLIENT_EVENT_MOVESPEED_CHANGED - %s - EntityID %d (NOT IN LIST), MoveSpeed %f\n", bPlayer ? "PLAYER" : "OTHER", (int)eid, pEventData->speed);
				kbengine_PrintMessage(str);
			}
			break;
		}

		pEntity->setMoveSpeed(pEventData->speed);

		if (isDebugEnabled())
		{
			sprintf(str, "KBE Event received ==> CLIENT_EVENT_MOVESPEED_CHANGED - %s - EntityID %d, MoveSpeed %f\n", bPlayer ? "PLAYER" : "OTHER", (int)eid, pEventData->speed);
			kbengine_PrintMessage(str);
		}
	}
	break;

	case CLIENT_EVENT_SERVER_CLOSED:
	{
		//m_bServerClosed = true;
		//setShutdownRequired(true);
		setLoginStatus(LOGIN_NOT_DONE);
		setAppMode(TheWorld_ClientApp::InitialMenu);
		setInitAppModeRequired(true);
		
		if (isDebugEnabled())
		{
			strcpy(str, "KBE Event received ==> CLIENT_EVENT_SERVER_CLOSED\n");
			kbengine_PrintMessage(str);
		}
		
		onServerClosed();
	}
	break;

	case CLIENT_EVENT_POSITION_FORCE:
	{
		const KBEngine::EventData_PositionForce* pEventData = static_cast<const KBEngine::EventData_PositionForce*>(lpEventData);
		KBEngine::ENTITY_ID eid = pEventData->entityID;

		bool bPlayer = false;
		KBEntity* pEntity = getEntity(eid, bPlayer);
		if (!pEntity)
		{
			if (isDebugEnabled())
			{
				sprintf(str, "KBE Event received ==> CLIENT_EVENT_POSITION_FORCE - %s - EntityID %d (NOT IN LIST), X/Y/Z: %f/%f/%f\n", bPlayer ? "PLAYER" : "OTHER", (int)eid, pEventData->x, pEventData->y, pEventData->z);
				kbengine_PrintMessage(str);
			}
			break;
		}

		pEntity->setKnowByServerPosition(pEventData->x, pEventData->y, pEventData->z);
		//if (bPlayer)
		//	pEntity->setNewPlayerPosition(pEventData->x, pEventData->y, pEventData->z);

		if (isDebugEnabled())
		{
			sprintf(str, "KBE Event received ==> CLIENT_EVENT_POSITION_FORCE - %s - EntityID %d, X/Y/Z: %f/%f/%f\n", bPlayer ? "PLAYER" : "OTHER", (int)eid, pEventData->x, pEventData->y, pEventData->z);
			kbengine_PrintMessage(str);
		}
	}
	break;

	case CLIENT_EVENT_DIRECTION_FORCE:
	{
		const KBEngine::EventData_DirectionForce* pEventData = static_cast<const KBEngine::EventData_DirectionForce*>(lpEventData);
		KBEngine::ENTITY_ID eid = pEventData->entityID;

		bool bPlayer = false;
		KBEntity* pEntity = getEntity(eid, bPlayer);
		if (!pEntity)
		{
			if (isDebugEnabled())
			{
				sprintf(str, "KBE Event received ==> CLIENT_EVENT_DIRECTION_FORCE - %s - EntityID %d (NOT IN LIST), Yaw/Pitch/Roll: %f/%f/%f\n", bPlayer ? "PLAYER" : "OTHER", (int)eid, pEventData->yaw, pEventData->pitch, pEventData->roll);
				kbengine_PrintMessage(str);
			}
			break;
		}

		pEntity->setKnowByServerDirection(pEventData->yaw, pEventData->pitch, pEventData->roll);
		//if (bPlayer)
		//	pEntity->setNewPlayerDirection(pEventData->yaw, pEventData->pitch, pEventData->roll);

		if (isDebugEnabled())
		{
			sprintf(str, "KBE Event received ==> CLIENT_EVENT_DIRECTION_FORCE - %s - EntityID %d, Yaw/Pitch/Roll: %f/%f/%f\n", bPlayer ? "PLAYER" : "OTHER", (int)eid, pEventData->yaw, pEventData->pitch, pEventData->roll);
			kbengine_PrintMessage(str);
		}
	}
	break;

	case CLIENT_EVENT_ADDSPACEGEOMAPPING:
	{
		const KBEngine::EventData_AddSpaceGEOMapping* pEventData = static_cast<const KBEngine::EventData_AddSpaceGEOMapping*>(lpEventData);
		KBEngine::SPACE_ID spaceID = pEventData->spaceID;
		getSpaceWorld()->addSpace(spaceID, pEventData->respath.c_str());
		if (isDebugEnabled())
		{
			sprintf(str, "KBE Event received ==> CLIENT_EVENT_ADDSPACEGEOMAPPING, SpaceID %d, ResPath %s\n", (int)spaceID, pEventData->respath.c_str());
			kbengine_PrintMessage(str, true);
		}
		onAddSpaceGeoMapping(spaceID, pEventData->respath.c_str());
	}
	break;

	case CLIENT_EVENT_VERSION_NOT_MATCH:
	{
		const KBEngine::EventData_VersionNotMatch* info = static_cast<const KBEngine::EventData_VersionNotMatch*>(lpEventData);
		if (isDebugEnabled())
		{
			sprintf(str, "KBE Event received ==> CLIENT_EVENT_VERSION_NOT_MATCH - VerInfo=%s not match(server:%s)\n", info->verInfo.c_str(), info->serVerInfo.c_str());
			kbengine_PrintMessage(str);
		}
	}
	break;

	case CLIENT_EVENT_ON_KICKED:
	{
		const KBEngine::EventData_onKicked* info = static_cast<const KBEngine::EventData_onKicked*>(lpEventData);
		if (isDebugEnabled())
		{
			sprintf(str, "KBE Event received ==> CLIENT_EVENT_ON_KICKED (code=%u)!\n", info->failedcode);
			kbengine_PrintMessage(str);
		}
		onKicked(info->failedcode);
	}
	break;

	case CLIENT_EVENT_LAST_ACCOUNT_INFO:
	{
		const KBEngine::EventData_LastAccountInfo* info = static_cast<const KBEngine::EventData_LastAccountInfo*>(lpEventData);
		setAccountName(info->name);

		if (isDebugEnabled())
		{
			sprintf(str, "KBE Event received ==> CLIENT_EVENT_LAST_ACCOUNT_INFO - Last account name: %s\n", info->name.c_str());
			kbengine_PrintMessage(str);
		}
	}
	break;

	case CLIENT_EVENT_SCRIPT_VERSION_NOT_MATCH:
	{
		const KBEngine::EventData_ScriptVersionNotMatch* info = static_cast<const KBEngine::EventData_ScriptVersionNotMatch*>(lpEventData);
		if (isDebugEnabled())
		{
			sprintf(str, "KBE Event received ==> CLIENT_EVENT_SCRIPT_VERSION_NOT_MATCH - ScriptVerInfo=%s not match(server:%s)\n", info->verInfo.c_str(), info->serVerInfo.c_str());
			kbengine_PrintMessage(str);
		}
	}
	break;

	case CLIENT_EVENT_UNKNOWN:
	default:
	{
		//kbe_fireEvent("reset", NULL);
		//kbe_fireEvent("relive", NULL);
		if (isDebugEnabled())
		{
			strcpy(str, "KBE Event received ==> CLIENT_EVENT_UNKNOWN\n");
			kbengine_PrintMessage(str);
		}
	}
	break;
	};
}
