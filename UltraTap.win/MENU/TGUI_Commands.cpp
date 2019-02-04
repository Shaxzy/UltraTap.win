#include "../includes.h"

#include "TGUI.h" /// dumb as shit include but need to use menu for getting the frame and shit

#include "TGUI_Commands.h"

#include <sstream>
#include <iomanip>

namespace TGUI
{
	/// Command base that all commands inherit from
	void TGUI_Command::SetupCommandInfo(TGUI_Command_Info& info, HELPERS::TGUI_Vector2D position, HELPERS::TGUI_Vector2D size, const char* name, bool scroll_enabled)
	{
		info.flags = 0;
		if (scroll_enabled)
			info.flags |= TGUI_COMMAND_FLAG_SCROLL_ENABLED;
		if (menu.WasLastCommandAttached())
			info.flags |= TGUI_COMMAND_FLAG_IS_ATTACHED;
		if (menu.ShouldDisableNextCommand())
			info.flags |= TGUI_COMMAND_FLAG_IS_DISABLED;

		if (menu.GetNumCommandFrames())
		{
			menu.GetCommandFrame().GetFrameBounds(info.bounds_min, info.bounds_max, menu.GetColumn());
			menu.GetCommandFrame().GetOriginalFrameBounds(info.original_bounds_min, info.original_bounds_max, menu.GetColumn());
			menu.GetCommandFrame().GetEntireFrameBounds(info.entire_bounds_min, info.entire_bounds_max);
		}

		info.alpha_modulation = menu.GetAlphaModulation();
		info.column = menu.GetColumn();
		info.position = position;
		info.size = size;

		strcpy(info.name, name);
		strcpy(info.filtered_name, name);

		/// Filter the text 
		HELPERS::TGUI_FilterText(info.filtered_name);

		/// reset menu shit
		menu.SetAlphaModulation(1.f);
		menu.SetShouldDisableNextCommand(false);
	}
	uint32_t TGUI_Command::GetDataHash()
	{
		const auto command_type = TGUI_TranslateCommandType(m_command_info.command_type);
		return HELPERS::TGUI_FNVHash(m_command_info.name, strlen(m_command_info.name)) +
			HELPERS::TGUI_FNVHash(command_type, strlen(command_type));
	}

	/// Windows
	TGUI_Window_Command::TGUI_Window_Command(const char* name, HELPERS::TGUI_Vector2D position, HELPERS::TGUI_Vector2D size)
	{
		auto &command_info = GetCommandInfo();
		command_info.command_type = TGUI_Command_Types::TGUI_COMMAND_TYPE_WINDOW;

		SetupCommandInfo(command_info, position, size, name, false);

		auto &command_data = TGUI_command_data_base.Get<TGUI_Command_Data_Base::TGUI_Window_Command_Data>(GetDataHash(), command_info.command_type);
		command_info.position += command_data.title_bar_drag_delta;
	}
	void TGUI_Window_Command::Draw()
	{
		const auto command_info = GetCommandInfo();

		auto title_position = command_info.position;
		auto title_size = HELPERS::TGUI_Vector2D(command_info.size.x, tgui_attributes[TGUI_ATTRIBUTE_WINDOW_TITLE_BAR_HEIGHT]);

		auto body_position = title_position + HELPERS::TGUI_Vector2D(0, title_size.y);
		auto body_size = command_info.size - HELPERS::TGUI_Vector2D(0, title_size.y);

		const unsigned int name_font = tgui_fonts[TGUI_FONT_WINDOW_NAME];
		const unsigned int background_font = tgui_fonts[TGUI_FONT_WINDOW_NAME_BEHIND];

		/// Draw the window body
		{
			HELPERS::TGUI_DrawRectangle(body_position, body_size, tgui_colors[TGUI_COLOR_WINDOW_BODY]);

			constexpr int num_circles = 15;
			constexpr int max_radius = 4;

			/// Choose random numbers only once at initialization
			static float random_num[num_circles];
			static bool is_init = false;
			if (!is_init)
			{
				is_init = true;

				for (auto &num : random_num)
					num = MATH::RandomNumber(0.f, 1.f);
			}

			/// Draw the circles
			for (int i = 0; i < num_circles; i++)
			{
				/// Kinda retarded looking but it adds variation between windows
				const float rand_num = fmod(random_num[i] + (command_info.position.x / static_cast<float>(HELPERS::TGUI_GetScreenSize().x)), 1.f);

				const int width = (rand_num * body_size.x) + (HELPERS::TGUI_GetTime() * (50.f + (rand_num * 100.f))),
					height = (rand_num * body_size.y) + (HELPERS::TGUI_GetTime() * (50.f + (rand_num * 100.f)));

				const int radius = rand_num * max_radius;
				const int alpha = fabs(sin((HELPERS::TGUI_GetTime()) + (rand_num * 255.f))) * 255.f;
				const auto circle_color = HELPERS::TGUI_Color(200, 200, 200, alpha);
				const auto position = HELPERS::TGUI_Vector2D(body_position.x + (width % (body_size.x - radius)),
					body_position.y + (height % (body_size.y - radius)));

				/// Draw the circle
				HELPERS::TGUI_DrawCircle(position, circle_color, radius, 50, true);
			}

			HELPERS::TGUI_DrawRectangle(body_position, body_size, tgui_colors[TGUI_COLOR_WINDOW_BODY_OUTLINE], true);
		}

		HELPERS::TGUI_DrawRectangle(title_position, title_size, tgui_colors[TGUI_COLOR_WINDOW_TITLE_BAR]);
		HELPERS::TGUI_DrawRectangle(title_position, title_size, tgui_colors[TGUI_COLOR_WINDOW_TITLE_BAR_OUTLINE], true);

		const auto text_position = title_position + (title_size * 0.5f);
		if (background_font != 0)
			HELPERS::TGUI_DrawText(text_position, background_font, true, true, tgui_colors[TGUI_COLOR_WINDOW_TITLE_BAR_BACKGROUND_TEXT], command_info.filtered_name);

		HELPERS::TGUI_DrawText(text_position, name_font, true, true, tgui_colors[TGUI_COLOR_WINDOW_TITLE_BAR_TEXT], command_info.filtered_name);
	}
	void TGUI_Window_Command::TestInput(TGUI_Input &input)
	{
		auto &command_info = GetCommandInfo();
		auto &command_data = TGUI_command_data_base.Get<TGUI_Command_Data_Base::TGUI_Window_Command_Data>(GetDataHash(), command_info.command_type);

		if (command_info.flags & TGUI_COMMAND_FLAG_IS_DISABLED)
			return;

		const auto title_position = command_info.position;
		const auto title_size = HELPERS::TGUI_Vector2D(command_info.size.x, tgui_attributes[TGUI_ATTRIBUTE_WINDOW_TITLE_BAR_HEIGHT]);

		const bool is_mouse_in_title_bar = input.current_mouse.position.IsPointInBounds(title_position, title_position + title_size);

		/// Whether clicked on the title bar or not
		if (is_mouse_in_title_bar && input.DidLeftClick())
			command_data.is_held_on_title_bar = true;
		else if (!input.DidLeftHold()) /// If stopped holding down, set is_held_on_title_bar to false
			command_data.is_held_on_title_bar = false;

		/// If dragging the title bar
		if (command_data.is_held_on_title_bar)
			command_data.title_bar_drag_delta += input.GetMousePositionDelta();

		/// Is mouse in window? (exclude title bar)
		const bool is_mouse_in_window = input.current_mouse.position.IsPointInBounds(
			command_info.position + HELPERS::TGUI_Vector2D(0, title_size.y),
			command_info.position + command_info.size);

		/// Only scroll when mouse is in window area
		if (is_mouse_in_window)
		{
			const int scroll_speed = tgui_attributes[TGUI_ATTRIBUTE_SCROLL_SPEED];
			command_data.scroll_amount = HELPERS::MIN<int>(0, command_data.scroll_amount + (input.current_mouse.scroll * scroll_speed));
			input.current_mouse.scroll = 0; /// So no other commands process scroll
		}

		/// So that you don't click through windows into another window
		if (is_mouse_in_window || is_mouse_in_title_bar)
			input.current_mouse.left_button = false;
	}
	int TGUI_Window_Command::GetScrollAmount()
	{
		return TGUI_command_data_base.Get<TGUI_Command_Data_Base::TGUI_Window_Command_Data>
			(GetDataHash(), TGUI_Command_Types::TGUI_COMMAND_TYPE_WINDOW).scroll_amount;
	}

	/// Checkboxes
	TGUI_Checkbox_Command::TGUI_Checkbox_Command(const char* name, bool &variable, HELPERS::TGUI_Vector2D position, HELPERS::TGUI_Vector2D size, bool scroll_enabled)
	{
		auto &command_info = GetCommandInfo();

		command_info.command_type = TGUI_Command_Types::TGUI_COMMAND_TYPE_CHECKBOX;
		m_variable_ptr = &variable;

		SetupCommandInfo(command_info, position, size, name, scroll_enabled);
	}
	void TGUI_Checkbox_Command::Draw()
	{
		const auto &command_info = GetCommandInfo();
		const auto &command_data = TGUI_command_data_base.Get<TGUI_Command_Data_Base::TGUI_Checkbox_Command_Data>(GetDataHash(), command_info.command_type);

		const bool is_hovered = command_info.flags & TGUI_COMMAND_FLAG_IS_HOVERED;
		const bool is_selected = *m_variable_ptr;
		const bool is_scroll_enabled = command_info.flags & TGUI_COMMAND_FLAG_SCROLL_ENABLED;

		auto position = command_info.position;
		auto size = command_info.size;

		/// Scale from 0 to 1
		const float alpha_scale = GetScrollAlphaScale() * command_info.alpha_modulation;

		/// Only draw text if the min and max of the checkbox is in bounds
		const bool is_checkbox_fully_in = (position.IsPointInBounds(command_info.bounds_min, command_info.bounds_max) &&
			(position + size).IsPointInBounds(command_info.bounds_min, command_info.bounds_max));

		/// Dont draw if not in bounds
		if (!GetClampedPositionAndSize(position, size))
			return;

		auto body_color = tgui_colors[TGUI_COLOR_CHECKBOX_UNSELECTED_BODY], outline_color = tgui_colors[TGUI_COLOR_CHECKBOX_UNSELECTED_BODY_OUTLINE];

		/// Alpha modulate
		body_color.RGBA[3] *= alpha_scale;
		outline_color.RGBA[3] *= alpha_scale;

		/// Draw the body
		HELPERS::TGUI_DrawRectangle(position, size, body_color);
		HELPERS::TGUI_DrawRectangle(position, size, outline_color, true);

		/// Draw the checkbox body and outline
		if (is_selected)
		{
			body_color = tgui_colors[TGUI_COLOR_CHECKBOX_SELECTED_BODY], outline_color = tgui_colors[TGUI_COLOR_CHECKBOX_SELECTED_BODY_OUTLINE];

			/// Alpha modulate
			body_color.RGBA[3] *= alpha_scale;
			outline_color.RGBA[3] *= alpha_scale;

			HELPERS::TGUI_DrawRectangle(position + HELPERS::TGUI_Vector2D(1, 1), size - HELPERS::TGUI_Vector2D(2, 2), body_color);
			HELPERS::TGUI_DrawRectangle(position + HELPERS::TGUI_Vector2D(1, 1), size - HELPERS::TGUI_Vector2D(2, 2), outline_color, true);
		}

		if (is_checkbox_fully_in || !is_scroll_enabled)
		{
			auto text_color = is_hovered ? tgui_colors[TGUI_COLOR_CHECKBOX_HOVERED_TEXT] : tgui_colors[TGUI_COLOR_CHECKBOX_TEXT];
			text_color.RGBA[3] *= alpha_scale;

			const unsigned int font = tgui_fonts[TGUI_FONT_CHECKBOX];

			/// Draw the name, changing color and add an offset if hovered over
			if (is_hovered)
				HELPERS::TGUI_DrawText(position + HELPERS::TGUI_Vector2D(size.x + 6, size.y * 0.5f + 1), font, false, true, text_color, command_info.filtered_name);
			else
				HELPERS::TGUI_DrawText(position + HELPERS::TGUI_Vector2D(size.x + 5, size.y * 0.5f), font, false, true, text_color, command_info.filtered_name);
		}
	}
	void TGUI_Checkbox_Command::TestInput(TGUI_Input &input)
	{
		auto &command_info = GetCommandInfo();
		auto &command_data = TGUI_command_data_base.Get<TGUI_Command_Data_Base::TGUI_Checkbox_Command_Data>(GetDataHash(), command_info.command_type);

		if (command_info.flags & TGUI_COMMAND_FLAG_IS_DISABLED)
			return;

		const bool is_mouse_in_bounds = IsPointInBounds(input.current_mouse.position);

		const auto position = command_info.position;
		const auto size = command_info.size;

		const unsigned int font = tgui_fonts[TGUI_FONT_CHECKBOX];

		const auto text_size = HELPERS::TGUI_GetTextSize(font, command_info.filtered_name);

		/// If hovered over the checkbox or the text
		if (input.current_mouse.position.IsPointInBounds(position, position + HELPERS::TGUI_Vector2D(5 + text_size.x + size.x, HELPERS::MAX(size.y, text_size.y))))
			command_info.flags |= TGUI_COMMAND_FLAG_IS_HOVERED;

		/// If clicked on the checkbox
		if ((command_info.flags & TGUI_COMMAND_FLAG_IS_HOVERED) && input.DidLeftClick() && is_mouse_in_bounds)
			*m_variable_ptr = !(*m_variable_ptr);
	}

	/// Buttons
	TGUI_Button_Command::TGUI_Button_Command(const char* name, HELPERS::TGUI_Vector2D position, HELPERS::TGUI_Vector2D size, bool scroll_enabled)
	{
		auto &command_info = GetCommandInfo();

		command_info.command_type = TGUI_Command_Types::TGUI_COMMAND_TYPE_BUTTON;

		SetupCommandInfo(command_info, position, size, name, scroll_enabled);
	}
	void TGUI_Button_Command::Draw()
	{
		const auto command_info = GetCommandInfo();
		const auto command_data = TGUI_command_data_base.Get<TGUI_Command_Data_Base::TGUI_Button_Command_Data>(GetDataHash(), command_info.command_type);

		const bool is_hovered = command_info.flags & TGUI_COMMAND_FLAG_IS_HOVERED;
		const bool is_scroll_enabled = command_info.flags & TGUI_COMMAND_FLAG_SCROLL_ENABLED;

		auto position = command_info.position;
		auto size = command_info.size;

		const float time_since_clicked = fabs(HELPERS::TGUI_GetTime() - command_data.time_clicked);
		const float click_duration_attr = tgui_attributes[TGUI_ATTRIBUTE_BUTTON_CLICK_DURATION];

		/// Clamp position
		if (!GetClampedPositionAndSize(position, size))
			return;

		/// Scale from 0 to 1
		const float alpha_scale = GetScrollAlphaScale() * command_info.alpha_modulation;

		auto body_color = tgui_colors[TGUI_COLOR_BUTTON_BODY];
		auto body_outline_color = tgui_colors[TGUI_COLOR_BUTTON_OUTLINE];

		if (time_since_clicked < click_duration_attr)
			body_color = tgui_colors[TGUI_COLOR_BUTTON_BODY_CLICKED];

		body_color.RGBA[3] *= alpha_scale;
		body_outline_color.RGBA[3] *= alpha_scale;

		if (time_since_clicked < click_duration_attr)
		{
			HELPERS::TGUI_DrawRectangle(position + HELPERS::TGUI_Vector2D(1, 1), size, body_color);
			HELPERS::TGUI_DrawRectangle(position + HELPERS::TGUI_Vector2D(1, 1), size, body_outline_color, true);
		}
		else
		{
			HELPERS::TGUI_DrawRectangle(position, size, body_color);
			HELPERS::TGUI_DrawRectangle(position, size, body_outline_color, true);
		}

		if (IsRectInBounds(command_info.position, command_info.size))
		{
			auto text_color = time_since_clicked < click_duration_attr ? tgui_colors[TGUI_COLOR_BUTTON_TEXT_CLICKED] : tgui_colors[TGUI_COLOR_BUTTON_TEXT];
			text_color.RGBA[3] *= alpha_scale;

			const unsigned int font = tgui_fonts[TGUI_FONT_BUTTON];

			if (time_since_clicked < click_duration_attr || is_hovered)
				HELPERS::TGUI_DrawText(position + HELPERS::TGUI_Vector2D(size.x * 0.5f + 1, size.y * 0.5f + 1), font, true, true, text_color, command_info.filtered_name);
			else
				HELPERS::TGUI_DrawText(position + (size * 0.5f), font, true, true, text_color, command_info.filtered_name);
		}
	}
	void TGUI_Button_Command::TestInput(TGUI_Input &input)
	{
		auto &command_info = GetCommandInfo();
		auto &command_data = TGUI_command_data_base.Get<TGUI_Command_Data_Base::TGUI_Button_Command_Data>(GetDataHash(), command_info.command_type);

		if (command_info.flags & TGUI_COMMAND_FLAG_IS_DISABLED)
			return;

		const bool is_mouse_in_bounds = IsPointInBounds(input.current_mouse.position);

		const auto position = command_info.position;
		const auto size = command_info.size;

		const unsigned int font = tgui_fonts[TGUI_FONT_BUTTON];
		const auto text_size = HELPERS::TGUI_GetTextSize(font, command_info.filtered_name);

		/// If mouse is hovered over the button
		if (input.current_mouse.position.IsPointInBounds(position, position + size))
			command_info.flags |= TGUI_COMMAND_FLAG_IS_HOVERED;

		if ((command_info.flags & TGUI_COMMAND_FLAG_IS_HOVERED) && input.DidLeftClick() && is_mouse_in_bounds)
		{
			command_data.time_clicked = HELPERS::TGUI_GetTime();
			command_data.was_clicked = true;
		}
		else
			command_data.was_clicked = false;
	}

	/// Groupboxes
	TGUI_Groupbox_Command::TGUI_Groupbox_Command(const char* name, HELPERS::TGUI_Vector2D position, HELPERS::TGUI_Vector2D size, bool scroll_enabled)
	{
		auto &command_info = GetCommandInfo();
		command_info.command_type = TGUI_Command_Types::TGUI_COMMAND_TYPE_GROUPBOX;

		SetupCommandInfo(command_info, position, size, name, scroll_enabled);
	}
	void TGUI_Groupbox_Command::Draw()
	{
		const auto command_info = GetCommandInfo();
		const auto command_data = TGUI_command_data_base.Get<TGUI_Command_Data_Base::TGUI_Groupbox_Command_Data>(GetDataHash(), command_info.command_type);

		const bool is_hovered = command_info.flags & TGUI_COMMAND_FLAG_IS_HOVERED;
		const auto name = command_info.filtered_name;

		const unsigned int font = tgui_fonts[TGUI_FONT_GROUPBOX];
		const auto text_size = HELPERS::TGUI_GetTextSize(font, name);
		const bool should_draw_text = HELPERS::TGUI_Vector2D::IsPointInBounds(command_info.position, command_info.bounds_min, command_info.bounds_max) &&
			HELPERS::TGUI_Vector2D::IsPointInBounds(command_info.position + text_size, command_info.bounds_min, command_info.bounds_max);

		auto position = command_info.position, size = command_info.size;
		if (!GetClampedPositionAndSize(position, size))
			return;

		const auto body_color = is_hovered ? tgui_colors[TGUI_COLOR_GROUPBOX_BODY_HOVERED] : tgui_colors[TGUI_COLOR_GROUPBOX_BODY];
		const auto outline_color = tgui_colors[TGUI_COLOR_GROUPBOX_OUTLINE];

		/// Draw the body
		HELPERS::TGUI_DrawRectangle(position, size, body_color);
		HELPERS::TGUI_DrawRectangle(position, size, outline_color, true);

		/// Draw the text gg
		if (should_draw_text)
		{
			const auto text_color = is_hovered ? tgui_colors[TGUI_COLOR_GROUPBOX_TEXT_HOVERED] : tgui_colors[TGUI_COLOR_GROUPBOX_TEXT];
			auto text_position = position + HELPERS::TGUI_Vector2D(50, 0);

			if (is_hovered)
				text_position.x += 5;

			HELPERS::TGUI_DrawText(text_position, font, false, true, text_color, name);
		}
	}
	void TGUI_Groupbox_Command::TestInput(TGUI_Input &input)
	{
		auto &command_info = GetCommandInfo();
		auto &command_data = TGUI_command_data_base.Get<TGUI_Command_Data_Base::TGUI_Groupbox_Command_Data>(GetDataHash(), command_info.command_type);

		if (command_info.flags & TGUI_COMMAND_FLAG_IS_DISABLED)
			return;

		if (input.current_mouse.position.IsPointInBounds(command_info.position, command_info.position + command_info.size))
			command_info.flags |= TGUI_COMMAND_FLAG_IS_HOVERED;

		const bool is_mouse_in_bounds = input.current_mouse.position.IsPointInBounds(command_info.bounds_min, command_info.bounds_max);
		if (!is_mouse_in_bounds)
			return;

		if (!(command_info.flags & TGUI_COMMAND_FLAG_IS_HOVERED))
			command_data.last_time_not_hovered = HELPERS::TGUI_GetTime();

		/// Only scroll when mouse is in groupbox area
		if (command_info.flags & TGUI_COMMAND_FLAG_IS_HOVERED)
		{
			const int scroll_speed = tgui_attributes[TGUI_ATTRIBUTE_SCROLL_SPEED];
			command_data.scroll_amount = HELPERS::MIN<int>(0, command_data.scroll_amount + (input.current_mouse.scroll * scroll_speed));
			input.current_mouse.scroll = 0; /// So no other commands process scroll
		}
	}
	int TGUI_Groupbox_Command::GetScrollAmount()
	{
		return TGUI_command_data_base.Get<TGUI_Command_Data_Base::TGUI_Groupbox_Command_Data>(
			GetDataHash(), TGUI_Command_Types::TGUI_COMMAND_TYPE_GROUPBOX).scroll_amount;
	}

	/// Sliders
	TGUI_Slider_Command::TGUI_Slider_Command(const char* name, const char* suffix, float min, float max, float &variable, int decimal_precision, bool scroll_enabled, HELPERS::TGUI_Vector2D position, HELPERS::TGUI_Vector2D size)
	{
		auto &command_info = GetCommandInfo();

		command_info.command_type = TGUI_Command_Types::TGUI_COMMAND_TYPE_SLIDER;
		variable = HELPERS::CLAMP<float>(variable, min, max);
		m_variable_ptr = &variable, m_min = min, m_max = max, m_decimal_precision = decimal_precision;
		strcpy(m_suffix, suffix);

		SetupCommandInfo(command_info, position, size, name, scroll_enabled);
	}
	void TGUI_Slider_Command::Draw()
	{
		const auto command_info = GetCommandInfo();
		const auto command_data = TGUI_command_data_base.Get<TGUI_Command_Data_Base::TGUI_Slider_Command_Data>(GetDataHash(), command_info.command_type);

		const bool is_scroll = command_info.flags & TGUI_COMMAND_FLAG_SCROLL_ENABLED;
		const float alpha_scale = GetScrollAlphaScale() * command_info.alpha_modulation;
		const float slider_value = *m_variable_ptr;

		const bool is_hovered = command_info.flags & TGUI_COMMAND_FLAG_IS_HOVERED;
		const bool is_typing = command_data.is_typing;
		const bool should_draw_text = IsRectInBounds(command_info.position, command_info.size);

		auto position = command_info.position, size = command_info.size;
		if (!GetClampedPositionAndSize(position, size))
			return;

		auto body_color = tgui_colors[TGUI_COLOR_SLIDER_BODY];
		auto outline_color = tgui_colors[TGUI_COLOR_SLIDER_OUTLINE];
		auto body_value_color = tgui_colors[TGUI_COLOR_SLIDER_BODY_SELECTED];
		body_color.RGBA[3] *= alpha_scale, outline_color.RGBA[3] *= alpha_scale, body_value_color.RGBA[3] *= alpha_scale;

		/// Draw the slider body
		HELPERS::TGUI_DrawRectangle(position, size, body_color);

		/// Draw the value
		const auto body_value_position = position;

		const float frac = (slider_value / (m_max - m_min)) - (m_min / (m_max - m_min));
		const auto body_value_size = HELPERS::TGUI_Vector2D((command_info.size.x * frac), size.y);
		if (!is_typing)
			HELPERS::TGUI_DrawRectangle(body_value_position, body_value_size, body_value_color);

		/// Draw the outline
		HELPERS::TGUI_DrawRectangle(position, size, outline_color, true);

		/// Draw the value inside the slider
		if (should_draw_text)
		{
			auto value_color = tgui_colors[TGUI_COLOR_SLIDER_VALUE_TEXT];
			auto name_color = is_hovered ? tgui_colors[TGUI_COLOR_SLIDER_TEXT_NAME_HOVERED] : tgui_colors[TGUI_COLOR_SLIDER_TEXT_NAME];
			value_color.RGBA[3] *= alpha_scale, name_color.RGBA[3] *= alpha_scale;

			const unsigned int name_font = tgui_fonts[TGUI_FONT_SLIDER_NAME];
			const unsigned int value_font = tgui_fonts[TGUI_FONT_SLIDER_VALUE];

			char text_value[32];
			if (is_typing)
			{
				if (fmod(HELPERS::TGUI_GetTime(), 1.f) > 0.5f)
					sprintf(text_value, "%s_", command_data.typed_message);
				else
					sprintf(text_value, "%s ", command_data.typed_message);

				HELPERS::TGUI_DrawText(position + HELPERS::TGUI_Vector2D(5, size.y * 0.5f + 4), value_font, false, true, value_color, text_value);
			}
			else
			{
				/// Ghetto but gg
				char text_value_without_suffix[32];

				auto FloatToString = [](float num, int precision) -> std::string
				{
					std::stringstream stream;
					stream << std::fixed << std::setprecision(precision) << num;
					return stream.str();
				};

				sprintf(text_value, "%s%s", FloatToString(*m_variable_ptr, m_decimal_precision).c_str(), m_suffix);
				sprintf(text_value_without_suffix, "%s", FloatToString(*m_variable_ptr, m_decimal_precision).c_str());

				const auto text_value_size = HELPERS::TGUI_GetTextSize(value_font, text_value);
				const auto text_value_without_suffix_size = HELPERS::TGUI_GetTextSize(value_font, text_value_without_suffix);

				/// To make sure it doesn't go outside of the slider
				if (body_value_size.x + (text_value_without_suffix_size.x * 0.5f) > size.x + 5)
					HELPERS::TGUI_DrawText(position + HELPERS::TGUI_Vector2D(size.x + 5 - (text_value_without_suffix_size.x), size.y * 0.5f + 4), value_font, false, true, value_color, text_value_without_suffix);
				else if (body_value_size.x - (text_value_size.x * 0.5f) < 0)
					HELPERS::TGUI_DrawText(position + HELPERS::TGUI_Vector2D(0, size.y * 0.5f + 4), value_font, false, true, value_color, text_value);
				else
					HELPERS::TGUI_DrawText(position + HELPERS::TGUI_Vector2D(body_value_size.x, size.y * 0.5f + 4), value_font, true, true, value_color, text_value);
			}

			/// Draw the slider name
			if (is_hovered)
				HELPERS::TGUI_DrawText(position + HELPERS::TGUI_Vector2D(6 + size.x, size.y * 0.5f + 1), name_font, false, true, name_color, command_info.filtered_name);
			else
				HELPERS::TGUI_DrawText(position + HELPERS::TGUI_Vector2D(5 + size.x, size.y * 0.5f), name_font, false, true, name_color, command_info.filtered_name);
		}
	}
	void TGUI_Slider_Command::TestInput(TGUI_Input &input)
	{
		auto &command_info = GetCommandInfo();
		auto &command_data = TGUI_command_data_base.Get<TGUI_Command_Data_Base::TGUI_Slider_Command_Data>(GetDataHash(), command_info.command_type);

		if (command_info.flags & TGUI_COMMAND_FLAG_IS_DISABLED)
			return;

		const bool is_mouse_in_bounds = IsPointInBounds(input.current_mouse.position);

		const unsigned int name_font = tgui_fonts[TGUI_FONT_SLIDER_NAME];
		command_data.is_hovered_over_slider = input.current_mouse.position.IsPointInBounds(command_info.position, command_info.position + command_info.size);
		command_data.is_hovered_over_name = input.current_mouse.position.IsPointInBounds(command_info.position + HELPERS::TGUI_Vector2D(command_info.size.x, 0),
			command_info.position + HELPERS::TGUI_Vector2D(5 + command_info.size.x + HELPERS::TGUI_GetTextSize(name_font, command_info.filtered_name).x, command_info.size.y));

		if (command_data.is_hovered_over_slider || command_data.is_hovered_over_name)
			command_info.flags |= TGUI_COMMAND_FLAG_IS_HOVERED;

		if (command_data.is_hovered_over_slider && is_mouse_in_bounds && input.DidLeftClick())
			command_data.is_sliding = true;
		else if (!input.DidLeftHold())
			command_data.is_sliding = false;

		/// If setting a slider value
		if (command_data.is_sliding)
		{
			const float delta = (input.current_mouse.position.x - command_info.position.x) / static_cast<float>(command_info.size.x);

			*m_variable_ptr = HELPERS::CLAMP<float>((delta * (m_max - m_min)) + m_min, m_min, m_max);
		}

		/// If they click on the name, accept keyboard input
		if (input.DidLeftClick())
		{
			if (command_data.is_hovered_over_name && is_mouse_in_bounds)
				command_data.is_typing = true;
			else
				command_data.is_typing = false;
		}


		/// Handle text input
		if (command_data.is_typing)
		{
			/// if the value is out of range, don't accept input anymore
			if (strlen(command_data.typed_message) < 16)
			{
				for (int i = 0; i < 256; i++)
				{
					const bool is_valid_key = TGUI_Input::IsKeyNumber(i) && input.DidClickKey(i);
					if (is_valid_key)
					{
						const int str_len = strlen(command_data.typed_message);
						command_data.typed_message[str_len] = TGUI_Input::KeyToChar(i);
						command_data.typed_message[str_len + 1] = '\0';
					}
				}
			}

			/// Backspace
			if (input.DidClickKey(VK_BACK))
			{
				const int str_len = strlen(command_data.typed_message);
				if (str_len > 0)
					command_data.typed_message[str_len - 1] = '\0';
			}
			/// Enter
			if (input.DidClickKey(VK_RETURN))
			{
				*m_variable_ptr = HELPERS::CLAMP<float>(atof(command_data.typed_message), m_min, m_max);
				command_data.is_typing = false;
			}
		}

		if (!command_data.is_typing)
			command_data.typed_message[0] = '\0';
	}

	/// Separators
	TGUI_Separator_Command::TGUI_Separator_Command(const char* name, HELPERS::TGUI_Vector2D position, HELPERS::TGUI_Vector2D size, bool scroll_enabled)
	{
		auto &command_info = GetCommandInfo();

		command_info.command_type = TGUI_Command_Types::TGUI_COMMAND_TYPE_SEPARATOR;

		SetupCommandInfo(command_info, position, size, name, scroll_enabled);
	}
	void TGUI_Separator_Command::Draw()
	{
		const auto command_info = GetCommandInfo();

		const bool is_unnamed = strlen(command_info.filtered_name) < 1;
		const bool is_scroll = command_info.flags & TGUI_COMMAND_FLAG_SCROLL_ENABLED;

		const float alpha_scale = GetScrollAlphaScale() * command_info.alpha_modulation;

		const unsigned int font = tgui_fonts[TGUI_FONT_SEPARATOR];

		auto body_color = tgui_colors[TGUI_COLOR_SEPARATOR_BODY];
		auto text_color = tgui_colors[TGUI_COLOR_SEPARATOR_TEXT];
		body_color.RGBA[3] *= alpha_scale, text_color.RGBA[3] *= alpha_scale;

		const auto text_size = HELPERS::TGUI_GetTextSize(font, command_info.filtered_name);
		if (!IsRectInBounds(command_info.position, command_info.size))
			return;

		if (is_unnamed)
			HELPERS::TGUI_DrawLine(command_info.position, command_info.size, body_color);
		else
		{
			const int width = command_info.size.x;

			HELPERS::TGUI_DrawLine(command_info.position, HELPERS::TGUI_Vector2D((width * 0.5) - (text_size.x * 0.5) - 5, 0), body_color);
			HELPERS::TGUI_DrawLine(command_info.position + HELPERS::TGUI_Vector2D((width * 0.5) + (text_size.x * 0.5) + 5, 0), HELPERS::TGUI_Vector2D((width * 0.5) - (text_size.x * 0.5) - 10, 0), body_color);

			HELPERS::TGUI_DrawText(command_info.position + HELPERS::TGUI_Vector2D(command_info.size.x * 0.5, 0), font, true, true, text_color, command_info.filtered_name);
		}
	}

	/// Keybinds
	TGUI_Keybind_Command::TGUI_Keybind_Command(const char* name, int &variable, HELPERS::TGUI_Vector2D position, HELPERS::TGUI_Vector2D size, bool scroll_enabled)
	{
		auto &command_info = GetCommandInfo();

		/// invalid key
		if (variable != -1 && !TGUI_Input::IsKey(variable))
			variable = -1;

		m_variable_ptr = &variable;
		command_info.command_type = TGUI_Command_Types::TGUI_COMMAND_TYPE_KEYBIND;

		SetupCommandInfo(command_info, position, size, name, scroll_enabled);
	}
	void TGUI_Keybind_Command::Draw()
	{
		const auto command_info = GetCommandInfo();
		const auto command_data = TGUI_command_data_base.Get<TGUI_Command_Data_Base::TGUI_Keybind_Command_Data>(GetDataHash(), command_info.command_type);

		auto position = command_info.position, size = command_info.size;
		if (!GetClampedPositionAndSize(position, size))
			return;

		const float alpha_scale = GetScrollAlphaScale() * command_info.alpha_modulation;

		const bool is_typing = command_data.is_typing;
		const bool is_hovered = command_info.flags & TGUI_COMMAND_FLAG_IS_HOVERED;
		const bool should_draw_text = IsRectInBounds(command_info.position, command_info.size);
		const bool has_key_selected = *m_variable_ptr != -1;

		auto body_color = is_typing ? tgui_colors[TGUI_COLOR_KEYBIND_BODY_SELECTED] : tgui_colors[TGUI_COLOR_KEYBIND_BODY];
		auto outline_color = tgui_colors[TGUI_COLOR_KEYBIND_OUTLINE];
		body_color.RGBA[3] *= alpha_scale;
		outline_color.RGBA[3] *= alpha_scale;

		HELPERS::TGUI_DrawRectangle(position, size, body_color);
		HELPERS::TGUI_DrawRectangle(position, size, outline_color, true);

		if (should_draw_text)
		{
			auto text_name_color = is_hovered ? tgui_colors[TGUI_COLOR_KEYBIND_TEXT_NAME_HOVERED] : tgui_colors[TGUI_COLOR_KEYBIND_TEXT_NAME];
			auto text_value_color = tgui_colors[TGUI_COLOR_KEYBIND_TEXT_VALUE];
			text_name_color.RGBA[3] *= alpha_scale;
			text_value_color.RGBA[3] *= alpha_scale;

			const unsigned int name_font = tgui_fonts[TGUI_FONT_KEYBIND_NAME];
			const unsigned int value_font = tgui_fonts[TGUI_FONT_KEYBIND_VALUE];

			if (is_hovered)
				HELPERS::TGUI_DrawText(position + HELPERS::TGUI_Vector2D(size.x + 6, size.y * 0.5 + 1), name_font, false, true, text_name_color, command_info.filtered_name);
			else
				HELPERS::TGUI_DrawText(position + HELPERS::TGUI_Vector2D(size.x + 5, size.y * 0.5), name_font, false, true, text_name_color, command_info.filtered_name);

			if (is_typing)
				HELPERS::TGUI_DrawText(position + HELPERS::TGUI_Vector2D(size.x * 0.5, size.y * 0.5), value_font, true, true, text_value_color, (fmod(HELPERS::TGUI_GetTime(), 1.f) > 0.5f) ? " " : "-");
			else if (has_key_selected)
				HELPERS::TGUI_DrawText(position + HELPERS::TGUI_Vector2D(size.x * 0.5, size.y * 0.5), value_font, true, true, text_value_color, TGUI_Input::GetKeyName(*m_variable_ptr));
			else if (strlen(command_info.filtered_name) > 0)
				HELPERS::TGUI_DrawText(position + HELPERS::TGUI_Vector2D(5, size.y * 0.5), value_font, false, true, HELPERS::TGUI_Color(255, 255, 255, 75.f * alpha_scale), command_info.filtered_name);
			else
				HELPERS::TGUI_DrawText(position + HELPERS::TGUI_Vector2D(size.x * 0.5, size.y * 0.5), value_font, true, true, HELPERS::TGUI_Color(255, 255, 255, 75), "-");
		}

	}
	void TGUI_Keybind_Command::TestInput(TGUI_Input &input)
	{
		auto &command_info = GetCommandInfo();
		auto &command_data = TGUI_command_data_base.Get<TGUI_Command_Data_Base::TGUI_Keybind_Command_Data>(GetDataHash(), command_info.command_type);

		if (command_info.flags & TGUI_COMMAND_FLAG_IS_DISABLED)
			return;

		const bool is_mouse_in_bounds = IsPointInBounds(input.current_mouse.position);

		const unsigned int font = tgui_fonts[TGUI_FONT_KEYBIND_NAME];
		if (input.current_mouse.position.IsPointInBounds(command_info.position,
			command_info.position + command_info.size + HELPERS::TGUI_Vector2D(5 + HELPERS::TGUI_GetTextSize(font, command_info.filtered_name).x, 0)))
			command_info.flags |= TGUI_COMMAND_FLAG_IS_HOVERED;

		if (input.DidLeftClick())
		{
			if ((command_info.flags & TGUI_COMMAND_FLAG_IS_HOVERED) && is_mouse_in_bounds)
				command_data.is_typing = true;
			else
				command_data.is_typing = false;
		}

		if (command_data.is_typing)
		{
			for (int i = 0; i < 256; i++)
			{
				if (TGUI_Input::IsValidKey(i) && input.DidClickKey(i))
				{
					command_data.is_typing = false;
					*m_variable_ptr = i;
					break;
				}
			}

			if (input.DidClickKey(VK_ESCAPE))
			{
				command_data.is_typing = false;
				*m_variable_ptr = -1;
			}
		}
	}

	/// Text Input
	TGUI_Text_Input_Command::TGUI_Text_Input_Command(const char* name, HELPERS::TGUI_Vector2D position, HELPERS::TGUI_Vector2D size, char* variable, bool scroll_enabled)
	{
		auto &command_info = GetCommandInfo();

		m_variable_ptr = variable;
		command_info.command_type = TGUI_Command_Types::TGUI_COMMAND_TYPE_TEXT_INPUT;

		SetupCommandInfo(command_info, position, size, name, scroll_enabled);
	}
	void TGUI_Text_Input_Command::Draw()
	{
		const auto command_info = GetCommandInfo();
		const auto command_data = TGUI_command_data_base.Get<TGUI_Command_Data_Base::TGUI_Text_Input_Command_Data>(GetDataHash(), command_info.command_type);

		auto position = command_info.position, size = command_info.size;
		if (!GetClampedPositionAndSize(position, size))
			return;

		const bool is_hovered = command_info.flags & TGUI_COMMAND_FLAG_IS_HOVERED;
		const bool is_typing = command_data.is_typing;
		const bool should_draw_text = IsRectInBounds(command_info.position, command_info.size);
		const float alpha_scale = GetScrollAlphaScale() * command_info.alpha_modulation;

		auto body_color = is_typing ? tgui_colors[TGUI_COLOR_TEXT_INPUT_BODY_SELECTED] : tgui_colors[TGUI_COLOR_TEXT_INPUT_BODY];
		auto outline_color = tgui_colors[TGUI_COLOR_TEXT_INPUT_OUTLINE];
		body_color.RGBA[3] *= alpha_scale, outline_color.RGBA[3] *= alpha_scale;

		HELPERS::TGUI_DrawRectangle(position, size, body_color);
		HELPERS::TGUI_DrawRectangle(position, size, outline_color, true);

		if (should_draw_text)
		{
			auto text_name_color = is_hovered ? tgui_colors[TGUI_COLOR_TEXT_INPUT_TEXT_HOVERED] : tgui_colors[TGUI_COLOR_TEXT_INPUT_TEXT_NAME];
			auto text_value_color = tgui_colors[TGUI_COLOR_TEXT_INPUT_TEXT_NAME];
			text_name_color.RGBA[3] *= alpha_scale, text_value_color.RGBA[3] *= alpha_scale;

			const unsigned int name_font = tgui_fonts[TGUI_FONT_TEXT_INPUT_NAME];
			const unsigned int value_font = tgui_fonts[TGUI_FONT_TEXT_INPUT_VALUE];

			if (is_hovered)
				HELPERS::TGUI_DrawText(position + HELPERS::TGUI_Vector2D(size.x + 6, size.y * 0.5f + 1), name_font, false, true, text_name_color, command_info.filtered_name);
			else
				HELPERS::TGUI_DrawText(position + HELPERS::TGUI_Vector2D(size.x + 5, size.y * 0.5f), name_font, false, true, text_name_color, command_info.filtered_name);

			if (is_typing)
			{
				auto ShiftTextLeft = [](char* text) -> void
				{
					const int len = strlen(text);
					if (len < 2)
						return;

					for (int i = 0; i < len - 1; i++)
						text[i] = text[i + 1];
					text[len - 1] = '\0';
				};

				char buffer[TGUI_MAX_STRING_LENGTH], buffer_with_[TGUI_MAX_STRING_LENGTH];
				sprintf(buffer, "%s%c", m_variable_ptr, (fmod(HELPERS::TGUI_GetTime(), 1.f) > 0.5f) ? '_' : ' ');
				sprintf(buffer_with_, "%s%c", m_variable_ptr, '_');

				/// If the text width is greater than the keybind width, shorten the text width
				int buffer_text_width = HELPERS::TGUI_GetTextSize(value_font, buffer_with_).x;
				while (buffer_text_width > size.x - 10)
				{
					ShiftTextLeft(buffer);
					ShiftTextLeft(buffer_with_);
					buffer_text_width = HELPERS::TGUI_GetTextSize(value_font, buffer_with_).x;
				}

				HELPERS::TGUI_DrawText(position + HELPERS::TGUI_Vector2D(5, size.y * 0.5f), value_font, false, true, text_value_color, buffer);
			}
			else if (strlen(m_variable_ptr) <= 0)
			{
				if (strlen(command_info.filtered_name) > 0)
					HELPERS::TGUI_DrawText(position + HELPERS::TGUI_Vector2D(5, size.y * 0.5f), value_font, false, true, HELPERS::TGUI_Color(255, 255, 255, 75.f * alpha_scale), command_info.filtered_name);
				else
					HELPERS::TGUI_DrawText(position + HELPERS::TGUI_Vector2D(size.x * 0.5f, size.y * 0.5f), value_font, true, true, HELPERS::TGUI_Color(255, 255, 255, 75.f * alpha_scale), "-");
			}
			else
			{
				char buffer[TGUI_MAX_STRING_LENGTH];
				strcpy(buffer, m_variable_ptr);

				/// Truncate the text if too long
				int buffer_text_width = HELPERS::TGUI_GetTextSize(value_font, buffer).x;
				while (buffer_text_width > size.x - 5)
				{
					buffer[strlen(buffer) - 1] = '\0';
					buffer_text_width = HELPERS::TGUI_GetTextSize(value_font, buffer).x;
				}

				HELPERS::TGUI_DrawText(position + HELPERS::TGUI_Vector2D(5, size.y * 0.5f), value_font, false, true, text_value_color, buffer);
			}
		}
	}
	void TGUI_Text_Input_Command::TestInput(TGUI_Input &input)
	{
		auto &command_info = GetCommandInfo();
		auto &command_data = TGUI_command_data_base.Get<TGUI_Command_Data_Base::TGUI_Text_Input_Command_Data>(GetDataHash(), command_info.command_type);

		if (command_info.flags & TGUI_COMMAND_FLAG_IS_DISABLED)
			return;

		const bool is_mouse_in_bounds = IsPointInBounds(input.current_mouse.position);

		const unsigned int name_font = tgui_fonts[TGUI_FONT_TEXT_INPUT_NAME];
		if (input.current_mouse.position.IsPointInBounds(command_info.position, command_info.position + command_info.size +
			HELPERS::TGUI_Vector2D(5 + HELPERS::TGUI_GetTextSize(name_font, command_info.filtered_name).x, 0)))
			command_info.flags |= TGUI_COMMAND_FLAG_IS_HOVERED;

		if (input.DidLeftClick())
		{
			if ((command_info.flags & TGUI_COMMAND_FLAG_IS_HOVERED))
			{
				m_variable_ptr[0] = '\0';
				command_data.is_typing = true;
			}
			else
				command_data.is_typing = false;
		}

		if (command_data.is_typing)
		{
			const int str_length = strlen(m_variable_ptr);

			if (input.DidClickKey(VK_RETURN))
				command_data.is_typing = false;
			if (input.DidClickKey(VK_BACK) && str_length > 0)
				m_variable_ptr[str_length - 1] = '\0';
			if (input.DidClickKey(VK_ESCAPE))
				m_variable_ptr[0] = '\0';

			if (str_length < TGUI_MAX_STRING_LENGTH - 1)
			{
				for (int i = 0; i < 256; i++)
				{
					if (TGUI_Input::IsValidKey(i) && input.DidClickKey(i))
					{
						m_variable_ptr[str_length] = TGUI_Input::KeyToChar(i);
						m_variable_ptr[str_length + 1] = '\0';
						break;
					}
				}
			}
		}
	}

	/// Comboboxes
	TGUI_Combobox_Command::TGUI_Combobox_Command(const char* name, HELPERS::TGUI_Vector2D position, HELPERS::TGUI_Vector2D size, char(*items)[TGUI_MAX_STRING_LENGTH], int items_amount, int &variable, bool scroll_enabled)
	{
		auto &command_info = GetCommandInfo();

		variable = HELPERS::CLAMP(variable, 0, items_amount - 1);
		m_variable_ptr = &variable;
		m_items_amount = items_amount;
		m_items = items;
		command_info.command_type = TGUI_Command_Types::TGUI_COMMAND_TYPE_COMBOBOX;

		SetupCommandInfo(command_info, position, size, name, scroll_enabled);
	}
	void TGUI_Combobox_Command::Draw()
	{
		const auto command_info = GetCommandInfo();
		const auto command_data = TGUI_command_data_base.Get<TGUI_Command_Data_Base::TGUI_Combobox_Command_Data>(GetDataHash(), command_info.command_type);

		auto position = command_info.position, size = command_info.size;
		if (!GetClampedPositionAndSize(position, size))
			return;

		const float alpha_scale = GetScrollAlphaScale() * command_info.alpha_modulation;
		const int max_items_displayed = tgui_attributes[TGUI_ATTRIBUTE_COMBOBOX_MAX_ITEMS_DISPLAYED];
		const bool is_hovered = command_info.flags & TGUI_COMMAND_FLAG_IS_HOVERED;
		const bool is_typing = command_data.is_typing;
		const bool is_open = command_data.is_combobox_open;
		const bool should_draw_text = IsRectInBounds(command_info.position, command_info.size);

		auto body_color = tgui_colors[TGUI_COLOR_COMBOBOX_BODY_SELECTED];
		auto outline_color = tgui_colors[TGUI_COLOR_COMBOBOX_OUTLINE_SELECTED];
		body_color.RGBA[3] *= alpha_scale, outline_color.RGBA[3] *= alpha_scale;

		/// Draw each item if combobox open
		if (is_open)
		{
			const auto items_body_color = tgui_colors[TGUI_COLOR_COMBOBOX_BODY_ITEM];
			const auto items_outline_color = tgui_colors[TGUI_COLOR_COMBOBOX_OUTLINE_ITEM];

			const auto items_position = command_info.position + HELPERS::TGUI_Vector2D(0, command_info.size.y + 5);
			const auto items_size = HELPERS::TGUI_Vector2D(command_info.size.x, command_info.size.y * HELPERS::MIN(m_items_amount, max_items_displayed));

			/// Draw items body
			HELPERS::TGUI_DrawRectangle(items_position, items_size, items_body_color);
			HELPERS::TGUI_DrawRectangle(items_position, items_size, items_outline_color, true);

			/// Scrollbar, ugly code but fuck your mother
			if (m_items_amount > max_items_displayed)
			{
				const float scroll_bar_height = HELPERS::MAX<float>(10.f, (static_cast<float>(max_items_displayed) / static_cast<float>(m_items_amount)) * (items_size.y - 4));
				const float height_offset = static_cast<float>(items_size.y - scroll_bar_height - 4) *
					(command_data.scroll_amount / static_cast<float>((m_items_amount - max_items_displayed) * command_info.size.y));

				const auto pos = HELPERS::TGUI_Vector2D(items_position.x + items_size.x - 5, items_position.y + height_offset + 2);

				HELPERS::TGUI_DrawLine(pos, HELPERS::TGUI_Vector2D(0, scroll_bar_height), HELPERS::TGUI_Color(0, 0, 0, 255));
			}

			const auto text_color = tgui_colors[TGUI_COLOR_COMBOBOX_TEXT_ITEM];
			const auto selected_text_color = tgui_colors[TGUI_COLOR_COMBOBOX_SELECTED_TEXT_ITEM];

			const int scroll_offset = (max_items_displayed > m_items_amount) ? 0 : command_data.scroll_amount / command_info.size.y;
			const int max_items = scroll_offset + HELPERS::MIN(max_items_displayed, m_items_amount);
			for (int i = scroll_offset; i < max_items; i++)
			{
				const auto current_position = command_info.position + HELPERS::TGUI_Vector2D(5, command_info.size.y * (i + 1.5 - scroll_offset) + 5);
				const unsigned int font = tgui_fonts[TGUI_FONT_COMBOBOX_ITEM_NAME];

				if (i == *m_variable_ptr)
					HELPERS::TGUI_DrawText(current_position, font, false, true, selected_text_color, m_items[i]);
				else
					HELPERS::TGUI_DrawText(current_position, font, false, true, text_color, m_items[i]);
			}
		}

		/// draw body and shit
		HELPERS::TGUI_DrawRectangle(position, size, body_color);
		HELPERS::TGUI_DrawRectangle(position, size, outline_color, true);

		/// draw text
		if (should_draw_text)
		{
			auto text_name_color = is_hovered ? tgui_colors[TGUI_COLOR_COMBOBOX_TEXT_NAME_HOVERED] : tgui_colors[TGUI_COLOR_COMBOBOX_TEXT_NAME];
			auto text_item_color = is_hovered ? tgui_colors[TGUI_COLOR_COMBOBOX_TEXT_SELECTED_HOVERED] : tgui_colors[TGUI_COLOR_COMBOBOX_TEXT_SELECTED];
			text_name_color.RGBA[3] *= alpha_scale, text_item_color.RGBA[3] *= alpha_scale;

			const unsigned int selected_font = tgui_fonts[TGUI_FONT_COMBOBOX_SELECTED_ITEM];
			const unsigned int name_font = tgui_fonts[TGUI_FONT_COMBOBOX_NAME];

			if (is_typing)
			{
				char buffer[TGUI_MAX_STRING_LENGTH];
				sprintf(buffer, "%s%c", command_data.current_searched_text, fmod(HELPERS::TGUI_GetTime(), 1.f) > 0.5f ? '_' : ' ');

				HELPERS::TGUI_DrawText(position + HELPERS::TGUI_Vector2D(5, size.y * 0.5), selected_font, false, true, text_item_color, buffer);
			}
			else
				HELPERS::TGUI_DrawText(position + HELPERS::TGUI_Vector2D(5, size.y * 0.5), selected_font, false, true, text_item_color, m_items[*m_variable_ptr]);

			if (is_hovered)
				HELPERS::TGUI_DrawText(position + HELPERS::TGUI_Vector2D(size.x + 6, size.y * 0.5 + 1), name_font, false, true, text_name_color, command_info.filtered_name);
			else
				HELPERS::TGUI_DrawText(position + HELPERS::TGUI_Vector2D(size.x + 5, size.y * 0.5), name_font, false, true, text_name_color, command_info.filtered_name);
		}
	}
	void TGUI_Combobox_Command::TestInput(TGUI_Input &input)
	{
		auto &command_info = GetCommandInfo();
		auto &command_data = TGUI_command_data_base.Get<TGUI_Command_Data_Base::TGUI_Combobox_Command_Data>(GetDataHash(), command_info.command_type);

		if (command_info.flags & TGUI_COMMAND_FLAG_IS_DISABLED)
			return;

		const bool is_mouse_in_bounds = IsPointInBounds(input.current_mouse.position);
		const unsigned int font = tgui_fonts[TGUI_FONT_COMBOBOX_NAME];
		const auto text_size = HELPERS::TGUI_GetTextSize(font, command_info.filtered_name);

		if (input.current_mouse.position.IsPointInBounds(command_info.position,
			command_info.position + command_info.size + HELPERS::TGUI_Vector2D(5 + text_size.x, 0)))
			command_info.flags |= TGUI_COMMAND_FLAG_IS_HOVERED;

		const bool is_in_bounds = IsRectInBounds(command_info.position, command_info.size);
		if (!is_in_bounds)
		{
			command_data.is_typing = false;
			command_data.is_combobox_open = false;
			command_data.current_searched_text[0] = '\0';
			command_data.scroll_amount = 0;
			command_data.times_pressed_enter = 0;
			return;
		}

		const int max_items_displayed = tgui_attributes[TGUI_ATTRIBUTE_COMBOBOX_MAX_ITEMS_DISPLAYED];
		const int scroll_offset = (max_items_displayed > m_items_amount) ? 0 : command_data.scroll_amount / command_info.size.y;
		const auto items_box_position = command_info.position + HELPERS::TGUI_Vector2D(0, command_info.size.y + 5);
		const auto items_box_size = HELPERS::TGUI_Vector2D(command_info.size.x, command_info.size.y * HELPERS::MIN(m_items_amount, max_items_displayed));
		const bool is_mouse_in_items_box = input.current_mouse.position.IsPointInBounds(items_box_position, items_box_position + items_box_size);

		/// If clicked
		if (input.DidLeftClick())
		{
			/// Selected an item
			if (command_data.is_combobox_open && is_mouse_in_items_box)
			{
				input.current_mouse.left_button = false;

				*m_variable_ptr = HELPERS::CLAMP(((input.current_mouse.position.y - items_box_position.y) / command_info.size.y) + scroll_offset, 0, m_items_amount - 1);
			}

			/// Handles whether they clicked on the combobox or not
			if (command_info.flags & TGUI_COMMAND_FLAG_IS_HOVERED)
			{
				input.current_mouse.left_button = false;

				if (command_data.is_combobox_open && tgui_attributes[TGUI_ATTRIBUTE_COMBOBOX_MAX_ITEMS_DISPLAYED] < m_items_amount)
				{
					command_data.is_typing = true;
					command_data.current_searched_text[0] = '\0';
				}

				command_data.is_combobox_open = true;
			}
			else
			{
				command_data.is_combobox_open = false;
				command_data.is_typing = false;
				command_data.times_pressed_enter = 0;
			}
		}
		else if (input.DidRealLeftClick())
		{
			command_data.is_combobox_open = false;
			command_data.is_typing = false;
			command_data.times_pressed_enter = 0;
		}


		/// Handle keyboard input
		if (command_data.is_typing)
		{
			/// Search for the text input in the items and set the 
			/// command_data.scroll_amount so that it appears at the top
			auto SearchForText = [this, max_items_displayed, scroll_offset, &command_data, command_info](const char* search_string) -> void
			{
				/// No text
				if (strlen(search_string) <= 0)
					return;

				/// Only search if you can scroll (lazy fucks)
				if (m_items_amount <= max_items_displayed)
					return;

				/// Change a c string to all lowercase
				auto ToLowercase = [](char* text) -> void
				{
					const int str_len = strlen(text);
					for (int i = 0; i < str_len; i++)
						text[i] = static_cast<char>(tolower(text[i]));
				};

				/// Get the lowercase version of the search string
				char lower_case_search_string[TGUI_MAX_STRING_LENGTH];
				strcpy(lower_case_search_string, search_string);
				ToLowercase(lower_case_search_string);

				/// Just find how many valid items there are
				int valid_items = 0;
				for (int i = 0; i < m_items_amount; i++)
				{
					/// Change the item to lowercase as well
					char lower_case_item[TGUI_MAX_STRING_LENGTH];
					strcpy(lower_case_item, m_items[i]);
					ToLowercase(lower_case_item);

					if (strstr(lower_case_item, lower_case_search_string))
						valid_items++;
				}

				/// No valid items, return
				if (valid_items <= 0)
					return;

				/// Iterate through all the items, this time searching for ones matching the search_string
				int best_index = scroll_offset, counter = 0;
				for (int i = 0; i < m_items_amount; i++)
				{
					char lower_case_item[TGUI_MAX_STRING_LENGTH];
					strcpy(lower_case_item, m_items[i]);
					ToLowercase(lower_case_item);

					/// If the search_string is a sub string of the item
					if (strstr(lower_case_item, lower_case_search_string))
					{
						if (command_data.times_pressed_enter % valid_items == counter)
						{
							best_index = i;
							break;
						}
						counter++;
					}
				}

				/// Clamp scroll amount in case it goes out of range
				command_data.scroll_amount = HELPERS::CLAMP(best_index, 0, m_items_amount - max_items_displayed) * command_info.size.y;
			};

			/// Whenever they press enter, it cycles to the next valid search item
			if (input.DidClickKey(VK_RETURN))
			{
				command_data.times_pressed_enter++;
				SearchForText(command_data.current_searched_text);
			}

			/// Process clicked keys
			const int string_length = strlen(command_data.current_searched_text);
			for (int i = 0; i < 256; i++)
			{
				if (TGUI_Input::IsValidKey(i) && input.DidClickKey(i))
				{
					command_data.current_searched_text[string_length] = TGUI_Input::KeyToChar(i);
					command_data.current_searched_text[string_length + 1] = '\0';
					SearchForText(command_data.current_searched_text);
					break;
				}
			}

			/// Escape key
			if (input.DidClickKey(VK_ESCAPE))
			{
				command_data.is_typing = false;
				command_data.is_combobox_open = false;
				command_data.current_searched_text[0] = '\0';
				command_data.times_pressed_enter = 0;
			}

			/// Backspace
			if (input.DidClickKey(VK_BACK) && string_length > 0)
				command_data.current_searched_text[string_length - 1] = '\0';
		}

		/// Handle scrolling
		if (input.current_mouse.scroll && command_data.is_combobox_open && is_mouse_in_items_box)
		{
			const int scroll_speed = tgui_attributes[TGUI_ATTRIBUTE_SCROLL_SPEED];
			command_data.scroll_amount = HELPERS::CLAMP<int>(command_data.scroll_amount - (input.current_mouse.scroll * scroll_speed),
				0, (m_items_amount - max_items_displayed) * command_info.size.y);

			input.current_mouse.scroll = 0;
		}
	}

	/// Multiselect comboboxes
	TGUI_Multiselect_Combobox_Command::TGUI_Multiselect_Combobox_Command(const char* name, HELPERS::TGUI_Vector2D position, HELPERS::TGUI_Vector2D size, char(*items)[TGUI_MAX_STRING_LENGTH], bool* selected_items, int items_amount, bool scroll_enabled)
	{
		auto &command_info = GetCommandInfo();

		m_selected_items = selected_items;
		m_items_amount = items_amount;
		m_items = items;
		command_info.command_type = TGUI_Command_Types::TGUI_COMMAND_TYPE_MULTISELECT_COMBOBOX;

		SetupCommandInfo(command_info, position, size, name, scroll_enabled);
	}
	void TGUI_Multiselect_Combobox_Command::Draw()
	{
		const auto command_info = GetCommandInfo();
		const auto command_data = TGUI_command_data_base.Get<TGUI_Command_Data_Base::TGUI_Multiselect_Combobox_Command_Data>(GetDataHash(), command_info.command_type);

		if (command_info.flags & TGUI_COMMAND_FLAG_IS_DISABLED)
			return;

		auto position = command_info.position, size = command_info.size;
		if (!GetClampedPositionAndSize(position, size))
			return;

		const float alpha_scale = GetScrollAlphaScale() * command_info.alpha_modulation;

		const int max_items_displayed = tgui_attributes[TGUI_ATTRIBUTE_COMBOBOX_MAX_ITEMS_DISPLAYED];

		const bool is_hovered = command_info.flags & TGUI_COMMAND_FLAG_IS_HOVERED;
		const bool is_typing = command_data.is_typing;
		const bool is_open = command_data.is_combobox_open;
		const bool should_draw_text = IsRectInBounds(command_info.position, command_info.size);

		auto body_color = tgui_colors[TGUI_COLOR_COMBOBOX_BODY_SELECTED];
		auto outline_color = tgui_colors[TGUI_COLOR_COMBOBOX_OUTLINE_SELECTED];
		body_color.RGBA[3] *= alpha_scale, outline_color.RGBA[3] *= alpha_scale;

		/// Draw each item if combobox open
		if (is_open)
		{
			const auto items_body_color = tgui_colors[TGUI_COLOR_COMBOBOX_BODY_ITEM];
			const auto items_outline_color = tgui_colors[TGUI_COLOR_COMBOBOX_OUTLINE_ITEM];

			const auto items_position = command_info.position + HELPERS::TGUI_Vector2D(0, command_info.size.y + 5);
			const auto items_size = HELPERS::TGUI_Vector2D(command_info.size.x, command_info.size.y * HELPERS::MIN(m_items_amount, max_items_displayed));

			/// Draw items body
			HELPERS::TGUI_DrawRectangle(items_position, items_size, items_body_color);
			HELPERS::TGUI_DrawRectangle(items_position, items_size, items_outline_color, true);

			/// Scrollbar, ugly code but fuck your mother
			if (m_items_amount > max_items_displayed)
			{
				const float scroll_bar_height = HELPERS::MAX<float>(10.f, (static_cast<float>(max_items_displayed) / static_cast<float>(m_items_amount)) * (items_size.y - 4));
				const float height_offset = static_cast<float>(items_size.y - scroll_bar_height - 4) *
					(command_data.scroll_amount / static_cast<float>((m_items_amount - max_items_displayed) * command_info.size.y));

				const auto pos = HELPERS::TGUI_Vector2D(items_position.x + items_size.x - 5, items_position.y + height_offset + 2);

				HELPERS::TGUI_DrawLine(pos, HELPERS::TGUI_Vector2D(0, scroll_bar_height), HELPERS::TGUI_Color(0, 0, 0, 255));
			}

			const auto text_color = tgui_colors[TGUI_COLOR_COMBOBOX_TEXT_ITEM];
			const auto selected_text_color = tgui_colors[TGUI_COLOR_COMBOBOX_SELECTED_TEXT_ITEM];

			const int scroll_offset = (max_items_displayed > m_items_amount) ? 0 : command_data.scroll_amount / command_info.size.y;
			const int max_items = scroll_offset + HELPERS::MIN(max_items_displayed, m_items_amount);
			for (int i = scroll_offset; i < max_items; i++)
			{
				const auto current_position = command_info.position + HELPERS::TGUI_Vector2D(5, command_info.size.y * (i + 1.5 - scroll_offset) + 5);
				const unsigned int font = tgui_fonts[TGUI_FONT_COMBOBOX_ITEM_NAME];

				if (m_selected_items[i])
					HELPERS::TGUI_DrawText(current_position, font, false, true, selected_text_color, m_items[i]);
				else
					HELPERS::TGUI_DrawText(current_position, font, false, true, text_color, m_items[i]);
			}
		}

		/// draw body and shit
		HELPERS::TGUI_DrawRectangle(position, size, body_color);
		HELPERS::TGUI_DrawRectangle(position, size, outline_color, true);

		/// draw text
		if (should_draw_text)
		{
			auto text_name_color = is_hovered ? tgui_colors[TGUI_COLOR_COMBOBOX_TEXT_NAME_HOVERED] : tgui_colors[TGUI_COLOR_COMBOBOX_TEXT_NAME];
			auto text_item_color = is_hovered ? tgui_colors[TGUI_COLOR_COMBOBOX_TEXT_SELECTED_HOVERED] : tgui_colors[TGUI_COLOR_COMBOBOX_TEXT_SELECTED];
			text_name_color.RGBA[3] *= alpha_scale, text_item_color.RGBA[3] *= alpha_scale;

			const unsigned int selected_font = tgui_fonts[TGUI_FONT_COMBOBOX_SELECTED_ITEM];
			const unsigned int name_font = tgui_fonts[TGUI_FONT_COMBOBOX_NAME];

			if (is_typing)
			{
				char buffer[TGUI_MAX_STRING_LENGTH];
				sprintf(buffer, "%s%c", command_data.current_searched_text, fmod(HELPERS::TGUI_GetTime(), 1.f) > 0.5f ? '_' : ' ');

				HELPERS::TGUI_DrawText(position + HELPERS::TGUI_Vector2D(5, size.y * 0.5), selected_font, false, true, text_item_color, buffer);
			}
			else
			{
				/// get amount of items selected
				int num_items_selected = 0, selected_item = 0;
				for (int i = 0; i < m_items_amount; i++)
				{
					if (m_selected_items[i])
					{
						num_items_selected++;
						selected_item = i;
					}
				}

				/// no items selected
				if (num_items_selected <= 0)
				{
					if (is_hovered)
						HELPERS::TGUI_DrawText(position + HELPERS::TGUI_Vector2D(6, size.y * 0.5 + 1), selected_font, false, true, text_item_color, command_info.filtered_name);
					else
						HELPERS::TGUI_DrawText(position + HELPERS::TGUI_Vector2D(5, size.y * 0.5), selected_font, false, true, text_item_color, command_info.filtered_name);
				}
				else if (num_items_selected == 1) /// a single item selected
				{
					if (is_hovered)
						HELPERS::TGUI_DrawText(position + HELPERS::TGUI_Vector2D(6, size.y * 0.5 + 1), selected_font, false, true, text_item_color, m_items[selected_item]);
					else
						HELPERS::TGUI_DrawText(position + HELPERS::TGUI_Vector2D(5, size.y * 0.5), selected_font, false, true, text_item_color, m_items[selected_item]);
				}
				else /// more than 1 item selected
				{
					char buffer[TGUI_MAX_STRING_LENGTH];
					sprintf(buffer, "%i items selected", num_items_selected);

					if (is_hovered)
						HELPERS::TGUI_DrawText(position + HELPERS::TGUI_Vector2D(6, size.y * 0.5 + 1), selected_font, false, true, text_item_color, buffer);
					else
						HELPERS::TGUI_DrawText(position + HELPERS::TGUI_Vector2D(5, size.y * 0.5), selected_font, false, true, text_item_color, buffer);
				}
			}
		}
	}
	void TGUI_Multiselect_Combobox_Command::TestInput(TGUI_Input& input)
	{
		auto &command_info = GetCommandInfo();
		auto &command_data = TGUI_command_data_base.Get<TGUI_Command_Data_Base::TGUI_Multiselect_Combobox_Command_Data>(GetDataHash(), command_info.command_type);

		if (command_info.flags & TGUI_COMMAND_FLAG_IS_DISABLED)
			return;

		const bool is_mouse_in_bounds = IsPointInBounds(input.current_mouse.position);
		const unsigned int font = tgui_fonts[TGUI_FONT_COMBOBOX_NAME];

		if (input.current_mouse.position.IsPointInBounds(command_info.position, command_info.position + command_info.size))
			command_info.flags |= TGUI_COMMAND_FLAG_IS_HOVERED;

		const bool is_in_bounds = IsRectInBounds(command_info.position, command_info.size);
		if (!is_in_bounds)
		{
			command_data.is_typing = false;
			command_data.is_combobox_open = false;
			command_data.current_searched_text[0] = '\0';
			command_data.scroll_amount = 0;
			command_data.times_pressed_enter = 0;
			return;
		}

		const int max_items_displayed = tgui_attributes[TGUI_ATTRIBUTE_COMBOBOX_MAX_ITEMS_DISPLAYED];
		const int scroll_offset = (max_items_displayed > m_items_amount) ? 0 : command_data.scroll_amount / command_info.size.y;
		const auto items_box_position = command_info.position + HELPERS::TGUI_Vector2D(0, command_info.size.y + 5);
		const auto items_box_size = HELPERS::TGUI_Vector2D(command_info.size.x, command_info.size.y * HELPERS::MIN(m_items_amount, max_items_displayed));
		const bool is_mouse_in_items_box = input.current_mouse.position.IsPointInBounds(items_box_position, items_box_position + items_box_size);

		/// If clicked
		if (input.DidLeftClick())
		{
			/// Selected an item
			if (command_data.is_combobox_open && is_mouse_in_items_box)
			{
				input.current_mouse.left_button = false;

				const int selected_item = HELPERS::CLAMP(((input.current_mouse.position.y - items_box_position.y) / command_info.size.y) + scroll_offset, 0, m_items_amount - 1);

				m_selected_items[selected_item] = !m_selected_items[selected_item];
			}

			/// Handles whether they clicked on the combobox or not
			if (command_info.flags & TGUI_COMMAND_FLAG_IS_HOVERED)
			{
				input.current_mouse.left_button = false;

				if (command_data.is_combobox_open && tgui_attributes[TGUI_ATTRIBUTE_COMBOBOX_MAX_ITEMS_DISPLAYED] < m_items_amount)
				{
					command_data.is_typing = true;
					command_data.current_searched_text[0] = '\0';
				}

				command_data.is_combobox_open = true;
			}
			else if (!is_mouse_in_items_box)
			{
				command_data.is_combobox_open = false;
				command_data.is_typing = false;
				command_data.times_pressed_enter = 0;
			}
		}
		else if (input.DidRealLeftClick())
		{
			command_data.is_combobox_open = false;
			command_data.is_typing = false;
			command_data.times_pressed_enter = 0;
		}


		/// Handle keyboard input
		if (command_data.is_typing)
		{
			/// Search for the text input in the items and set the 
			/// command_data.scroll_amount so that it appears at the top
			auto SearchForText = [this, max_items_displayed, scroll_offset, &command_data, command_info](const char* search_string) -> void
			{
				/// No text
				if (strlen(search_string) <= 0)
					return;

				/// Only search if you can scroll (lazy fucks)
				if (m_items_amount <= max_items_displayed)
					return;

				/// Change a c string to all lowercase
				auto ToLowercase = [](char* text) -> void
				{
					const int str_len = strlen(text);
					for (int i = 0; i < str_len; i++)
						text[i] = static_cast<char>(tolower(text[i]));
				};

				/// Get the lowercase version of the search string
				char lower_case_search_string[TGUI_MAX_STRING_LENGTH];
				strcpy(lower_case_search_string, search_string);
				ToLowercase(lower_case_search_string);

				/// Just find how many valid items there are
				int valid_items = 0;
				for (int i = 0; i < m_items_amount; i++)
				{
					/// Change the item to lowercase as well
					char lower_case_item[TGUI_MAX_STRING_LENGTH];
					strcpy(lower_case_item, m_items[i]);
					ToLowercase(lower_case_item);

					if (strstr(lower_case_item, lower_case_search_string))
						valid_items++;
				}

				/// No valid items, return
				if (valid_items <= 0)
					return;

				/// Iterate through all the items, this time searching for ones matching the search_string
				int best_index = scroll_offset, counter = 0;
				for (int i = 0; i < m_items_amount; i++)
				{
					char lower_case_item[TGUI_MAX_STRING_LENGTH];
					strcpy(lower_case_item, m_items[i]);
					ToLowercase(lower_case_item);

					/// If the search_string is a sub string of the item
					if (strstr(lower_case_item, lower_case_search_string))
					{
						if (command_data.times_pressed_enter % valid_items == counter)
						{
							best_index = i;
							break;
						}
						counter++;
					}
				}

				/// Clamp scroll amount in case it goes out of range
				command_data.scroll_amount = HELPERS::CLAMP(best_index, 0, m_items_amount - max_items_displayed) * command_info.size.y;
			};

			/// Whenever they press enter, it cycles to the next valid search item
			if (input.DidClickKey(VK_RETURN))
			{
				command_data.times_pressed_enter++;
				SearchForText(command_data.current_searched_text);
			}

			/// Process clicked keys
			const int string_length = strlen(command_data.current_searched_text);
			for (int i = 0; i < 256; i++)
			{
				if (TGUI_Input::IsValidKey(i) && input.DidClickKey(i))
				{
					command_data.current_searched_text[string_length] = TGUI_Input::KeyToChar(i);
					command_data.current_searched_text[string_length + 1] = '\0';
					SearchForText(command_data.current_searched_text);
					break;
				}
			}

			/// Escape key
			if (input.DidClickKey(VK_ESCAPE))
			{
				command_data.is_typing = false;
				command_data.is_combobox_open = false;
				command_data.current_searched_text[0] = '\0';
				command_data.times_pressed_enter = 0;
			}

			/// Backspace
			if (input.DidClickKey(VK_BACK) && string_length > 0)
				command_data.current_searched_text[string_length - 1] = '\0';
		}

		/// Handle scrolling
		if (input.current_mouse.scroll && command_data.is_combobox_open && is_mouse_in_items_box)
		{
			const int scroll_speed = tgui_attributes[TGUI_ATTRIBUTE_SCROLL_SPEED];
			command_data.scroll_amount = HELPERS::CLAMP<int>(command_data.scroll_amount - (input.current_mouse.scroll * scroll_speed),
				0, (m_items_amount - max_items_displayed) * command_info.size.y);

			input.current_mouse.scroll = 0;
		}
	}

	/// Color pickers
	TGUI_Color_Picker_Command::TGUI_Color_Picker_Command(const char* name, HELPERS::TGUI_Vector2D position, HELPERS::TGUI_Vector2D size, unsigned char &R, unsigned char &G, unsigned char &B, unsigned char &A, bool scroll_enabled)
	{
		auto &command_info = GetCommandInfo();

		m_red_ptr = &R;
		m_green_ptr = &G;
		m_blue_ptr = &B;
		m_alpha_ptr = &A;

		SetupCommandInfo(command_info, position, size, name, scroll_enabled);
	}
	void TGUI_Color_Picker_Command::Draw()
	{
		const auto command_info = GetCommandInfo();
		const auto command_data = TGUI_command_data_base.Get<TGUI_Command_Data_Base::TGUI_Color_Picker_Command_Data>(GetDataHash(), command_info.command_type);

		auto position = command_info.position, size = command_info.size;
		if (!GetClampedPositionAndSize(position, size))
			return;

		const int padding = tgui_attributes[TGUI_ATTRIBUTE_COLOR_PICKER_PADDING];
		const float alpha_scale = GetScrollAlphaScale() * command_info.alpha_modulation;

		const bool should_draw_text = IsRectInBounds(command_info.position, command_info.size);
		const bool is_hovered = command_info.flags & TGUI_COMMAND_FLAG_IS_HOVERED;
		const bool is_colorpicker_open = command_data.is_color_picker_open;

		const auto selected_color = HELPERS::TGUI_Color(*m_red_ptr, *m_green_ptr, *m_blue_ptr, *m_alpha_ptr);

		auto selected_body_color = HELPERS::TGUI_Color(*m_red_ptr, *m_green_ptr, *m_blue_ptr, 255);
		auto outline_color = tgui_colors[TGUI_COLOR_COLOR_PICKER_OUTLINE];
		selected_body_color.RGBA[3] *= alpha_scale, outline_color.RGBA[3] *= alpha_scale;

		HELPERS::TGUI_DrawRectangle(position, size, selected_body_color);
		HELPERS::TGUI_DrawRectangle(position, size, outline_color, true);

		if (is_colorpicker_open)
		{
			const auto color_picker_position = command_info.position + HELPERS::TGUI_Vector2D(0, command_info.size.y + padding);
			const auto color_picker_size = HELPERS::TGUI_Vector2D(tgui_attributes[TGUI_ATTRIBUTE_COLOR_PICKER_OPEN_WIDTH],
				tgui_attributes[TGUI_ATTRIBUTE_COLOR_PICKER_OPEN_HEIGHT]);

			const auto body_color = tgui_colors[TGUI_COLOR_COLOR_PICKER_BODY_COLOR];
			const auto body_outline_color = tgui_colors[TGUI_COLOR_COLOR_PICKER_OUTLINE];
			const int slider_width = tgui_attributes[TGUI_ATTRIBUTE_COLOR_PICKER_SLIDER_WIDTH];

			/// Color information, all in a range of 0 - 1
			const float current_hue = selected_color.Hue();
			const float current_saturation = selected_color.Saturation();
			const float current_brightness = selected_color.Brightness();
			const float current_alpha = (*m_alpha_ptr) / 255.f;

			/// Draw the body background
			HELPERS::TGUI_DrawRectangle(color_picker_position, color_picker_size, body_color);
			HELPERS::TGUI_DrawRectangle(color_picker_position, color_picker_size, body_outline_color, true);

			/// Draw the saturation and brightness sort of "gradient" box
			{
				const auto s_b_position = color_picker_position + HELPERS::TGUI_Vector2D(padding, padding);
				const auto s_b_size = color_picker_size - HELPERS::TGUI_Vector2D((padding * 4) + (slider_width * 2), (padding * 2));

				const float pixelation_amount = tgui_attributes[TGUI_ATTRIBUTE_COLOR_PICKER_PIXELATION];
				for (int x = 0; x < s_b_size.x; x += pixelation_amount)
				{
					const float x_frac = static_cast<float>(x) / static_cast<float>(s_b_size.x);
					for (int y = 0; y < s_b_size.y; y += pixelation_amount)
					{
						const float y_frac = static_cast<float>(y) / static_cast<float>(s_b_size.y);
						const auto clr = HELPERS::TGUI_Color::HSBtoRGB(current_hue, x_frac, 1.f - y_frac);

						auto pos = s_b_position + HELPERS::TGUI_Vector2D(x, y), sz = HELPERS::TGUI_Vector2D(pixelation_amount, pixelation_amount);
						HELPERS::TGUI_Vector2D::ClampToBounds(pos, sz, s_b_position, s_b_position + s_b_size);

						HELPERS::TGUI_DrawRectangle(pos, sz, clr);
					}
				}

				/// Outline
				HELPERS::TGUI_DrawRectangle(s_b_position, s_b_size, body_outline_color, true);

				/// A circle hovering over what the color currently is
				const auto highlight_size = HELPERS::TGUI_Vector2D(4, 4);
				const auto clr = selected_body_color.Inverse(true);
				HELPERS::TGUI_DrawRectangle(s_b_position + (HELPERS::TGUI_Vector2D(current_saturation * s_b_size.x,
					(1.f - current_brightness) * s_b_size.y) - (highlight_size * 0.5f)), highlight_size, clr, true);
			}
			/// Draw the hue slider
			{
				const auto slider_position = color_picker_position + HELPERS::TGUI_Vector2D(color_picker_size.x - ((padding * 2) + (slider_width * 2)), padding);
				const auto slider_size = HELPERS::TGUI_Vector2D(slider_width, color_picker_size.y - (padding * 2));

				for (int i = 0; i < slider_size.y; i++)
				{
					const float frac = static_cast<float>(i) / static_cast<float>(slider_size.y);
					const auto clr = HELPERS::TGUI_Color::HSBtoRGB(frac, 1.f, 1.f, 255);
					HELPERS::TGUI_DrawLine(slider_position + HELPERS::TGUI_Vector2D(0, i), HELPERS::TGUI_Vector2D(slider_size.x - 1, 0), clr);
				}

				/// Outline
				HELPERS::TGUI_DrawRectangle(slider_position, slider_size, body_outline_color, true);

				/// A line hovering over what the hue currently is
				HELPERS::TGUI_DrawLine(slider_position + HELPERS::TGUI_Vector2D(-2, current_hue * slider_size.y),
					HELPERS::TGUI_Vector2D(slider_size.x + 3, 0), HELPERS::TGUI_Color::White());
			}
			/// Draw the alpha slider
			{
				const auto slider_position = color_picker_position + HELPERS::TGUI_Vector2D(color_picker_size.x - (padding + slider_width), padding);
				const auto slider_size = HELPERS::TGUI_Vector2D(slider_width, color_picker_size.y - (padding * 2));

				constexpr float side_length = 4.f;

				/// Draw the checkered background
				int counter = 0;
				for (int y = 0; y < slider_size.y; y += side_length)
				{
					counter++;
					for (int x = 0; x < slider_size.x; x += side_length)
					{
						auto box_pos = slider_position + HELPERS::TGUI_Vector2D(x, y);
						auto box_size = HELPERS::TGUI_Vector2D(side_length, side_length);

						HELPERS::TGUI_Vector2D::ClampToBounds(box_pos, box_size, slider_position, slider_position + slider_size);

						const bool switch_color = (static_cast<int>((counter + (x / side_length))) % 2);
						HELPERS::TGUI_DrawRectangle(box_pos, box_size, switch_color ? HELPERS::TGUI_Color::White() : HELPERS::TGUI_Color::Black());

					}
				}
				/// Draw the actual alpha slider over the checkerboard
				for (int i = 0; i < slider_size.y; i++)
				{
					const float frac = static_cast<float>(i) / static_cast<float>(slider_size.y);
					const auto clr = HELPERS::TGUI_Color(255, 255, 255, frac * 255.f);

					HELPERS::TGUI_DrawLine(slider_position + HELPERS::TGUI_Vector2D(0, i), HELPERS::TGUI_Vector2D(slider_size.x - 1, 0), clr);
				}

				/// Outline
				HELPERS::TGUI_DrawRectangle(slider_position, slider_size, body_outline_color, true);

				/// A line hovering over what the hue currently is
				HELPERS::TGUI_DrawLine(slider_position + HELPERS::TGUI_Vector2D(-2, current_alpha * slider_size.y),
					HELPERS::TGUI_Vector2D(slider_size.x + 3, 0), HELPERS::TGUI_Color::White());
			}
		}

		/// Draw the color picker name
		if (should_draw_text)
		{
			const unsigned int font = tgui_fonts[TGUI_FONT_COLOR_PICKER];
			auto text_color = is_hovered ? tgui_colors[TGUI_COLOR_COLOR_PICKER_TEXT_HOVERED] : tgui_colors[TGUI_COLOR_COLOR_PICKER_TEXT];
			text_color.RGBA[3] *= alpha_scale;

			if (is_hovered)
				HELPERS::TGUI_DrawText(position + HELPERS::TGUI_Vector2D(size.x + 6, size.y * 0.5 + 1), font, false, true, text_color, command_info.filtered_name);
			else
				HELPERS::TGUI_DrawText(position + HELPERS::TGUI_Vector2D(size.x + 5, size.y * 0.5), font, false, true, text_color, command_info.filtered_name);
		}
	}
	void TGUI_Color_Picker_Command::TestInput(TGUI_Input &input)
	{
		auto &command_info = GetCommandInfo();
		auto &command_data = TGUI_command_data_base.Get<TGUI_Command_Data_Base::TGUI_Color_Picker_Command_Data>(GetDataHash(), command_info.command_type);

		if (command_info.flags & TGUI_COMMAND_FLAG_IS_DISABLED)
			return;

		/// Positions and sizes and shit
		const int padding = tgui_attributes[TGUI_ATTRIBUTE_COLOR_PICKER_PADDING];
		const int slider_width = tgui_attributes[TGUI_ATTRIBUTE_COLOR_PICKER_SLIDER_WIDTH];
		const auto color_picker_position = command_info.position + HELPERS::TGUI_Vector2D(0, command_info.size.y + padding);
		const auto color_picker_size = HELPERS::TGUI_Vector2D(tgui_attributes[TGUI_ATTRIBUTE_COLOR_PICKER_OPEN_WIDTH],
			tgui_attributes[TGUI_ATTRIBUTE_COLOR_PICKER_OPEN_HEIGHT]);

		const bool is_mouse_in_bounds = IsPointInBounds(input.current_mouse.position);
		const bool is_mouse_over_color_picker = input.current_mouse.position.IsPointInBounds(color_picker_position, color_picker_position + color_picker_size);

		const unsigned int font = tgui_fonts[TGUI_FONT_COLOR_PICKER];
		const bool is_mouse_over_preview = input.current_mouse.position.IsPointInBounds(command_info.position, command_info.position +
			HELPERS::TGUI_Vector2D(command_info.size.x + HELPERS::TGUI_GetTextSize(font, command_info.filtered_name).x + 5, command_info.size.y));

		/// Set hovered flag 
		if ((is_mouse_in_bounds && is_mouse_over_preview) || (command_data.is_color_picker_open && is_mouse_over_color_picker))
			command_info.flags |= TGUI_COMMAND_FLAG_IS_HOVERED;

		/// Clicked inside the actual color picker (not the preview rectangle)
		if (input.DidLeftHold() && is_mouse_over_color_picker && command_data.is_color_picker_open)
		{
			input.current_mouse.left_button = false;

			/// Positions and sizes 
			const auto s_b_position = color_picker_position + HELPERS::TGUI_Vector2D(padding, padding);
			const auto s_b_size = color_picker_size - HELPERS::TGUI_Vector2D((padding * 4) + (slider_width * 2), (padding * 2));

			const auto hue_slider_position = color_picker_position + HELPERS::TGUI_Vector2D(color_picker_size.x - ((padding * 2) + (slider_width * 2)), padding);
			const auto hue_slider_size = HELPERS::TGUI_Vector2D(slider_width, color_picker_size.y - (padding * 2));

			const auto alpha_slider_position = color_picker_position + HELPERS::TGUI_Vector2D(color_picker_size.x - (padding + slider_width), padding);
			const auto alpha_slider_size = HELPERS::TGUI_Vector2D(slider_width, color_picker_size.y - (padding * 2));

			/// What did they click on?
			const bool did_click_alpha_slider = input.current_mouse.position.IsPointInBounds(alpha_slider_position, alpha_slider_position + alpha_slider_size);
			const bool did_click_hue_slider = input.current_mouse.position.IsPointInBounds(hue_slider_position, hue_slider_position + hue_slider_size);
			const bool did_click_saturation_brightness = input.current_mouse.position.IsPointInBounds(s_b_position, s_b_position + s_b_size);

			/// Handle according to what they clicked
			if (did_click_alpha_slider)
			{
				const float frac = (input.current_mouse.position.y - alpha_slider_position.y) / static_cast<float>(alpha_slider_size.y);
				*m_alpha_ptr = frac * 255.f;
			}
			else if (did_click_hue_slider)
			{
				const float frac = (input.current_mouse.position.y - hue_slider_position.y) / static_cast<float>(hue_slider_size.y);

				const auto current_color = HELPERS::TGUI_Color(*m_red_ptr, *m_green_ptr, *m_blue_ptr);
				const auto new_color = HELPERS::TGUI_Color::HSBtoRGB(frac, current_color.Saturation(), current_color.Brightness());

				*m_red_ptr = new_color.RGBA[0];
				*m_green_ptr = new_color.RGBA[1];
				*m_blue_ptr = new_color.RGBA[2];
			}
			else if (did_click_saturation_brightness)
			{
				const float y_frac = (input.current_mouse.position.y - s_b_position.y) / static_cast<float>(s_b_size.y);
				const float x_frac = (input.current_mouse.position.x - s_b_position.x) / static_cast<float>(s_b_size.x);

				const auto current_color = HELPERS::TGUI_Color(*m_red_ptr, *m_green_ptr, *m_blue_ptr);
				const auto new_color = HELPERS::TGUI_Color::HSBtoRGB(current_color.Hue(), x_frac, 1.f - y_frac);

				*m_red_ptr = new_color.RGBA[0];
				*m_green_ptr = new_color.RGBA[1];
				*m_blue_ptr = new_color.RGBA[2];
			}
		}

		/// Left clicked
		if (input.DidRealLeftClick())
		{
			if (command_info.flags & TGUI_COMMAND_FLAG_IS_HOVERED && input.DidLeftClick())
			{
				input.current_mouse.left_button = false;
				command_data.is_color_picker_open = true;
			}
			else
				command_data.is_color_picker_open = false;
		}
	}

	/// Tabs
	TGUI_Tab_Command::TGUI_Tab_Command(HELPERS::TGUI_Vector2D position, HELPERS::TGUI_Vector2D size, int &variable, char(*tabs)[TGUI_MAX_STRING_LENGTH], int tabs_amount, bool scroll_enabled)
	{
		auto &command_info = GetCommandInfo();

		variable = HELPERS::CLAMP(variable, 0, tabs_amount - 1);
		m_variable_ptr = &variable;
		m_tabs_amount = tabs_amount;
		m_tabs = tabs;
		command_info.command_type = TGUI_Command_Types::TGUI_COMMAND_TYPE_TAB;

		SetupCommandInfo(command_info, position, size, "", scroll_enabled);
	}
	void TGUI_Tab_Command::Draw()
	{
		const auto command_info = GetCommandInfo();

		auto position = command_info.position, size = command_info.size;
		if (!GetClampedPositionAndSize(position, size))
			return;

		const float alpha_scale = GetScrollAlphaScale() * command_info.alpha_modulation;
		const bool draw_text = IsRectInBounds(command_info.position, command_info.size);

		const unsigned int item_font = tgui_fonts[TGUI_FONT_TAB_SELECTED_ITEM_NAME];
		const unsigned int selected_font = tgui_fonts[TGUI_FONT_TAB_SELECTED_ITEM_NAME];

		auto tab_color = tgui_colors[TGUI_COLOR_TAB_BODY];
		auto tab_selected_color = tgui_colors[TGUI_COLOR_TAB_BODY_SELECTED];
		tab_color.RGBA[3] *= alpha_scale, tab_selected_color.RGBA[3] *= alpha_scale;

		auto text_color = tgui_colors[TGUI_COLOR_TAB_TEXT];
		auto text_selected_color = tgui_colors[TGUI_COLOR_TAB_TEXT_SELECTED];
		text_color.RGBA[3] *= alpha_scale, text_selected_color.RGBA[3] *= alpha_scale;

		auto outline_color = tgui_colors[TGUI_COLOR_TAB_OUTLINE];
		outline_color.RGBA[3] *= alpha_scale;

		const float padding = tgui_attributes[TGUI_ATTRIBUTE_TAB_ITEM_PADDING];
		const float size_per_tab = ((size.x - ((m_tabs_amount - 1) * padding)) / static_cast<float>(m_tabs_amount));

		/// Iterate through each tab
		for (int i = 0; i < m_tabs_amount; i++)
		{
			const auto pos = HELPERS::TGUI_Vector2D(position.x + ((size_per_tab + padding) * i), position.y);

			/// If selected
			if (i == *m_variable_ptr)
			{
				HELPERS::TGUI_DrawRectangle(pos - HELPERS::TGUI_Vector2D(1, 1), HELPERS::TGUI_Vector2D(size_per_tab + 2, size.y + 2), tab_selected_color);

				if (draw_text)
					HELPERS::TGUI_DrawText(pos + HELPERS::TGUI_Vector2D(size_per_tab * 0.5, size.y * 0.5), selected_font, true, true, text_selected_color, m_tabs[i]);

				/// Outline
				HELPERS::TGUI_DrawRectangle(pos - HELPERS::TGUI_Vector2D(1, 1), HELPERS::TGUI_Vector2D(size_per_tab + 2, size.y + 2), outline_color, true);
			}
			else /// If not selected
			{
				HELPERS::TGUI_DrawRectangle(pos, HELPERS::TGUI_Vector2D(size_per_tab, size.y), tab_color);

				if (draw_text)
					HELPERS::TGUI_DrawText(pos + HELPERS::TGUI_Vector2D(size_per_tab * 0.5, size.y * 0.5), item_font, true, true, text_color, m_tabs[i]);

				/// Outline
				HELPERS::TGUI_DrawRectangle(pos, HELPERS::TGUI_Vector2D(size_per_tab, size.y), outline_color, true);
			}

		}
	}
	void TGUI_Tab_Command::TestInput(TGUI_Input &input)
	{
		auto &command_info = GetCommandInfo();

		if (command_info.flags & TGUI_COMMAND_FLAG_IS_DISABLED)
			return;

		const bool is_mouse_in_bounds = IsPointInBounds(input.current_mouse.position);

		const float padding = tgui_attributes[TGUI_ATTRIBUTE_TAB_ITEM_PADDING];
		const float size_per_tab = ((command_info.size.x - ((m_tabs_amount - 1) * padding)) / static_cast<float>(m_tabs_amount));

		if (input.DidLeftClick() && is_mouse_in_bounds)
		{
			/// Iterate through each tab cuz too lazy to do math
			for (int i = 0; i < m_tabs_amount; i++)
			{
				const auto pos = HELPERS::TGUI_Vector2D(command_info.position.x + ((size_per_tab + padding) * i), command_info.position.y);

				if (input.current_mouse.position.IsPointInBounds(pos, pos + HELPERS::TGUI_Vector2D(size_per_tab, command_info.size.y)))
				{
					*m_variable_ptr = i;
					break;
				}
			}
		}
	}

	/// Tool tips
	TGUI_Tool_Tip_Command::TGUI_Tool_Tip_Command(const char* description, int lines_amount, HELPERS::TGUI_Vector2D offset, TGUI_Command* parent_command)
	{
		auto &command_info = GetCommandInfo();

		command_info.command_type = TGUI_Command_Types::TGUI_COMMAND_TYPE_TOOL_TIP;
		m_parent_command = parent_command;
		m_lines_amount = lines_amount;

		const auto previous_command_info = parent_command->GetCommandInfo();

		const bool scroll_enabled = previous_command_info.flags & TGUI_COMMAND_FLAG_SCROLL_ENABLED;

		const int font = tgui_fonts[TGUI_FONT_TOOL_TIP];
		const int line_height = tgui_attributes[TGUI_ATTRIBUTE_TOOL_TIP_LINE_HEIGHT];

		const auto text_size = HELPERS::TGUI_GetFilteredTextWidth(description, font);
		const auto size = HELPERS::TGUI_Vector2D(text_size / lines_amount + 10, line_height * lines_amount);
		const auto position = menu.GetInput().current_mouse.position + offset;

		SetupCommandInfo(command_info, position, size, description, scroll_enabled);
	}
	void TGUI_Tool_Tip_Command::Draw()
	{
		const auto command_info = GetCommandInfo();
		const auto command_data = TGUI_command_data_base.Get<TGUI_Command_Data_Base::TGUI_Tool_Tip_Command_Data>(GetDataHash(), command_info.command_type);

		const auto description = command_info.filtered_name;
		const auto description_length = strlen(description);

		const int font = tgui_fonts[TGUI_FONT_TOOL_TIP];
		const int line_size = tgui_attributes[TGUI_ATTRIBUTE_TOOL_TIP_LINE_HEIGHT];
		const int size_per_line = description_length / m_lines_amount;

		/// Draw
		{
			const float alpha_modifier = HELPERS::CLAMP(command_data.time_hovered / 1.f, 0.f, 1.f);

			/// Colors
			auto text_color = tgui_colors[TGUI_COLOR_TOOL_TIP_TEXT];
			auto body_color = tgui_colors[TGUI_COLOR_TOOL_TIP_BODY];
			auto outline_color = tgui_colors[TGUI_COLOR_TOOL_TIP_OUTLINE];
			text_color.RGBA[3] *= alpha_modifier, body_color.RGBA[3] *= alpha_modifier, outline_color.RGBA[3] *= alpha_modifier;

			/// Draw background
			HELPERS::TGUI_DrawRectangle(command_info.position, command_info.size, body_color);
			HELPERS::TGUI_DrawRectangle(command_info.position, command_info.size, outline_color, true);

			auto current_position = command_info.position + HELPERS::TGUI_Vector2D(5, static_cast<int>(line_size * 0.5));
			for (int i = 0; i < m_lines_amount; i++)
			{
				/// Copy a portion of the description into the current_line
				char current_line[TGUI_MAX_STRING_LENGTH];
				memcpy(current_line, description + uintptr_t(size_per_line * i), size_per_line);
				current_line[size_per_line] = '\0';

				HELPERS::TGUI_DrawText(current_position, font, false, true, text_color, current_line);

				current_position.y += line_size;
			}
		}
	}
	void TGUI_Tool_Tip_Command::TestInput(TGUI_Input &input)
	{
		auto &command_info = GetCommandInfo();
		auto &command_data = TGUI_command_data_base.Get<TGUI_Command_Data_Base::TGUI_Tool_Tip_Command_Data>(GetDataHash(), command_info.command_type);

		const auto parent_command_info = m_parent_command->GetCommandInfo();
		const auto min = parent_command_info.position, max = min + parent_command_info.space;

		if (m_parent_command->IsPointInBounds(input.current_mouse.position) && input.current_mouse.position.IsPointInBounds(min, max))
			command_data.time_hovered += input.update_delta_time;
		else
			command_data.time_hovered -= input.update_delta_time * 2.f;

		command_data.time_hovered = HELPERS::CLAMP<float>(command_data.time_hovered, -0.5f, 1.f);
	}
}