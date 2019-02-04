#include "../includes.h"

#include "NetvarHookManager.h"
#include "offsets.h"


namespace OFFSETS
{
	uintptr_t m_iHealth;
	uintptr_t m_fFlags;
	uintptr_t m_vecVelocity;
	uintptr_t m_flLowerBodyYawTarget;
	uintptr_t deadflag;
	uintptr_t m_vecOrigin;
	uintptr_t m_iTeamNum;
	uintptr_t m_nTickBase;
	uintptr_t m_iClip1;
	uintptr_t m_bDormant;
	uintptr_t animstate;
	uintptr_t m_Collision;
	uintptr_t m_angEyeAngles;
	uintptr_t m_flSimulationTime;
	uintptr_t m_vecViewOffset;
	uintptr_t m_dwBoneMatrix;
	uintptr_t m_aimPunchAngle;
	uintptr_t m_bGunGameImmunity;
	uintptr_t m_nForceBone;
	uintptr_t m_flPoseParameter;
	uintptr_t dwGlowObjectManager;
	uintptr_t m_hMyWearables;
	uintptr_t m_Item;
	uintptr_t m_nModelIndex;
	uintptr_t m_iViewModelIndex;
	uintptr_t m_hWeaponWorldModel;
	uintptr_t m_flNextPrimaryAttack;
	uintptr_t m_flNextSecondaryAttack;
	uintptr_t m_flNextAttack;
	uintptr_t m_hActiveWeapon;
	uintptr_t m_ArmorValue;
	uintptr_t m_bHasHelmet;
	uintptr_t m_iObserverMode;
	uintptr_t m_bIsScoped;
	uintptr_t m_iAccount;
	uintptr_t m_iPlayerC4;
	uintptr_t dwPlayerResource;
	uintptr_t m_iItemDefinitionIndex;
	uintptr_t m_lifeState;
	uintptr_t m_flPostponeFireReadyTime;
	uintptr_t m_fThrowTime;
	uintptr_t m_bPinPulled;
	uintptr_t m_MoveType;
	uintptr_t m_viewPunchAngle;

	void InitOffsets()
	{
		UTILS::netvar_hook_manager.Initialize();
		m_iHealth = UTILS::netvar_hook_manager.GetOffset(("DT_BasePlayer"), ("m_iHealth"));
		m_fFlags = UTILS::netvar_hook_manager.GetOffset(("DT_CSPlayer"), ("m_fFlags"));
		m_vecVelocity = UTILS::netvar_hook_manager.GetOffset(("DT_CSPlayer"), ("m_vecVelocity[0]"));
		m_flLowerBodyYawTarget = UTILS::netvar_hook_manager.GetOffset(("DT_CSPlayer"), ("m_flLowerBodyYawTarget"));
		m_vecOrigin = UTILS::netvar_hook_manager.GetOffset(("DT_BasePlayer"), ("m_vecOrigin"));
		m_iTeamNum = UTILS::netvar_hook_manager.GetOffset(("DT_CSPlayer"), ("m_iTeamNum"));
		m_nTickBase = UTILS::netvar_hook_manager.GetOffset(("DT_BasePlayer"), ("m_nTickBase"));
		m_iClip1 = UTILS::netvar_hook_manager.GetOffset(("DT_BaseCombatWeapon"), ("m_iClip1"));
		m_Collision = UTILS::netvar_hook_manager.GetOffset(("DT_BasePlayer"), ("m_Collision"));
		m_angEyeAngles = UTILS::netvar_hook_manager.GetOffset(("DT_CSPlayer"), ("m_angEyeAngles[0]"));
		m_flSimulationTime = UTILS::netvar_hook_manager.GetOffset(("DT_BaseEntity"), ("m_flSimulationTime"));
		m_vecViewOffset = UTILS::netvar_hook_manager.GetOffset(("DT_CSPlayer"), ("m_vecViewOffset[0]"));
		m_dwBoneMatrix = 0x26A8;
		m_aimPunchAngle = UTILS::netvar_hook_manager.GetOffset(("DT_BasePlayer"), ("m_aimPunchAngle"));
		m_bGunGameImmunity = UTILS::netvar_hook_manager.GetOffset(("DT_CSPlayer"), ("m_bGunGameImmunity"));
		m_nForceBone = 0x268C;
		m_flPoseParameter = UTILS::netvar_hook_manager.GetOffset(("DT_BaseAnimating"), ("m_flPoseParameter"));
		m_flNextPrimaryAttack = UTILS::netvar_hook_manager.GetOffset(("DT_BaseCombatWeapon"), ("m_flNextPrimaryAttack"));
		m_flNextSecondaryAttack = 0x320C;
		m_flNextAttack = UTILS::netvar_hook_manager.GetOffset(("DT_BaseCombatCharacter"), ("m_flNextAttack"));
		m_hActiveWeapon = UTILS::netvar_hook_manager.GetOffset(("DT_CSPlayer"), ("m_hActiveWeapon"));
		m_ArmorValue = UTILS::netvar_hook_manager.GetOffset(("DT_CSPlayer"), ("m_ArmorValue"));
		m_bHasHelmet = UTILS::netvar_hook_manager.GetOffset(("DT_CSPlayer"), ("m_bHasHelmet"));
		m_iObserverMode = UTILS::netvar_hook_manager.GetOffset(("DT_BasePlayer"), ("m_iObserverMode"));
		m_bIsScoped = UTILS::netvar_hook_manager.GetOffset(("DT_CSPlayer"), ("m_bIsScoped"));
		m_iAccount = UTILS::netvar_hook_manager.GetOffset(("DT_CSPlayer"), ("m_iAccount"));
		m_iPlayerC4 = UTILS::netvar_hook_manager.GetOffset(("DT_CSPlayerResource"), ("m_iPlayerC4"));
		dwPlayerResource = 0x30FD69C;
		m_iItemDefinitionIndex = UTILS::netvar_hook_manager.GetOffset(("DT_BaseAttributableItem"), ("m_iItemDefinitionIndex"));
		m_lifeState = UTILS::netvar_hook_manager.GetOffset(("DT_CSPlayer"), ("m_lifeState"));
		m_flPostponeFireReadyTime = UTILS::netvar_hook_manager.GetOffset(("DT_WeaponCSBase"), ("m_flPostponeFireReadyTime"));
		m_fThrowTime = UTILS::netvar_hook_manager.GetOffset(("DT_BaseCSGrenade"), ("m_fThrowTime"));
		m_bPinPulled = UTILS::netvar_hook_manager.GetOffset(("DT_BaseCSGrenade"), ("m_bPinPulled"));
		m_viewPunchAngle = UTILS::netvar_hook_manager.GetOffset(("DT_BasePlayer"), ("m_viewPunchAngle"));
		dwGlowObjectManager = UTILS::FindSignature("client_panorama.dll", "0F 11 05 ? ? ? ? 83 C8 01" + 0x3);
		m_hMyWearables = UTILS::netvar_hook_manager.GetOffset(("DT_BaseCombatCharacter"), ("m_hMyWearables"));
		m_Item = UTILS::netvar_hook_manager.GetOffset(("DT_BaseCombatWeapon"), ("m_Item"));
		m_nModelIndex = 0x258;
		m_iViewModelIndex = 0x3220;
		m_hWeaponWorldModel = 0x3234;
		m_bDormant = 0xED;
		animstate = 0x3900;
		m_MoveType = 0x25C;
	}
}