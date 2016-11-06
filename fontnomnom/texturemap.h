#pragma once

#include <string>
#include <vector>
#include <json/json.h>
#include <2DLib/buffer.h>

#include "texture.h"
#include "node.h"

#define INVALID_HANDLE 0xFFFF

class TextureMap : public Buffer
{
	std::string file_output;
	std::vector<Texture> textures;
	Node *root;

	Json::Value json;
	
	void Store(int unicode, int diff_lowest, const Rect &rc);
	void Draw(Node *n, const Buffer &from);


public:

	TextureMap();
	~TextureMap();
	void Create(int w, int h);
	void Insert(const Texture& tex);
	void SaveJSON(const std::string& jsonName, const std::string& pngName);
	std::vector<Texture> Pack(const Buffer& buffer);
};
