#include "stdafx.h"
#include "PlayerEntity.h"


PlayerEntity::PlayerEntity(int eid, SpaceWorld *pSpaceWorld) : KBEntity(eid, pSpaceWorld)
{
	m_bIsInWorld = false;
	m_bIsPlayer = true;
}


PlayerEntity::~PlayerEntity()
{
}
