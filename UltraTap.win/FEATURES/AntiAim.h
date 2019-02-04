#pragma once
namespace SDK
{
	class CUserCmd;
	class CBaseEntity;
}

class CAntiAim
{
public:
	void do_antiaim(SDK::CUserCmd* cmd);
	float TimeUntilNextLBYUpdate()
	{
		return m_next_lby_update_time - UTILS::GetCurtime();
	}
	void freestand(SDK::CUserCmd * cmd, float yawangs);
	bool DoEdging(float & yaw, float & lby, SDK::CUserCmd* cmd);
	void freestand(SDK::CUserCmd * cmd, SDK::CBaseEntity * entity);
	void fix_movement(SDK::CUserCmd* cmd);
	Vector fix_movement(SDK::CUserCmd* cmd, SDK::CUserCmd orignal);
private:
	void backwards(SDK::CUserCmd* cmd);
	void ZAnti(SDK::CUserCmd* cmd);
	void lowerj(SDK::CUserCmd* cmd);
	float m_next_lby_update_time = 0.f, m_last_move_time = 0.f, m_last_attempted_lby = 0.f;
	bool m_will_lby_update = false;
	void lowerbody(SDK::CUserCmd* cmd);
	void lowerbody_pysen(SDK::CUserCmd* cmd);

};

extern CAntiAim* antiaim;