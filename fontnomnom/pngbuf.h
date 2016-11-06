#pragma once

#include <png.h>
#include <string>
#include "color.h"
#include "buffer.h"

using namespace std;

class PNG_Exception
{
	string m_strFilename, m_strError;

public:

	PNG_Exception(const string &fn, const string &err) : m_strFilename(fn), m_strError(err) { }
	~PNG_Exception() { }
	string GetError() { return m_strError; }
};

class PNG
{
protected:
	int m_nW, m_nH;
	png_bytep * row_pointers;
	png_byte m_nColorType;
	png_byte m_nBitDepth;

public:
	PNG();
	~PNG();
	void Create(int h, int w);
	void Read(const char *filename);
	void Write(const char *filename);
	int GetWidth();
	int GetHeight();
	Color GetPixel(int x, int y);
	void SetPixel(int x, int y, const Color &c);
	void SaveFromBuffer(Buffer &buffer, const char *filename);
	void LoadInBuffer(Buffer &b);
};

