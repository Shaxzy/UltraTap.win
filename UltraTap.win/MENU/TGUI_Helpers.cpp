#include "../includes.h"

#include "../UTILS/render.h"
#include "../UTILS/interfaces.h"
#include "../SDK/IEngine.h"

#include "TGUI_Helpers.h"

namespace TGUI
{
	namespace HELPERS
	{
		void TGUI_DrawRectangle(TGUI_Vector2D position, TGUI_Vector2D size, TGUI_Color color, bool outline)
		{
			if (outline)
				RENDER::DrawEmptyRect(position.x - 1, position.y - 1, position.x + size.x, position.y + size.y, TGUI_Color::ToCColor(color));
			else
				RENDER::DrawFilledRect(position.x, position.y, position.x + size.x, position.y + size.y, TGUI_Color::ToCColor(color));
		}
		void TGUI_DrawText(TGUI_Vector2D position, unsigned int font, bool center_width, bool center_height, TGUI_Color color, const char* text)
		{
			RENDER::DrawF(position.x, position.y, font, center_width, center_height, TGUI_Color::ToCColor(color), text);
		}
		void TGUI_DrawLine(TGUI_Vector2D position, TGUI_Vector2D size, TGUI_Color color)
		{
			RENDER::DrawLine(position.x, position.y, position.x + size.x, position.y + size.y, TGUI_Color::ToCColor(color));
		}
		void TGUI_DrawLineEx(TGUI_Vector2D pos1, TGUI_Vector2D pos2, TGUI_Color color)
		{
			RENDER::DrawLine(pos1.x, pos1.y, pos2.x, pos2.y, TGUI_Color::ToCColor(color));
		}
		void TGUI_DrawCircle(TGUI_Vector2D position, TGUI_Color color, int radius, int segments, bool filled)
		{
			if (filled)
				RENDER::DrawFilledCircle(position.x, position.y, radius, segments, TGUI_Color::ToCColor(color));
			else
				RENDER::DrawCircle(position.x, position.y, radius, segments, TGUI_Color::ToCColor(color));

		}
		int TGUI_GetFilteredTextWidth(const char* name, int font)
		{
			char filtered_name[256];

			strcpy(filtered_name, name);
			TGUI_FilterText(filtered_name);

			return TGUI_GetTextSize(font, filtered_name).x;
		}

		TGUI_Vector2D TGUI_GetTextSize(unsigned int font, const char* text)
		{
			return RENDER::GetTextSize(font, text);
		}
		TGUI_Vector2D TGUI_GetScreenSize()
		{
			int width, height;
			INTERFACES::Engine->GetScreenSize(width, height);

			return TGUI_Vector2D(width, height);
		}
	}
}