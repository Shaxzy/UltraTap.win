#pragma once

/*
global variables and other stuff that is probably too small to put in another 
file (and i'm too lazy to make files for all of them) would go in here.
Stuff like fonts and shit
*/

#include "../MENU/TGUI.h"

inline void setclantag(const char* tag)
{
	static auto ClanTagOffset = UTILS::FindPattern("engine.dll", (PBYTE)"\x53\x56\x57\x8B\xDA\x8B\xF9\xFF\x15", "xxxxxxxxx");
	if (ClanTagOffset)
	{
		auto tag_ = std::string(tag);
		if (strlen(tag) > 0) {
			auto newline = tag_.find("\\n");
			auto tab = tag_.find("\\t");
			if (newline != std::string::npos) {
				tag_.replace(newline, newline + 2, "\n");
			}
			if (tab != std::string::npos) {
				tag_.replace(tab, tab + 2, "\t");
			}
		}
		static auto dankesttSetClanTag = reinterpret_cast<void(__fastcall*)(const char*, const char*)>(ClanTagOffset);
		dankesttSetClanTag(tag_.data(), tag_.data());
	}
}

namespace GLOBAL
{
	extern HWND csgo_hwnd;

	extern bool should_send_packet;
	extern bool is_fakewalking;
	extern int choke_amount;
	extern Vector aim_point;
	extern Vector real_angles;
	extern Vector fake_angles;
	extern Vector strafe_angle;
	extern 	void* last_cmd;
	extern int randomnumber;
	extern float flHurtTime;
	extern bool DisableAA;
	extern bool Aimbotting;

	using msg_t = void(__cdecl*)(const char*, ...);
	extern msg_t		Msg;

	extern Vector FakePosition;
	extern int ground_tickz;
	extern bool CircleStraferActive;
	extern SDK::CUserCmd originalCMD;

	extern float cheat_start_time;
}
namespace FONTS
{
	extern unsigned int menu_tab_font;
	extern unsigned int menu_checkbox_font;
	extern unsigned int menu_slider_font;
	extern unsigned int menu_slider_value_font;
	extern unsigned int menu_groupbox_font;
	extern unsigned int menu_combobox_name_font;
	extern unsigned int menu_combobox_value_font;
	extern unsigned int menu_window_font;
	extern unsigned int menu_separator_font;
	extern unsigned int menu_window_blurr_font;
	extern unsigned int menu_keybind_value_font;
	extern unsigned int menu_keybind_name_font;
	extern unsigned int menu_text_input_name_font;
	extern unsigned int menu_text_input_value_font;
	extern unsigned int menu_button_font;
	extern unsigned int menu_colorpicker_font;
	extern unsigned int menu_tool_tip_font;

	extern unsigned int visuals_esp_font;
	extern unsigned int lby_indicator_font;
	extern unsigned int in_game_logging_font;
	extern unsigned int weapon_icon_font;
	extern unsigned int welcome_font;
	extern unsigned int visuals_name_font;
	extern unsigned int visuals_lby_font;
	extern unsigned int visuals_grenade_pred_font;

	bool ShouldReloadFonts();
	void InitFonts();
}
namespace SETTINGS
{
	class CSettings
	{
	public:
		// returns true/false whether the function succeeds, usually returns false if file doesn't exist
		bool Save(std::string file_name);
		bool Load(std::string file_name);

		void CreateConfig(); // creates a blank config

		std::vector<std::string> GetConfigs();
		bool autorevolver_b1g;
		bool bhop_bool;
		bool fakewalk_bool;
		int fakewalk_key;
		bool autozeus_bool;
		bool strafe_bool;
		bool esp_bool;
		int chams_type;
		int xhair_type;
		bool tp_bool;
		bool aim_bool;
		bool slowwalk;
		bool asus_bool;
		float slowwalkspeed;
		int slowwalkkey;
		int aim_type;
		bool aa_bool;
		int aa_pitch;
		int aa_type;
		int acc_type;
		bool hitscan_type[5];
		bool up_bool;
		bool misc_bool;
		int config_sel;
		float viewmodel_x = 6;
		float viewmodel_y = 5;
		float viewmodel_z = 4;
		bool beam_bool;
		bool stop_bool;
		bool night_bool;
		bool box_bool;
		bool name_bool;
		bool weap_bool;
		bool health_bool;
		bool info_bool;
		bool baim_fake;
		bool baim_inair;
		bool baim_fakewalk;
		bool baim_lethal;
		int forcebaimkey;
		bool back_bool;
		bool lag_bool;
		int box_type;
		bool reverse_bool;
		bool multi_bool;
		bool fakefix_bool;
		bool angle_bool;
		bool tp_angle_bool;
		bool glow_bool;
		bool noflash;
		bool dist_bool;
		bool fov_bool;
		bool smoke_bool;
		bool scope_bool;
		bool predict_bool;
		bool fake_bool;
		int media_type;
		bool novis_bool;
		bool localglow_bool;
		bool duck_bool;
		bool money_bool;
		int delay_shot;
		int lag_type;
		bool cham_bool;
		bool resolve_bool;
		bool ammo_bool;
		bool spread_bool;

		float move_lag;
		float jump_lag;

		bool debug_bool;

		CColor vmodel_col = CColor(205, 55, 0);
		CColor imodel_col = CColor(205, 0, 0);
		CColor btvis_col = CColor(205, 0, 0, 155);
		CColor box_col = CColor(205, 0, 0);
		CColor name_col;
		CColor weapon_col;
		CColor distance_col;
		CColor localchams_col = CColor(0, 205, 0);
		CColor grenadepredline_col;
		CColor grenadepredbox_col;

		CColor bulletlocal_col = CColor(0, 205, 0);
		CColor bulletenemy_col = CColor(205, 0, 0);
		CColor bulletteam_col = CColor(0, 0, 205);

		CColor menu_col = CColor(60, 60, 60);
		CColor checkbox_col = CColor(175, 200, 5);
		CColor slider_col = CColor(175, 200, 5);
		CColor tab_col = CColor(91, 91, 91);
		CColor glow_col = CColor(205, 0, 0);
		CColor glowlocal_col = CColor(0, 205, 0);
		CColor fov_col = CColor(205, 0, 0);

		bool backtrack_bool;
		float chance_val;
		float damage_val;
		float awall_damage_val;
		float delta_val;
		float point_val;
		float body_val;
		bool misc_enable_clantag;
		int misc_clantag_style = 0;
		float misc_clantag_speed = 0.f;
		char misc_clantag_text[TGUI::TGUI_MAX_STRING_LENGTH];

		bool localesp;
		int localchams;
		float fov_val = 90;
		float viewfov_val = 120;

		bool flip_bool;
		int aa_side;

		bool legit_bool;
		int legit_key;
		bool rcs_bool;
		float legitfov_val;
		int legitbone_int;
		float rcsamount_min;
		float rcsamount_max;
		float legitaim_val;
		bool legittrigger_bool;
		int legittrigger_key;

		int thirdperson_int = -1;
		int flip_int;

		bool glowenable;
		int glowstyle;
		bool glowlocal;
		int glowstylelocal;
		int hitmarker_val;

		int aa_mode;

		int aa_real_type;
		int aa_real1_type;
		int aa_real2_type;

		int aa_fake_type;
		int aa_fake1_type;
		int aa_fake2_type;

		int aa_pitch_type;
		int aa_pitch1_type;
		int aa_pitch2_type;

		float aa_realadditive_val;
		float aa_fakeadditive_val;

		float aa_realadditive1_val;
		float aa_fakeadditive1_val;
		float delta1_val;

		float aa_realadditive2_val;
		float aa_fakeadditive2_val;
		float delta2_val;

		float spinangle;
		float spinspeed;

		float spinangle1;
		float spinspeed1;

		float spinangle2;
		float spinspeed2;

		float spinanglefake;
		float spinspeedfake;

		float spinanglefake1;
		float spinspeedfake1;

		float spinanglefake2;
		float spinspeedfake2;

		bool lbyflickup;
		bool lbyflickup1;
		bool lbyflickup2;

		bool aa_fakeangchams_bool;

		int chamstype;
		float fov_time;
		bool antiaim_arrow;

		int glowteamselection;
		bool glowteam;

		int chamsteamselection;
		int chamsteam;

		int espteamselection;
		int espteamcolourselection;
		bool boxteam;
		bool nameteam;
		bool weaponteam;
		bool flagsteam;
		bool healthteam;
		bool moneyteam;
		bool ammoteam;
		bool arrowteam;
		CColor boxteam_col = CColor(0, 0, 205);
		CColor nameteam_col;
		CColor weaponteam_col;
		CColor arrowteam_col = CColor(0, 0, 205);

		CColor teamvis_color = CColor(0, 55, 205);
		CColor teaminvis_color = CColor(0, 0, 205);
		CColor teamglow_color = CColor(0, 0, 205);

		bool matpostprocessenable;
		bool removescoping;
		bool fixscopesens;
		bool forcecrosshair;

		int quickstopkey;
		bool stop_flip;
		bool chamsmetallic;
		int flashlightkey;
		int overridekey;
		int autostopmethod;
		int overridemethod;
		bool overridething;
		bool overrideenable;
		bool lbyenable;
		int circlestrafekey;
		float circlstraferetract;
		float fakewalkspeed;
		float daytimevalue = 100;

		float circlemin;
		float circlemax;
		float circlevel;
		float circlenormalizemultiplier;

		bool skinchangerenable;

		int knifeToUse;
		int bayonetID, karambitID, flipID, gutID, m9ID, huntsmanID;
		int gloveToUse;
		int bloodhoundID, driverID, handwrapsID, motoID, specialistID, sportID, hydraID;

		int uspID, p2000ID, glockID, dualberettaID, p250ID, fivesevenID, tech9ID, r8ID, deagleID;
		int novaID, xm1014ID, mag7ID, sawedoffID, m249ID, negevID;
		int mp9ID, mac10ID, mp7ID, ump45ID, p90ID, ppbizonID;
		int famasID, galilID, ak47ID, m4a4ID, m4a1sID, ssg08ID, augID, sg553ID, awpID, scar20ID, g3sg1ID;
		bool friendfire;

	private:
	}; extern CSettings settings;
}

extern bool using_fake_angles[65];
extern bool full_choke;
extern bool is_shooting;

extern bool in_tp;
extern bool fake_walk;

extern int resolve_type[65];

extern int target;
extern int shots_fired[65];
extern int shots_hit[65];
extern int shots_missed[65];
extern int backtrack_missed[65];

extern bool didShot;
extern bool didMiss;

extern float tick_to_back[65];
extern float lby_to_back[65];
extern bool backtrack_tick[65];

extern float lby_delta;
extern float update_time[65];
extern float walking_time[65];

extern float local_update;

extern int hitmarker_time;
extern int random_number;

extern bool menu_hide;

extern int oldest_tick[65];
extern float compensate[65][12];
extern Vector backtrack_hitbox[65][20][12];
extern float backtrack_simtime[65][12];