#pragma once
#include "Entity.h"

class OtherEntity : public KBEntity
{
public:
	__declspec(dllexport) OtherEntity(int eid, SpaceWorld *pSpaceWorld);
	virtual ~OtherEntity();
};

