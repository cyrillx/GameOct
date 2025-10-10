#pragma once

#include "Scene.h"

class Region final
{
public:
	void Init();
	void Close();

	Entity2 model;
};

extern Region gRegion;