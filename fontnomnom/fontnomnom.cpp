#include "stdafx.h"
#include <2DLib/buffer.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <bitset>
#include <algorithm>
#include <locale>
#include <codecvt>

#include "texturemap.h"
#include "ft.h"

static const int border = 20;

void DrawBits(Buffer& b, int pos, int utf16)
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
		b.DrawRect(s, (i%2) ? RGBA::Blue : RGBA::Cyan);

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
		b.DrawVerticalLine(Point(sep, 0), Point(sep, border), (i % 2) ? RGBA::Blue : RGBA::Cyan);
		DrawBits(b, sep, theString[i]);
		b.Set(Point(sep, 0), RGBA::Red);
		std::cout << nb - i << std::endl;
	}

	// ##### Draw green from side up to border - 1.
	b.DrawHorizontalLine(Point(0, mid), Point(border - 1, mid), RGBA::Green);
	b.Save("01-params.png", true);

	b.Reset(RGBA::Black);
	b.Save("00-fond.png", true);
}

unsigned int ReadBits(const Buffer &b, int pos)
{
	std::bitset<16> bits;

	// When reading, start one cell lower then 0. (cf. DrawBits());
	for (int i = 0; i < 16; i++)
		bits[i] = (b.Get(Point(pos, i + 1)) == RGBA::White);

	return (unsigned int)bits.to_ulong();
}

// Sort textures by area size, biggest to smallest.
bool texture_sort(Texture a, Texture b)
{
	return (a.rc.GetWidth() * a.rc.GetHeight() > b.rc.GetWidth() * b.rc.GetHeight());
}

bool Pack(Buffer &from, std::vector<Texture> &textures, int pot, const std::string& atlas)
{
	TextureMap tmap;
	tmap.Create(pot, pot);

	// Sort textures, insert them into the map engine and pack them.
	std::sort(textures.begin(), textures.end(), texture_sort);

	size_t max = textures.size();

	for (size_t it = 0; it < max; it++)
		tmap.Insert(textures[it]);

	if (tmap.Pack(from).size())
		return false;

	std::string jsonName = atlas + ".json";
	std::string pngName = atlas + ".png";

	tmap.SaveJSON(jsonName, pngName);
	tmap.Save(pngName);

	std::cout << "Conversion complétée: " << jsonName << ", " << pngName << std::endl;

	return true;
}

void ReadTemplate(std::string& name)
{
	// ######################## Load template
	Buffer b;
	b.Load(name);
	b.Sanitize();

	Size size = b.GetSize();

	// ####################### Get the green line mid.
	int mid;
	Point hit;

	if (!b.Scan(Point::Origin, Point::Origin.MaxDown(), Buffer::VERT, Buffer::MUST_FIND, RGBA::Green, hit))
		throw "Cannot find green mid point";

	mid = hit.Y;

	// ####################### Get each red separation dot and decode the bit signature for each character.
	std::vector<int> redPos;
	std::vector<unsigned int> utf16s;
	std::vector<Texture> textures;

	Point start = Point::Origin;

	while (b.Scan(start, Point::Origin.MaxRight(), Buffer::HORZ, Buffer::MUST_FIND, RGBA::Red, hit))
	{
		redPos.push_back(hit.X);

		const int bits = ReadBits(b, hit.X);

		if (bits == 0)
			break;

		utf16s.push_back(bits);
				
		start = hit + Point(1, 0);
	}

	// ####################### Get each character's rectangle.
	for (size_t i=0; i<redPos.size() - 1; i++)
	{
		Point p(redPos[i], border + 1);
		Point q(redPos[i + 1], size.H - border - 1);

		textures.push_back(Texture());

		Texture &t = textures[textures.size() - 1];
		t.unicode = utf16s[i];
		t.rc = b.IsolateRect(Rect(p, q), RGBA::NoAlpha);
		t.SetLowest(mid);
	}

	// ####################### Pack
	int pot = 32;

	while (Pack(b, textures, pot, "atlas") == false)
		pot <<= 1;
}

std::wstring stringToWstring(const std::string& t_str)
{
	//setup converter
	typedef std::codecvt_utf8<wchar_t> convert_type;
	std::wstring_convert<convert_type, wchar_t> converter;

	//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
	return converter.from_bytes(t_str);
}

int main(int argc, char **argv)
{
	int ret = EXIT_SUCCESS;

	if (argc < 2)
	{
		CreateTemplate(Rect(Point(), Size(100, 200)), L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmonpqrstuvwxyz.!,;:\"'#/$%?&*()_+=");
	}
	else
	{
		std::string file = argv[1];

		if (file.substr(file.size() - 4) == ".png")
		{
			try
			{
				ReadTemplate(file);
			}
			catch (PNG_Exception e)
			{
				std::cerr << e.GetError() << std::endl;
			}
			catch (const char *e)
			{
				std::cerr << e << " : aborting." << std::endl;
				ret = EXIT_FAILURE;
			}
		}
		else
		{
			std::ifstream ifs(file.c_str());

			if (ifs)
			{
				bool success = false;
				Json::Value root;

				{
					Json::Reader reader;
					success = reader.parse(ifs, root);
				}

				if (success && root["fontnomnom"].isObject())
				{
					Json::Value fnn = root["fontnomnom"];

					if (fnn["size"].isArray() && fnn["size"][0].isInt() && fnn["size"][1].isInt() && fnn["string"].isString())
					{					
						std::wstring ws = stringToWstring(fnn["string"].asString());				
						CreateTemplate(Rect(Point(), Size(fnn["size"][0].asInt(), fnn["size"][1].asInt())), ws);
					}
				}
			}
		}
	}

	system("pause");
	return ret;
}