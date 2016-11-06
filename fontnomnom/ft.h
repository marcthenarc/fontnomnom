#pragma once

#include <2DLib/buffer.h>
#include <ft2build.h>
#include FT_FREETYPE_H

class FT
{
	const char *font;
	FT_Library library; /* handle to library */
	FT_Face face; /* handle to face object */ 
	Color fg_color;
	FT_GlyphSlot slot;


public:

	FT();
	~FT();

	void SetFGColor(const Color& color);
	void SetFont(const char *f);
	int FreeTypeSetup(int char_width, int char_height, int hor_res, int vert_res);
	void RenderCharacter(unsigned long unicode, int& w, int& h, int& l, int&t);
	void DrawBitmap(Buffer &buffer, int x, int y);
};
