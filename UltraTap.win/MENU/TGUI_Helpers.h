#pragma once

#include <time.h>

namespace TGUI
{
	namespace HELPERS
	{
		template <class T>
		constexpr const T MIN(const T& x, const T& y)
		{
			return (x > y ? y : x);
		}
		template <class T>
		constexpr const T MAX(const T& x, const T& y)
		{
			return (x < y ? y : x);
		}
		template <class T>
		constexpr const T CLAMP(const T& x, const T& low, const T& high)
		{
			return (x >= low && x <= high) ? x : (x < low ? low : high);
		}

		/// 32 bit FNV-1a hash, used for creating command ID's
		constexpr uint32_t TGUI_FNVHash(const void* const data, const int &len)
		{
			constexpr uint32_t fnv_prime = 16777619;
			constexpr uint32_t fnv_offset = 2166136261;

			const auto bytes = reinterpret_cast<const unsigned char*>(data);

			uint32_t hash = fnv_offset;
			for (int i = 0; i < len; i++)
				hash = (hash ^ bytes[i]) * fnv_prime;

			return hash;
		}

		/// Removes everything after ##
		inline void TGUI_FilterText(char* text)
		{
			for (int i = 0; text[i]; i++)
			{
				if (text[i] == '#' && text[i + 1] == '#')
				{
					text[i] = '\0';
					break;
				}
			}
		};

		/// All time related shit uses this function
		inline float TGUI_GetTime()
		{
			timeBeginPeriod(1);
			return timeGetTime() * 0.001f;
		}

		constexpr void TGUI_Assert(bool statement, const char* message = "")
		{
			if (!statement)
				throw std::exception(message);
		}

		class TGUI_Color
		{
		public:
			unsigned char RGBA[4];

		public:
			TGUI_Color()
			{
				RGBA[0] = 255;
				RGBA[1] = 255;
				RGBA[2] = 255;
				RGBA[3] = 255;
			}
			TGUI_Color(int r, int g, int b, int a = 255)
			{
				RGBA[0] = r;
				RGBA[1] = g;
				RGBA[2] = b;
				RGBA[3] = a;
			}

			static constexpr float Base(const unsigned char &element)
			{
				return element / 255.f;
			}

			static TGUI_Color Inverse(const TGUI_Color &color, bool ignore_alpha = false)
			{
				return TGUI_Color(255 - color.RGBA[0], 255 - color.RGBA[1], 255 - color.RGBA[2], ignore_alpha ? color.RGBA[3] : 255 - color.RGBA[3]);
			}
			TGUI_Color Inverse(bool ignore_alpha = false) const
			{
				return Inverse(*this, ignore_alpha);
			}

			static CColor ToCColor(const TGUI_Color &color) { return CColor(color.RGBA[0], color.RGBA[1], color.RGBA[2], color.RGBA[3]); }

			TGUI_Color AlphaModulate(float frac) { RGBA[3] *= frac; return *this; }

			// RGB -> HSB conversions
			static float Hue(const TGUI_Color &color)
			{
				float R = Base(color.RGBA[0]);
				float G = Base(color.RGBA[1]);
				float B = Base(color.RGBA[2]);

				float mx = max(R, max(G, B));
				float mn = min(R, min(G, B));
				if (mx == mn)
					return 0.f;

				float delta = mx - mn;

				float hue = 0.f;
				if (mx == R)
					hue = (G - B) / delta;
				else if (mx == G)
					hue = 2.f + (B - R) / delta;
				else
					hue = 4.f + (R - G) / delta;

				hue *= 60.f;
				if (hue < 0.f)
					hue += 360.f;

				return hue / 360.f;
			}
			static float Saturation(const TGUI_Color &color)
			{
				float R = Base(color.RGBA[0]);
				float G = Base(color.RGBA[1]);
				float B = Base(color.RGBA[2]);

				float mx = max(R, max(G, B));
				float mn = min(R, min(G, B));

				float delta = mx - mn;

				if (mx == 0.f)
					return delta;

				return delta / mx;
			}
			static float Brightness(const TGUI_Color &color)
			{
				float R = Base(color.RGBA[0]);
				float G = Base(color.RGBA[1]);
				float B = Base(color.RGBA[2]);

				return max(R, max(G, B));
			}

			float Hue() const
			{
				return Hue(*this);
			}
			float Saturation() const
			{
				return Saturation(*this);
			}
			float Brightness() const
			{
				return Brightness(*this);
			}

			// HSB -> RGB conversions
			static TGUI_Color HSBtoRGB(float hue        /* 0.f - 1.f */,
				float saturation /* 0.f - 1.f */,
				float brightness /* 0.f - 1.f */,
				int alpha = 255  /* 0   - 255 */)
			{
				hue = UTILS::clamp(hue, 0.f, 1.f);
				saturation = UTILS::clamp(saturation, 0.f, 1.f);
				brightness = UTILS::clamp(brightness, 0.f, 1.f);

				float h = (hue == 1.f) ? 0.f : (hue * 6.f);
				float f = h - static_cast<int>(h);
				float p = brightness * (1.f - saturation);
				float q = brightness * (1.f - saturation * f);
				float t = brightness * (1.f - (saturation * (1.f - f)));

				if (h < 1.f)
					return TGUI_Color(brightness * 255.f, t * 255.f, p * 255.f, alpha);
				else if (h < 2.f)
					return TGUI_Color(q * 255.f, brightness * 255.f, p * 255.f, alpha);
				else if (h < 3.f)
					return TGUI_Color(p * 255.f, brightness * 255.f, t * 255.f, alpha);
				else if (h < 4)
					return TGUI_Color(p * 255.f, q * 255.f, brightness * 255.f, alpha);
				else if (h < 5)
					return TGUI_Color(t * 255.f, p * 255.f, brightness * 255.f, alpha);
				else
					return TGUI_Color(brightness * 255.f, p * 255.f, q * 255.f, alpha);
			}

			static TGUI_Color White() { return TGUI_Color(255, 255, 255, 246); }
			static TGUI_Color Black() { return TGUI_Color(0, 0, 0); }
			static TGUI_Color Red() { return TGUI_Color(255, 0, 0); }
			static TGUI_Color Green() { return TGUI_Color(0, 255, 0); }
			static TGUI_Color Blue() { return TGUI_Color(0, 0, 255); }
			static TGUI_Color LightBlue() { return TGUI_Color(6, 137, 255, 246); }

		private:
		};
		class TGUI_Vector2D
		{
		public:
			int x, y;

		public:
			TGUI_Vector2D() { x = 0; y = 0; }
			TGUI_Vector2D(int X, int Y) { x = X; y = Y; }
			TGUI_Vector2D(Vector2D vec) { x = vec.x; y = vec.y; }

			TGUI_Vector2D operator+(const TGUI_Vector2D &vec) const { return TGUI_Vector2D(x + vec.x, y + vec.y); }
			TGUI_Vector2D operator-(const TGUI_Vector2D &vec) const { return TGUI_Vector2D(x - vec.x, y - vec.y); }
			TGUI_Vector2D operator*(const TGUI_Vector2D &vec) const { return TGUI_Vector2D(x * vec.x, y * vec.y); }
			TGUI_Vector2D operator/(const TGUI_Vector2D &vec) const { return TGUI_Vector2D(x / vec.x, y / vec.y); }

			TGUI_Vector2D operator+(const float &val) const { return TGUI_Vector2D(x + val, y + val); }
			TGUI_Vector2D operator-(const float &val) const { return TGUI_Vector2D(x - val, y - val); }
			TGUI_Vector2D operator*(const float &val) const { return TGUI_Vector2D(x * val, y * val); }
			TGUI_Vector2D operator/(const float &val) const { return TGUI_Vector2D(x / val, y / val); }

			void operator+=(const TGUI_Vector2D &vec) { x += vec.x; y += vec.y; }


			/// Check whether the vector is within the bounds of min and max
			bool IsPointInBounds(const TGUI_Vector2D &min, const TGUI_Vector2D &max) const
			{
				if (x < min.x || y < min.y)
					return false;

				if (x > max.x || y > max.y)
					return false;

				return true;
			}
			static inline bool IsPointInBounds(const TGUI_Vector2D &point, const TGUI_Vector2D &min, const TGUI_Vector2D &max)
			{
				if (point.x < min.x || point.y < min.y)
					return false;

				if (point.x > max.x || point.y > max.y)
					return false;

				return true;
			}

			/// If points is not withen bounds_min and bounds_max, return false
			static inline bool ClampToBounds(TGUI_Vector2D &clamped_min, TGUI_Vector2D &clamped_size, const TGUI_Vector2D &bounds_min, const TGUI_Vector2D &bounds_max)
			{
				const auto clamped_max = clamped_min + clamped_size;

				const bool is_min_in_bounds = IsPointInBounds(clamped_min, bounds_min, bounds_max);
				const bool is_max_in_bounds = IsPointInBounds(clamped_max, bounds_min, bounds_max);

				/// Neither min or max is inside bounds
				if (!is_min_in_bounds && !is_max_in_bounds)
					return false;

				/// Clamp the mins and maxs
				if (is_min_in_bounds) /// Max needs to be clamped
				{
					if (clamped_max.y > bounds_max.y)
						clamped_size.y = bounds_max.y - clamped_min.y;

					if (clamped_max.x > bounds_max.x)
						clamped_size.x = bounds_max.x - clamped_min.x;
				}
				else /// Min needs to be clamped
				{
					if (clamped_min.x < bounds_min.x)
						clamped_min.x = bounds_min.x;

					if (clamped_min.y < bounds_min.y)
						clamped_min.y = bounds_min.y;

					clamped_size = clamped_max - clamped_min;
				}

				return true;
			}

		private:
		};

		/// To make it more portable, just need to link these rendering functions, how input is taken and the fonts
		void TGUI_DrawRectangle(TGUI_Vector2D position, TGUI_Vector2D size, TGUI_Color color, bool outline = false);
		void TGUI_DrawText(TGUI_Vector2D position, unsigned int font, bool center_width, bool center_height, TGUI_Color color, const char* text);
		void TGUI_DrawLine(TGUI_Vector2D position, TGUI_Vector2D size, TGUI_Color color);
		void TGUI_DrawLineEx(TGUI_Vector2D pos1, TGUI_Vector2D pos2, TGUI_Color color);
		void TGUI_DrawCircle(TGUI_Vector2D position, TGUI_Color color, int radius, int segments, bool filled = false);
		int TGUI_GetFilteredTextWidth(const char* name, int font);
		TGUI_Vector2D TGUI_GetTextSize(unsigned int font, const char* text);
		TGUI_Vector2D TGUI_GetScreenSize();
	}
}