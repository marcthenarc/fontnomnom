#include "stdafx.h"

#include "texturemap.h"
#include <bitset>
#include <algorithm>

#include "read.h"

// Sort textures by area size, biggest to smallest.
static bool texture_sort(Texture a, Texture b)
{
	return (a.rc.GetWidth() * a.rc.GetHeight() > b.rc.GetWidth() * b.rc.GetHeight());
}

unsigned int ReadBits(const Buffer &b, int pos)
{
	std::bitset<16> bits;

	// When reading, start one cell lower then 0. (cf. DrawBits());
	for (int i = 0; i < 16; i++)
		bits[i] = (b.Get(Point(pos, i + 1)) == RGBA::White);

	return (unsigned int)bits.to_ulong();
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
	std::vector<bool> keep;

	Point start = Point::Origin;

	while (b.Scan(start, Point::Origin.MaxRight(), Buffer::HORZ, Buffer::MUST_FIND, RGBA::Red, hit))
	{
		// But first, let's scan the check box to see if we keep it
		Rect rc = checkbox + Point(hit.X + 5, size.H - checkbox.GetHeight() - 2);
		rc.Shrink(1);

		if (!b.IsRectEmpty(rc))
		{
			redPos.push_back(hit.X);

			const int bits = ReadBits(b, hit.X);

			if (bits == 0)
				break;

			utf16s.push_back(bits);
		}
		
		start = hit + Point(1, 0);
	}

	// ####################### Get each character's rectangle.
	for (size_t i = 0; i<redPos.size() - 1; i++)
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

