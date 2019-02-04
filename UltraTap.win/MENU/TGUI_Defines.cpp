#include "../includes.h"

#include "TGUI_Defines.h"

namespace TGUI
{
	HELPERS::TGUI_Color tgui_colors[TGUI_Color_Names::TGUI_COLOR_NUM];
	float tgui_attributes[TGUI_Attribute_Names::TGUI_ATTRIBUTE_NUM];
	unsigned int tgui_fonts[TGUI_Font_Names::TGUI_FONT_NUM];

	/// Command data base
	TGUI_Command_Data_Base TGUI_command_data_base;

	/// TGUI_Input
	void TGUI_Input::Update()
	{
		/// Record previous input
		previous_mouse = real_current_mouse;
		memcpy(previous_keyboard, current_keyboard, 256 * sizeof(int));

		/// Mouse shitz
		const auto mouse_info = UTILS::INPUT::input_handler.GetMouseInfo();

		current_mouse.position = mouse_info.position;
		current_mouse.left_button = mouse_info.left;
		current_mouse.right_button = mouse_info.right;
		current_mouse.scroll = (mouse_info.scroll == 0) ? 0 : (mouse_info.scroll > 0 ? 1 : -1);

		/// Time between clicks, useful if u wanna use double click features or shit
		if (current_mouse.left_button && !previous_mouse.left_button)
		{
			click_time_delta = HELPERS::TGUI_GetTime() - last_click_time;
			last_click_time = HELPERS::TGUI_GetTime();
		}

		/// Time shit
		update_delta_time = HELPERS::TGUI_GetTime() - last_update_time;
		last_update_time = HELPERS::TGUI_GetTime();

		/// Update the keyboard
		UTILS::INPUT::input_handler.GetKeyboardState(current_keyboard);

		/// Copy to real shit
		real_previous_mouse = real_current_mouse;
		real_current_mouse = current_mouse;
	}
}