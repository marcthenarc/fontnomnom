#include "stdafx.h"
#include "texture.h"

Texture::Texture()
{

}

Texture::~Texture()
{

}

void Texture::SetLowest(int y)
{
	diff_lowest = y - rc.GetBottomRight().Y;
}
