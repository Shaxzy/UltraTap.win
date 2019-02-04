#pragma once

#include "TGUI_Commands.h"

/*

Windows, Checkboxes, Groupboxes, Buttons, Comboboxes, Tabs, Keybinds, Text Input, Color Pickers, Sliders, Seperators

*/

namespace TGUI
{
	/// Name can be a bit confusing, windows, groupboxes and etc are TGUI_Frames, they hold stuff
	class TGUI_Command_Frame
	{
	public:
		TGUI_Command_Frame(TGUI_Command* command, int columns);

		/// TODO: Fix bug when a command has scroll and the other doesn't on the same line
		bool AllocateSpace(HELPERS::TGUI_Vector2D size, HELPERS::TGUI_Vector2D &position, bool &scroll_enabled, bool same_line);
		void GetFrameBounds(HELPERS::TGUI_Vector2D &min, HELPERS::TGUI_Vector2D &max, int column) const
		{
			const auto &clmn = m_columns[HELPERS::MIN<int>(column, m_columns.size() - 1)];
			min = clmn.m_bounds_min, max = clmn.m_bounds_max;
		}
		void GetOriginalFrameBounds(HELPERS::TGUI_Vector2D &min, HELPERS::TGUI_Vector2D &max, int column) const
		{
			const auto &clmn = m_columns[HELPERS::MIN<int>(column, m_columns.size() - 1)];
			min = clmn.m_original_bounds_min, max = clmn.m_original_bounds_max;
		}
		void GetEntireFrameBounds(HELPERS::TGUI_Vector2D &min, HELPERS::TGUI_Vector2D &max) const
		{
			const auto command_info = m_command->GetCommandInfo();

			/// Need to add the title bar height as padding for when the Command_Frame is a window
			auto title_bar_size = HELPERS::TGUI_Vector2D(0, 0);
			if (command_info.command_type == TGUI_Command_Types::TGUI_COMMAND_TYPE_WINDOW)
				title_bar_size = HELPERS::TGUI_Vector2D(0, tgui_attributes[TGUI_Attribute_Names::TGUI_ATTRIBUTE_WINDOW_TITLE_BAR_HEIGHT]);

			min = command_info.position + title_bar_size, max = command_info.position + command_info.size;
		}
		void GetParentFrameBounds(HELPERS::TGUI_Vector2D &min, HELPERS::TGUI_Vector2D &max) const
		{
			min = m_parent_bounds_min, max = m_parent_bounds_max;
		}

		HELPERS::TGUI_Vector2D GetFrameSize()
		{
			const auto &command_info = m_command->GetCommandInfo();
			const int size_per_column = command_info.size.x / m_columns.size();

			return HELPERS::TGUI_Vector2D(size_per_column, command_info.size.y);
		}
		HELPERS::TGUI_Vector2D GetEntireFrameSize()
		{
			return m_parent_bounds_max - m_parent_bounds_min;
		}

		TGUI_Command* GetCommand() { return m_command; }

	private:
		struct TGUI_Column
		{
			HELPERS::TGUI_Vector2D m_frame_min, m_frame_max;
			HELPERS::TGUI_Vector2D m_last_position, m_last_size;
			HELPERS::TGUI_Vector2D m_bounds_min, m_bounds_max;
			HELPERS::TGUI_Vector2D m_original_bounds_min, m_original_bounds_max;
			bool m_did_scroll;
		};

	private:
		TGUI_Command * m_command;
		std::vector<TGUI_Column> m_columns;
		HELPERS::TGUI_Vector2D m_parent_bounds_min, m_parent_bounds_max;
	};

	/// Takes care of holding commands and deallocating them 
	class TGUI_Command_List
	{
	public:
		~TGUI_Command_List() { Clear(); }

		void Insert(TGUI_Command* command) { m_commands.insert(m_commands.begin(), command); }
		void Pushback(TGUI_Command* command) { m_commands.push_back(command); }

		void Clear() { DeleteCommands(); m_commands.clear(); }

		std::vector<TGUI_Command*> Get() { return m_commands; }

	private:
		std::vector<TGUI_Command*> m_commands;

	private:
		void DeleteCommands()
		{
			for (const auto &command : m_commands)
				delete command;
		}
	};

	/// Main menu class O_O
	class TGUI_Framework
	{
	public:
		void BeginFrame();
		void EndFrame();

		bool AllocateSpace(HELPERS::TGUI_Vector2D size, HELPERS::TGUI_Vector2D &position, bool &scroll_enabled);

		/// Adding commands
		void AddCommand(TGUI_Command* command, bool ignore_last_command = false)
		{
			m_command_list.Insert(command);

			if (!ignore_last_command)
				m_last_command = command;
		}
		void AddCommandToEnd(TGUI_Command* command, bool ignore_last_command = false)
		{
			m_end_commands.insert(m_end_commands.begin(), command);

			if (!ignore_last_command)
				m_last_command = command;
		}

		/// Command frame stuff
		void AddCommandFrame(TGUI_Command_Frame &command_frame) { m_command_frames.insert(m_command_frames.begin(), command_frame); }
		void RemoveCommandFrame()
		{
			const bool is_window = m_command_frames.front().GetCommand()->GetCommandInfo().command_type == TGUI_Command_Types::TGUI_COMMAND_TYPE_WINDOW;

			m_command_frames.erase(m_command_frames.begin());

			/// Add end commands when a window stack frame is removed so that it doesn't end up with weird bugs such as
			/// comboboxes showing through windows and not clipping properly 
			if (is_window)
			{
				for (const auto &command : m_end_commands)
					m_command_list.Insert(command);

				m_end_commands.clear();
			}
		}
		int GetNumCommandFrames() { return m_command_frames.size(); }
		TGUI_Command_Frame& GetCommandFrame();

		/// Get the last command added, nullptr if a command hasn't been added yet
		TGUI_Command* GetLastCommand() { return m_last_command; }

		TGUI_Input GetInput() { return m_input; }

		/// Get the current frame size
		HELPERS::TGUI_Vector2D GetCurrentFrameSize()
		{
			if (m_column == -1)
				return GetCommandFrame().GetEntireFrameSize();
			else
				return GetCommandFrame().GetFrameSize();
		}

		/// Same line functionality
		void SetSameLine(bool value) { m_should_same_line = value; }
		bool GetSameLine() { return m_should_same_line; }

		void SetShouldAttachToLastCommand(bool value) { m_should_attach_to_last_command = value; }
		bool ShouldAttachToLastCommand() { return m_should_attach_to_last_command; }

		void SetWasLastCommandAttached(bool value) { m_was_last_command_attached = value; }
		bool WasLastCommandAttached() { return m_was_last_command_attached; }

		void SetAlphaModulation(float value) { m_alpha_modulation = value; }
		float GetAlphaModulation() { return m_alpha_modulation; }

		void SetShouldDisableNextCommand(bool value) { m_should_disable_next_command = value; }
		bool ShouldDisableNextCommand() { return m_should_disable_next_command; }

		void SetColumn(int column) { m_column = column; }
		int GetColumn() { return m_column; }

		void SetDrawMouse(bool value) { m_should_draw_mouse = value; }

	private:
		std::vector<TGUI_Command_Frame> m_command_frames;
		std::vector<TGUI_Command*> m_end_commands;
		TGUI_Command* m_last_command = nullptr;
		TGUI_Command_List m_command_list;
		TGUI_Input m_input;

		int m_column = 0;
		float m_alpha_modulation = 1.f;

		bool m_should_same_line = false;
		bool m_should_attach_to_last_command = false;
		bool m_was_last_command_attached = false;
		bool m_should_disable_next_command = false;
		bool m_should_draw_mouse = false;

		void DrawMouse();

	}; extern TGUI_Framework menu;


	/// Wrappers for class methods
	inline void TGUI_BeginFrame()
	{
		menu.BeginFrame();
	}
	inline void TGUI_EndFrame()
	{
		menu.EndFrame();
	}

	/// Next widget is on the same line as the last one
	inline void TGUI_SameLine()
	{
		menu.SetSameLine(true);
	}

	/// Alpha modulates the next command called, accepts a value from 0 to 1
	inline void TGUI_AlphaModulate(float value)
	{
		HELPERS::TGUI_Assert(value >= 0.f && value <= 1.f);

		menu.SetAlphaModulation(value);
	}

	/// Doesn't accept input from the next command called but still draws it on the screen
	inline void TGUI_Disable()
	{
		menu.SetShouldDisableNextCommand(true);
	}

	/// Basically the same thing as SameLine() except it's right aligned
	inline void TGUI_AttachToPreviousWidget()
	{
		menu.SetShouldAttachToLastCommand(true);
	}

	/// TGUI_SetColumn(0) makes it so the next commands ignore columns and shit... useful for tabs...
	inline void TGUI_SetColumn(int column)
	{
		menu.SetColumn(column - 1);
	}

	/// Draws a mouse, always going to be drawn on top of everything else no matter the order its called
	inline void TGUI_DrawMouse()
	{
		menu.SetDrawMouse(true);
	}

	/// Invisible padding, good for formatting shit
	inline void TGUI_Padding(int width, int height, bool scroll_enabled = true)
	{
		HELPERS::TGUI_Vector2D position, size(width, height);
		menu.AllocateSpace(size, position, scroll_enabled);
	}



	/// Windows
	void TGUI_BeginWindow(const char* name, HELPERS::TGUI_Vector2D position, HELPERS::TGUI_Vector2D size, int columns = 1);
	void TGUI_EndWindow();

	/// Checkboxes
	bool TGUI_Checkbox(const char* name, bool &variable, bool scroll_enabled = true);

	/// Buttons
	bool TGUI_Button(const char* name, bool scroll_enabled = true);

	/// Groupboxes
	void TGUI_BeginGroupbox(const char* name, int height, int columns = 1, bool scroll_enabled = true);
	void TGUI_EndGroupbox();

	/// Sliders
	float TGUI_Slider(const char* name, const char* suffix, float min, float max, float &variable, int decimal_precision = 1, bool scroll_enabled = true);

	/// Separators
	void TGUI_Separator(const char* name, bool scroll_enabled = true);

	/// Keybinds
	int TGUI_Keybind(const char* name, int &variable, bool scroll_enabled = true);

	/// Text Input
	const char* TGUI_TextInput(const char* name, char* variable, bool scroll_enabled = true);

	/// Comboboxes, accepts an array of c strings, items may not be deleted until TGUI_EndFrame() is called again
	int TGUI_Combobox(const char* name, int items_amount, char(*items)[TGUI_MAX_STRING_LENGTH], int &variable, bool scroll_enabled = true);

	/// Multiselect comboboxes
	void TGUI_MultiselectCombobox(const char* name, int items_amount, char(*items)[TGUI_MAX_STRING_LENGTH], bool* selected_items, bool scroll_enabled = true);

	/// Color pickers
	void TGUI_ColorPicker(const char* name, unsigned char &R, unsigned char &G, unsigned char &B, unsigned char &A, bool scroll_enabled = true);

	/// Tabs
	int TGUI_Tab(int tabs_amount, char(*tabs)[TGUI_MAX_STRING_LENGTH], int &variable, bool scroll_enabled = true);

	/// Tool tips
	void TGUI_ToolTip(const char* description, int lines_amount = 1, HELPERS::TGUI_Vector2D offset = HELPERS::TGUI_Vector2D(20, 20));



	/// Function that demonstrates features
	void TGUI_Demo();
}