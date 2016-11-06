#pragma once

#include "texture.h"

class Node
{
	enum Fit { TOO_SMALL, PERFECTLY, TOO_LARGE };

public:

	Node *child[2];
	Rect rc;
	size_t id;

	static const size_t undefined = std::numeric_limits<size_t>::max();

	Node();
	virtual ~Node();

	Node * Insert(const Texture &tex);
	Fit CanFit(const Rect &rc);
};

