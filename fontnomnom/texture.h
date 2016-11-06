#pragma once

#include <2DLib/rect.h>

class Texture
{

public:

	unsigned long unicode;
	int diff_lowest;
	Rect rc;;

	Texture();
	~Texture();
	void SetLowest(int y);

};

