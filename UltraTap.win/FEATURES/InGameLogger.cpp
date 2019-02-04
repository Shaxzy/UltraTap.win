#include "../includes.h"

#include "../UTILS/render.h"

#include "InGameLogger.h"

InGameLogger in_game_logger;

void ColorLine::Draw(int x, int y, unsigned int font)
{
	for (int i = 0; i < texts.size(); i++)
	{
		RENDER::DrawF(x, y, font, false, false, colors[i], texts[i]);
		x += RENDER::GetTextSize(font, texts[i]).x;
	}
}

void InGameLogger::Do()
{
	if (log_queue.size() > max_lines_at_once)
		log_queue.erase(log_queue.begin() + max_lines_at_once, log_queue.end());

	for (int i = 0; i < log_queue.size(); i++)
	{
		auto log = log_queue[i];
		float time_delta = fabs(UTILS::GetCurtime() - log.time);

		int height = ideal_height + (16 * i);

		/// erase dead logs
		if (time_delta > text_time)
		{
			log_queue.erase(log_queue.begin() + i);
			break;
		}
		if (time_delta > text_time - slide_out_speed)
			height = height + (((slide_out_speed - (text_time - time_delta)) / slide_out_speed) * slide_out_distance);

		/// fade out
		if (time_delta > text_time - text_fade_out_time)
			log.color_line.ChangeAlpha(255 - (((time_delta - (text_time - text_fade_out_time)) / text_fade_out_time) * 255.f));
		/// fade in
		if (time_delta < text_fade_in_time)
			log.color_line.ChangeAlpha((time_delta / text_fade_in_time) * 255.f);

		int width = ideal_width;

		/// slide from left
		if (time_delta < text_fade_in_time)
			width = (time_delta / text_fade_in_time) * static_cast<float>(slide_in_distance) + (ideal_width - slide_in_distance);
		/// slider from right
		if (time_delta > text_time - text_fade_out_time)
			width = ideal_width + (((time_delta - (text_time - text_fade_out_time)) / text_fade_out_time) * static_cast<float>(slide_out_distance));

		log.color_line.Draw(width, height, FONTS::in_game_logging_font);
	}
}