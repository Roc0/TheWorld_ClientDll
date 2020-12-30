#pragma once
#include "Entity.h"

class PlayerEntity : public KBEntity
{
public:
	__declspec(dllexport) PlayerEntity(int eid, SpaceWorld *pSpaceWorld);
	virtual ~PlayerEntity();
};

