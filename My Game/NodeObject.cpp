#pragma once

#include "NodeObject.h"

void NodeObject::draw()
{
	if (state == GameState::Map)
	{
		CObject::draw();

		if (m_nSpriteIndex != (UINT)eSprite::Nerd)
		{
			std::string s = std::to_string(numEnemies);
			m_pRenderer->DrawScreenText(s.c_str(), Vector2(m_vPos.x - 13, LSettings::m_nWinHeight - m_vPos.y - 35), Colors::White); //draw to screen
		}
		
		if (complete)
		{
			LSpriteDesc2D checkmarkDesc;
			checkmarkDesc.m_nSpriteIndex = (UINT)eSprite::Checkmark;
			checkmarkDesc.m_vPos = m_vPos + Vector2(0, -15);
			checkmarkDesc.m_fXScale = 0.05f;
			checkmarkDesc.m_fYScale = 0.05f;

			m_pRenderer->Draw(&checkmarkDesc);
		}
	}
}

NodeObject::NodeObject(const Vector2& p) : CObject(eSprite::DoorClosed, p) 
{
	//m_f4Tint = Vector4(0.75, 0.25, 0.0, 1.0);
	m_fXScale = 0.05f;
	m_fYScale = 0.05f;
}

void NodeObject::SetSpecial()
{
	m_nSpriteIndex = (UINT)eSprite::Nerd;
	m_fXScale = 0.6f;
	m_fYScale = 0.6f;
}
