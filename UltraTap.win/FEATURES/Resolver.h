#pragma once

namespace SDK
{
	class CUserCmd;
	class CBaseEntity;
	class CBaseWeapon;
}
struct extra
{
	int current_flag[64];
};
enum correction_flags
{
	DESYNC,
	NO_DESYNC,
	SLOW_WALK
};
struct Info
{
	Info() {}

	SDK::CAnimationLayer backup_layer, prev_layer;
	Vector last_lby, inverse, inverse_right, inverse_left, lby, back, left, right, backtrack, wideright, wideleft, forwards;
	float stored_simtime, last_move_time, pre_anim_lby;
	int last_ammo;
	bool breaking_lby, reset_state, could_be_faking;
	std::vector<float> unresolved_yaw, lby_deltas;
	bool lby_changed;
	bool could_be_slowmo;
	bool is_moving;
	bool is_jumping;
	bool is_crouching;
	bool lby_updated;
	bool using_fake_angles;
	float last_moving_lby;
	float stored_lby;
	float next_lby_update_time;
	int stored_missed;
};

class CResolver
{
public:

	Info player_info[65];
	void record(SDK::CBaseEntity * entity, float new_yaw);
	bool is_slow_walking(SDK::CBaseEntity * entity);
	void resolve(SDK::CBaseEntity* entity);
	bool enemy_fakewalk;
	bool enemy_inair;
	bool enemy_fake;
};

extern CResolver* resolver;