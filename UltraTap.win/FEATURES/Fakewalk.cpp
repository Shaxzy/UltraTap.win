#include "../includes.h"
#include "../UTILS/interfaces.h"
#include "../SDK/IEngine.h"
#include "../SDK/CUserCmd.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/CTrace.h"
#include "../SDK/CBaseWeapon.h"
#include "../SDK/CGlobalVars.h"
#include "../SDK/NetChannel.h"
#include "../SDK/CBaseAnimState.h"
#include "../SDK/ConVar.h"
#include "../FEATURES/AutoWall.h"
#include "../FEATURES/Fakewalk.h"
#include "../FEATURES/Aimbot.h"

#include <time.h>
#include <iostream>
void CSlowwalk::SlowWalk(SDK::CUserCmd* cmd)
{
	if (!SETTINGS::settings.slowwalk || !GetAsyncKeyState(SETTINGS::settings.slowwalkkey))
		return;
	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
	if (!local_player)
		return;
	auto wphandle = reinterpret_cast<SDK::CBaseWeapon*>(INTERFACES::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex()));
	if (!wphandle)
		return;
	float amount = 0.0034f * SETTINGS::settings.slowwalkspeed;
	Vector velocity = local_player->GetVelocity();
	Vector direction;
	MATH::VectorAngles2(velocity, direction);
	float speed = velocity.Length2D();
	direction.y = cmd->viewangles.y - direction.y;
	Vector forward;
	MATH::AngleVectors3(direction, forward);
	Vector source = forward * -speed;
	if (speed >= (wphandle->get_full_info()->max_speed * amount))
	{
		GLOBAL::originalCMD.move.x = source.x;
		GLOBAL::originalCMD.move.y = source.y;
	}
}
CSlowwalk* slowwalk = new CSlowwalk();