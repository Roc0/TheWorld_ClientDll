#pragma once
#include "Entity.h"
class __declspec(dllexport) PlayerEntity : public KBEntity
{
public:
	PlayerEntity(KBEngine::ENTITY_ID eid, SpaceWorld *pSpaceWorld);
	virtual ~PlayerEntity();
};

