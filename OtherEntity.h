#pragma once
#include "Entity.h"
class __declspec(dllexport) OtherEntity : public KBEntity
{
public:
	OtherEntity(KBEngine::ENTITY_ID eid, SpaceWorld *pSpaceWorld);
	virtual ~OtherEntity();
};

