#include "includes.h"

#include "UTILS\interfaces.h"
#include "HOOKS\hooks.h"
#include "UTILS\offsets.h"
#include "FEATURES\EventListener.h"
#include "SDK\RecvData.h"
#include "SDK\CClientEntityList.h"

#include "SDK\IEngine.h"

#include "MENU\TMenu.h"

#include <chrono>
#include <thread>

bool using_fake_angles[65];
bool full_choke;
bool is_shooting;

bool in_tp;
bool fake_walk;

int resolve_type[65];

int target;
int shots_fired[65];
int shots_hit[65];
int shots_missed[65];
bool didMiss = true;
bool didShot = false;
int backtrack_missed[65];

float tick_to_back[65];
float lby_to_back[65];
bool backtrack_tick[65];

float lby_delta;
float update_time[65];
float walking_time[65];

float local_update;

int hitmarker_time;
int random_number;

bool menu_hide;

int oldest_tick[65];
float compensate[65][12];
Vector backtrack_hitbox[65][20][12];
float backtrack_simtime[65][12];
bool fake = false;

void Start()
{
	INTERFACES::InitInterfaces();
	OFFSETS::InitOffsets();

	UTILS::INPUT::input_handler.Init();
	FONTS::InitFonts();

	HOOKS::InitHooks();
	HOOKS::InitNetvarHooks();
	CGameEvents::InitializeEventListeners();

	GLOBAL::cheat_start_time = GetTickCount() * 0.001f;
}

BOOL WINAPI DllMain(HINSTANCE Instance, DWORD Reason, LPVOID Reserved)
{
	switch (Reason)
	{

	case DLL_PROCESS_ATTACH:
	{
		CreateDirectoryA(("C:\\ULTRATAP.WIN"), NULL);
		CreateDirectoryA(("C:\\ULTRATAP.WIN\\Logs"), NULL);
		CreateDirectoryA(("C:\\ULTRATAP.WIN\\Configs"), NULL);
		CreateDirectoryA(("C:\\ULTRATAP.WIN\\Resources"), NULL);
		CreateDirectoryA(("C:\\ULTRATAP.WIN\\Resources\\Fonts"), NULL);

		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Start, NULL, NULL, NULL);
	}
	break;	
	case DLL_PROCESS_DETACH:
	{
		LOG("DETACHING");
		RemoveFontResource("C:\\ULTRATAP.WIN\\Resources\\Fonts\\againts.ttf");
	}
	break;
	}

	return true;
}