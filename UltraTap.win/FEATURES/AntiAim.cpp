#include "../includes.h"
#include "../UTILS/interfaces.h"
#include "../SDK/IEngine.h"
#include "../SDK/CUserCmd.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/CBaseAnimState.h"
#include "../SDK/CGlobalVars.h"
#include "../SDK/CTrace.h"
#include "../HOOKS/Hooks.h"
#include "../SDK/CBaseWeapon.h"
#include "../FEATURES/Aimbot.h"
#include "../FEATURES/AntiAim.h"
#include "../FEATURES/AutoWall.h"
#define clamp(val, min, max) (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))

float randnum(float Min, float Max)
{
	return ((float(rand()) / float(RAND_MAX)) * (Max - Min)) + Min;
}
bool next_lby_update(const float yaw_to_break, SDK::CUserCmd* cmd)
{
	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());

	if (!local_player)
		return false;

	static float next_lby_update_time = 0;
	float curtime = INTERFACES::Globals->curtime;
	local_update = next_lby_update_time;

	auto animstate = local_player->GetAnimState();

	if (!animstate)
		return false;

	if (!(local_player->GetFlags() & FL_ONGROUND))
		return false;

	if (animstate->speed_2d > 0.1f && !fake_walk)
		next_lby_update_time = curtime + 0.12f;

	if (next_lby_update_time < curtime)
	{
		next_lby_update_time = curtime + 0.6f;
		return true;
	}

	return false;
}

float fov_player(Vector ViewOffSet, Vector View, SDK::CBaseEntity* entity, int hitbox)
{
	const float MaxDegrees = 180.0f;
	Vector Angles = View, Origin = ViewOffSet;
	Vector Delta(0, 0, 0), Forward(0, 0, 0);
	Vector AimPos = aimbot->get_hitbox_pos(entity, hitbox);

	MATH::AngleVectors(Angles, &Forward);
	MATH::VectorSubtract(AimPos, Origin, Delta);
	MATH::NormalizeNum(Delta, Delta);

	float DotProduct = Forward.Dot(Delta);
	return (acos(DotProduct) * (MaxDegrees / M_PI));
}

int closest_to_crosshair()
{
	int index = -1;
	float lowest_fov = INT_MAX;

	SDK::CBaseEntity* local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());

	if (!local_player)
		return -1;

	Vector local_position = local_player->GetVecOrigin() + local_player->GetViewOffset();

	Vector angles;
	INTERFACES::Engine->GetViewAngles(angles);

	for (int i = 1; i <= INTERFACES::Globals->maxclients; i++)
	{
		SDK::CBaseEntity *entity = INTERFACES::ClientEntityList->GetClientEntity(i);

		if (!entity || entity->GetHealth() <= 0 || entity->GetTeam() == local_player->GetTeam() || entity->GetIsDormant() || entity == local_player)
			continue;

		float fov = fov_player(local_position, angles, entity, 0);

		if (fov < lowest_fov)
		{
			lowest_fov = fov;
			index = i;
		}
	}

	return index;
}
bool next_lby_update2(const float yaw_to_break, SDK::CUserCmd* cmd)
{
	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());

	if (!local_player)
		return false;

	static int next_lby_update_time = 0;
	float curtime = INTERFACES::Globals->curtime;
	local_update = next_lby_update_time;

	auto animstate = local_player->GetAnimState();

	if (!animstate)
		return false;

	if (!(local_player->GetFlags() & FL_ONGROUND))
		return false;

	if (animstate->speed_2d > 0.1f && !fake_walk)
		next_lby_update_time = curtime + 0.20f;

	if (next_lby_update_time < curtime)
	{
		next_lby_update_time = curtime + 1.0f;
		return true;
	}

	return false;
}
void CAntiAim::freestand(SDK::CUserCmd* cmd, float yawangs)
{
	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());

	if (!local_player)
		return;

	static float last_real;
	bool no_active = true;
	float bestrotation = 0.f;
	float highestthickness = 0.f;
	Vector besthead;

	auto leyepos = local_player->GetVecOrigin() + local_player->GetViewOffset();
	auto headpos = aimbot->get_hitbox_pos(local_player, 0);
	auto origin = local_player->GetAbsOrigin();

	auto checkWallThickness = [&](SDK::CBaseEntity* pPlayer, Vector newhead) -> float
	{
		Vector endpos1, endpos2;
		Vector eyepos = pPlayer->GetVecOrigin() + pPlayer->GetViewOffset();

		SDK::Ray_t ray;
		ray.Init(newhead, eyepos);

		SDK::CTraceFilterSkipTwoEntities filter(pPlayer, local_player);

		SDK::trace_t trace1, trace2;
		INTERFACES::Trace->TraceRay(ray, MASK_SHOT_BRUSHONLY, &filter, &trace1);

		if (trace1.DidHit())
			endpos1 = trace1.end;
		else
			return 0.f;

		ray.Init(eyepos, newhead);
		INTERFACES::Trace->TraceRay(ray, MASK_SHOT_BRUSHONLY, &filter, &trace2);

		if (trace2.DidHit())
			endpos2 = trace2.end;

		float add = newhead.DistTo(eyepos) - leyepos.DistTo(eyepos) + 3.f;
		return endpos1.DistTo(endpos2) + add / 3;
	};

	int index = closest_to_crosshair();
	auto entity = INTERFACES::ClientEntityList->GetClientEntity(index);

	float step = (2 * M_PI) / 18.f;
	float radius = fabs(Vector(headpos - origin).Length2D());

	if (index == -1)
	{
		no_active = true;
	}
	else
	{
		for (float rotation = 0; rotation < (M_PI * 2.0); rotation += step)
		{
			Vector newhead(radius * cos(rotation) + leyepos.x, radius * sin(rotation) + leyepos.y, leyepos.z);

			float totalthickness = 0.f;

			no_active = false;

			totalthickness += checkWallThickness(entity, newhead);

			if (totalthickness > highestthickness)
			{
				highestthickness = totalthickness;
				bestrotation = rotation;
				besthead = newhead;
			}
		}
	}
	if (!GLOBAL::should_send_packet)
	{
		if (next_lby_update(cmd->viewangles.y, cmd))
		{
		}
		else
		{
			if (no_active)
			{
				cmd->viewangles.y += 0 + ((rand() % 35) - (35 * 1.0f));
			}
			else
				cmd->viewangles.y = RAD2DEG(bestrotation) + ((rand() % 50) - (30 * 2.0f));

			last_real = cmd->viewangles.y;
		}
	}
}


void autoDirection(SDK::CUserCmd* cmd)
{
	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());

	if (!local_player)
		return;

	static float last_real;
	bool no_active = true;
	float bestrotation = 0.f;
	float highestthickness = 0.f;
	Vector besthead;

	auto leyepos = local_player->GetVecOrigin() + local_player->GetViewOffset();
	auto headpos = aimbot->get_hitbox_pos(local_player, 0);
	auto origin = local_player->GetAbsOrigin();

	auto checkWallThickness = [&](SDK::CBaseEntity* pPlayer, Vector newhead) -> float
	{
		Vector endpos1, endpos2;
		Vector eyepos = pPlayer->GetVecOrigin() + pPlayer->GetViewOffset();

		SDK::Ray_t ray;
		ray.Init(newhead, eyepos);

		SDK::CTraceFilterSkipTwoEntities filter(pPlayer, local_player);

		SDK::trace_t trace1, trace2;
		INTERFACES::Trace->TraceRay(ray, MASK_SHOT_BRUSHONLY, &filter, &trace1);

		if (trace1.DidHit())
			endpos1 = trace1.end;
		else
			return 0.f;

		ray.Init(eyepos, newhead);
		INTERFACES::Trace->TraceRay(ray, MASK_SHOT_BRUSHONLY, &filter, &trace2);

		if (trace2.DidHit())
			endpos2 = trace2.end;

		float add = newhead.DistTo(eyepos) - leyepos.DistTo(eyepos) + 3.f;
		return endpos1.DistTo(endpos2) + add / 3;
	};

	int index = closest_to_crosshair();
	auto entity = INTERFACES::ClientEntityList->GetClientEntity(index);

	float step = (2 * M_PI) / 18.f;
	float radius = fabs(Vector(headpos - origin).Length2D());

	if (index == -1)
	{
		no_active = true;
	}
	else
	{
		for (float rotation = 0; rotation < (M_PI * 2.0); rotation += step)
		{
			Vector newhead(radius * cos(rotation) + leyepos.x, radius * sin(rotation) + leyepos.y, leyepos.z);

			float totalthickness = 0.f;

			no_active = false;

			totalthickness += checkWallThickness(entity, newhead);

			if (totalthickness > highestthickness)
			{
				highestthickness = totalthickness;
				bestrotation = rotation;
				besthead = newhead;
			}
		}
	}
	if (no_active)
		cmd->viewangles.y += 180.f;
	else
		cmd->viewangles.y = RAD2DEG(bestrotation);

	last_real = cmd->viewangles.y;
}
float GetLBYRotatedYaw(float lby, float yaw)
{
	float delta = MATH::NormalizeYaw(yaw - lby);
	if (fabs(delta) < 25.f)
		return lby;

	if (delta > 0.f)
		return yaw + 25.f;

	return yaw;
}
bool IsPressingMovementKeys(SDK::CUserCmd* cmd)
{
	if (!cmd)
		return false;

	if (cmd->buttons & IN_FORWARD ||
		cmd->buttons & IN_BACK || cmd->buttons & IN_LEFT || cmd->buttons & IN_RIGHT ||
		cmd->buttons & IN_MOVELEFT || cmd->buttons & IN_MOVERIGHT)
		return true;

	return false;
}
inline float NormalizePitch(float pitch)
{
	while (pitch > 89.f)
		pitch -= 180.f;
	while (pitch < -89.f)
		pitch += 180.f;

	return pitch;
}
Vector TraceToEnd(Vector start, Vector end)
{
	SDK::trace_t trace;
	SDK::CTraceWorldOnly filter;
	SDK::Ray_t ray;

	ray.Init(start, end);
	INTERFACES::Trace->TraceRay(ray, MASK_ALL, &filter, &trace);

	return trace.end;
}
bool CAntiAim::DoEdging(float& yaw, float& lby, SDK::CUserCmd* cmd)
{

	//auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
	//if (!local_player)
	//	return false;

	///// check if enabled
	//const bool is_jumping = !(local_player->GetFlags() & FL_ONGROUND);
	//const bool is_moving = (local_player->GetVelocity().Length2D() > 0.1) && !is_jumping;

	//Vector viewangles;
	//INTERFACES::Engine->GetViewAngles(viewangles);
	//auto DoFreestanding = [local_player, viewangles, is_moving, this](float& yaw, float& lby, SDK::CUserCmd* cmd) -> bool
	//{
	//	static constexpr int damage_tolerance = 10;

	//	std::vector<SDK::CBaseEntity*> enemies;

	//	/// Find the lowest fov enemy
	//	SDK::CBaseEntity* closest_enemy = nullptr;
	//	float lowest_fov = 360.f;
	//	for (int i = 0; i < 64; i++)
	//	{
	//		auto entity = INTERFACES::ClientEntityList->GetClientEntity(i);
	//		if (!entity || entity->GetIsDormant() || entity->GetTeam() == local_player->GetTeam() || entity->GetHealth() <= 0)
	//			continue;

	//		const float current_fov = fabs(MATH::NormalizeYaw(UTILS::CalcAngle(local_player->GetVecOrigin(), entity->GetVecOrigin()).y - viewangles.y));
	//		if (current_fov < lowest_fov)
	//		{
	//			lowest_fov = current_fov;
	//			closest_enemy = entity;
	//		}

	//		enemies.push_back(entity);
	//	}

	//	/// No enemies
	//	if (closest_enemy == nullptr)
	//		return false;

	//	const float at_target_yaw = UTILS::CalcAngle(local_player->GetVecOrigin(), closest_enemy->GetVecOrigin()).y;
	//	const float right_yaw = at_target_yaw - 90.f;
	//	const float left_yaw = at_target_yaw + 90.f;

	//	/// Misc functions
	//	auto CalcDamage = [local_player, enemies](Vector point) -> int
	//	{
	//		int damage = 0;
	//		for (auto& enemy : enemies)
	//		{
	//			damage += UTILS::Min<int>(AutoWall->GetDamagez(Vector(0, 0, 64.f)),
	//				AutoWall->GetDamagez(Vector(0, 0, 49.f)));
	//		}

	//		return damage;
	//	};
	//	auto RotateAndExtendPosition = [](Vector position, float yaw, float distance) -> Vector
	//	{
	//		Vector direction;
	//		MATH::AngleVectors(Vector(0, yaw, 0), &direction);

	//		return position + (direction * distance);
	//	};
	//	auto RotateLBYAndYaw = [right_yaw, left_yaw, local_player](int right_damage, int left_damage, float lby_delta, float& yaw, float& lby, bool prefect_angle = false, bool symetrical = true) -> bool
	//	{
	//		const bool prefer_right = right_damage < left_damage;

	//		yaw = prefer_right ? right_yaw : left_yaw;
	//		if (symetrical)
	//			lby_delta = lby_delta * (prefer_right ? -1.f : 1.f);

	//		/// If not moving
	//		if (prefect_angle)
	//		{
	//			if (local_player->GetVelocity().Length2D() < 0.1f)
	//				yaw = (yaw);
	//			else
	//				yaw = (local_player->GetLowerBodyYaw(), yaw);
	//		}

	//		lby = yaw + lby_delta;

	//		if (fabs(MATH::NormalizeYaw(local_player->GetLowerBodyYaw() - lby)) < 35.f)
	//		{
	//			yaw = local_player->GetLowerBodyYaw() - lby_delta;
	//			lby = yaw + lby_delta;
	//		}

	//		return true;
	//	};
	//	auto DoBackFreestanding = [at_target_yaw, local_player](float& yaw, float& lby, SDK::CUserCmd* cmd) -> bool
	//	{
	//		//cmd->viewangles.y += 180.f;
	//		yaw = at_target_yaw;
	//		lby = yaw;

	//		return true;
	//	};

	//	const auto head_position = local_player->GetVecOrigin() + Vector(0, 0, 74.f);
	//	const auto back_head_position = RotateAndExtendPosition(head_position, at_target_yaw + 180.f, 17.f);
	//	auto right_head_position = RotateAndExtendPosition(head_position, right_yaw, 17.f);
	//	auto left_head_position = RotateAndExtendPosition(head_position, left_yaw, 17.f);

	//	int right_damage = CalcDamage(right_head_position), left_damage = CalcDamage(left_head_position), back_damage = CalcDamage(back_head_position);

	//	/// too much damage to both sides
	//	if (right_damage > damage_tolerance && left_damage > damage_tolerance)
	//	{
	//		/// do backwards if valid
	//		if (back_damage < damage_tolerance)
	//			return DoBackFreestanding(yaw, lby, cmd);

	//		return false;
	//	}

	//	/// keep searching for a better angle
	//	if (right_damage == left_damage)
	//	{
	//		/// if on top of them, prioritise backwards antiaim
	//		if (NormalizePitch(UTILS::CalcAngle(local_player->GetVecOrigin(), closest_enemy->GetVecOrigin()).x) > 15.f && back_damage < damage_tolerance)
	//			return DoBackFreestanding(yaw, lby, cmd);

	//		/// do some traces a bit further out
	//		right_head_position = RotateAndExtendPosition(head_position, right_yaw, 50.f);
	//		left_head_position = RotateAndExtendPosition(head_position, left_yaw, 50.f);

	//		right_damage = CalcDamage(right_head_position), left_damage = CalcDamage(left_head_position);
	//		if (right_damage == left_damage)
	//		{
	//			/// just return the side closest to a wall
	//			right_head_position = TraceToEnd(head_position, RotateAndExtendPosition(head_position, right_yaw, 17.f));
	//			left_head_position = TraceToEnd(head_position, RotateAndExtendPosition(head_position, left_yaw, 17.f));

	//			float distance_1, distance_2;
	//			SDK::trace_t trace;
	//			SDK::Ray_t ray;
	//			SDK::CTraceWorldOnly filter;
	//			const auto end_pos = closest_enemy->GetVecOrigin() + Vector(0, 0, 64.f);

	//			/// right position
	//			ray.Init(right_head_position, end_pos);
	//			INTERFACES::Trace->TraceRay(ray, MASK_ALL, &filter, &trace);
	//			distance_1 = (right_head_position - trace.end).Length();

	//			/// left position
	//			ray.Init(left_head_position, end_pos);
	//			INTERFACES::Trace->TraceRay(ray, MASK_ALL, &filter, &trace);
	//			distance_2 = (left_head_position - trace.end).Length();

	//			if (fabs(distance_1 - distance_2) > 15.f)
	//			{
	//				RotateLBYAndYaw(distance_1, distance_2, 0, yaw, lby);

	//				if ((is_moving && !IsPressingMovementKeys(reinterpret_cast<SDK::CUserCmd*>(GLOBAL::last_cmd))) ||
	//					(UTILS::GetCurtime() - m_last_move_time < 0.22 && !is_moving))
	//					yaw = lby + 60.f;

	//				return true;
	//			}

	//			return DoBackFreestanding(yaw, lby, cmd);
	//		}
	//		else
	//		{
	//			RotateLBYAndYaw(right_damage, left_damage, 0, yaw, lby);

	//			if ((is_moving && !IsPressingMovementKeys(reinterpret_cast<SDK::CUserCmd*>(GLOBAL::last_cmd))) ||
	//				(UTILS::GetCurtime() - m_last_move_time < 0.22 && !is_moving))
	//				yaw = lby + 60.f;

	//			return true;
	//		}
	//	}
	//	else /// found an angle that does less damage than the other
	//	{
	//		/// if on top of them, prioritise backwards antiaim
	//		if (NormalizePitch(UTILS::CalcAngle(local_player->GetVecOrigin(), closest_enemy->GetVecOrigin()).x) > 15.f && back_damage < damage_tolerance)
	//			return DoBackFreestanding(yaw, lby, cmd);

	//		const bool prefer_right = (right_damage < left_damage);

	//		yaw = prefer_right ? right_yaw : left_yaw;
	//		lby = yaw;

	//		if (fabs(MATH::NormalizeYaw(local_player->GetLowerBodyYaw() - lby)) < 35.f)
	//		{
	//			yaw = local_player->GetLowerBodyYaw();
	//			lby = yaw;
	//		}

	//		if (CalcDamage(RotateAndExtendPosition(head_position, lby, 18.f)) > 0)
	//		{
	//			lby = yaw + (prefer_right ? -115.f : 115.f);
	//			if (CalcDamage(RotateAndExtendPosition(head_position, lby, 18.f)) > 0)
	//				lby = yaw;
	//		}
	//		else
	//		{
	//			if ((is_moving && !IsPressingMovementKeys(reinterpret_cast<SDK::CUserCmd*>(GLOBAL::last_cmd))) ||
	//				(UTILS::GetCurtime() - m_last_move_time < 0.22 && !is_moving))
	//				yaw = lby + 60.f;
	//		}

	//		return true;
	//	}

	//	cmd->viewangles.y += 180.f;
	//};

	//if (DoFreestanding(yaw, lby, cmd))
	//	return true;
	return 0;
	//cmd->viewangles.y += 180.f;
}
void CAntiAim::freestand(SDK::CUserCmd* cmd, SDK::CBaseEntity* entity)
{
	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());

	if (!local_player)
		return;

	static float last_real;
	bool no_active = true;
	float bestrotation = 0.f;
	float highestthickness = 0.f;
	Vector besthead;

	auto leyepos = local_player->GetVecOrigin() + local_player->GetViewOffset();
	auto headpos = aimbot->get_hitbox_pos(local_player, 0);
	auto origin = local_player->GetAbsOrigin();

	auto checkWallThickness = [&](SDK::CBaseEntity* pPlayer, Vector newhead) -> float
	{
		Vector endpos1, endpos2;
		Vector eyepos = pPlayer->GetVecOrigin() + pPlayer->GetViewOffset();

		SDK::Ray_t ray;
		ray.Init(newhead, eyepos);

		SDK::CTraceFilterSkipTwoEntities filter(pPlayer, local_player);

		SDK::trace_t trace1, trace2;
		INTERFACES::Trace->TraceRay(ray, MASK_SHOT_BRUSHONLY, &filter, &trace1);

		if (trace1.DidHit())
			endpos1 = trace1.end;
		else
			return 0.f;

		ray.Init(eyepos, newhead);
		INTERFACES::Trace->TraceRay(ray, MASK_SHOT_BRUSHONLY, &filter, &trace2);

		if (trace2.DidHit())
			endpos2 = trace2.end;

		float add = newhead.DistToSqr(eyepos) - leyepos.DistToSqr(eyepos) + 3.f;
		return endpos1.DistToSqr(endpos2) + add / 3;
	};

	int index = closest_to_crosshair();

	if (index != -1)
		entity = INTERFACES::ClientEntityList->GetClientEntity(index);

	float step = (2 * M_PI) / 12.f; // One PI = half a circle ( for stacker cause low iq :sunglasses: ), 28

	float radius = fabs(Vector(headpos - origin).Length2D());

	if (index == -1)
	{
		no_active = true;
	}
	else
	{
		for (float rotation = 0; rotation < (M_PI * 2.0); rotation += step)
		{
			Vector newhead(radius * cos(rotation) + leyepos.x, radius * sin(rotation) + leyepos.y, leyepos.z);

			float totalthickness = 0.f;

			no_active = false;

			totalthickness += checkWallThickness(entity, newhead);

			if (totalthickness > highestthickness)
			{
				highestthickness = totalthickness;
				bestrotation = rotation;
				besthead = newhead;
			}
		}
	}
	if (no_active)
		cmd->viewangles.y += 180.f;
	else
		cmd->viewangles.y = RAD2DEG(bestrotation);

	last_real = cmd->viewangles.y;
}


void CAntiAim::do_antiaim(SDK::CUserCmd* cmd)
{
	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
	if (!local_player) return;
	if (local_player->GetHealth() <= 0) return;
	float spoonfizonelove;
	auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(INTERFACES::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex()));
	if (!weapon) return; auto grenade = (SDK::CBaseCSGrenade*)weapon;
	if (local_player->GetMoveType() == SDK::MOVETYPE_LADDER) return;
	if (cmd->buttons & IN_USE) return;//autoDirection(cmd);
	if (cmd->buttons & IN_ATTACK && aimbot->can_shoot(cmd))return;
	if (!SETTINGS::settings.aa_bool) return;
	if (
		weapon->GetItemDefenitionIndex() == SDK::ItemDefinitionIndex::WEAPON_HEGRENADE
		|| weapon->GetItemDefenitionIndex() == SDK::ItemDefinitionIndex::WEAPON_INCGRENADE
		|| weapon->GetItemDefenitionIndex() == SDK::ItemDefinitionIndex::WEAPON_SMOKEGRENADE
		|| weapon->GetItemDefenitionIndex() == SDK::ItemDefinitionIndex::WEAPON_MOLOTOV)
		return;
	static float last_real2;
	static float last_real; int local_flags = local_player->GetFlags();
	if ((local_player->GetVelocity().Length2D() < 0.01) && !(cmd->buttons & IN_JUMP) && !(GetAsyncKeyState(VK_SHIFT)))
	{
		switch (SETTINGS::settings.aa_pitch_type)
		{
		case 0: break;
		case 1: cmd->viewangles.x = 89.f; break;
		case 2: cmd->viewangles.x = 0; break;
		case 3: cmd->viewangles.x = -89.f; break;
		case 4: cmd->viewangles.x = 1080.f; break;
		}
		if (!GLOBAL::should_send_packet)
		{
			switch (SETTINGS::settings.aa_real_type)
			{
			case 0: break;
			case 1:
			{
				DoEdging(cmd->viewangles.y, spoonfizonelove, cmd); break;
			}
			break;
			case 2: autoDirection(cmd); break;
			case 3:
				bool Moving = local_player->GetVelocity().Length2D() > 0.1f || (cmd->move.y != 0.f || cmd->move.x != 0.f);
				bool InAir = !(local_player->GetFlags() & FL_ONGROUND);
				bool Standing = !Moving && !InAir;

				int FakeLagTicks = SETTINGS::settings.move_lag;

				if (FakeLagTicks == 0)
				{
					static bool sw = false;
					GLOBAL::should_send_packet = sw;
					sw = !sw;
				}

				static Vector LastRealAngle = Vector(0, 0, 0);

				if (!GLOBAL::should_send_packet && !(cmd->buttons & IN_ATTACK))
				{
					static bool bFlip = false;
					cmd->viewangles.y += bFlip ? 58.f : -58.f;
				}

				if (GLOBAL::should_send_packet)
				{
					LastRealAngle = cmd->viewangles;
				}break;
			}
		}
	}
	else if (GetAsyncKeyState(VK_SHIFT))
	{
		switch (SETTINGS::settings.aa_pitch2_type)
		{
		case 0: break;
		case 1: cmd->viewangles.x = 89.f; break;
		case 2: cmd->viewangles.x = -179.f; break;
		case 3: cmd->viewangles.x = 179.f; break;
		case 4: cmd->viewangles.x = 1080.f; break;
		}
		switch (SETTINGS::settings.aa_real_type)
		{
		case 0: break;
		case 1:
		{
			DoEdging(cmd->viewangles.y, spoonfizonelove, cmd); break;
		}
		break;
		case 2: autoDirection(cmd); break;
		}
	}
	else if ((local_player->GetVelocity().Length2D() > 0.1) && (!(cmd->buttons & IN_JUMP) && (local_flags & FL_ONGROUND)) && !(GetAsyncKeyState(VK_SHIFT)))
	{
		switch (SETTINGS::settings.aa_pitch1_type)
		{
		case 0: break;
		case 1: cmd->viewangles.x = 89.f; break;
		case 2: cmd->viewangles.x = -179.f; break;
		case 3: cmd->viewangles.x = 179.f; break;
		case 4: cmd->viewangles.x = 1080.f; break;
		}
		switch (SETTINGS::settings.aa_real1_type)
		{
		case 0: break;
		case 1:
		{
			DoEdging(cmd->viewangles.y, spoonfizonelove, cmd); break;
		}
		break;
		case 2: autoDirection(cmd); break;
		}
	}
	else
	{
		switch (SETTINGS::settings.aa_pitch2_type)
		{
		case 0: break;
		case 1: cmd->viewangles.x = 89.f; break;
		case 2: cmd->viewangles.x = -179.f; break;
		case 3: cmd->viewangles.x = 179.f; break;
		case 4: cmd->viewangles.x = 1080.f; break;
		}
		switch (SETTINGS::settings.aa_real2_type)
		{
		case 0: break;
		case 1:
		{
			DoEdging(cmd->viewangles.y, spoonfizonelove, cmd); break;
		}
		break;
		case 2: autoDirection(cmd); break;
		}
	}
}

void CAntiAim::backwards(SDK::CUserCmd* cmd)
{
	if (!GLOBAL::should_send_packet)
		cmd->viewangles.y += 179.000000; //cya fake angle checks
	else
		cmd->viewangles.y += 180.000000;
}

void CAntiAim::ZAnti(SDK::CUserCmd* cmd)
{
	constexpr auto maxRange = 90.0f;

	// where you want your head to go
	constexpr auto angleAdditive = 180.0f;

	// to make it equal on both sides / by 2
	cmd->viewangles.y += angleAdditive - maxRange / 2.f + std::fmodf(INTERFACES::Globals->curtime * 60, maxRange);
}

void CAntiAim::lowerj(SDK::CUserCmd* cmd)
{
	cmd->viewangles.y = +180.f - (float)(rand() % 25);
}



void CAntiAim::lowerbody(SDK::CUserCmd* cmd)
{
	static float last_real;
	auto nci = INTERFACES::Engine->GetNetChannelInfo();

	if (SETTINGS::settings.flip_bool)
	{
		if (!GLOBAL::should_send_packet)
			cmd->viewangles.y += randnum(-180, 180);
		else
		{
			if (next_lby_update(cmd->viewangles.y + SETTINGS::settings.delta_val, cmd)) //else if
			{
				cmd->viewangles.y = last_real + SETTINGS::settings.delta_val;
			}
			else
			{
				cmd->viewangles.y -= 90;
				last_real = cmd->viewangles.y;
			}
		}
	}
	else
	{
		if (GLOBAL::should_send_packet)
			cmd->viewangles.y += randnum(-180, 180);
		else
		{
			if (next_lby_update(cmd->viewangles.y - SETTINGS::settings.delta_val, cmd))
			{
				cmd->viewangles.y = last_real - SETTINGS::settings.delta_val;
			}
			else
			{
				cmd->viewangles.y += 90;
				last_real = cmd->viewangles.y;
			}
		}
	}
}


void CAntiAim::lowerbody_pysen(SDK::CUserCmd* cmd)
{
	static float last_real;
	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
	if (next_lby_update(cmd->viewangles.y - SETTINGS::settings.delta_val, cmd)) {
		cmd->viewangles.y = local_player->GetLowerBodyYaw() + 1809;
		cmd->viewangles.z = 1050.f;
	}


	if (SETTINGS::settings.flip_bool)
	{
		if (!GLOBAL::should_send_packet) {
			if (next_lby_update(cmd->viewangles.y - SETTINGS::settings.delta_val, cmd))
				cmd->viewangles.y = last_real - SETTINGS::settings.delta_val;
			cmd->viewangles.y = local_player->GetLowerBodyYaw() + 1814;
			cmd->viewangles.z = 1050.f;
		}
		else
			cmd->viewangles.y -= 36078;
		last_real = cmd->viewangles.y;

	}
	else
	{
		if (!GLOBAL::should_send_packet)
		{
			if (next_lby_update(cmd->viewangles.y - SETTINGS::settings.delta_val, cmd))
				cmd->viewangles.y = last_real - SETTINGS::settings.delta_val;
			cmd->viewangles.y = local_player->GetLowerBodyYaw() + 1814;
			cmd->viewangles.z = 1050.f;
		}
		else {
			cmd->viewangles.y += 36078;
			last_real = cmd->viewangles.y;

		}

	}
}

void CAntiAim::fix_movement(SDK::CUserCmd* cmd)
{
	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());

	if (!local_player)
		return;

	if (SETTINGS::settings.aa_pitch > 1 || SETTINGS::settings.aa_pitch > 1 || SETTINGS::settings.aa_pitch > 1)
	{
		cmd->move.x = clamp(cmd->move.x, -450.f, 450.f);
		cmd->move.y = clamp(cmd->move.y, -450.f, 450.f);
		cmd->move.z = clamp(cmd->move.z, -320.f, 320.f);

		cmd->viewangles.x = clamp(cmd->viewangles.x, -89.f, 89.f);
	}

	Vector real_viewangles;
	INTERFACES::Engine->GetViewAngles(real_viewangles);

	Vector vecMove(cmd->move.x, cmd->move.y, cmd->move.z);
	float speed = sqrt(vecMove.x * vecMove.x + vecMove.y * vecMove.y);

	Vector angMove;
	MATH::VectorAngles(vecMove, angMove);

	float yaw = DEG2RAD(cmd->viewangles.y - real_viewangles.y + angMove.y);

	cmd->move.x = cos(yaw) * speed;
	cmd->move.y = sin(yaw) * speed;

	cmd->viewangles = MATH::NormalizeAngle(cmd->viewangles);

	if (cmd->viewangles.x < -89.f || cmd->viewangles.x > 89.f) cmd->move.x *= -1;
}

Vector CAntiAim::fix_movement(SDK::CUserCmd* cmd, SDK::CUserCmd originalCMD)
{
	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
	if (!local_player) return Vector(0, 0, 0);

	Vector wish_forward, wish_right, wish_up, cmd_forward, cmd_right, cmd_up;

	auto viewangles = cmd->viewangles;
	auto movedata = originalCMD.move;
	viewangles.Normalize();

	if (!(local_player->GetFlags() & FL_ONGROUND) && viewangles.z != 0.f)
		movedata.y = 0.f;

	MATH::AngleVectors2(originalCMD.viewangles, &wish_forward, &wish_right, &wish_up);
	MATH::AngleVectors2(viewangles, &cmd_forward, &cmd_right, &cmd_up);

	auto v8 = sqrt(wish_forward.x * wish_forward.x + wish_forward.y * wish_forward.y), v10 = sqrt(wish_right.x * wish_right.x + wish_right.y * wish_right.y), v12 = sqrt(wish_up.z * wish_up.z);

	Vector wish_forward_norm(1.0f / v8 * wish_forward.x, 1.0f / v8 * wish_forward.y, 0.f),
		wish_right_norm(1.0f / v10 * wish_right.x, 1.0f / v10 * wish_right.y, 0.f),
		wish_up_norm(0.f, 0.f, 1.0f / v12 * wish_up.z);

	auto v14 = sqrt(cmd_forward.x * cmd_forward.x + cmd_forward.y * cmd_forward.y), v16 = sqrt(cmd_right.x * cmd_right.x + cmd_right.y * cmd_right.y), v18 = sqrt(cmd_up.z * cmd_up.z);

	Vector cmd_forward_norm(1.0f / v14 * cmd_forward.x, 1.0f / v14 * cmd_forward.y, 1.0f / v14 * 0.0f),
		cmd_right_norm(1.0f / v16 * cmd_right.x, 1.0f / v16 * cmd_right.y, 1.0f / v16 * 0.0f),
		cmd_up_norm(0.f, 0.f, 1.0f / v18 * cmd_up.z);

	auto v22 = wish_forward_norm.x * movedata.x, v26 = wish_forward_norm.y * movedata.x, v28 = wish_forward_norm.z * movedata.x, v24 = wish_right_norm.x * movedata.y, v23 = wish_right_norm.y * movedata.y, v25 = wish_right_norm.z * movedata.y, v30 = wish_up_norm.x * movedata.z, v27 = wish_up_norm.z * movedata.z, v29 = wish_up_norm.y * movedata.z;

	Vector correct_movement;
	correct_movement.x = cmd_forward_norm.x * v24 + cmd_forward_norm.y * v23 + cmd_forward_norm.z * v25
		+ (cmd_forward_norm.x * v22 + cmd_forward_norm.y * v26 + cmd_forward_norm.z * v28)
		+ (cmd_forward_norm.y * v30 + cmd_forward_norm.x * v29 + cmd_forward_norm.z * v27);
	correct_movement.y = cmd_right_norm.x * v24 + cmd_right_norm.y * v23 + cmd_right_norm.z * v25
		+ (cmd_right_norm.x * v22 + cmd_right_norm.y * v26 + cmd_right_norm.z * v28)
		+ (cmd_right_norm.x * v29 + cmd_right_norm.y * v30 + cmd_right_norm.z * v27);
	correct_movement.z = cmd_up_norm.x * v23 + cmd_up_norm.y * v24 + cmd_up_norm.z * v25
		+ (cmd_up_norm.x * v26 + cmd_up_norm.y * v22 + cmd_up_norm.z * v28)
		+ (cmd_up_norm.x * v30 + cmd_up_norm.y * v29 + cmd_up_norm.z * v27);

	correct_movement.x = clamp(correct_movement.x, -450.f, 450.f);
	correct_movement.y = clamp(correct_movement.y, -450.f, 450.f);
	correct_movement.z = clamp(correct_movement.z, -320.f, 320.f);

	return correct_movement;
}

CAntiAim* antiaim = new CAntiAim();