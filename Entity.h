#pragma once
#include "SpaceWorld.h"

//#include "TheWorld_ClientDll.h"

class _Vector3
{
public:
	float x;
	float y;
	float z;
};

class KBEntity
{
protected:
	KBEntity(int eid, SpaceWorld* pSpaceWorld);
public:
	virtual ~KBEntity();

	/*void setNewPlayerPosition(float x, float y, float z)
	{
		//setForClientPosition(x, y, z);
		return;

		if (x)
			m_destPos.x = x;
		if (y)
			m_destPos.y = y;
		if (z)
			m_destPos.z = z;
	}

	void getNewPlayerPosition(float& x, float& y, float& z)
	{
		//return getForClientPosition(x, y, z);
		
		x = m_destPos.x;
		y = m_destPos.y;
		z = m_destPos.z;
	}

	void setNewPlayerDirection(float yaw, float pitch, float roll)
	{
		if (yaw)
			m_destDir.x = roll;
		if (pitch)
			m_destDir.y = pitch;
		if (roll)
			m_destDir.z = yaw;
	}

	void getNewPlayerDirection(float& yaw, float& pitch, float& roll)
	{
		yaw = m_destDir.z;
		pitch = m_destDir.y;
		roll = m_destDir.x;
	}*/

	void setKnowByServerPosition(float x, float y, float z)
	{
		setForClientPosition(x, y, z);

		m_ServerPos.x = x;
		m_ServerPos.y = y;
		m_ServerPos.z = z;
	}

	void getKnowByServerPosition(float& x, float& y, float& z)
	{
		x = m_ServerPos.x;
		y = m_ServerPos.y;
		z = m_ServerPos.z;
	}
		
	void setForClientPosition(float x, float y, float z)
	{
		//if (x)
			m_pos.x = x;
		//if (y)
			m_pos.y = y;
		//if (z)
			m_pos.z = z;
	}

	void getForClientPosition(float& x, float& y, float& z)
	{
		x = m_pos.x;
		y = m_pos.y;
		z = m_pos.z;
	}

	void setKnowByServerDirection(float yaw, float pitch, float roll)
	{
		setForClientDirection(yaw, pitch, roll);
		
		m_ServerDir.z = yaw;
		m_ServerDir.y = pitch;
		m_ServerDir.x = roll;
	}

	void getKnowByServerDirection(float& yaw, float& pitch, float& roll)
	{
		yaw = m_ServerDir.z;
		pitch = m_ServerDir.y;
		roll = m_ServerDir.x;
	}

	void setForClientDirection(float yaw, float pitch, float roll)
	{
		//if (yaw)
			m_dir.z = yaw;
		//if (pitch)
			m_dir.y = pitch;
		//if (roll)
			m_dir.x = roll;
	}

	void getForClientDirection(float& yaw, float& pitch, float& roll)
	{
		yaw = m_dir.z;
		pitch = m_dir.y;
		roll = m_dir.x;
	}

	void setMoveSpeed(float speed)
	{
		m_moveSpeed = speed;
	}

	float getMoveSpeed(void)
	{
		return m_moveSpeed;
	}

	void setSpaceID(int spaceID)
	{
		m_spaceId = spaceID;
	}

	void setIsOnGround(bool isOnGround)
	{
		m_bIsOnGround = isOnGround;
	}

	__declspec(dllexport) bool getIsOnGround(void);

	void setRes(const char* res)
	{
		m_res = res;
	}
	
	void setModelScale(float modelScale)
	{
		m_modelScale = modelScale;
	}

	void setName(const char* name)
	{
		m_name = name;
	}
	
	const char* getName(void)
	{
		return m_name.c_str();
	}

	void setClassName(const std::string& name)
	{
		m_className = name;
	}
	
	const char* getClassName(void)
	{
		return m_className.c_str();
	}

	void setIsInWorld(bool isInWorld)
	{
		m_bIsInWorld = isInWorld;
	}

	bool getIsInWorld(void)
	{
		return m_bIsInWorld;
	}

	void setModelID(uint32_t modelID)
	{
		m_modelId = modelID;
	}
	
	void setState(uint32_t state)
	{
		m_state = state;
	}

	uint32_t getState(void)
	{
		return m_state;
	}

	void setHPMax(uint32_t HPMax)
	{
		m_HPMax = HPMax;
	}

	void setHP(uint32_t HP)
	{
		m_HP = HP;
	}

	uint32_t getHP(void)
	{
		return m_HP;
	}

	void setMPMax(uint32_t MPMax)
	{
		m_MPMax = MPMax;
	}

	int id()const { return m_eid; }

	bool isPlayer() { return m_bIsPlayer;  }

	void attack(KBEntity* receiver, uint32_t skillID, uint32_t damageType, uint32_t damage);
	void recvDamage(KBEntity* attacker, uint32_t skillID, uint32_t damageType, uint32_t damage);

	__declspec(dllexport) void dumpStatus(int idx, bool minidump);

protected:
	int m_eid;				// entityID
	int m_spaceId;
	float m_moveSpeed;
	_Vector3 m_destPos, m_pos, m_ServerPos, m_destDir, m_dir, m_ServerDir;
	bool m_bIsOnGround;
	std::string m_res;
	SpaceWorld *m_pSpaceWorld;
	float m_modelScale;
	bool m_bIsInWorld;
	bool m_bIsPlayer;
	std::string m_name;
	std::string m_className;

	uint32_t m_modelId;
	int32_t m_state;
#define ENTITY_STATE_UNKNOW		-1
#define ENTITY_STATE_FREE		0
#define ENTITY_STATE_DEAD		1
#define ENTITY_STATE_REST		2
#define ENTITY_STATE_FIGHT		3
#define ENTITY_STATE_MAX		4
	int32_t m_HPMax, m_HP;
	int32_t m_MPMax, m_MP;
};
