#pragma once

#include "Game.h"

#include "GameDefines.h"
#include "SpriteRenderer.h"
#include "ComponentIncludes.h"

class Node : CCommon
{
	public:
		int id;
		int layer;
		Vector2 position;
		bool unlocked = false;
		int numEnemies;
		bool special = false;
};
