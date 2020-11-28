#include "TheWorld_MyClientApp.h"
#include "network/channel.h"
#include "network/tcp_packet_sender.h"
#include "network/tcp_packet_receiver.h"

//#include "server/components.h"

namespace KBEngine {

	TheWorld_KBEClientApp::TheWorld_KBEClientApp(Network::EventDispatcher& dispatcher,
		Network::NetworkInterface& ninterface,
		COMPONENT_TYPE componentType,
		COMPONENT_ID componentID) :
		ClientApp(dispatcher, ninterface, componentType, componentID)
	{
		EntityDef::setGetEntityFunc(std::tr1::bind(&TheWorld_KBEClientApp::tryGetEntity, this,
			std::tr1::placeholders::_1, std::tr1::placeholders::_2));

		//KBEngine::Network::MessageHandlers::pMainMessageHandlers = &BotsInterface::messageHandlers;
		//Components::getSingleton().initialize(&ninterface, componentType, componentID);
	}

	TheWorld_KBEClientApp::~TheWorld_KBEClientApp()
	{
		//Components::getSingleton().finalise();
		//SAFE_RELEASE(pEventPoller_);
	}
	
	bool TheWorld_KBEClientApp::initialize()
	{
		//this->dispatcher().addTask(&Components::getSingleton());
		return ClientApp::initialize();
	}
	bool TheWorld_KBEClientApp::initializeBegin()
	{
		//Network::g_extReceiveWindowBytesOverflow = 0;
		//Network::g_intReceiveWindowBytesOverflow = 0;
		//Network::g_intReceiveWindowMessagesOverflow = 0;
		//Network::g_extReceiveWindowMessagesOverflow = 0;
		//Network::g_receiveWindowMessagesOverflowCritical = 0;

		//gameTimer_ = this->dispatcher().addTimer(1000000 / g_kbeSrvConfig.gameUpdateHertz(), this,
		//	reinterpret_cast<void *>(TIMEOUT_GAME_TICK));

		//ProfileVal::setWarningPeriod(stampsPerSecond() / g_kbeSrvConfig.gameUpdateHertz());
		
		//return true;

		return ClientApp::initializeBegin();
	}

	//-------------------------------------------------------------------------------------	
	bool TheWorld_KBEClientApp::initializeEnd()
	{
		/*
		pTelnetServer_ = new TelnetServer(&dispatcher(), &networkInterface());
		pTelnetServer_->pScript(&getScript());

		if (!pTelnetServer_->start(g_kbeSrvConfig.getBots().telnet_passwd,
			g_kbeSrvConfig.getBots().telnet_deflayer,
			g_kbeSrvConfig.getBots().telnet_port))
		{
			ERROR_MSG("Bots::initialize: initializeEnd error!\n");
			return false;
		}

		PyObject* pyResult = PyObject_CallMethod(getEntryScript().get(),
			const_cast<char*>("onInit"),
			const_cast<char*>("i"),
			0);

		if (pyResult != NULL)
		{
			Py_DECREF(pyResult);
		}
		else
		{
			SCRIPT_ERROR_CHECK();
			return false;
		}
		*/

		//return true;

		return ClientApp::initializeEnd();
	}

	//-------------------------------------------------------------------------------------
	void TheWorld_KBEClientApp::finalise()
	{
		/*
		PyObject* pyResult = PyObject_CallMethod(getEntryScript().get(),
			const_cast<char*>("onFinish"),
			const_cast<char*>(""));

		if (pyResult != NULL)
		{
			Py_DECREF(pyResult);
		}
		else
		{
			SCRIPT_ERROR_CHECK();
		}

		CLIENTS::iterator iter = clients_.begin();
		for (; iter != clients_.end(); ++iter)
		{
			iter->second->finalise();
			Py_DECREF(iter->second);
		}

		clients_.clear();

		reqCreateAndLoginTotalCount_ = 0;
		SAFE_RELEASE(pCreateAndLoginHandler_);

		if (pTelnetServer_)
		{
			pTelnetServer_->stop();
			SAFE_RELEASE(pTelnetServer_);
		}
		*/

		ClientApp::finalise();
	}
	
	PyObject* TheWorld_KBEClientApp::tryGetEntity(COMPONENT_ID componentID, ENTITY_ID eid)
	{
		return ClientApp::tryGetEntity(componentID, eid);
	}

	bool TheWorld_KBEClientApp::hasBreakProcessing()
	{
		return dispatcher_.hasBreakProcessing();
	}
	
	void TheWorld_KBEClientApp::shutDown()
	{
		ClientApp::shutDown();
	}
		
	void TheWorld_KBEClientApp::reset()
	{
		PyGILState_STATE gil;
		gil = PyGILState_Ensure();

		ClientApp::reset();

		PyGILState_Release(gil);

		/*state_ = C_STATE_INIT;

		if (pServerChannel_ && pServerChannel_->pEndPoint())
		{
			pServerChannel_->stopSend();
			networkInterface().dispatcher().deregisterReadFileDescriptor((int)*pServerChannel_->pEndPoint());
			networkInterface().deregisterChannel(pServerChannel_);
		}

		pServerChannel_->pFilter(NULL);
		pServerChannel_->pPacketSender(NULL);
		pServerChannel_->stopInactivityDetection();

		SAFE_RELEASE(pTCPPacketSender_);
		SAFE_RELEASE(pTCPPacketReceiver_);
		SAFE_RELEASE(pBlowfishFilter_);

		//ClientObjectBase::reset();

		PyGILState_STATE gil;
		gil = PyGILState_Ensure();
		pEntities_->finalise();
		pEntityIDAliasIDList_.clear();
		pyCallbackMgr_.finalise();
		PyGILState_Release(gil);


		entityID_ = 0;
		dbid_ = 0;

		ip_ = "";
		tcp_port_ = 0;
		udp_port_ = 0;

		lastSentActiveTickTime_ = timestamp();
		lastSentUpdateDataTime_ = timestamp();
		connectedBaseapp_ = false;

		name_ = "";
		password_ = "";
		clientDatas_ = "";
		serverDatas_ = "";

		bufferedCreateEntityMessage_.clear();
		canReset_ = false;
		locktime_ = 0;

		if (pServerChannel_ && !pServerChannel_->isDestroyed())
		{
			pServerChannel_->destroy();
			Network::Channel::reclaimPoolObject(pServerChannel_);
			pServerChannel_ = NULL;
		}

		pServerChannel_ = Network::Channel::createPoolObject(OBJECTPOOL_POINT);
		pServerChannel_->pNetworkInterface(&networkInterface_);*/

	}
}

