#include <stdafx.h>
#include <stdlib.h>
#include <stdio.h>

#include <2DLib/buffer.h>
#include "ft.h"

FT::FT()
{

}

FT::~FT()
{

}

void FT::SetFGColor(const Color& color)
{
	fg_color = color;
}

void FT::SetFont(const char *f)
{
	font = f;
}

void FT::DrawBitmap(Buffer &buffer, int x, int y)
{
	size_t keep = (size_t)x;
	unsigned char *ptr = slot->bitmap.buffer;

	for (size_t j=0; j<slot->bitmap.rows; j++, y++)
	{
		x = (int)keep;

		for (size_t i=0; i<slot->bitmap.width; i++, ptr++, x++)
		{
			// Get alpha from the slot's bitmap
			unsigned char xxx = *ptr;
			fg_color.a = (float)*ptr / 255.f;

			buffer.Set(Point(x, y), fg_color);
		}
	}
} 

void FT::RenderCharacter(unsigned long unicode, int& w, int& h, int& l, int&t)
{
	int error;
	FT_UInt glyph_index;

	///*char *to_write = */Unicode_Char2Num(&unicode, c);
//UTF8_To_Unicode(unicode, c);
	
	/* retrieve glyph index from character code */
	glyph_index = FT_Get_Char_Index( face, unicode);

	/* load glyph image into the slot (erase previous one) */
	error = FT_Load_Glyph( face, glyph_index, FT_LOAD_DEFAULT );

	if ( error )
	{
		fprintf(stderr, "Could not load glyph\n");
		exit(0);
	}

	/* convert to an anti-aliased bitmap */
	error = FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL );

	/* ignore errors, just warn the user */
	if ( error )
	{
		fprintf(stderr, "Could not render glyph\n");
		exit(0);
	}

	slot = face->glyph;

	w = slot->bitmap.width;
	h = slot->bitmap.rows;
	l = slot->bitmap_left;
	t = slot->bitmap_top;
}

int FT::FreeTypeSetup(int char_width, int char_height, int hor_res, int vert_res)
{
	int error = FT_Init_FreeType( &library );

	if ( error ) 
	{
		fprintf(stderr, "Error\n");
		return EXIT_FAILURE;
	}

	error = FT_New_Face( library, font, 0, &face );

	if ( error == FT_Err_Unknown_File_Format )
	{
		fprintf(stderr, "Error: Unknown file format\n");
		return EXIT_FAILURE;
	}
	else if ( error)
	{
		fprintf(stderr, "Error\n");
		return EXIT_FAILURE;
	}

	error = FT_Set_Char_Size( face, /* handle to face object */
								char_width, /* char_width in 1/64th of points */
								char_height, /* char_height in 1/64th of points */
								hor_res, /* horizontal device resolution */
								vert_res ); /* vertical device resolution */ 

	return (error) ? EXIT_FAILURE : EXIT_SUCCESS;
}
