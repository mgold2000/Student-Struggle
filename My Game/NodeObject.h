#pragma once

#include "Object.h"
#include "Common.h"

class NodeObject : public CObject, CCommon, LSettings
{
	public:
		NodeObject(const Vector2& p);
		void draw();
		bool complete = false;
		int numEnemies = 0;

		void SetSpecial();
};