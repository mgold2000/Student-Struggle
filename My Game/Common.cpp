/// \file Common.cpp
/// \brief Code for the class CCommon.

#include "Common.h"

LSpriteRenderer* CCommon::m_pRenderer = nullptr;
CObjectManager* CCommon::m_pObjectManager = nullptr;
Player* CCommon::player = nullptr;
LRandom* CCommon::rng = nullptr;
int CCommon::enemyUpdateIndex = -1;
GameState CCommon::state = GameState::Menu;