#pragma once

namespace SDK
{
	class CUserCmd;
	class CBaseEntity;
}

class CSlowwalk
{
public:
	int choked;
	void SlowWalk(SDK::CUserCmd *cmd);
};

extern CSlowwalk* slowwalk;