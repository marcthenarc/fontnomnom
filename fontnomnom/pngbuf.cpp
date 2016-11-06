#include <cstdlib>
#include <cstdio>
#include "pngbuf.h"

PNG::PNG()
{

}

PNG::~PNG()
{
	if (row_pointers)
	{
		for (int y=0; y<m_nH; y++)
			delete [] row_pointers[y];

		delete [] row_pointers;
	}
}

void PNG::Create(int w, int h)
{
	m_nW = w;
	m_nH = h;

	row_pointers = new png_bytep[sizeof(png_bytep) * m_nH];

	for (int i=0; i<h; i++)
		row_pointers[i] = new png_byte[w*4];

	m_nColorType = 6;
	m_nBitDepth = 8;
}

void PNG::SaveFromBuffer(Buffer &b, const char *filename)
{
	int w = b.GetWidth(), h = b.GetHeight();

	Create(w, h);

	for (int j=0; j<h; j++)
	{
		for (int i=0; i<w; i++)
			SetPixel(i, j, b.GetPixel(i,j));
	}

	Write(filename);
}

void PNG::Read(const char *filename)
{
	png_structp png_ptr;
	//int number_of_passes;
	png_infop info_ptr;
	png_byte header[8];    // 8 is the maximum size that can be checked

	/* open file and test for it being a png */
	FILE *fp = fopen(filename, "r");

	if (!fp)
		PNG_Exception(filename, "[read_png_file] could open file for reading");

	fread(header, 1, 8, fp);

	if (png_sig_cmp(header, 0, 8))
		PNG_Exception(filename, "[read_png_file] File %s is not recognized as a PNG file");

	/* initialize stuff */
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr)
		PNG_Exception(filename, "[read_png_file] png_create_read_struct failed");

	info_ptr = png_create_info_struct(png_ptr);

    if (!info_ptr)
		PNG_Exception(filename, "[read_png_file] png_create_info_struct failed");

	if (setjmp(png_jmpbuf(png_ptr)))
		PNG_Exception(filename, "[read_png_fileme, ] Error during init_io");

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);

	m_nW = png_get_image_width(png_ptr, info_ptr);
	m_nH = png_get_image_height(png_ptr, info_ptr);
	m_nColorType = png_get_color_type(png_ptr, info_ptr);
	m_nBitDepth = png_get_bit_depth(png_ptr, info_ptr);

	//number_of_passes = png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr, info_ptr);

	/* read file */
	if (setjmp(png_jmpbuf(png_ptr)))
		PNG_Exception(filename, "[read_png_file] Error during read_image");

	row_pointers = new png_bytep[sizeof(png_bytep) * m_nH];

	for (int y=0; y<m_nH; y++)
		row_pointers[y] = new png_byte[png_get_rowbytes(png_ptr,info_ptr)];

	png_read_image(png_ptr, row_pointers);
/*
	if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGB)
	    PNG_Exception(filename, "[process_file] input file is PNG_COLOR_TYPE_RGB but must be PNG_COLOR_TYPE_RGBA "
                       "(lacks the alpha channel)");

	if (png_get_color_type(png_ptr, info_ptr) != PNG_COLOR_TYPE_RGBA)
    	PNG_Exception(filename, "[process_file] color_type of input file must be PNG_COLOR_TYPE_RGBA (%d) (is %d)",
        PNG_COLOR_TYPE_RGBA, png_get_color_type(png_ptr, info_ptr));
*/
	fclose(fp);
}

void PNG::Write(const char *filename)
{
	png_structp png_ptr;
	//int number_of_passes;
	png_infop info_ptr;
//	png_byte header[8];    // 8 is the maximum size that can be checked

 	/* create file */
	FILE *fp = fopen(filename, "w");

    if (!fp)
		PNG_Exception(filename, "[write_png_file] File %s could not be opened for writing");

	/* initialize stuff */
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr)
		PNG_Exception(filename, "[write_png_file] png_create_write_struct failed");

	info_ptr = png_create_info_struct(png_ptr);

	if (!info_ptr)
		PNG_Exception(filename, "[write_png_file] png_create_info_struct failed");

	if (setjmp(png_jmpbuf(png_ptr)))
		PNG_Exception(filename, "[write_png_file] Error during init_io");

	png_init_io(png_ptr, fp);

	/* write header */
	if (setjmp(png_jmpbuf(png_ptr)))
		PNG_Exception(filename, "[write_png_file] Error during writing header");

	png_set_IHDR(png_ptr, info_ptr, m_nW, m_nH,
		m_nBitDepth, m_nColorType, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);

	/* write bytes */
	if (setjmp(png_jmpbuf(png_ptr)))
		PNG_Exception(filename, "[write_png_file] Error during writing bytes");

	png_write_image(png_ptr, row_pointers);

	/* end write */
	if (setjmp(png_jmpbuf(png_ptr)))
		PNG_Exception(filename, "[write_png_file] Error during end of write");

	png_write_end(png_ptr, NULL);

	fclose(fp);
}

int PNG::GetWidth()
{
	return m_nW;
}

int PNG::GetHeight()
{
	return m_nH;
}

Color PNG::GetPixel(int x, int y)
{
    png_byte* row = row_pointers[y];
    png_byte* ptr = &(row[x*4]);

	return Color(ptr[0], ptr[1], ptr[2], ptr[3]);
}

void PNG::SetPixel(int x, int y, const Color &c)
{
    png_byte* row = row_pointers[y];
    png_byte* ptr = &(row[x*4]);

	ptr[0] = c.r;
	ptr[1] = c.g;
	ptr[2] = c.b;
	ptr[3] = c.a;
}

void PNG::LoadInBuffer(Buffer &b)
{
	b.Create(m_nW, m_nH);

	for (int j=0; j<m_nH; j++)
	{
		for (int i=0; i<m_nW; i++)
			b.SetPixel(i, j, GetPixel(i, j));
	}
}
