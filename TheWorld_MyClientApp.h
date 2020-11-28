#ifndef MY_CLIENTAPP_H
#define MY_CLIENTAPP_H

#include "common/common.h"
#include "client_lib/kbemain.h"
#include "server/serverconfig.h"
#include "server/telnet_server.h"
#include "common/memorystream.h"
#include "common/kbekey.h"
#include "thread/threadtask.h"
#include "thread/concurrency.h"
#include "helper/debug_helper.h"
#include "network/address.h"
#include "client_lib/event.h"
#include "client_lib/config.h"
#include "pyscript/pythread_lock.h"

namespace KBEngine {

	class TheWorld_KBEClientApp : public ClientApp
	{
	public:
		TheWorld_KBEClientApp(Network::EventDispatcher& dispatcher,
			Network::NetworkInterface& ninterface,
			COMPONENT_TYPE componentType,
			COMPONENT_ID componentID);

		~TheWorld_KBEClientApp();

		virtual bool initialize();
		virtual void finalise();

		virtual bool initializeBegin();
		virtual bool initializeEnd();

		virtual void reset(void);
		virtual void shutDown(void);
		virtual bool hasBreakProcessing(void);

		virtual PyObject* tryGetEntity(COMPONENT_ID componentID, ENTITY_ID entityID);
	};

}

#endif // MY_CLIENTAPP_H