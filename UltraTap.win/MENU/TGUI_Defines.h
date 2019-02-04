#pragma once

#include "TGUI_Helpers.h"

namespace TGUI
{
	/// For defining how stuff looks
	enum TGUI_Color_Names
	{
		TGUI_COLOR_WINDOW_BODY,
		TGUI_COLOR_WINDOW_BODY_OUTLINE,
		TGUI_COLOR_WINDOW_TITLE_BAR,
		TGUI_COLOR_WINDOW_TITLE_BAR_TEXT,
		TGUI_COLOR_WINDOW_TITLE_BAR_OUTLINE,
		TGUI_COLOR_WINDOW_TITLE_BAR_BACKGROUND_TEXT,

		TGUI_COLOR_CHECKBOX_UNSELECTED_BODY,
		TGUI_COLOR_CHECKBOX_UNSELECTED_BODY_OUTLINE,
		TGUI_COLOR_CHECKBOX_SELECTED_BODY,
		TGUI_COLOR_CHECKBOX_SELECTED_BODY_OUTLINE,
		TGUI_COLOR_CHECKBOX_TEXT,
		TGUI_COLOR_CHECKBOX_HOVERED_TEXT,

		TGUI_COLOR_BUTTON_BODY,
		TGUI_COLOR_BUTTON_BODY_CLICKED,
		TGUI_COLOR_BUTTON_TEXT_CLICKED,
		TGUI_COLOR_BUTTON_OUTLINE,
		TGUI_COLOR_BUTTON_TEXT,

		TGUI_COLOR_GROUPBOX_BODY,
		TGUI_COLOR_GROUPBOX_BODY_HOVERED,
		TGUI_COLOR_GROUPBOX_OUTLINE,
		TGUI_COLOR_GROUPBOX_TEXT,
		TGUI_COLOR_GROUPBOX_TEXT_HOVERED,

		TGUI_COLOR_SLIDER_BODY,
		TGUI_COLOR_SLIDER_BODY_SELECTED,
		TGUI_COLOR_SLIDER_OUTLINE,
		TGUI_COLOR_SLIDER_TEXT_NAME,
		TGUI_COLOR_SLIDER_TEXT_NAME_HOVERED,
		TGUI_COLOR_SLIDER_VALUE_TEXT,

		TGUI_COLOR_SEPARATOR_BODY,
		TGUI_COLOR_SEPARATOR_TEXT,

		TGUI_COLOR_KEYBIND_BODY,
		TGUI_COLOR_KEYBIND_BODY_SELECTED,
		TGUI_COLOR_KEYBIND_OUTLINE,
		TGUI_COLOR_KEYBIND_TEXT_NAME,
		TGUI_COLOR_KEYBIND_TEXT_NAME_HOVERED,
		TGUI_COLOR_KEYBIND_TEXT_VALUE,

		TGUI_COLOR_TEXT_INPUT_BODY,
		TGUI_COLOR_TEXT_INPUT_BODY_SELECTED,
		TGUI_COLOR_TEXT_INPUT_OUTLINE,
		TGUI_COLOR_TEXT_INPUT_TEXT_VALUE,
		TGUI_COLOR_TEXT_INPUT_TEXT_NAME,
		TGUI_COLOR_TEXT_INPUT_TEXT_HOVERED,

		TGUI_COLOR_COMBOBOX_BODY_SELECTED,
		TGUI_COLOR_COMBOBOX_OUTLINE_SELECTED,
		TGUI_COLOR_COMBOBOX_OUTLINE_ITEM,
		TGUI_COLOR_COMBOBOX_BODY_ITEM,
		TGUI_COLOR_COMBOBOX_TEXT_ITEM,
		TGUI_COLOR_COMBOBOX_SELECTED_TEXT_ITEM,
		TGUI_COLOR_COMBOBOX_TEXT_SELECTED,
		TGUI_COLOR_COMBOBOX_TEXT_SELECTED_HOVERED,
		TGUI_COLOR_COMBOBOX_TEXT_NAME,
		TGUI_COLOR_COMBOBOX_TEXT_NAME_HOVERED,

		TGUI_COLOR_COLOR_PICKER_BODY_COLOR,
		TGUI_COLOR_COLOR_PICKER_OUTLINE,
		TGUI_COLOR_COLOR_PICKER_TEXT,
		TGUI_COLOR_COLOR_PICKER_TEXT_HOVERED,

		TGUI_COLOR_TAB_BODY,
		TGUI_COLOR_TAB_BODY_SELECTED,
		TGUI_COLOR_TAB_TEXT,
		TGUI_COLOR_TAB_TEXT_SELECTED,
		TGUI_COLOR_TAB_OUTLINE,

		TGUI_COLOR_TOOL_TIP_BODY,
		TGUI_COLOR_TOOL_TIP_OUTLINE,
		TGUI_COLOR_TOOL_TIP_TEXT,

		TGUI_COLOR_NUM
	};
	extern HELPERS::TGUI_Color tgui_colors[TGUI_Color_Names::TGUI_COLOR_NUM];
	enum TGUI_Attribute_Names
	{
		TGUI_ATTRIBUTE_PADDING_WIDTH,
		TGUI_ATTRIBUTE_PADDING_HEIGHT,
		TGUI_ATTRIBUTE_SCROLL_SPEED,

		TGUI_ATTRIBUTE_WINDOW_TITLE_BAR_HEIGHT,

		TGUI_ATTRIBUTE_CHECKBOX_WIDTH,
		TGUI_ATTRIBUTE_CHECKBOX_HEIGHT,

		TGUI_ATTRIBUTE_BUTTON_WIDTH,
		TGUI_ATTRIBUTE_BUTTON_HEIGHT,
		TGUI_ATTRIBUTE_BUTTON_CLICK_DURATION,

		TGUI_ATTRIBUTE_SLIDER_WIDTH,
		TGUI_ATTRIBUTE_SLIDER_HEIGHT,

		TGUI_ATTRIBUTE_KEYBIND_WIDTH,
		TGUI_ATTRIBUTE_KEYBIND_HEIGHT,

		TGUI_ATTRIBUTE_TEXT_INPUT_WIDTH,
		TGUI_ATTRIBUTE_TEXT_INPUT_HEIGHT,

		TGUI_ATTRIBUTE_COMBOBOX_WIDTH,
		TGUI_ATTRIBUTE_COMBOBOX_HEIGHT,
		TGUI_ATTRIBUTE_COMBOBOX_MAX_ITEMS_DISPLAYED,

		TGUI_ATTRIBUTE_COLOR_PICKER_WIDTH,
		TGUI_ATTRIBUTE_COLOR_PICKER_HEIGHT,
		TGUI_ATTRIBUTE_COLOR_PICKER_OPEN_WIDTH,
		TGUI_ATTRIBUTE_COLOR_PICKER_OPEN_HEIGHT,
		TGUI_ATTRIBUTE_COLOR_PICKER_SLIDER_WIDTH,
		TGUI_ATTRIBUTE_COLOR_PICKER_PADDING,
		TGUI_ATTRIBUTE_COLOR_PICKER_PIXELATION,

		TGUI_ATTRIBUTE_TAB_HEIGHT,
		TGUI_ATTRIBUTE_TAB_ITEM_PADDING,

		TGUI_ATTRIBUTE_TOOL_TIP_LINE_HEIGHT,

		TGUI_ATTRIBUTE_NUM
	};
	extern float tgui_attributes[TGUI_Attribute_Names::TGUI_ATTRIBUTE_NUM];
	enum TGUI_Font_Names
	{
		TGUI_FONT_WINDOW_NAME,
		TGUI_FONT_WINDOW_NAME_BEHIND, /// Drawn behind TGUI_FONT_WINDOW_NAME

		TGUI_FONT_GROUPBOX,

		TGUI_FONT_SEPARATOR,

		TGUI_FONT_CHECKBOX,

		TGUI_FONT_BUTTON,

		TGUI_FONT_SLIDER_NAME,
		TGUI_FONT_SLIDER_VALUE,

		TGUI_FONT_KEYBIND_NAME,
		TGUI_FONT_KEYBIND_VALUE,

		TGUI_FONT_TEXT_INPUT_NAME,
		TGUI_FONT_TEXT_INPUT_VALUE,

		TGUI_FONT_COMBOBOX_NAME,
		TGUI_FONT_COMBOBOX_SELECTED_ITEM,
		TGUI_FONT_COMBOBOX_ITEM_NAME,

		TGUI_FONT_COLOR_PICKER,

		TGUI_FONT_TAB_SELECTED_ITEM_NAME,
		TGUI_FONT_TAB_ITEM_NAME,

		TGUI_FONT_TOOL_TIP,

		TGUI_FONT_NUM
	};
	extern unsigned int tgui_fonts[TGUI_Font_Names::TGUI_FONT_NUM];

	/// Random shit
	constexpr unsigned int TGUI_MAX_STRING_LENGTH = 256;

	/// All command types
	enum TGUI_Command_Types
	{
		TGUI_COMMAND_TYPE_WINDOW,
		TGUI_COMMAND_TYPE_CHECKBOX,
		TGUI_COMMAND_TYPE_BUTTON,
		TGUI_COMMAND_TYPE_GROUPBOX,
		TGUI_COMMAND_TYPE_SLIDER,
		TGUI_COMMAND_TYPE_SEPARATOR,
		TGUI_COMMAND_TYPE_KEYBIND,
		TGUI_COMMAND_TYPE_TEXT_INPUT,
		TGUI_COMMAND_TYPE_COMBOBOX,
		TGUI_COMMAND_TYPE_MULTISELECT_COMBOBOX,
		TGUI_COMMAND_TYPE_COLOR_PICKER,
		TGUI_COMMAND_TYPE_TAB,
		TGUI_COMMAND_TYPE_TOOL_TIP
	};

	/// TGUI_Command_Types to C string (not sure why i added this)
	inline const char* TGUI_TranslateCommandType(const TGUI_Command_Types &type)
	{
		switch (type)
		{
		case TGUI_Command_Types::TGUI_COMMAND_TYPE_WINDOW:
			return "Window";
		case TGUI_Command_Types::TGUI_COMMAND_TYPE_CHECKBOX:
			return "Checkbox";
		case TGUI_Command_Types::TGUI_COMMAND_TYPE_BUTTON:
			return "Button";
		case TGUI_Command_Types::TGUI_COMMAND_TYPE_GROUPBOX:
			return "Groupbox";
		case TGUI_Command_Types::TGUI_COMMAND_TYPE_SLIDER:
			return "Slider";
		case TGUI_Command_Types::TGUI_COMMAND_TYPE_SEPARATOR:
			return "Separator";
		case TGUI_Command_Types::TGUI_COMMAND_TYPE_KEYBIND:
			return "Key Bind";
		case TGUI_Command_Types::TGUI_COMMAND_TYPE_TEXT_INPUT:
			return "Text Input";
		case TGUI_Command_Types::TGUI_COMMAND_TYPE_COMBOBOX:
			return "Combobox";
		case TGUI_Command_Types::TGUI_COMMAND_TYPE_MULTISELECT_COMBOBOX:
			return "Multiselect Combobox";
		case TGUI_Command_Types::TGUI_COMMAND_TYPE_COLOR_PICKER:
			return "Color Picker";
		case TGUI_Command_Types::TGUI_COMMAND_TYPE_TAB:
			return "Tab";
		case TGUI_Command_Types::TGUI_COMMAND_TYPE_TOOL_TIP:
			return "Tool Tip";
		default:
			return "";
		}
	}

	/// Command flags
	constexpr unsigned char TGUI_COMMAND_FLAG_SCROLL_ENABLED = 0b00000001;
	constexpr unsigned char TGUI_COMMAND_FLAG_IS_HOVERED = 0b00000010;
	constexpr unsigned char TGUI_COMMAND_FLAG_IS_ATTACHED = 0b00000100;
	constexpr unsigned char TGUI_COMMAND_FLAG_IS_DISABLED = 0b00001000;

	/// All commands have this basic info
	struct TGUI_Command_Info
	{
		TGUI_Command_Info() {};

		char name[TGUI_MAX_STRING_LENGTH];
		char filtered_name[TGUI_MAX_STRING_LENGTH];
		char frame_name[TGUI_MAX_STRING_LENGTH];

		int column;
		float alpha_modulation;
		unsigned char flags;

		TGUI_Command_Types command_type;

		HELPERS::TGUI_Vector2D position, size, space; /// space is what's used when called with AllocSpace()

		HELPERS::TGUI_Vector2D bounds_min, bounds_max;
		HELPERS::TGUI_Vector2D original_bounds_min, original_bounds_max;
		HELPERS::TGUI_Vector2D entire_bounds_min, entire_bounds_max;
	};

	/// How input is handled in TGUI (horribly implemented btw)
	class TGUI_Input
	{
	public:
		struct TGUI_Mouse_Input
		{
			HELPERS::TGUI_Vector2D position;
			bool right_button = false, left_button = false;
			int scroll = 0;
		};

	public:
		TGUI_Mouse_Input previous_mouse;
		TGUI_Mouse_Input current_mouse;
		TGUI_Mouse_Input real_previous_mouse; /// ONLY TO BE MODIFIED IN UPDATE()
		TGUI_Mouse_Input real_current_mouse;  /// ONLY TO BE MODIFIED IN UPDATE()

		int previous_keyboard[256];
		int current_keyboard[256];

		float last_update_time = 0.f;
		float update_delta_time = 0.f;

		float last_click_time = 0.f;
		float click_time_delta = 0.f; /// Time between the last 2 clicks

	public:
		void Update();

		/// Wrapper functions for checking whether clicked or holding the mouse buttons
		bool DidLeftClick() const { return current_mouse.left_button && !previous_mouse.left_button; }
		bool DidLeftHold() const { return current_mouse.left_button && previous_mouse.left_button; }
		bool DidRightClick() const { return current_mouse.right_button && !previous_mouse.right_button; }
		bool DidRightHold() const { return current_mouse.right_button && previous_mouse.right_button; }

		bool DidRealLeftClick() const { return real_current_mouse.left_button && !real_previous_mouse.left_button; }
		bool DidRealLeftHold() const { return real_current_mouse.left_button && real_previous_mouse.left_button; }
		bool DidRealRightClick() const { return real_current_mouse.right_button && !real_previous_mouse.right_button; }
		bool DidRealRightHold() const { return real_current_mouse.right_button && real_previous_mouse.right_button; }

		/// Keyboard functions
		bool DidClickKey(int key) const { return current_keyboard[key] && !previous_keyboard[key]; }
		static bool IsKeyNumber(int key)
		{
			switch (key)
			{
			case 0x30:
			case 0x31:
			case 0x32:
			case 0x33:
			case 0x34:
			case 0x35:
			case 0x36:
			case 0x37:
			case 0x38:
			case 0x39:
			case VK_OEM_PERIOD:
			case VK_OEM_MINUS:
				return true;
			default:
				return false;
			}
		}
		static bool IsKey(int key)
		{
			return key > 0 && key < 256;
		}
		static bool IsValidKey(int key)
		{
			if ((key >= 0x30 && key <= 0x39) ||
				(key >= 0x41 && key <= 0x5A) ||
				(key >= VK_LEFT && key <= VK_DOWN) ||
				(key >= VK_MBUTTON && key <= VK_XBUTTON2) ||
				(key >= VK_NUMPAD0 && key <= VK_NUMPAD9) ||
				(key >= VK_F1 && key <= VK_F24) ||
				(key >= VK_OEM_1 && key <= VK_OEM_3) ||
				(key >= VK_OEM_4 && key <= VK_OEM_7) ||
				(key == VK_CONTROL) || (key == VK_SHIFT) ||
				(key == VK_TAB) || (key == VK_CAPITAL) || (key == VK_SPACE))
				return true;

			return false;
		}

		/// Key to text functions
		static char KeyToChar(int key)
		{
			unsigned char keyboard[256];
			GetKeyboardState(keyboard);

			char buffer[2];
			if (ToAscii(key, MapVirtualKey(key, MAPVK_VK_TO_VSC), keyboard, (LPWORD)buffer, 0) == 1)
				return buffer[0];

			const bool upper_case = keyboard[VK_SHIFT];

			switch (key)
			{
			case VK_OEM_1:
				return upper_case ? ':' : ';';
			case VK_OEM_PLUS:
				return upper_case ? '+' : '=';
			case VK_OEM_COMMA:
				return upper_case ? '\"' : ',';
			case VK_OEM_MINUS:
				return upper_case ? '_' : '-';
			case VK_OEM_PERIOD:
				return upper_case ? '>' : '.';
			case VK_OEM_2:
				return upper_case ? '?' : '/';
			case VK_OEM_3:
				return upper_case ? '~' : '`';
			case VK_OEM_4:
				return upper_case ? '{' : '[';
			case VK_OEM_5:
				return upper_case ? '|' : '\\';
			case VK_OEM_6:
				return upper_case ? '}' : ']';
			case VK_OEM_7:
				return upper_case ? '\"' : '\'';
			default:
				return '\0';
			}
		}
		static const char* GetKeyName(int key)
		{
			/// hhhhh
			switch (key)
			{
			case VK_MBUTTON:
				return "Mouse 3";
			case VK_XBUTTON1:
				return "Mouse 4";
			case VK_XBUTTON2:
				return "Mouse 5";
			case VK_CONTROL:
				return "Control";
			case VK_SHIFT:
				return "Shift";
			case VK_TAB:
				return "Tab";
			case VK_LEFT:
				return "Left Arrow";
			case VK_RIGHT:
				return "Right Arrow";
			case VK_UP:
				return "Up Arrow";
			case VK_DOWN:
				return "Down Arrow";
			case VK_CAPITAL:
				return "Caps Lock";
			case VK_NUMPAD0:
				return "Numpad 0";
			case VK_NUMPAD1:
				return "Numpad 1";
			case VK_NUMPAD2:
				return "Numpad 2";
			case VK_NUMPAD3:
				return "Numpad 3";
			case VK_NUMPAD4:
				return "Numpad 4";
			case VK_NUMPAD5:
				return "Numpad 5";
			case VK_NUMPAD6:
				return "Numpad 6";
			case VK_NUMPAD7:
				return "Numpad 7";
			case VK_NUMPAD8:
				return "Numpad 8";
			case VK_NUMPAD9:
				return "Numpad 9";
			case VK_F1:
				return "F1";
			case VK_F2:
				return "F2";
			case VK_F3:
				return "F3";
			case VK_F4:
				return "F4";
			case VK_F5:
				return "F5";
			case VK_F6:
				return "F6";
			case VK_F7:
				return "F7";
			case VK_F8:
				return "F8";
			case VK_F9:
				return "F9";
			case VK_F10:
				return "F10";
			case VK_F11:
				return "F11";
			case VK_F12:
				return "F12";
			case VK_F13:
				return "F13";
			case VK_F14:
				return "F14";
			case VK_F15:
				return "F15";
			case VK_F16:
				return "F16";
			case VK_F17:
				return "F17";
			case VK_F18:
				return "F18";
			case VK_F19:
				return "F19";
			case VK_F20:
				return "F20";
			case VK_F21:
				return "F21";
			case VK_F22:
				return "F22";
			case VK_F23:
				return "F23";
			case VK_F24:
				return "F24";
			case 'A':
				return "A";
			case 'B':
				return "B";
			case 'C':
				return "C";
			case 'D':
				return "D";
			case 'E':
				return "E";
			case 'F':
				return "F";
			case 'G':
				return "G";
			case 'H':
				return "H";
			case 'I':
				return "I";
			case 'J':
				return "J";
			case 'K':
				return "K";
			case 'L':
				return "L";
			case 'M':
				return "M";
			case 'N':
				return "N";
			case 'O':
				return "O";
			case 'P':
				return "P";
			case 'Q':
				return "Q";
			case 'R':
				return "R";
			case 'S':
				return "S";
			case 'T':
				return "T";
			case 'U':
				return "U";
			case 'V':
				return "V";
			case 'W':
				return "W";
			case 'X':
				return "X";
			case 'Y':
				return "Y";
			case 'Z':
				return "Z";
			case 0x30:
				return "0";
			case 0x31:
				return "1";
			case 0x32:
				return "2";
			case 0x33:
				return "3";
			case 0x34:
				return "4";
			case 0x35:
				return "5";
			case 0x36:
				return "6";
			case 0x37:
				return "7";
			case 0x38:
				return "8";
			case 0x39:
				return "9";
			default:
				return "";
			}
		}

		/// Get the change in mouse position
		HELPERS::TGUI_Vector2D GetMousePositionDelta() const { return current_mouse.position - previous_mouse.position; }
	};

	/// Holds unique data for each command, stored in an std::unordered_map
	class TGUI_Command_Data_Base
	{
	public:
		struct TGUI_Window_Command_Data
		{
			bool is_held_on_title_bar;
			int scroll_amount;
			HELPERS::TGUI_Vector2D title_bar_drag_delta;
		};
		struct TGUI_Checkbox_Command_Data
		{

		};
		struct TGUI_Button_Command_Data
		{
			bool was_clicked;
			float time_clicked;
		};
		struct TGUI_Groupbox_Command_Data
		{
			int scroll_amount;
			float last_time_not_hovered;
		};
		struct TGUI_Slider_Command_Data
		{
			bool is_hovered_over_slider, is_hovered_over_name, is_typing, is_sliding;
			char typed_message[32];
		};
		struct TGUI_Keybind_Command_Data
		{
			bool is_typing;
		};
		struct TGUI_Text_Input_Command_Data
		{
			bool is_typing;
		};
		struct TGUI_Combobox_Command_Data
		{
			bool is_typing, is_combobox_open;
			int scroll_amount, times_pressed_enter;
			char current_searched_text[TGUI_MAX_STRING_LENGTH];
		};
		struct TGUI_Multiselect_Combobox_Command_Data
		{
			bool is_typing, is_combobox_open;
			int scroll_amount, times_pressed_enter;
			char current_searched_text[TGUI_MAX_STRING_LENGTH];
		};
		struct TGUI_Color_Picker_Command_Data
		{
			bool is_color_picker_open;
		};
		struct TGUI_Tool_Tip_Command_Data
		{
			float time_hovered;
		};

	private:
		union TGUI_Command_Data
		{
			TGUI_Command_Data() {};

			TGUI_Window_Command_Data m_window_data;
			TGUI_Checkbox_Command_Data m_checkbox_data;
			TGUI_Button_Command_Data m_button_data;
			TGUI_Groupbox_Command_Data m_groupbox_data;
			TGUI_Slider_Command_Data m_slider_data;
			TGUI_Keybind_Command_Data m_keybind_data;
			TGUI_Text_Input_Command_Data m_text_input_data;
			TGUI_Combobox_Command_Data m_combobox_data;
			TGUI_Multiselect_Combobox_Command_Data m_multiselect_combobox_data;
			TGUI_Color_Picker_Command_Data m_color_picker_data;
			TGUI_Tool_Tip_Command_Data m_tool_tip_data;
		};

	private:
		/// Still waiting for template lambdas >:(
		template <class T>
		T &ReturnCorrectData(TGUI_Command_Data &data, const TGUI_Command_Types &type)
		{
			/// Return the appropriate data (gotta cast it to T& or else it wont compile)
			switch (type)
			{
			case TGUI_Command_Types::TGUI_COMMAND_TYPE_WINDOW:
				return reinterpret_cast<T&>(data.m_window_data);
			case TGUI_Command_Types::TGUI_COMMAND_TYPE_CHECKBOX:
				return reinterpret_cast<T&>(data.m_checkbox_data);
			case TGUI_Command_Types::TGUI_COMMAND_TYPE_BUTTON:
				return reinterpret_cast<T&>(data.m_button_data);
			case TGUI_Command_Types::TGUI_COMMAND_TYPE_GROUPBOX:
				return reinterpret_cast<T&>(data.m_groupbox_data);
			case TGUI_Command_Types::TGUI_COMMAND_TYPE_SLIDER:
				return reinterpret_cast<T&>(data.m_slider_data);
			case TGUI_Command_Types::TGUI_COMMAND_TYPE_KEYBIND:
				return reinterpret_cast<T&>(data.m_keybind_data);
			case TGUI_Command_Types::TGUI_COMMAND_TYPE_TEXT_INPUT:
				return reinterpret_cast<T&>(data.m_text_input_data);
			case TGUI_Command_Types::TGUI_COMMAND_TYPE_COMBOBOX:
				return reinterpret_cast<T&>(data.m_combobox_data);
			case TGUI_Command_Types::TGUI_COMMAND_TYPE_MULTISELECT_COMBOBOX:
				return reinterpret_cast<T&>(data.m_multiselect_combobox_data);
			case TGUI_Command_Types::TGUI_COMMAND_TYPE_COLOR_PICKER:
				return reinterpret_cast<T&>(data.m_color_picker_data);
			case TGUI_Command_Types::TGUI_COMMAND_TYPE_TOOL_TIP:
				return reinterpret_cast<T&>(data.m_tool_tip_data);
			default:
				throw std::exception("Unhandled command data type");
			}
		}

	public:
		/// Get the data corresponding to a command's type
		template <class T>
		T &Get(const uint32_t &hash, const TGUI_Command_Types &type)
		{
			auto &data = m_command_data.find(hash);
			if (data != m_command_data.end()) /// If the data has already been initialized for this hash
				return ReturnCorrectData<T>(data->second, type);

			/// Initialized the data differently depending on the command type, since you can only initialize one member in a union
			auto InitializeData = [](TGUI_Command_Data &dt, const TGUI_Command_Types &tp) -> void
			{
				switch (tp)
				{
				case TGUI_Command_Types::TGUI_COMMAND_TYPE_WINDOW:
					dt.m_window_data.is_held_on_title_bar = false;
					dt.m_window_data.scroll_amount = 0;
					dt.m_window_data.title_bar_drag_delta = HELPERS::TGUI_Vector2D(0, 0);
					break;
				case TGUI_Command_Types::TGUI_COMMAND_TYPE_CHECKBOX:
					break;
				case TGUI_Command_Types::TGUI_COMMAND_TYPE_BUTTON:
					dt.m_button_data.was_clicked = false;
					dt.m_button_data.time_clicked = 0.f;
					break;
				case TGUI_Command_Types::TGUI_COMMAND_TYPE_GROUPBOX:
					dt.m_groupbox_data.scroll_amount = 0;
					dt.m_groupbox_data.last_time_not_hovered = 0;
					break;
				case TGUI_Command_Types::TGUI_COMMAND_TYPE_SLIDER:
					dt.m_slider_data.is_hovered_over_slider = false;
					dt.m_slider_data.is_hovered_over_name = false;
					dt.m_slider_data.is_typing = false;
					dt.m_slider_data.is_sliding = false;
					dt.m_slider_data.typed_message[0] = '\0';
					break;
				case TGUI_Command_Types::TGUI_COMMAND_TYPE_KEYBIND:
					dt.m_keybind_data.is_typing = false;
					break;
				case TGUI_Command_Types::TGUI_COMMAND_TYPE_TEXT_INPUT:
					dt.m_text_input_data.is_typing = false;
					break;
				case TGUI_Command_Types::TGUI_COMMAND_TYPE_COMBOBOX:
					dt.m_combobox_data.is_typing = false;
					dt.m_combobox_data.is_combobox_open = false;
					dt.m_combobox_data.scroll_amount = 0;
					dt.m_combobox_data.times_pressed_enter = 0;
					dt.m_combobox_data.current_searched_text[0] = '\0';
					break;
				case TGUI_Command_Types::TGUI_COMMAND_TYPE_MULTISELECT_COMBOBOX:
					dt.m_multiselect_combobox_data.is_typing = false;
					dt.m_multiselect_combobox_data.is_combobox_open = false;
					dt.m_multiselect_combobox_data.scroll_amount = 0;
					dt.m_multiselect_combobox_data.times_pressed_enter = 0;
					dt.m_multiselect_combobox_data.current_searched_text[0] = '\0';
					break;
				case TGUI_Command_Types::TGUI_COMMAND_TYPE_COLOR_PICKER:
					dt.m_color_picker_data.is_color_picker_open = false;
					break;
				case TGUI_Command_Types::TGUI_COMMAND_TYPE_TOOL_TIP:
					dt.m_tool_tip_data.time_hovered = 0.f;
					break;
				default:
					throw std::exception("Unhandled command data type");
				}
			};

			m_command_data[hash] = TGUI_Command_Data();
			InitializeData(m_command_data[hash], type);

			return ReturnCorrectData<T>(m_command_data[hash], type);
		}

	private:
		std::unordered_map<uint32_t, TGUI_Command_Data> m_command_data;

	}; extern TGUI_Command_Data_Base TGUI_command_data_base;
}