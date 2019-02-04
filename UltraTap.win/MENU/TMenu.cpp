#include "../includes.h"
#include "../UTILS/interfaces.h"
#include "../SDK/IEngine.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/CInputSystem.h"
#include "../UTILS/render.h"
#include "../SDK/ConVar.h"

#include "../FEATURES/InGameLogger.h"

#include "TMenu.h"
bool menu_open = false;
int AutoCalc(int va)
{
	if (va == 1)
		return va * 35;
	else if (va == 2)
		return va * 34;
	else
		return va * 25 + 7.5;
}

#define RANDOMIZECOL CColor(rand() % 255 + 1, rand() % 255 + 1, rand() % 255 + 1)

namespace TGUI
{
	namespace MENU
	{
		void Do()
		{
			auto SetColor = []() -> void
			{
				const auto theme_color = HELPERS::TGUI_Color::LightBlue();

				tgui_colors[TGUI_COLOR_WINDOW_BODY] = HELPERS::TGUI_Color(40, 40, 40, 246);
				tgui_colors[TGUI_COLOR_WINDOW_BODY_OUTLINE] = HELPERS::TGUI_Color(0, 0, 0, 100);
				tgui_colors[TGUI_COLOR_WINDOW_TITLE_BAR] = HELPERS::TGUI_Color(50, 50, 50, 246);
				tgui_colors[TGUI_COLOR_WINDOW_TITLE_BAR_TEXT] = theme_color;
				tgui_colors[TGUI_COLOR_WINDOW_TITLE_BAR_OUTLINE] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_WINDOW_TITLE_BAR_BACKGROUND_TEXT] = HELPERS::TGUI_Color(19, 160, 216, 30);

				tgui_colors[TGUI_COLOR_CHECKBOX_UNSELECTED_BODY] = HELPERS::TGUI_Color(200, 200, 200, 246);
				tgui_colors[TGUI_COLOR_CHECKBOX_UNSELECTED_BODY_OUTLINE] = HELPERS::TGUI_Color(0, 0, 0, 150);
				tgui_colors[TGUI_COLOR_CHECKBOX_SELECTED_BODY] = theme_color;
				tgui_colors[TGUI_COLOR_CHECKBOX_SELECTED_BODY_OUTLINE] = HELPERS::TGUI_Color(0, 0, 0, 150);
				tgui_colors[TGUI_COLOR_CHECKBOX_TEXT] = HELPERS::TGUI_Color(200, 200, 200, 255);
				tgui_colors[TGUI_COLOR_CHECKBOX_HOVERED_TEXT] = HELPERS::TGUI_Color::White();

				tgui_colors[TGUI_COLOR_BUTTON_BODY] = HELPERS::TGUI_Color(80, 80, 80, 246);
				tgui_colors[TGUI_COLOR_BUTTON_BODY_CLICKED] = HELPERS::TGUI_Color(100, 100, 100, 246);
				tgui_colors[TGUI_COLOR_BUTTON_OUTLINE] = HELPERS::TGUI_Color(0, 0, 0, 150);
				tgui_colors[TGUI_COLOR_BUTTON_TEXT] = HELPERS::TGUI_Color(200, 200, 200, 246);
				tgui_colors[TGUI_COLOR_BUTTON_TEXT_CLICKED] = theme_color;

				tgui_colors[TGUI_COLOR_GROUPBOX_BODY] = HELPERS::TGUI_Color(50, 50, 50, 246);
				tgui_colors[TGUI_COLOR_GROUPBOX_BODY_HOVERED] = HELPERS::TGUI_Color(50, 50, 50, 246);
				tgui_colors[TGUI_COLOR_GROUPBOX_OUTLINE] = HELPERS::TGUI_Color(0, 0, 0, 150);
				tgui_colors[TGUI_COLOR_GROUPBOX_TEXT] = HELPERS::TGUI_Color(200, 200, 200, 246);
				tgui_colors[TGUI_COLOR_GROUPBOX_TEXT_HOVERED] = HELPERS::TGUI_Color::White();

				tgui_colors[TGUI_COLOR_SLIDER_BODY] = HELPERS::TGUI_Color(80, 80, 80, 246);
				tgui_colors[TGUI_COLOR_SLIDER_BODY_SELECTED] = theme_color;
				tgui_colors[TGUI_COLOR_SLIDER_OUTLINE] = HELPERS::TGUI_Color(0, 0, 0, 150);
				tgui_colors[TGUI_COLOR_SLIDER_TEXT_NAME] = HELPERS::TGUI_Color(200, 200, 200, 246);
				tgui_colors[TGUI_COLOR_SLIDER_TEXT_NAME_HOVERED] = HELPERS::TGUI_Color::White();
				tgui_colors[TGUI_COLOR_SLIDER_VALUE_TEXT] = HELPERS::TGUI_Color::White();

				tgui_colors[TGUI_COLOR_SEPARATOR_BODY] = HELPERS::TGUI_Color(100, 100, 100, 246);
				tgui_colors[TGUI_COLOR_SEPARATOR_TEXT] = HELPERS::TGUI_Color::White();

				tgui_colors[TGUI_COLOR_KEYBIND_BODY] = HELPERS::TGUI_Color(80, 80, 80, 246);
				tgui_colors[TGUI_COLOR_KEYBIND_BODY_SELECTED] = HELPERS::TGUI_Color(110, 110, 110, 246);
				tgui_colors[TGUI_COLOR_KEYBIND_OUTLINE] = HELPERS::TGUI_Color(0, 0, 0, 150);
				tgui_colors[TGUI_COLOR_KEYBIND_TEXT_NAME] = HELPERS::TGUI_Color(200, 200, 200, 246);
				tgui_colors[TGUI_COLOR_KEYBIND_TEXT_NAME_HOVERED] = HELPERS::TGUI_Color::White();
				tgui_colors[TGUI_COLOR_KEYBIND_TEXT_VALUE] = HELPERS::TGUI_Color::White();

				tgui_colors[TGUI_COLOR_TEXT_INPUT_BODY] = HELPERS::TGUI_Color(80, 80, 80, 246);
				tgui_colors[TGUI_COLOR_TEXT_INPUT_BODY_SELECTED] = HELPERS::TGUI_Color(110, 110, 110, 246);
				tgui_colors[TGUI_COLOR_TEXT_INPUT_OUTLINE] = HELPERS::TGUI_Color(0, 0, 0, 150);
				tgui_colors[TGUI_COLOR_TEXT_INPUT_TEXT_VALUE] = HELPERS::TGUI_Color::White();
				tgui_colors[TGUI_COLOR_TEXT_INPUT_TEXT_NAME] = HELPERS::TGUI_Color(200, 200, 200, 246);
				tgui_colors[TGUI_COLOR_TEXT_INPUT_TEXT_HOVERED] = HELPERS::TGUI_Color::White();

				tgui_colors[TGUI_COLOR_COMBOBOX_BODY_SELECTED] = HELPERS::TGUI_Color(80, 80, 80, 246);
				tgui_colors[TGUI_COLOR_COMBOBOX_OUTLINE_SELECTED] = HELPERS::TGUI_Color(0, 0, 0, 150);
				tgui_colors[TGUI_COLOR_COMBOBOX_BODY_ITEM] = HELPERS::TGUI_Color(50, 50, 50, 246);
				tgui_colors[TGUI_COLOR_COMBOBOX_OUTLINE_ITEM] = HELPERS::TGUI_Color(0, 0, 0, 150);
				tgui_colors[TGUI_COLOR_COMBOBOX_SELECTED_TEXT_ITEM] = theme_color;
				tgui_colors[TGUI_COLOR_COMBOBOX_TEXT_ITEM] = HELPERS::TGUI_Color(200, 200, 200, 246);
				tgui_colors[TGUI_COLOR_COMBOBOX_TEXT_SELECTED] = HELPERS::TGUI_Color(200, 200, 200, 246);
				tgui_colors[TGUI_COLOR_COMBOBOX_TEXT_SELECTED_HOVERED] = HELPERS::TGUI_Color::White();
				tgui_colors[TGUI_COLOR_COMBOBOX_TEXT_NAME] = HELPERS::TGUI_Color(200, 200, 200, 246);
				tgui_colors[TGUI_COLOR_COMBOBOX_TEXT_NAME_HOVERED] = HELPERS::TGUI_Color::White();

				tgui_colors[TGUI_COLOR_COLOR_PICKER_BODY_COLOR] = HELPERS::TGUI_Color(50, 50, 50, 246);
				tgui_colors[TGUI_COLOR_COLOR_PICKER_OUTLINE] = HELPERS::TGUI_Color(0, 0, 0, 150);
				tgui_colors[TGUI_COLOR_COLOR_PICKER_TEXT] = HELPERS::TGUI_Color(200, 200, 200, 246);
				tgui_colors[TGUI_COLOR_COLOR_PICKER_TEXT_HOVERED] = HELPERS::TGUI_Color::White();

				tgui_colors[TGUI_COLOR_TAB_BODY] = HELPERS::TGUI_Color(40, 40, 40, 246);
				tgui_colors[TGUI_COLOR_TAB_BODY_SELECTED] = HELPERS::TGUI_Color(70, 70, 70, 246);
				tgui_colors[TGUI_COLOR_TAB_TEXT] = HELPERS::TGUI_Color(255, 255, 255, 255);
				tgui_colors[TGUI_COLOR_TAB_TEXT_SELECTED] = theme_color;
				tgui_colors[TGUI_COLOR_TAB_OUTLINE] = HELPERS::TGUI_Color(0, 0, 0, 150);

				tgui_colors[TGUI_COLOR_TOOL_TIP_BODY] = HELPERS::TGUI_Color(200, 200, 200, 246);
				tgui_colors[TGUI_COLOR_TOOL_TIP_OUTLINE] = HELPERS::TGUI_Color(0, 0, 0, 246);
				tgui_colors[TGUI_COLOR_TOOL_TIP_TEXT] = HELPERS::TGUI_Color(0, 0, 0, 246);
			};
			auto SetAttributes = []() -> void
			{
				tgui_attributes[TGUI_ATTRIBUTE_PADDING_HEIGHT] = 12;
				tgui_attributes[TGUI_ATTRIBUTE_PADDING_WIDTH] = 12;
				tgui_attributes[TGUI_ATTRIBUTE_SCROLL_SPEED] = 10;

				tgui_attributes[TGUI_ATTRIBUTE_WINDOW_TITLE_BAR_HEIGHT] = 30;

				tgui_attributes[TGUI_ATTRIBUTE_CHECKBOX_HEIGHT] = 10;
				tgui_attributes[TGUI_ATTRIBUTE_CHECKBOX_WIDTH] = 10;

				tgui_attributes[TGUI_ATTRIBUTE_BUTTON_HEIGHT] = 16;
				tgui_attributes[TGUI_ATTRIBUTE_BUTTON_WIDTH] = 100;
				tgui_attributes[TGUI_ATTRIBUTE_BUTTON_CLICK_DURATION] = 0.15f;

				tgui_attributes[TGUI_ATTRIBUTE_SLIDER_HEIGHT] = 6;
				tgui_attributes[TGUI_ATTRIBUTE_SLIDER_WIDTH] = 200;

				tgui_attributes[TGUI_ATTRIBUTE_KEYBIND_HEIGHT] = 14;
				tgui_attributes[TGUI_ATTRIBUTE_KEYBIND_WIDTH] = 100;

				tgui_attributes[TGUI_ATTRIBUTE_TEXT_INPUT_HEIGHT] = 14;
				tgui_attributes[TGUI_ATTRIBUTE_TEXT_INPUT_WIDTH] = 200;

				tgui_attributes[TGUI_ATTRIBUTE_COMBOBOX_WIDTH] = 200;
				tgui_attributes[TGUI_ATTRIBUTE_COMBOBOX_HEIGHT] = 14;
				tgui_attributes[TGUI_ATTRIBUTE_COMBOBOX_MAX_ITEMS_DISPLAYED] = 8;

				tgui_attributes[TGUI_ATTRIBUTE_COLOR_PICKER_WIDTH] = 30;
				tgui_attributes[TGUI_ATTRIBUTE_COLOR_PICKER_HEIGHT] = 10;
				tgui_attributes[TGUI_ATTRIBUTE_COLOR_PICKER_OPEN_WIDTH] = 250;
				tgui_attributes[TGUI_ATTRIBUTE_COLOR_PICKER_OPEN_HEIGHT] = 150;
				tgui_attributes[TGUI_ATTRIBUTE_COLOR_PICKER_SLIDER_WIDTH] = 14;
				tgui_attributes[TGUI_ATTRIBUTE_COLOR_PICKER_PADDING] = 8;
				tgui_attributes[TGUI_ATTRIBUTE_COLOR_PICKER_PIXELATION] = 6;

				tgui_attributes[TGUI_ATTRIBUTE_TAB_HEIGHT] = 16;
				tgui_attributes[TGUI_ATTRIBUTE_TAB_ITEM_PADDING] = 10;

				tgui_attributes[TGUI_ATTRIBUTE_TOOL_TIP_LINE_HEIGHT] = 16;
			};
			auto SetFonts = []()
			{
				tgui_fonts[TGUI_FONT_WINDOW_NAME] = FONTS::menu_window_font;
				tgui_fonts[TGUI_FONT_WINDOW_NAME_BEHIND] = FONTS::menu_window_blurr_font;

				tgui_fonts[TGUI_FONT_GROUPBOX] = FONTS::menu_groupbox_font;

				tgui_fonts[TGUI_FONT_SEPARATOR] = FONTS::menu_separator_font;

				tgui_fonts[TGUI_FONT_CHECKBOX] = FONTS::menu_checkbox_font;

				tgui_fonts[TGUI_FONT_BUTTON] = FONTS::menu_button_font;

				tgui_fonts[TGUI_FONT_SLIDER_NAME] = FONTS::menu_slider_font;
				tgui_fonts[TGUI_FONT_SLIDER_VALUE] = FONTS::menu_slider_value_font;

				tgui_fonts[TGUI_FONT_KEYBIND_NAME] = FONTS::menu_keybind_name_font;
				tgui_fonts[TGUI_FONT_KEYBIND_VALUE] = FONTS::menu_keybind_value_font;

				tgui_fonts[TGUI_FONT_TEXT_INPUT_NAME] = FONTS::menu_text_input_name_font;
				tgui_fonts[TGUI_FONT_TEXT_INPUT_VALUE] = FONTS::menu_text_input_value_font;

				tgui_fonts[TGUI_FONT_COMBOBOX_NAME] = FONTS::menu_combobox_name_font;
				tgui_fonts[TGUI_FONT_COMBOBOX_SELECTED_ITEM] = FONTS::menu_combobox_value_font;
				tgui_fonts[TGUI_FONT_COMBOBOX_ITEM_NAME] = FONTS::menu_combobox_value_font;

				tgui_fonts[TGUI_FONT_TAB_SELECTED_ITEM_NAME] = FONTS::menu_tab_font;
				tgui_fonts[TGUI_FONT_TAB_ITEM_NAME] = FONTS::menu_tab_font;

				tgui_fonts[TGUI_FONT_TOOL_TIP] = FONTS::menu_tool_tip_font;

				tgui_fonts[TGUI_FONT_COLOR_PICKER] = FONTS::menu_colorpicker_font;
			};

			auto SetDebugColor = []()
			{
				tgui_colors[TGUI_COLOR_WINDOW_BODY] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_WINDOW_BODY_OUTLINE] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_WINDOW_TITLE_BAR] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_WINDOW_TITLE_BAR_TEXT] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_WINDOW_TITLE_BAR_OUTLINE] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_WINDOW_TITLE_BAR_BACKGROUND_TEXT] = HELPERS::TGUI_Color(0, 0, 0, 0);

				tgui_colors[TGUI_COLOR_CHECKBOX_UNSELECTED_BODY] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_CHECKBOX_UNSELECTED_BODY_OUTLINE] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_CHECKBOX_SELECTED_BODY] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_CHECKBOX_SELECTED_BODY_OUTLINE] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_CHECKBOX_TEXT] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_CHECKBOX_HOVERED_TEXT] = HELPERS::TGUI_Color(0, 0, 0, 0);

				tgui_colors[TGUI_COLOR_BUTTON_BODY] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_BUTTON_BODY_CLICKED] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_BUTTON_OUTLINE] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_BUTTON_TEXT] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_BUTTON_TEXT_CLICKED] = HELPERS::TGUI_Color(0, 0, 0, 0);

				tgui_colors[TGUI_COLOR_GROUPBOX_BODY] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_GROUPBOX_BODY_HOVERED] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_GROUPBOX_OUTLINE] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_GROUPBOX_TEXT] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_GROUPBOX_TEXT_HOVERED] = HELPERS::TGUI_Color(0, 0, 0, 0);

				tgui_colors[TGUI_COLOR_SLIDER_BODY] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_SLIDER_BODY_SELECTED] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_SLIDER_OUTLINE] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_SLIDER_TEXT_NAME] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_SLIDER_TEXT_NAME_HOVERED] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_SLIDER_VALUE_TEXT] = HELPERS::TGUI_Color(0, 0, 0, 0);

				tgui_colors[TGUI_COLOR_SEPARATOR_BODY] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_SEPARATOR_TEXT] = HELPERS::TGUI_Color(0, 0, 0, 0);

				tgui_colors[TGUI_COLOR_KEYBIND_BODY] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_KEYBIND_BODY_SELECTED] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_KEYBIND_OUTLINE] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_KEYBIND_TEXT_NAME] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_KEYBIND_TEXT_NAME_HOVERED] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_KEYBIND_TEXT_VALUE] = HELPERS::TGUI_Color(0, 0, 0, 0);

				tgui_colors[TGUI_COLOR_TEXT_INPUT_BODY] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_TEXT_INPUT_BODY_SELECTED] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_TEXT_INPUT_OUTLINE] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_TEXT_INPUT_TEXT_VALUE] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_TEXT_INPUT_TEXT_NAME] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_TEXT_INPUT_TEXT_HOVERED] = HELPERS::TGUI_Color(0, 0, 0, 0);

				tgui_colors[TGUI_COLOR_COMBOBOX_BODY_SELECTED] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_COMBOBOX_OUTLINE_SELECTED] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_COMBOBOX_BODY_ITEM] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_COMBOBOX_OUTLINE_ITEM] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_COMBOBOX_SELECTED_TEXT_ITEM] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_COMBOBOX_TEXT_ITEM] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_COMBOBOX_TEXT_SELECTED] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_COMBOBOX_TEXT_SELECTED_HOVERED] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_COMBOBOX_TEXT_NAME] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_COMBOBOX_TEXT_NAME_HOVERED] = HELPERS::TGUI_Color(0, 0, 0, 0);

				tgui_colors[TGUI_COLOR_COLOR_PICKER_BODY_COLOR] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_COLOR_PICKER_OUTLINE] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_COLOR_PICKER_TEXT] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_COLOR_PICKER_TEXT_HOVERED] = HELPERS::TGUI_Color(0, 0, 0, 0);

				tgui_colors[TGUI_COLOR_TAB_BODY] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_TAB_BODY_SELECTED] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_TAB_TEXT] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_TAB_TEXT_SELECTED] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_TAB_OUTLINE] = HELPERS::TGUI_Color(0, 0, 0, 0);

				tgui_colors[TGUI_COLOR_TOOL_TIP_BODY] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_TOOL_TIP_OUTLINE] = HELPERS::TGUI_Color(0, 0, 0, 0);
				tgui_colors[TGUI_COLOR_TOOL_TIP_TEXT] = HELPERS::TGUI_Color(0, 0, 0, 0);
			};

			///SetDebugColor();
			SetColor();
			SetAttributes();
			SetFonts();

			/// Start
			TGUI_BeginFrame();

			/// Menu toggle
			
			if (menu.GetInput().DidClickKey(VK_INSERT))
			{
				menu_open = !menu_open;

				INTERFACES::Engine->ClientCmd(menu_open ? "cl_mouseenable 0" : "cl_mouseenable 1");
				INTERFACES::InputSystem->EnableInput(!menu_open);
			}

			if (!menu_open)
			{
				TGUI_EndFrame();
				return;
			}

			/// Draw mouse
			TGUI_DrawMouse();

			if (UTILS::INPUT::input_handler.GetKeyState(UTILS::INPUT::input_handler.keyBindings(SETTINGS::settings.flip_int)) & 1)
				SETTINGS::settings.flip_bool = !SETTINGS::settings.flip_bool;

			if (UTILS::INPUT::input_handler.GetKeyState(UTILS::INPUT::input_handler.keyBindings(SETTINGS::settings.quickstopkey)) & 1)
				SETTINGS::settings.stop_flip = !SETTINGS::settings.stop_flip;

			if (UTILS::INPUT::input_handler.GetKeyState(UTILS::INPUT::input_handler.keyBindings(SETTINGS::settings.overridekey)) & 1)
				SETTINGS::settings.overridething = !SETTINGS::settings.overridething;

			TGUI_BeginWindow("ULTRATAP.WIN", HELPERS::TGUI_Vector2D(200, 200), HELPERS::TGUI_Vector2D(800, 650), 2);
			{
				static char aim_mode[][TGUI_MAX_STRING_LENGTH] = { "rage", "legit" };
				static char acc_mode[][TGUI_MAX_STRING_LENGTH] = { "hit-scan" };
				static char hitscan_items[][TGUI_MAX_STRING_LENGTH] = { "head", "neck", "chest", "stomach", "arms", "legs", "feet" };
				static char chams_mode[][TGUI_MAX_STRING_LENGTH] = { "none", "visible", "invisible" };
				static char aa_pitch[][TGUI_MAX_STRING_LENGTH] = { "none", "emotion", "fake down", "fake up", "fake zero" };
				static char aa_mode[][TGUI_MAX_STRING_LENGTH] = { "none", "backwards", "freestanding(b1g skeet)", "backjitter", "lowerbody", "legit troll", "rotational", "freestanding" };
				static char aa_fake[][TGUI_MAX_STRING_LENGTH] = { "none", "backjitter", "random", "local view", "opposite", "rotational" };
				static char configs[][TGUI_MAX_STRING_LENGTH] = { "default", "legit", "autos", "scouts", "pistols", "awps", "nospread" };
				static char box_style[][TGUI_MAX_STRING_LENGTH] = { "none", "full", "debug" };
				static char media_style[][TGUI_MAX_STRING_LENGTH] = { "perfect", "random" };
				static char delay_shot[][TGUI_MAX_STRING_LENGTH] = { "off", "lag compensation" };
				static char fakelag_mode[][TGUI_MAX_STRING_LENGTH] = { "factor", "adaptive" };
				static char hitmarker[][TGUI_MAX_STRING_LENGTH] = { "none", "gamesense", "bameware", "custom" };
				static char antiaimmode[][TGUI_MAX_STRING_LENGTH] = { "standing", "moving", "jumping" };
				static char glow_styles[][TGUI_MAX_STRING_LENGTH] = { "regular", "pulsing", "outline" };
				static char local_chams[][TGUI_MAX_STRING_LENGTH] = { "none", "sim fakelag: normal", "non-sim fakelag", "sim fakelag: color", "sim fake" };
				static char chams_type[][TGUI_MAX_STRING_LENGTH] = { "none", "basic", "metallic" };
				static char team_select[][TGUI_MAX_STRING_LENGTH] = { "enemy", "team" };
				static char crosshair_select[][TGUI_MAX_STRING_LENGTH] = { "none", "static", "recoil" };
				static char override_method[][TGUI_MAX_STRING_LENGTH] = { "set", "key-press" };

				static char clan_tag_types[][TGUI_MAX_STRING_LENGTH] = { "static", "marquee", "shift" };

				std::string config;

				/// Main tabs
				static char main_tabs[][TGUI_MAX_STRING_LENGTH] =
				{
					"Aimbot",
					"Visuals",
					"Miscellaneous",
					"Settings"
				};

				static int selected_main_tab = 0;
				TGUI_SetColumn(0);
				switch (TGUI_Tab(4, main_tabs, selected_main_tab))
				{
					/* Aimbot */
				case 0: {

					TGUI_SetColumn(1);

					TGUI_Checkbox("enable aimbot", SETTINGS::settings.aim_bool);
					if (SETTINGS::settings.aim_bool)
					{
						TGUI_BeginGroupbox("main", AutoCalc(1));
						{
							TGUI_Combobox("aimbot type", 2, aim_mode, SETTINGS::settings.aim_type);
						}
						TGUI_EndGroupbox();

						if (SETTINGS::settings.aim_type == 0)
						{
							TGUI_BeginGroupbox("hitbox selection", AutoCalc(2));
							{
								TGUI_Combobox("hitbox", 1, acc_mode, SETTINGS::settings.acc_type);
								TGUI_MultiselectCombobox("hit-scan", 5, hitscan_items, SETTINGS::settings.hitscan_type);
							}
							TGUI_EndGroupbox();

							TGUI_BeginGroupbox("damage control", AutoCalc(2));
							{
								TGUI_Slider("minimum damage", "dmg", 1, 100, SETTINGS::settings.damage_val); TGUI_ToolTip("Shot only if can deal x dmg.");
								TGUI_Slider("minimum a.wall damage", "dmg", 1, 100, SETTINGS::settings.awall_damage_val); TGUI_ToolTip("Shot only if can deal x dmg through walls.");
							}
							TGUI_EndGroupbox();

							TGUI_BeginGroupbox("accuracy", AutoCalc(5));
							{
								TGUI_Checkbox("backtrack", SETTINGS::settings.backtrack_bool);

								TGUI_Slider("minimum hit-chance", "%", 0, 100, SETTINGS::settings.chance_val); TGUI_ToolTip("Only shot if accuracy is lower than hit-chance.");
								TGUI_Checkbox("multipoint", SETTINGS::settings.multi_bool);
								if (SETTINGS::settings.multi_bool)
								{
									TGUI_Slider("head scale", "°", 0, 1, SETTINGS::settings.point_val);
									TGUI_Slider("body scale", "°", 0, 1, SETTINGS::settings.body_val);
								}
								else
								{
									TGUI_AlphaModulate(0.2f);
									TGUI_Disable();
									TGUI_Slider("head scale", "°", 0, 1, SETTINGS::settings.point_val);
									TGUI_AlphaModulate(0.2f);
									TGUI_Disable();
									TGUI_Slider("body scale", "°", 0, 1, SETTINGS::settings.body_val);
								}
							}
							TGUI_EndGroupbox();

							TGUI_BeginGroupbox("corrections", AutoCalc(7));
							{
								TGUI_Checkbox("auto stop", SETTINGS::settings.stop_bool); TGUI_ToolTip("Stop automatically if enemy is visble & hittable..");
								TGUI_Checkbox("fakewalk baim", SETTINGS::settings.baim_fakewalk); TGUI_ToolTip("baim when entity used fakewalk/slowwalk");;
								TGUI_Checkbox("baim in missed shot", SETTINGS::settings.baim_fake); TGUI_ToolTip("missed_shot > 2");;
								TGUI_Checkbox("baim in air", SETTINGS::settings.baim_inair); TGUI_ToolTip("shot baim in air");;
								TGUI_Keybind("force baim key", SETTINGS::settings.forcebaimkey);
								//TGUI_Combobox("auto stop method", autostop_method, SETTINGS::settings.autostopmethod);
								TGUI_Checkbox("desync resolver", SETTINGS::settings.resolve_bool); TGUI_ToolTip("Correct enemy's desync angles.");
							}
							TGUI_EndGroupbox();

							TGUI_SetColumn(2);
							if (TGUI_Checkbox("enable antiaim", SETTINGS::settings.aa_bool))
							{
								TGUI_BeginGroupbox("antiaim", AutoCalc(4));
								{
									TGUI_Combobox("antiaim mode", 3, antiaimmode, SETTINGS::settings.aa_mode);
									switch (SETTINGS::settings.aa_mode)
									{
									case 0:
										TGUI_Combobox("antiaim pitch - standing", 5, aa_pitch, SETTINGS::settings.aa_pitch_type);
										TGUI_Combobox("antiaim real - standing", 8, aa_mode, SETTINGS::settings.aa_real_type);
										TGUI_Combobox("antiaim fake - standing", 6, aa_fake, SETTINGS::settings.aa_fake_type);
										break;
									case 1:
										TGUI_Combobox("antiaim pitch - moving", 5, aa_pitch, SETTINGS::settings.aa_pitch1_type);
										TGUI_Combobox("antiaim real - moving", 8, aa_mode, SETTINGS::settings.aa_real1_type);
										TGUI_Combobox("antiaim fake - moving", 6, aa_fake, SETTINGS::settings.aa_fake1_type);
										break;
									case 2:
										TGUI_Combobox("antiaim pitch - jumping", 5, aa_pitch, SETTINGS::settings.aa_pitch2_type);
										TGUI_Combobox("antiaim real - jumping", 8, aa_mode, SETTINGS::settings.aa_real2_type);
										TGUI_Combobox("antiaim fake - jumping", 6, aa_fake, SETTINGS::settings.aa_fake2_type);
										break;
									}

								}
								TGUI_EndGroupbox();

								TGUI_BeginGroupbox("additional options", AutoCalc(5));
								{
									TGUI_Keybind("flip key", SETTINGS::settings.flip_int);
									//TGUI_Checkbox("fake angle chams", SETTINGS::settings.aa_fakeangchams_bool);
									TGUI_Checkbox("anti-aim arrows", SETTINGS::settings.antiaim_arrow); TGUI_ToolTip("Shows you where your real angle is with indicators.");
									switch (SETTINGS::settings.aa_mode)
									{
									case 0:
										TGUI_Slider("real additive", "°", -180, 180, SETTINGS::settings.aa_realadditive_val);
										TGUI_Slider("fake additive", "°", -180, 180, SETTINGS::settings.aa_fakeadditive_val);
										TGUI_Slider("lowerbodyyaw delta", "°", -119.9, 119.9, SETTINGS::settings.delta_val);
										break; //TGUI_Checkbox("lby flick up", SETTINGS::settings.lbyflickup);
									case 1:
										TGUI_Slider("real additive", "°", -180, 180, SETTINGS::settings.aa_realadditive1_val);
										TGUI_Slider("fake additive", "°", -180, 180, SETTINGS::settings.aa_fakeadditive1_val);
										TGUI_Slider("lowerbodyyaw delta", "°", -119.9, 119.9, SETTINGS::settings.delta1_val);
										break; //TGUI_Checkbox("lby flick up", SETTINGS::settings.lbyflickup1);
									case 2:
										TGUI_Slider("real additive", "°", -180, 180, SETTINGS::settings.aa_realadditive2_val);
										TGUI_Slider("fake additive", "°", -180, 180, SETTINGS::settings.aa_fakeadditive2_val);
										TGUI_Slider("lowerbodyyaw delta", "°", -119.9, 119.9, SETTINGS::settings.delta2_val);
										break; //TGUI_Checkbox("lby flick up", SETTINGS::settings.lbyflickup2);
									}

								}
								TGUI_EndGroupbox();

								TGUI_BeginGroupbox("rotate", AutoCalc(4));
								{

									TGUI_Slider("rotate fake - angle", "°", 0, 180, SETTINGS::settings.spinanglefake);
									TGUI_Slider("rotate fake - speed", "%", 0, 100, SETTINGS::settings.spinspeedfake);

									switch (SETTINGS::settings.aa_mode)
									{
									case 0:
										TGUI_Slider("rotate angle - standing", "°", 0, 180, SETTINGS::settings.spinangle);
										TGUI_Slider("rotate speed - standing", "%", 0, 100, SETTINGS::settings.spinspeed);
										break;
									case 1:
										TGUI_Slider("rotate angle - moving", "°", 0, 180, SETTINGS::settings.spinangle1);
										TGUI_Slider("rotate speed - moving", "%", 0, 100, SETTINGS::settings.spinspeed1);
										break;
									case 2:
										TGUI_Slider("rotate angle - jumping", "°", 0, 180, SETTINGS::settings.spinangle2);
										TGUI_Slider("rotate speed - jumping", "%", 0, 100, SETTINGS::settings.spinspeed2);
										break;
									}

								}
								TGUI_EndGroupbox();
							}
						}
						else
						{
							TGUI_BeginGroupbox("trigger and others", AutoCalc(3));
							{
								TGUI_Checkbox("triggerbot", SETTINGS::settings.legittrigger_bool); TGUI_ToolTip("Shot automatically if you aim at enemies.");
								TGUI_Keybind("triggerbot key", SETTINGS::settings.legittrigger_key);
								TGUI_Checkbox("backtrack", SETTINGS::settings.back_bool);
							}
							TGUI_EndGroupbox();
						}
					}
				} break;
					/* Visuals */
				case 1: {
					TGUI_SetColumn(1);
					TGUI_Checkbox("enable visuals", SETTINGS::settings.esp_bool);
					if (SETTINGS::settings.esp_bool)
					{
						TGUI_BeginGroupbox("players", AutoCalc(9));
						{
							TGUI_Combobox("team selection", 2, team_select, SETTINGS::settings.espteamselection);
							if (SETTINGS::settings.espteamselection == 0)
							{
								TGUI_Checkbox("draw enemy box", SETTINGS::settings.box_bool);
								TGUI_Checkbox("draw enemy name", SETTINGS::settings.name_bool);
								TGUI_Checkbox("draw enemy weapon", SETTINGS::settings.weap_bool);
								TGUI_Checkbox("draw enemy flags", SETTINGS::settings.info_bool);
								TGUI_Checkbox("draw enemy health", SETTINGS::settings.health_bool);
								TGUI_Checkbox("draw enemy money", SETTINGS::settings.money_bool);
								TGUI_Checkbox("draw enemy ammo", SETTINGS::settings.ammo_bool);
								TGUI_Checkbox("draw enemy fov arrows", SETTINGS::settings.fov_bool);
							}
							else if (SETTINGS::settings.espteamselection == 1)
							{
								TGUI_Checkbox("draw team box", SETTINGS::settings.boxteam);
								TGUI_Checkbox("draw team name", SETTINGS::settings.nameteam);
								TGUI_Checkbox("draw team weapon", SETTINGS::settings.weaponteam);
								TGUI_Checkbox("draw team flags", SETTINGS::settings.flagsteam);
								TGUI_Checkbox("draw team health", SETTINGS::settings.healthteam);
								TGUI_Checkbox("draw team money", SETTINGS::settings.moneyteam);
								TGUI_Checkbox("draw team ammo", SETTINGS::settings.ammoteam);
								TGUI_Checkbox("draw team fov arrows", SETTINGS::settings.arrowteam);
							}
						}
						TGUI_EndGroupbox();

						TGUI_BeginGroupbox("chams", AutoCalc(3));
						{
							TGUI_Combobox("model team selection", 2, team_select, SETTINGS::settings.chamsteamselection);
							if (SETTINGS::settings.chamsteamselection == 0)
							{
								TGUI_Combobox("enemy coloured models", 3, chams_mode, SETTINGS::settings.chams_type);
							}
							else if (SETTINGS::settings.chamsteamselection == 1)
								TGUI_Combobox("team coloured models", 3, chams_mode, SETTINGS::settings.chamsteam);
							TGUI_Combobox("model type", 3, chams_type, SETTINGS::settings.chamstype);
						}
						TGUI_EndGroupbox();

						TGUI_BeginGroupbox("glow", AutoCalc(5));
						{
							TGUI_Combobox("glow team selection", 3, team_select, SETTINGS::settings.glowteamselection);
							if (SETTINGS::settings.glowteamselection == 0)
								TGUI_Checkbox("enemy glow enable", SETTINGS::settings.glowenable);
							else if (SETTINGS::settings.glowteamselection == 1)
								TGUI_Checkbox("team glow enable", SETTINGS::settings.glowteam);
							TGUI_Combobox("glow style", 3, glow_styles, SETTINGS::settings.glowstyle);
							TGUI_Checkbox("local glow", SETTINGS::settings.glowlocal);
							TGUI_Combobox("local glow style", 3, glow_styles, SETTINGS::settings.glowstylelocal);
						}
						TGUI_EndGroupbox();
						TGUI_SetColumn(2);
						TGUI_BeginGroupbox("world", AutoCalc(11));
						{
							TGUI_Checkbox("night mode", SETTINGS::settings.night_bool);
							TGUI_Checkbox("bullet tracers", SETTINGS::settings.beam_bool);
							TGUI_Checkbox("thirdperson", SETTINGS::settings.tp_bool);
							TGUI_SameLine();
							TGUI_Keybind("thirdperson key", SETTINGS::settings.thirdperson_int);
							TGUI_Combobox("crosshair", 3, crosshair_select, SETTINGS::settings.xhair_type);
							TGUI_Checkbox("render spread", SETTINGS::settings.spread_bool);
							TGUI_Checkbox("remove smoke", SETTINGS::settings.smoke_bool);
							TGUI_Checkbox("remove scope", SETTINGS::settings.scope_bool); TGUI_ToolTip("Remove black circle while scoping to have better vision.");
							TGUI_Checkbox("remove zoom", SETTINGS::settings.removescoping);
							TGUI_Checkbox("fix zoom sensitivity", SETTINGS::settings.fixscopesens);
							if (!SETTINGS::settings.matpostprocessenable)
								TGUI_Checkbox("enable postprocessing", SETTINGS::settings.matpostprocessenable);
							else
								TGUI_Checkbox("disable postprocessing", SETTINGS::settings.matpostprocessenable);
							//TGUI_Keybind("flashlight", SETTINGS::settings.flashlightkey);
						}
						TGUI_EndGroupbox();

						TGUI_BeginGroupbox("local player", AutoCalc(7));
						{
							TGUI_Combobox("local chams", 5, local_chams, SETTINGS::settings.localchams);
							TGUI_Slider("render fov", "", 0, 179, SETTINGS::settings.fov_val, 0);
							TGUI_Slider("viewmodel fov", "", 0, 179, SETTINGS::settings.viewfov_val, 0);
							TGUI_Slider("viewmodel x", "", 0, 10, SETTINGS::settings.viewmodel_x, 0);
							TGUI_Slider("viewmodel y", "", 0, 10, SETTINGS::settings.viewmodel_y, 0);
							TGUI_Slider("viewmodel z", "", 0, 20, SETTINGS::settings.viewmodel_z, 0);
							TGUI_Combobox("hitmarker sound", 4, hitmarker, SETTINGS::settings.hitmarker_val);;
						}
						TGUI_EndGroupbox();
					}
				} break;
					/* Miscellaneous */
				case 2: {
					TGUI_SetColumn(1);

					TGUI_Checkbox("enable misc", SETTINGS::settings.misc_bool);
					if (SETTINGS::settings.misc_bool)
					{
						TGUI_BeginGroupbox("movement", AutoCalc(8));
						{
							TGUI_Checkbox("auto bunnyhop", SETTINGS::settings.bhop_bool); TGUI_ToolTip("Automatically jump for you.");
							TGUI_Checkbox("auto strafer", SETTINGS::settings.strafe_bool); TGUI_ToolTip("Helps acheive higher speed while jumping.");
		
							TGUI_Keybind("circle strafe", SETTINGS::settings.circlestrafekey); TGUI_ToolTip("Automatically circlestrafe while in air.");
							TGUI_Checkbox("slow-walk", SETTINGS::settings.slowwalk);
							TGUI_Keybind("bind##slow-walk", SETTINGS::settings.slowwalkkey);
							TGUI_Slider("slow-walk speed", "", 1, 50, SETTINGS::settings.slowwalkspeed);
						}
						TGUI_EndGroupbox();

						TGUI_SetColumn(2);

						TGUI_BeginGroupbox("fakelag", AutoCalc(5));
						{
							TGUI_Checkbox("enable", SETTINGS::settings.lag_bool);
							TGUI_Combobox("fakelag type", 2, fakelag_mode, SETTINGS::settings.lag_type);

							if (SETTINGS::settings.lag_type == 1)
							{ TGUI_AlphaModulate(0.2f); TGUI_Disable(); }
							TGUI_Slider("moving lag", "", 1, 14, SETTINGS::settings.move_lag);
							if (SETTINGS::settings.lag_type == 1)
							{ TGUI_AlphaModulate(0.2f); TGUI_Disable(); }
							TGUI_Slider("jumping lag", "", 1, 14, SETTINGS::settings.jump_lag);
						}
						TGUI_EndGroupbox();

						TGUI_BeginGroupbox("extra", AutoCalc(2));
						{
							TGUI_Checkbox("auto revolver(beta)", SETTINGS::settings.autorevolver_b1g); TGUI_ToolTip("Automatically shoots enemy with revolver(by skoot.cc)");
							TGUI_Checkbox("auto zeus", SETTINGS::settings.autozeus_bool); TGUI_ToolTip("Automatically shoots enemy with zeus.");
						}
						TGUI_EndGroupbox();
					}
				} break;
					/* Settings */
				case 3: {
					TGUI_SetColumn(1);
					TGUI_BeginGroupbox("esp colours", AutoCalc(6));
					{
						TGUI_Combobox("esp colour selection", 2, team_select, SETTINGS::settings.espteamcolourselection);
						if (SETTINGS::settings.espteamcolourselection == 0)
						{
							TGUI_ColorPicker("enemy box colour", SETTINGS::settings.box_col.RGBA[0], SETTINGS::settings.box_col.RGBA[1], SETTINGS::settings.box_col.RGBA[2], SETTINGS::settings.box_col.RGBA[3], false);
							TGUI_ColorPicker("enemy name colour", SETTINGS::settings.name_col.RGBA[0], SETTINGS::settings.name_col.RGBA[1], SETTINGS::settings.name_col.RGBA[2], SETTINGS::settings.name_col.RGBA[3], false);
							TGUI_ColorPicker("enemy weapon colour", SETTINGS::settings.weapon_col.RGBA[0], SETTINGS::settings.weapon_col.RGBA[1], SETTINGS::settings.weapon_col.RGBA[2], SETTINGS::settings.weapon_col.RGBA[3], false);
							TGUI_ColorPicker("enemy fov arrows colour", SETTINGS::settings.fov_col.RGBA[0], SETTINGS::settings.fov_col.RGBA[1], SETTINGS::settings.fov_col.RGBA[2], SETTINGS::settings.fov_col.RGBA[3], false);
						}	
						else if (SETTINGS::settings.espteamcolourselection == 1)
						{
							TGUI_ColorPicker("team box colour", SETTINGS::settings.boxteam_col.RGBA[0], SETTINGS::settings.boxteam_col.RGBA[1], SETTINGS::settings.boxteam_col.RGBA[2], SETTINGS::settings.boxteam_col.RGBA[3], false);
							TGUI_ColorPicker("team name colour", SETTINGS::settings.nameteam_col.RGBA[0], SETTINGS::settings.nameteam_col.RGBA[1], SETTINGS::settings.nameteam_col.RGBA[2], SETTINGS::settings.nameteam_col.RGBA[3], false);
							TGUI_ColorPicker("team weapon colour", SETTINGS::settings.weaponteam_col.RGBA[0], SETTINGS::settings.weaponteam_col.RGBA[1], SETTINGS::settings.weaponteam_col.RGBA[2], SETTINGS::settings.weaponteam_col.RGBA[3], false);
							TGUI_ColorPicker("team fov arrows colour", SETTINGS::settings.arrowteam_col.RGBA[0], SETTINGS::settings.arrowteam_col.RGBA[1], SETTINGS::settings.arrowteam_col.RGBA[2], SETTINGS::settings.arrowteam_col.RGBA[3], false);
						}
						TGUI_ColorPicker("grenade prediction colour", SETTINGS::settings.grenadepredline_col.RGBA[0], SETTINGS::settings.grenadepredline_col.RGBA[1], SETTINGS::settings.grenadepredline_col.RGBA[2], SETTINGS::settings.grenadepredline_col.RGBA[3], false);
					}
					TGUI_EndGroupbox();

					TGUI_BeginGroupbox("chams colours", AutoCalc(6));
					{

						TGUI_ColorPicker("enemy vis. colour", SETTINGS::settings.vmodel_col.RGBA[0], SETTINGS::settings.vmodel_col.RGBA[1], SETTINGS::settings.vmodel_col.RGBA[2], SETTINGS::settings.vmodel_col.RGBA[3], false);
						TGUI_ColorPicker("enemy invis. colour", SETTINGS::settings.imodel_col.RGBA[0], SETTINGS::settings.imodel_col.RGBA[1], SETTINGS::settings.imodel_col.RGBA[2], SETTINGS::settings.imodel_col.RGBA[3], false);
						TGUI_ColorPicker("enemy backtrack colour", SETTINGS::settings.btvis_col.RGBA[0], SETTINGS::settings.btvis_col.RGBA[1], SETTINGS::settings.btvis_col.RGBA[2], SETTINGS::settings.btvis_col.RGBA[3], false);

						TGUI_ColorPicker("team vis. colour", SETTINGS::settings.teamvis_color.RGBA[0], SETTINGS::settings.teamvis_color.RGBA[1], SETTINGS::settings.teamvis_color.RGBA[2], SETTINGS::settings.teamvis_color.RGBA[3], false);
						TGUI_ColorPicker("team invis. colour", SETTINGS::settings.teaminvis_color.RGBA[0], SETTINGS::settings.teaminvis_color.RGBA[1], SETTINGS::settings.teaminvis_color.RGBA[2], SETTINGS::settings.teaminvis_color.RGBA[3], false);

						TGUI_ColorPicker("local colour", SETTINGS::settings.localchams_col.RGBA[0], SETTINGS::settings.localchams_col.RGBA[1], SETTINGS::settings.localchams_col.RGBA[2], SETTINGS::settings.localchams_col.RGBA[3], false);

					}
					TGUI_EndGroupbox();

					TGUI_BeginGroupbox("glow colours", AutoCalc(3));
					{

						TGUI_ColorPicker("glow enemy colour", SETTINGS::settings.glow_col.RGBA[0], SETTINGS::settings.glow_col.RGBA[1], SETTINGS::settings.glow_col.RGBA[2], SETTINGS::settings.glow_col.RGBA[3], false);
						TGUI_ColorPicker("glow team colour", SETTINGS::settings.teamglow_color.RGBA[0], SETTINGS::settings.teamglow_color.RGBA[1], SETTINGS::settings.teamglow_color.RGBA[2], SETTINGS::settings.teamglow_color.RGBA[3], false);
						TGUI_ColorPicker("glow local colour", SETTINGS::settings.glowlocal_col.RGBA[0], SETTINGS::settings.glowlocal_col.RGBA[1], SETTINGS::settings.glowlocal_col.RGBA[2], SETTINGS::settings.glowlocal_col.RGBA[3], false);

					}
					TGUI_EndGroupbox();

					TGUI_SetColumn(2);
					TGUI_BeginGroupbox("bullet tracer colours", AutoCalc(3));
					{

						TGUI_ColorPicker("local player", SETTINGS::settings.bulletlocal_col.RGBA[0], SETTINGS::settings.bulletlocal_col.RGBA[1], SETTINGS::settings.bulletlocal_col.RGBA[2], SETTINGS::settings.bulletlocal_col.RGBA[3], false);
						TGUI_ColorPicker("enemy player", SETTINGS::settings.bulletenemy_col.RGBA[0], SETTINGS::settings.bulletenemy_col.RGBA[1], SETTINGS::settings.bulletenemy_col.RGBA[2], SETTINGS::settings.bulletenemy_col.RGBA[3], false);
						TGUI_ColorPicker("team player", SETTINGS::settings.bulletteam_col.RGBA[0], SETTINGS::settings.bulletteam_col.RGBA[1], SETTINGS::settings.bulletteam_col.RGBA[2], SETTINGS::settings.bulletteam_col.RGBA[3], false);

					}
					TGUI_EndGroupbox();

					TGUI_BeginGroupbox("menu options", AutoCalc(4));
					{

						TGUI_ColorPicker("menu colour", SETTINGS::settings.menu_col.RGBA[0], SETTINGS::settings.menu_col.RGBA[1], SETTINGS::settings.menu_col.RGBA[2], SETTINGS::settings.menu_col.RGBA[3], false);
						TGUI_ColorPicker("checkbox colour", SETTINGS::settings.checkbox_col.RGBA[0], SETTINGS::settings.checkbox_col.RGBA[1], SETTINGS::settings.checkbox_col.RGBA[2], SETTINGS::settings.checkbox_col.RGBA[3], false);
						TGUI_ColorPicker("slider colour", SETTINGS::settings.slider_col.RGBA[0], SETTINGS::settings.slider_col.RGBA[1], SETTINGS::settings.slider_col.RGBA[2], SETTINGS::settings.slider_col.RGBA[3], false);
						TGUI_ColorPicker("tab colour", SETTINGS::settings.tab_col.RGBA[0], SETTINGS::settings.tab_col.RGBA[1], SETTINGS::settings.tab_col.RGBA[2], SETTINGS::settings.tab_col.RGBA[3], false);

					}
					TGUI_EndGroupbox();

					TGUI_BeginGroupbox("colors settings", AutoCalc(1));
					{
						if (TGUI_Button("reset colors"))
						{
							SETTINGS::settings.box_col = CColor(205, 0, 0);
							SETTINGS::settings.name_col = CColor(255, 255, 255);
							SETTINGS::settings.weapon_col = CColor(255, 255, 255);
							SETTINGS::settings.fov_col = CColor(205, 0, 0);

							SETTINGS::settings.boxteam_col = CColor(0, 0, 205);
							SETTINGS::settings.nameteam_col = CColor(255, 255, 255);
							SETTINGS::settings.weaponteam_col = CColor(255, 255, 255);
							SETTINGS::settings.arrowteam_col = CColor(0, 0, 205);
							//
							//
							SETTINGS::settings.vmodel_col = CColor(205, 55, 0);
							SETTINGS::settings.imodel_col = CColor(205, 0, 0);

							SETTINGS::settings.teamvis_color = CColor(0, 55, 205);
							SETTINGS::settings.teaminvis_color = CColor(0, 0, 205);

							SETTINGS::settings.localchams_col = CColor(0, 205, 0);
							//
							//
							SETTINGS::settings.glow_col = CColor(205, 0, 0);
							SETTINGS::settings.teamglow_color = CColor(0, 0, 205);
							SETTINGS::settings.glowlocal_col = CColor(0, 205, 0);
							//
							//
							SETTINGS::settings.bulletlocal_col = CColor(0, 205, 0);
							SETTINGS::settings.bulletenemy_col = CColor(205, 0, 0);
							SETTINGS::settings.bulletteam_col = CColor(0, 0, 205);
							//
							//
							SETTINGS::settings.menu_col = CColor(60, 60, 60);
							SETTINGS::settings.checkbox_col = CColor(175, 200, 5);
							SETTINGS::settings.slider_col = CColor(175, 200, 5);
							SETTINGS::settings.tab_col = CColor(91, 91, 91);
						}
						TGUI_SameLine();
						if (TGUI_Button("random colors"))
						{
							SETTINGS::settings.box_col = RANDOMIZECOL;
							SETTINGS::settings.name_col = RANDOMIZECOL;
							SETTINGS::settings.weapon_col = RANDOMIZECOL;
							SETTINGS::settings.fov_col = RANDOMIZECOL;

							SETTINGS::settings.boxteam_col = RANDOMIZECOL;
							SETTINGS::settings.nameteam_col = RANDOMIZECOL;
							SETTINGS::settings.weaponteam_col = RANDOMIZECOL;
							SETTINGS::settings.arrowteam_col = RANDOMIZECOL;
							//
							//
							SETTINGS::settings.vmodel_col = RANDOMIZECOL;
							SETTINGS::settings.imodel_col = RANDOMIZECOL;

							SETTINGS::settings.teamvis_color = RANDOMIZECOL;
							SETTINGS::settings.teaminvis_color = RANDOMIZECOL;

							SETTINGS::settings.localchams_col = RANDOMIZECOL;
							//
							//
							SETTINGS::settings.glow_col = RANDOMIZECOL;
							SETTINGS::settings.teamglow_color = RANDOMIZECOL;
							SETTINGS::settings.glowlocal_col = RANDOMIZECOL;
							//
							//
							SETTINGS::settings.bulletlocal_col = RANDOMIZECOL;
							SETTINGS::settings.bulletenemy_col = RANDOMIZECOL;
							SETTINGS::settings.bulletteam_col = RANDOMIZECOL;
							//
							//
							SETTINGS::settings.menu_col = RANDOMIZECOL;
							SETTINGS::settings.checkbox_col = RANDOMIZECOL;
							SETTINGS::settings.slider_col = RANDOMIZECOL;
							SETTINGS::settings.tab_col = RANDOMIZECOL;
						}
					}
					TGUI_EndGroupbox();

					TGUI_BeginGroupbox("configuration", 90);
					{
						switch (TGUI_Combobox("config", 7, configs, SETTINGS::settings.config_sel))
						{
						case 0: config = "default"; break;
						case 1: config = "legit"; break;
						case 2: config = "auto_hvh"; break;
						case 3: config = "scout_hvh"; break;
						case 4: config = "pistol_hvh"; break;
						case 5: config = "awp_hvh"; break;
						case 6: config = "nospread_hvh"; break;
						}

						if (TGUI_Button("Load Config"))
						{
							InGameLogger::Log log;

							log.color_line.PushBack("[ULTRATAP.WIN] ", LIGHTBLUE);
							log.color_line.PushBack("Configuration loaded.", CColor(200, 200, 200));

							in_game_logger.AddLog(log);

							//INTERFACES::cvar->ConsoleColorPrintf(CColor(175, 200, 5), "[BXDYAIM.WIN] ");
							//GLOBAL::Msg("Configuration loaded.     \n");

							SETTINGS::settings.Load(config);
						}
						TGUI_SameLine();
						if (TGUI_Button("Save Config"))
						{
							InGameLogger::Log log;

							log.color_line.PushBack("[ULTRATAP.WIN] ", LIGHTBLUE);
							log.color_line.PushBack("Configuration saved.", CColor(200, 200, 200));

							in_game_logger.AddLog(log);

							//INTERFACES::cvar->ConsoleColorPrintf(CColor(175, 200, 5), "[BXDYAIM.WIN] ");
							//GLOBAL::Msg("Configuration saved.     \n");

							SETTINGS::settings.Save(config);
						}
					}
					TGUI_EndGroupbox();
				} break;
					/* Colours */
				}
			}
			TGUI_EndWindow();

			TGUI_EndFrame();
		}
	}
}