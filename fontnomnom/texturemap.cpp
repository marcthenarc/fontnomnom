#include "stdafx.h"
#include <cstdlib>
#include <fstream>
#include "texturemap.h"

TextureMap::TextureMap()
{
	root = new Node();
}

TextureMap::~TextureMap()
{
	if (root)
		delete root;
}

void TextureMap::Create(int w, int h)
{
	Reset(Size(w, h), RGBA::NoAlpha);
	root->rc = Rect(Point(0, 0), Size(w, h));
}

void TextureMap::Insert(const Texture& tex)
{
	textures.push_back(tex);
}

/* Store()

	Store coordinate values in JSON.
*/
void TextureMap::Store(int unicode, int diff_lowest, const Rect &rc)
{
	Json::Value arr;
	arr.append(rc.left);
	arr.append(rc.top);
	arr.append(rc.right);
	arr.append(rc.bottom);

	Json::Value character;
	character["unicode"] = unicode;
	character["rect"] = arr;
	character["diff_lowest"] = diff_lowest;

	json.append(character);
}

/* Save()

	Save JSON content to file.
*/
void TextureMap::SaveJSON(const std::string& jsonName, const std::string& pngName)
{
	Json::Value object;
	object["fontmap"]["atlas"] = pngName;
	object["fontmap"]["pots"].append(size.W);
	object["fontmap"]["pots"].append(size.H);
	object["fontmap"]["characters"] = json;

	std::ofstream ofs(jsonName.c_str());
	ofs << object;
}

std::vector<Texture> TextureMap::Pack(const Buffer& buffer)
{
	std::vector<Texture> overflow;

	size_t max = textures.size();

	for (size_t i=0; i<max; i++)
	{
		Texture tex = textures[i];

		Rect rc = tex.rc.GetNormal();

	    Node *pnode = root->Insert(tex);

		if (pnode != NULL)
		{
			pnode->id = i;
		}
	    else
    	   overflow.push_back(tex);
	}

	Draw(root, buffer);

	return overflow;
}

void TextureMap::Draw(Node *n, const Buffer &from)
{
	if (!n)
		return;

	if (n->id != Node::undefined)
	{
		Texture tex = textures[n->id];
		Store(tex.unicode, tex.diff_lowest, n->rc);

		CopyRectFromBuffer(n->rc, tex.rc, from);
	}

	Draw(n->child[0], from);
	Draw(n->child[1], from);
}

