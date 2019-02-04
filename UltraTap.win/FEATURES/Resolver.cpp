#include "../includes.h"
#include "../UTILS/interfaces.h"
#include "../SDK/IEngine.h"
#include "../SDK/CUserCmd.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/CClientEntityList.h"
#include "../UTILS/render.h"
#include "../SDK/CTrace.h"
#include "../SDK/CBaseWeapon.h"
#include "../SDK/CGlobalVars.h"
#include "../SDK/ConVar.h"
#include "../SDK/AnimLayer.h"
#include "../UTILS/qangle.h"
#include "../FEATURES/Aimbot.h"
#include "../FEATURES/Resolver.h"
#include "../SDK/CBaseAnimState.h"
#include "../SDK/GameEvents.h"
void CResolver::record(SDK::CBaseEntity* entity, float new_yaw)
{
	if (entity->GetVelocity().Length2D() > 36) return;

	auto c_baseweapon = reinterpret_cast<SDK::CBaseWeapon*>(INTERFACES::ClientEntityList->GetClientEntity(entity->GetActiveWeaponIndex()));
	if (!c_baseweapon) return;

	auto &info = player_info[entity->GetIndex()];
	if (entity->GetActiveWeaponIndex() && info.last_ammo < c_baseweapon->GetLoadedAmmo()) {
		info.last_ammo = c_baseweapon->GetLoadedAmmo();
		return;
	}

	info.unresolved_yaw.insert(info.unresolved_yaw.begin(), new_yaw);
	if (info.unresolved_yaw.size() > 20) info.unresolved_yaw.pop_back();
	if (info.unresolved_yaw.size() < 2) return;

	auto average_unresolved_yaw = 0;
	for (auto val : info.unresolved_yaw)
		average_unresolved_yaw += val;
	average_unresolved_yaw /= info.unresolved_yaw.size();

	int delta = average_unresolved_yaw - entity->GetLowerBodyYaw();
	auto big_math_delta = abs((((delta + 180) % 360 + 360) % 360 - 180));

	info.lby_deltas.insert(info.lby_deltas.begin(), big_math_delta);
	if (info.lby_deltas.size() > 10) {
		info.lby_deltas.pop_back();
	}
}
float random(int Min, int Max)
{
	return ((rand() % (Max - Min)) + Min);
}
bool CResolver::is_slow_walking(SDK::CBaseEntity* entity) {
	float velocity_2D[64], old_velocity_2D[64];

	if (entity->GetVelocity().Length2D() != velocity_2D[entity->GetIndex()] && entity->GetVelocity().Length2D() != NULL) {
		old_velocity_2D[entity->GetIndex()] = velocity_2D[entity->GetIndex()];
		velocity_2D[entity->GetIndex()] = entity->GetVelocity().Length2D();
	}

	if (velocity_2D[entity->GetIndex()] > 0.1) {
		int tick_counter[64];

		if (velocity_2D[entity->GetIndex()] == old_velocity_2D[entity->GetIndex()])
			++tick_counter[entity->GetIndex()];
		else
			tick_counter[entity->GetIndex()] = 0;

		while (tick_counter[entity->GetIndex()] > (1 / INTERFACES::Globals->interval_per_tick) * fabsf(0.1f))// should give use 100ms in ticks if their speed stays the same for that long they are definetely up to something..
			return true;

	}
	return false;
}
int total_missed[64];
int total_hit[64];
SDK::IGameEvent* event = nullptr;
extra s_extra;
void missed_due_to_desync(SDK::IGameEvent* event) {

	if (event == nullptr)
		return;
	int user = event->GetInt("userid");
	int attacker = event->GetInt("attacker");
	bool player_hurt[64], hit_entity[64];

	if (INTERFACES::Engine->GetPlayerForUserID(user) != INTERFACES::Engine->GetLocalPlayer()
		&& INTERFACES::Engine->GetPlayerForUserID(attacker) == INTERFACES::Engine->GetLocalPlayer()) {
		player_hurt[INTERFACES::Engine->GetPlayerForUserID(user)] = true;
	}

	if (INTERFACES::Engine->GetPlayerForUserID(user) != INTERFACES::Engine->GetLocalPlayer())
	{
		Vector bullet_impact_location = Vector(event->GetFloat("x"), event->GetFloat("y"), event->GetFloat("z"));
		if (GLOBAL::aim_point != bullet_impact_location) return;
		hit_entity[INTERFACES::Engine->GetPlayerForUserID(user)] = true;
	}
	if (!player_hurt[INTERFACES::Engine->GetPlayerForUserID(user)] && hit_entity[INTERFACES::Engine->GetPlayerForUserID(user)]) {
		s_extra.current_flag[INTERFACES::Engine->GetPlayerForUserID(user)] = correction_flags::DESYNC;
		++total_missed[INTERFACES::Engine->GetPlayerForUserID(user)];
	}
	if (player_hurt[INTERFACES::Engine->GetPlayerForUserID(user)] && hit_entity[INTERFACES::Engine->GetPlayerForUserID(user)]) {
		++total_hit[INTERFACES::Engine->GetPlayerForUserID(user)];
	}
}

void CResolver::resolve(SDK::CBaseEntity* entity)
{
	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());

	int i = entity->GetIndex();

	static float MoveReal[65], FakeWalkandslowwalkingfix[65];
	auto animation_state = entity->GetAnimState();
	auto animstate = uintptr_t(local_player->GetAnimState());
	float duckammount = *(float *)(animstate + 0xA4);
	float speedfraction = max(0, min(*reinterpret_cast<float*>(animstate + 0xF8), 1));
	float speedfactor = max(0, min(1, *reinterpret_cast<float*> (animstate + 0xFC)));
	float unk1 = ((*reinterpret_cast<float*> (animstate + 0x11C) * -0.30000001) - 0.19999999) * speedfraction;
	float unk2 = unk1 + 1.f;
	float unk3;
	if (duckammount > 0) {
		unk2 += ((duckammount * speedfactor) * (0.5f - unk2));
	}
	unk3 = *(float *)(animstate + 0x334) * unk2;
	/*float duckammount = animation_state->m_fDuckAmount;
	float speedfraction = max(0, min(animation_state->m_flFeetSpeedForwardsOrSideWays, 2));
	float speedfactor = max(0, min(1, animation_state->m_flFeetSpeedUnknownForwardOrSideways));
	float unk1 = (animation_state->m_flStopToFullRunningFraction * -0.30000001) - 0.19999999 * speedfraction;
	float unk2 = unk1 + 1.f;
	float unk3;

	if (duckammount > 0)
	{
	unk2 += ((duckammount * speedfactor) * (0.5f - unk2));
	}
	unk3 = *(float *)(animation_state + 0x334) * unk2;*/

	auto feet_yaw = animation_state->m_flCurrentFeetYaw;
	float body_yaw = 58.f;//animation_state->m_flCurrentTorsoYaw;
	auto move_yaw = 29.f;
	auto goal_feet_yaw = animation_state->m_flGoalFeetYaw;
	auto shit = body_yaw - feet_yaw;
	auto shitv2 = body_yaw + feet_yaw;

	auto poses = entity->GetPoseParamaters();
	float feet_yaw_rate = animation_state->m_flFeetYawRate;
	float fff = animation_state->m_flFeetSpeedForwardsOrSideWays;
	float forwardorsideways = animation_state->m_flFeetSpeedUnknownForwardOrSideways;
	float feet_cucle = animation_state->m_flFeetCycle;
	float headheighanimation = animation_state->m_flHeadHeightOrOffsetFromHittingGroundAnimation;
	float new_body_yaw = animation_state->m_flCurrentTorsoYaw;
	auto body_max_rotation = animation_state->pad10[516];
	auto normalized_eye_abs_yaw_diff = fmod((animation_state->m_flEyeYaw - feet_yaw), 360.0);
	auto body_min_rotation = animation_state->pad10[512];
	if (entity->GetVelocity().Length2D() < 32.f) // проверка на мувы(upd:нужно переписать)(UDP:уже переписал)
	{
		if (move_yaw)
		{
			animation_state->m_flEyeYaw = animation_state->m_flEyeYaw + move_yaw + feet_yaw * 29.f && feet_yaw + feet_yaw_rate / 58.f;
		}
		else
		{
			if (feet_yaw && move_yaw)
			{
				animation_state->m_flEyeYaw = animation_state->m_flEyeYaw + feet_yaw + feet_yaw_rate * -29.f && goal_feet_yaw + feet_yaw / 29.f;
			}
		}
	}
	else
	{
		if (entity->GetVelocity().Length2D() > 0 && entity->GetFlags() & FL_ONGROUND) // проверка на стенды
		{
			if (normalized_eye_abs_yaw_diff > 0 || normalized_eye_abs_yaw_diff == 0)
			{
				body_min_rotation / feet_yaw / 58.f;
			}
			else
			{
				body_max_rotation / feet_yaw / -58.f;
			}
			if (new_body_yaw == 58.f)
			{
				animation_state->m_flEyeYaw = animation_state->m_flEyeYaw - body_yaw * -58.f + goal_feet_yaw + feet_yaw_rate + feet_yaw / 58.f;
			}
			else if (new_body_yaw >= -46.f && new_body_yaw == body_yaw)
			{
				animation_state->m_flEyeYaw = animation_state->m_flEyeYaw - new_body_yaw / 46.f || 58.f && goal_feet_yaw - feet_yaw * 58.f;
			}
			else if (new_body_yaw <= 58.f)
			{
				animation_state->m_flEyeYaw = animation_state->m_flEyeYaw - body_yaw * 58.f + feet_yaw / -58.f && goal_feet_yaw * 58.f;
			}
			else if (new_body_yaw == 58.f && new_body_yaw <= 58.f)
			{
				animation_state->m_flEyeYaw = animation_state->m_flEyeYaw - goal_feet_yaw / 58.f + feet_yaw * -58.f && new_body_yaw * 58.f - body_yaw / -58.f;
			}
			else if (new_body_yaw >= -58.f && body_yaw == 58.f)
			{
				animation_state->m_flEyeYaw = animation_state->m_flEyeYaw - new_body_yaw * 58.f - feet_yaw * -58.f && goal_feet_yaw - 58.f && feet_yaw / -58.f;
			}
		}
		if (is_slow_walking(entity))// дальше проверка на слоуволк(такая же ебливая как и я) p.s L I S T O N
		{
			if (normalized_eye_abs_yaw_diff > 0 || normalized_eye_abs_yaw_diff == 0)
			{
				body_min_rotation / move_yaw / -29.f;
			}
			else
			{
				body_max_rotation / move_yaw / 29.f;
			}
			if (goal_feet_yaw <= -29.f && feet_yaw >= -29.f)
			{
				animation_state->m_flEyeYaw = animation_state->m_flEyeYaw - move_yaw / 29.f + feet_yaw - goal_feet_yaw * 29.f;
			}
			else if (feet_yaw >= 29.f && feet_yaw_rate <= 29.f)
			{
				animation_state->m_flEyeYaw = animation_state->m_flEyeYaw + move_yaw + 29.f - feet_yaw + feet_yaw_rate / 29.f;
			}
			else if (goal_feet_yaw >= -29.f)
			{
				animation_state->m_flEyeYaw = animation_state->m_flEyeYaw - move_yaw / 29.f + feet_yaw_rate - feet_cucle + 29.f && goal_feet_yaw * 29.f;
			}
			resolver->enemy_fakewalk = true;
		}
		else
			resolver->enemy_fakewalk = false;
	}
}
CResolver* resolver = new CResolver();