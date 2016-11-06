#include "stdafx.h"
#include "node.h"
#include <limits>



Node::Node() : id(undefined)
{
	child[0] = NULL;
	child[1] = NULL;
}

Node::~Node()
{
	if (child[0])
		delete child[0];

	if (child[1])
		delete child[1];
}

Node::Fit Node::CanFit(const Rect &tex_rc)
{
	Rect r1 = rc.GetNormal(), r2 = tex_rc.GetNormal();

	if (r1 == r2)
		return PERFECTLY;

	if (r1 < r2)
		return TOO_SMALL;

	return TOO_LARGE;
}

Node * Node::Insert(const Texture &tex)
{
	// Not a leaf
	if (child[0] != NULL)
	{
		Node *newNode = child[0]->Insert(tex);

		if (newNode != NULL)
			return newNode;

		return child[1]->Insert(tex);
	}
	else
	{
		// Node has image id 
		if (id != undefined)
			return NULL;

		// Can image fit in node ?
		Fit fit = CanFit(tex.rc);

		if (fit == TOO_SMALL)
			return NULL;

		if (fit == PERFECTLY)
			return this;

		child[0] = new Node();
		child[1] = new Node();

		int tex_width = tex.rc.GetWidth();
		int tex_height = tex.rc.GetHeight();

		if (rc.GetWidth() - tex_width > rc.GetHeight() - tex_height)
		{
			std::vector<Rect> r = rc.SplitVertically(tex_width);
			child[0]->rc = r[0];
			child[1]->rc = r[1];
		}
		else
		{
			std::vector<Rect> r = rc.SplitHorizontally(tex_height);
			child[0]->rc = r[0];
			child[1]->rc = r[1];
		}

		return child[0]->Insert(tex);
	}
}
