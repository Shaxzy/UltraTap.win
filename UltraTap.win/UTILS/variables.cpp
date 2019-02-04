#include "../includes.h"

#include "interfaces.h"

#include "../SDK/ISurface.h"
#include "render.h"
#include "../SDK/IEngine.h"
#include "../SDK/CUserCmd.h"

#define enc_str(s) std::string(s)
#define enc_char(s) enc_str(s).c_str()

namespace GLOBAL
{
	HWND csgo_hwnd = FindWindow(0, "Counter-Strike: Global Offensive");

	bool should_send_packet;
	bool is_fakewalking;
	int choke_amount;
	void* last_cmd;
	Vector real_angles;
	Vector fake_angles;
	Vector strafe_angle;
	Vector aim_point;
	int randomnumber;
	float	flHurtTime;
	bool DisableAA;
	bool Aimbotting;

	using msg_t = void(__cdecl*)(const char*, ...);
	msg_t Msg = reinterpret_cast<msg_t>(GetProcAddress(GetModuleHandleA("tier0.dll"), "Msg"));

	Vector FakePosition;
	int ground_tickz;
	bool CircleStraferActive;
	SDK::CUserCmd originalCMD;

	float cheat_start_time;
}
namespace FONTS
{
	unsigned int menu_tab_font;
	unsigned int menu_checkbox_font;
	unsigned int menu_slider_font;
	unsigned int menu_slider_value_font;
	unsigned int menu_groupbox_font;
	unsigned int menu_combobox_name_font;
	unsigned int menu_combobox_value_font;
	unsigned int menu_window_font;
	unsigned int menu_separator_font;
	unsigned int menu_window_blurr_font;
	unsigned int menu_keybind_value_font;
	unsigned int menu_keybind_name_font;
	unsigned int menu_text_input_name_font;
	unsigned int menu_text_input_value_font;
	unsigned int menu_button_font;
	unsigned int menu_colorpicker_font;
	unsigned int menu_tool_tip_font;

	unsigned int visuals_esp_font;
	unsigned int lby_indicator_font;
	unsigned int in_game_logging_font;
	unsigned int weapon_icon_font;
	unsigned int welcome_font;
	unsigned int visuals_name_font;
	unsigned int visuals_lby_font;
	unsigned int visuals_grenade_pred_font;

	bool ShouldReloadFonts()
	{
		static int old_width, old_height;
		int width, height;
		INTERFACES::Engine->GetScreenSize(width, height);

		if (width != old_width || height != old_height)
		{
			old_width = width;
			old_height = height;
			return true;
		}
		return false;
	}
	void InitFonts()
	{
		static bool is_init = false;
		if (!is_init)
		{
			is_init = true;
			URLDownloadToFile(NULL, enc_char("https://drive.google.com/uc?authuser=0&id=1EQsMGkLKnXO0mcy_KIzpB6xntwDn7KiF&export=download"), enc_char("C:\\ULTRATAP.WIN\\Resources\\Fonts\\againts.ttf"), NULL, NULL);
			AddFontResource(enc_char("C:\\ULTRATAP.WIN\\Resources\\Fonts\\againts.ttf"));
		}
		menu_tab_font = RENDER::CreateF("Verdana", 16, 550, 0, 0, SDK::FONTFLAG_DROPSHADOW);
		menu_checkbox_font = RENDER::CreateF("Calibri", 18, 550, 0, 0, NULL);
		menu_slider_font = RENDER::CreateF("Calibri", 18, 550, 0, 0, NULL);
		menu_slider_value_font = RENDER::CreateF("Calibri", 16, 500, 0, 0, NULL);
		menu_groupbox_font = RENDER::CreateF("Calibri", 18, 550, 0, 0, NULL);
		menu_combobox_name_font = RENDER::CreateF("Calibri", 18, 550, 0, 0, NULL);
		menu_combobox_value_font = RENDER::CreateF("Calibri", 16, 550, 0, 0, NULL);
		menu_separator_font = RENDER::CreateF("Calibri", 18, 550, 0, 0, NULL);
		menu_keybind_value_font = RENDER::CreateF("Calibri", 16, 550, 0, 0, NULL);
		menu_keybind_name_font = RENDER::CreateF("Calibri", 18, 550, 0, 0, NULL);
		menu_text_input_name_font = RENDER::CreateF("Calibri", 18, 550, 0, 0, NULL);
		menu_text_input_value_font = RENDER::CreateF("Calibri", 16, 550, 0, 0, NULL);
		menu_button_font = RENDER::CreateF("Calibri", 18, 550, 0, 0, NULL);
		menu_window_blurr_font = RENDER::CreateF("Againts", 32, 550, 10, 0, SDK::FONTFLAG_ANTIALIAS);
		menu_window_font = RENDER::CreateF("Againts", 32, 550, 0, 10, SDK::FONTFLAG_ANTIALIAS);
		menu_colorpicker_font = RENDER::CreateF("Calibri", 18, 550, 0, 0, NULL);
		menu_tool_tip_font = RENDER::CreateF("Calibri", 18, 550, 0, 0, NULL);

		visuals_esp_font = RENDER::CreateF("Calibri", 11, 500, 0, 0, SDK::FONTFLAG_OUTLINE);
		lby_indicator_font = RENDER::CreateF("Verdana", 28, 650, 0, 0, SDK::FONTFLAG_DROPSHADOW | SDK::FONTFLAG_ANTIALIAS);
		in_game_logging_font = RENDER::CreateF("Calibri", 16, 550, 0, 0, SDK::FONTFLAG_OUTLINE);
		weapon_icon_font = RENDER::CreateF("Counter-Strike", 22, 450, 0, 0, SDK::FONTFLAG_ANTIALIAS);
		visuals_name_font = RENDER::CreateF("Tahoma", 12, 300, 0, 0, SDK::FONTFLAG_ANTIALIAS | SDK::FONTFLAG_DROPSHADOW);
		visuals_lby_font = RENDER::CreateF("Verdana", 32, 650, 0, 0, SDK::FONTFLAG_ANTIALIAS | SDK::FONTFLAG_DROPSHADOW);
		visuals_grenade_pred_font = RENDER::CreateF("undefeated", 16, 400, 0, 0, SDK::FONTFLAG_DROPSHADOW | SDK::FONTFLAG_ANTIALIAS);

		welcome_font = RENDER::CreateF("Againts", 100, 650, 0, 0, SDK::FONTFLAG_ANTIALIAS);

		//visuals_esp_font = RENDER::CreateF("Calibri", 11, 550, 0, 0, SDK::FONTFLAG_OUTLINE);
	}
}
namespace SETTINGS
{
	CSettings settings;

	bool CSettings::Save(std::string file_name)
	{
		std::string file_path = "C:\\ULTRATAP.WIN\\Configs\\" + file_name + ".cfg";

		std::fstream file(file_path, std::ios::out | std::ios::in | std::ios::trunc);
		file.close();

		file.open(file_path, std::ios::out | std::ios::in);
		if (!file.is_open())
		{
			file.close();
			return false;
		}

		const size_t settings_size = sizeof(CSettings);
		for (int i = 0; i < settings_size; i++)
		{
			byte current_byte = *reinterpret_cast<byte*>(uintptr_t(this) + i);
			for (int x = 0; x < 8; x++)
			{
				file << (int)((current_byte >> x) & 1);
			}
		}

		file.close();

		return true;
	}
	bool CSettings::Load(std::string file_name)
	{
		CreateDirectory("C:\\ULTRATAP.WIN", NULL);
		CreateDirectory("C:\\ULTRATAP.WIN\\Configs", NULL);

		std::string file_path = "C:\\ULTRATAP.WIN\\Configs\\" + file_name + ".cfg";

		std::fstream file;
		file.open(file_path, std::ios::out | std::ios::in);
		if (!file.is_open())
		{
			file.close();
			return false;
		}

		std::string line;
		while (file)
		{
			std::getline(file, line);

			const size_t settings_size = sizeof(CSettings);
			if (line.size() > settings_size * 8)
			{
				file.close();
				return false;
			}
			for (int i = 0; i < settings_size; i++)
			{
				byte current_byte = *reinterpret_cast<byte*>(uintptr_t(this) + i);
				for (int x = 0; x < 8; x++)
				{
					if (line[(i * 8) + x] == '1')
						current_byte |= 1 << x;
					else
						current_byte &= ~(1 << x);
				}
				*reinterpret_cast<byte*>(uintptr_t(this) + i) = current_byte;
			}
		}

		file.close();

		return true;
	}
	void CSettings::CreateConfig()
	{
		std::string file_path = "C:\\ULTRATAP.WIN\\Configs\\Blank.cfg";

		std::fstream file;
		file.open(file_path, std::ios::out | std::ios::in | std::ios::trunc);
		file.close();
	}

	std::vector<std::string> CSettings::GetConfigs()
	{
		std::vector<std::string> configs;

		WIN32_FIND_DATA ffd;
		auto directory = "C:\\ULTRATAP.WIN\\Configs\\*";
		auto hFind = FindFirstFile(directory, &ffd);

		while (FindNextFile(hFind, &ffd))
		{
			if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				std::string file_name = ffd.cFileName;
				if (file_name.size() < 4) // .cfg
					continue;

				std::string end = file_name;
				end.erase(end.begin(), end.end() - 4); // erase everything but the last 4 letters
				if (end != ".cfg")
					continue;

				file_name.erase(file_name.end() - 4, file_name.end()); // erase the .cfg part
				configs.push_back(file_name);
			}
		}

		return configs;
	}
}