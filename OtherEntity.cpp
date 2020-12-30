#include "stdafx.h"
#include "OtherEntity.h"

OtherEntity::OtherEntity(int eid, SpaceWorld *pSpaceWorld) : KBEntity(eid, pSpaceWorld)
{
	m_bIsInWorld = false;
	m_bIsPlayer = false;
}


OtherEntity::~OtherEntity()
{
}
