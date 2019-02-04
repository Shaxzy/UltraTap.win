class ColorLine
{
public:
	ColorLine() {}

	ColorLine(std::string text, CColor color = WHITE)
	{
		texts.push_back(text);
		colors.push_back(color);
	}

	void PushBack(std::string text, CColor color = WHITE)
	{
		texts.push_back(text);
		colors.push_back(color);
	}

	void PushFront(std::string text, CColor color = WHITE)
	{
		texts.insert(texts.begin(), text);
		colors.insert(colors.begin(), color);
	}

	void ChangeAlpha(int alpha)
	{
		for (auto& color : colors)
			color.RGBA[3] = alpha;
	}

	void Draw(int x, int y, unsigned int font);

	std::string Text()
	{
		std::string text;
		for (const auto string : texts)
			text += string;

		return text;
	}

private:
	std::vector<std::string> texts;
	std::vector<CColor> colors;
};

class InGameLogger
{
public:
	struct Log
	{
		Log()
		{
			time = UTILS::GetCurtime();
		}

		Log(std::string text, CColor color = WHITE)
		{
			color_line = ColorLine(text, color);
			time = UTILS::GetCurtime();
		}

		ColorLine color_line;
		float time;
	};

public:
	void Do();

	void AddLog(Log log)
	{
		log_queue.insert(log_queue.begin(), log);
	}

private:
	std::vector<Log> log_queue;

private:
	const float text_time = 5.f;
	const float text_fade_in_time = 0.3f;
	const float text_fade_out_time = 0.2f;

	const int max_lines_at_once = 10;

	const int ideal_height = 20;
	const int ideal_width = 20;

	const int slide_in_distance = 20;
	const int slide_out_distance = 20;
	const float slide_out_speed = 0.2f;
};

extern InGameLogger in_game_logger;
