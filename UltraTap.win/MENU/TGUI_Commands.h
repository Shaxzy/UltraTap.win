#pragma once

#include "TGUI_Defines.h"

namespace TGUI
{
	/// Base class that all commands inherit from
	class TGUI_Command
	{
	public:
		virtual ~TGUI_Command() {};

		virtual void Draw() {};
		virtual void TestInput(TGUI_Input &input) {};

		void SetupCommandInfo(TGUI_Command_Info& info, HELPERS::TGUI_Vector2D position, HELPERS::TGUI_Vector2D size, const char* name, bool scroll_enabled);
		uint32_t GetDataHash();

		/// Scale from 0 to 1
		float GetScrollAlphaScale()
		{
			if (!(m_command_info.flags & TGUI_COMMAND_FLAG_SCROLL_ENABLED))
				return 1.f;

			constexpr float alpha_fade_pixel_distance = 5.f;
			return HELPERS::CLAMP(HELPERS::MIN<float>(
				(m_command_info.position.y - m_command_info.bounds_min.y) / alpha_fade_pixel_distance,
				(m_command_info.bounds_max.y - (m_command_info.position.y + m_command_info.size.y)) /
				alpha_fade_pixel_distance), 0.25f, 1.f);
		}
		float GetFadeAlphaScale()
		{
			return 0.f;
		}

		/// Returns false if nothing is in bounds
		bool GetClampedPositionAndSize(HELPERS::TGUI_Vector2D &position, HELPERS::TGUI_Vector2D &size)
		{
			const HELPERS::TGUI_Vector2D pixel(1, 1);
			const bool is_scroll_enabled = m_command_info.flags & TGUI_COMMAND_FLAG_SCROLL_ENABLED;

			if (m_command_info.column == -1)
			{
				if (!HELPERS::TGUI_Vector2D::ClampToBounds(position, size, m_command_info.entire_bounds_min + pixel, m_command_info.entire_bounds_max - pixel))
					return false;
				else
					return true;
			}
			else
			{
				if (is_scroll_enabled && !HELPERS::TGUI_Vector2D::ClampToBounds(position, size, m_command_info.bounds_min + pixel, m_command_info.bounds_max - pixel))
					return false;
				if (!is_scroll_enabled && !HELPERS::TGUI_Vector2D::ClampToBounds(position, size, m_command_info.original_bounds_min + pixel, m_command_info.original_bounds_max - pixel))
					return false;
			}

			return true;
		}

		/// Returns whether the rectangle is ENTIRELY in bounds
		static bool IsRectInBounds(TGUI_Command* command, HELPERS::TGUI_Vector2D position, HELPERS::TGUI_Vector2D size)
		{
			const auto command_info = command->GetCommandInfo();

			const bool is_scroll_enabled = command_info.flags & TGUI_COMMAND_FLAG_SCROLL_ENABLED;

			if (command_info.column == -1)
			{
				return position.IsPointInBounds(command_info.entire_bounds_min, command_info.entire_bounds_max) &&
					(position + size).IsPointInBounds(command_info.entire_bounds_min, command_info.entire_bounds_max);
			}
			else
			{
				if (is_scroll_enabled)
					return position.IsPointInBounds(command_info.bounds_min, command_info.bounds_max) &&
					(position + size).IsPointInBounds(command_info.bounds_min, command_info.bounds_max);
				else
					return position.IsPointInBounds(command_info.original_bounds_min, command_info.original_bounds_max) &&
					(position + size).IsPointInBounds(command_info.original_bounds_min, command_info.original_bounds_max);
			}
		}
		bool IsRectInBounds(HELPERS::TGUI_Vector2D position, HELPERS::TGUI_Vector2D size)
		{
			return IsRectInBounds(this, position, size);
		}

		/// Returns whether the point is in bounds
		bool IsPointInBounds(HELPERS::TGUI_Vector2D position)
		{
			const bool is_scroll_enabled = m_command_info.flags & TGUI_COMMAND_FLAG_SCROLL_ENABLED;

			if (m_command_info.column == -1)
				return position.IsPointInBounds(m_command_info.entire_bounds_min, m_command_info.entire_bounds_max);
			else
			{
				if (is_scroll_enabled)
					return position.IsPointInBounds(m_command_info.bounds_min, m_command_info.bounds_max);
				else
					return position.IsPointInBounds(m_command_info.original_bounds_min, m_command_info.original_bounds_max);
			}
		}

		TGUI_Command_Info& GetCommandInfo() { return m_command_info; }

		virtual int GetScrollAmount() { return 0; };

	private:
		TGUI_Command_Info m_command_info;
	};

	class TGUI_Window_Command : public TGUI_Command
	{
	public:
		TGUI_Window_Command(const char* name, HELPERS::TGUI_Vector2D position, HELPERS::TGUI_Vector2D size);

		void Draw();
		void TestInput(TGUI_Input &input);

		int GetScrollAmount();
	};

	class TGUI_Checkbox_Command : public TGUI_Command
	{
	public:
		TGUI_Checkbox_Command(const char* name, bool &variable, HELPERS::TGUI_Vector2D position, HELPERS::TGUI_Vector2D size, bool scroll_enabled);

		void Draw();
		void TestInput(TGUI_Input &input);

	private:
		bool *m_variable_ptr = nullptr;
	};

	class TGUI_Button_Command : public TGUI_Command
	{
	public:
		TGUI_Button_Command(const char* name, HELPERS::TGUI_Vector2D position, HELPERS::TGUI_Vector2D size, bool scroll_enabled);

		void Draw();
		void TestInput(TGUI_Input &input);
	};

	class TGUI_Groupbox_Command : public TGUI_Command
	{
	public:
		TGUI_Groupbox_Command(const char* name, HELPERS::TGUI_Vector2D position, HELPERS::TGUI_Vector2D size, bool scroll_enabled);

		void Draw();
		void TestInput(TGUI_Input &input);

		int GetScrollAmount();

	private:
	};

	class TGUI_Slider_Command : public TGUI_Command
	{
	public:
		TGUI_Slider_Command(const char* name, const char* suffix, float min, float max, float &variable, int decimal_precision, bool scroll_enabled, HELPERS::TGUI_Vector2D position, HELPERS::TGUI_Vector2D size);

		void Draw();
		void TestInput(TGUI_Input &input);

	private:
		char m_suffix[16];
		float *m_variable_ptr = nullptr;
		float m_min, m_max;
		int m_decimal_precision;
	};

	class TGUI_Separator_Command : public TGUI_Command
	{
	public:
		TGUI_Separator_Command(const char* name, HELPERS::TGUI_Vector2D position, HELPERS::TGUI_Vector2D size, bool scroll_enabled);

		void Draw();
	};

	class TGUI_Keybind_Command : public TGUI_Command
	{
	public:
		TGUI_Keybind_Command(const char* name, int &variable, HELPERS::TGUI_Vector2D position, HELPERS::TGUI_Vector2D size, bool scroll_enabled);

		void Draw();
		void TestInput(TGUI_Input &input);

	private:
		int *m_variable_ptr = nullptr;
	};

	class TGUI_Text_Input_Command : public TGUI_Command
	{
	public:
		TGUI_Text_Input_Command(const char* name, HELPERS::TGUI_Vector2D position, HELPERS::TGUI_Vector2D size, char* variable, bool scroll_enabled);

		void Draw();
		void TestInput(TGUI_Input &input);

	private:
		char* m_variable_ptr = nullptr;
	};

	class TGUI_Combobox_Command : public TGUI_Command
	{
	public:
		TGUI_Combobox_Command(const char* name, HELPERS::TGUI_Vector2D position, HELPERS::TGUI_Vector2D size, char(*items)[TGUI_MAX_STRING_LENGTH], int items_amount, int &variable, bool scroll_enabled);

		void Draw();
		void TestInput(TGUI_Input &input);

	private:
		int* m_variable_ptr = nullptr;
		char(*m_items)[TGUI_MAX_STRING_LENGTH];
		int m_items_amount;
	};

	class TGUI_Multiselect_Combobox_Command : public TGUI_Command
	{
	public:
		TGUI_Multiselect_Combobox_Command(const char* name, HELPERS::TGUI_Vector2D position, HELPERS::TGUI_Vector2D size, char(*items)[TGUI_MAX_STRING_LENGTH], bool* selected_items, int items_amount, bool scroll_enabled);

		void Draw();
		void TestInput(TGUI_Input& input);

	private:
		bool *m_selected_items = nullptr;
		char(*m_items)[TGUI_MAX_STRING_LENGTH];
		int m_items_amount;
	};

	class TGUI_Color_Picker_Command : public TGUI_Command
	{
	public:
		TGUI_Color_Picker_Command(const char* name, HELPERS::TGUI_Vector2D position, HELPERS::TGUI_Vector2D size, unsigned char &R, unsigned char &G, unsigned char &B, unsigned char &A, bool scroll_enabled);

		void Draw();
		void TestInput(TGUI_Input &input);

	private:
		unsigned char* m_red_ptr = nullptr, *m_green_ptr = nullptr,
			*m_blue_ptr = nullptr, *m_alpha_ptr = nullptr;
	};

	class TGUI_Tab_Command : public TGUI_Command
	{
	public:
		TGUI_Tab_Command(HELPERS::TGUI_Vector2D position, HELPERS::TGUI_Vector2D size, int &variable, char(*tabs)[TGUI_MAX_STRING_LENGTH], int tabs_amount, bool scroll_enabled);

		void Draw();
		void TestInput(TGUI_Input &input);

	private:
		int m_tabs_amount;
		int *m_variable_ptr = nullptr;
		char(*m_tabs)[TGUI_MAX_STRING_LENGTH];
	};

	class TGUI_Tool_Tip_Command : public TGUI_Command
	{
	public:
		TGUI_Tool_Tip_Command(const char* description, int lines_amount, HELPERS::TGUI_Vector2D offset, TGUI_Command* parent_command);

		void Draw();
		void TestInput(TGUI_Input &input);

	private:
		TGUI_Command * m_parent_command = nullptr;
		int m_lines_amount = 0;
	};
}