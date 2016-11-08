#include "stdafx.h"

#include <bitset>
#include "ft.h"
#include "create.h"

static void DrawBits(Buffer& b, int pos, int utf16)
{
	std::bitset<16> bits = utf16;

	// When drawing, start one cell lower then 0 so that the original blue indicates
	// clearly the separator.
	for (int i = 0; i < 16; i++)
		b.Set(Point(pos, i + 1), (bits[i]) ? RGBA::White : RGBA::Black);
}

void CreateTemplate(const Rect& rect, const std::wstring theString)
{
	size_t nb = theString.size();

	const int width = rect.GetWidth() * (int)nb + 2 * border;
	const int height = rect.GetHeight() + 2 * border;
	const int mid = height / 2;

	Buffer b(Size(width, height), RGBA::NoAlpha);

	// ##### Draw rects for each characters
	Rect s = rect + Point(border, border);

	for (int i = 0; i < nb; i++, s += Point(rect.GetWidth(), 0))
		b.DrawRect(s, (i % 2) ? RGBA::Blue : RGBA::Cyan);

	b.DrawHorizontalLine(Point(border, mid), Point(width - border - 1, mid), RGBA::Green);
	b.Save("02-guide.png", true);

	// ##### Draw the red rect (a.k.a: the danger zone, beyond which there be dragons).
	b.Reset(RGBA::NoAlpha);
	Rect t(Point(border, border), Point(width - border - 1, s.bottom));
	b.DrawRect(t, RGBA::Red);
	b.Save("03-danger.png", true);
	b.Reset(RGBA::NoAlpha);

	// ##### Draw reference characters 
	FT ft;
	ft.SetFont("C:\\Windows\\Fonts\\YuGothic-Bold.ttf");
	ft.FreeTypeSetup(100, 0, 800, 0);
	ft.SetFGColor(RGBA::White);
	int font_width, font_height, font_left, font_right;

	int start = border + 10;

	for (auto c : theString)
	{
		ft.RenderCharacter(c, font_width, font_height, font_left, font_right);
		ft.DrawBitmap(b, start, rect.bottom - 3 - font_height);
		start += rect.GetWidth();
	}

	b.Save("04-font.png");
	b.Reset(RGBA::NoAlpha);

	// ##### The parameters zone
	for (int i = 0, sep = border; i <= nb; i++, sep += rect.GetWidth())
	{
		// Info line
		b.DrawVerticalLine(Point(sep, 0), Point(sep, border), (i % 2) ? RGBA::Blue : RGBA::Cyan);
		DrawBits(b, sep, theString[i]);
		b.Set(Point(sep, 0), RGBA::Red);

		if (i < nb)
		{
			// Draw the checkbox
			Rect rc = checkbox + Point(sep + 5, height - checkbox.GetHeight() - 2);
			b.DrawRect(rc, RGBA::Yellow);
		}
	}

	// Draw green from side up to border - 1.
	b.DrawHorizontalLine(Point(0, mid), Point(border - 1, mid), RGBA::Green);
	b.Save("01-params.png", true);

	b.Reset(RGBA::Black);
	b.Save("00-fond.png", true);
}