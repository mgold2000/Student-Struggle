#include "Card.h"
#include "Enemy.h"
#include "Player.h"

Card::Card(const Vector2& p) : CObject(eSprite::Card, p)
{
	dmgAmount = 0;
	shieldAmount = 0;
	healthAmount = 0;

	Unselect();
	hovered = false;
}

void Card::createCard(int dAmount, int sAmount, int hAmount)
{
	dmgAmount = dAmount;
	shieldAmount = sAmount;
	healthAmount = hAmount;

	if (dmgAmount > 0)
	{
		m_nSpriteIndex = (UINT)eSprite::CardDamage;
	}
	else if (shieldAmount > 0)
	{
		m_nSpriteIndex = (UINT)eSprite::CardShield;
	}
	else if (healthAmount > 0)
	{
		m_nSpriteIndex = (UINT)eSprite::CardHealth;
	}
}

int Card::dealDamage()
{
	return dmgAmount;
}

int Card::giveShield()
{
	return shieldAmount;
}

int Card::giveHealth()
{
	return healthAmount;
}

void Card::draw()
{
	if (state == GameState::Battle || state == GameState::NewCard)
	{
		CObject::draw();
		std::string s;

		if (this->dmgAmount != 0) {
			s = std::to_string(this->dmgAmount);
		}
		else if (this->shieldAmount != 0) {
			s = std::to_string(this->shieldAmount);
		}
		else if (this->healthAmount != 0) {
			s = std::to_string(this->healthAmount);
		}
		
		m_pRenderer->DrawScreenText(s.c_str(), Vector2(m_vPos.x - 13, LSettings::m_nWinHeight - m_vPos.y - 30), Colors::Black); //draw to screen
	}
}

void Card::RemoveCard(int pos)
{
	switch (pos) {
	case 0:
		m_vPos.y -= 515;
		break;
	case 1:
		m_vPos.x -= 80;
		m_vPos.y -= 515;
		break;
	case 2:
		m_vPos.x -= 160;
		m_vPos.y -= 515;
		break;
	case 3:
		m_vPos.x -= 240;
		m_vPos.y -= 515;
		break;
	case 4:
		m_vPos.x -= 320;
		m_vPos.y -= 515;
		break;
	case 5:
		m_vPos.y -= 515;
		break;
	case 6:
		m_vPos.x -= 80;
		m_vPos.y -= 515;
		break;
	case 7:
		m_vPos.x -= 160;
		m_vPos.y -= 515;
		break;
	case 8:
		m_vPos.x -= 240;
		m_vPos.y -= 515;
		break;
	case 9:
		m_vPos.x -= 320;
		m_vPos.y -= 515;
		break;
	default:
		break;
	}
}

void Card::ReplaceCard()
{
	m_vPos.y += 515;
}

void Card::RepositionCard(int pos) {
	switch (pos) {
	case 1:
		m_vPos.x += 80;
		break;
	case 2:
		m_vPos.x += 160;
		break;
	case 3:
		m_vPos.x += 240;
		break;
	case 4:
		m_vPos.x += 320;
		break;
	case 6:
		m_vPos.x += 80;
		break;
	case 7:
		m_vPos.x += 160;
		break;
	case 8:
		m_vPos.x += 240;
		break;
	case 9:
		m_vPos.x += 320;
		break;
	default:
		break;
	}
}

void Card::AddCardMove(int pos) {
	switch (pos) {
	case 0:
		m_vPos.x -= 200;
		m_vPos.y += 725;
		break;
	case 1:
		m_vPos.x -= 120;
		m_vPos.y += 725;
		break;
	case 2:
		m_vPos.x -= 40;
		m_vPos.y += 725;
		break;
	case 3:
		m_vPos.x += 40;
		m_vPos.y += 725;
		break;
	case 4:
		m_vPos.x += 120;
		m_vPos.y += 725;
		break;
	case 5:
		m_vPos.x += 200;
		m_vPos.y += 725;
		break;
	case 6:
		m_vPos.x += 280;
		m_vPos.y += 725;
		break;
	case 7:
		m_vPos.x += 360;
		m_vPos.y += 725;
		break;
	case 8:
		m_vPos.x += 440;
		m_vPos.y += 725;
		break;
	case 9:
		m_vPos.x += 520;
		m_vPos.y += 725;
		break;
	default:
		break;
	}
}

void Card::AddCardRemove(int pos) {
	switch (pos) {
	case 0:
		m_vPos.x += 200;
		m_vPos.y -= 725;
		break;
	case 1:
		m_vPos.x += 120;
		m_vPos.y -= 725;
		break;
	case 2:
		m_vPos.x += 40;
		m_vPos.y -= 725;
		break;
	case 3:
		m_vPos.x -= 40;
		m_vPos.y -= 725;
		break;
	case 4:
		m_vPos.x -= 120;
		m_vPos.y -= 725;
		break;
	case 5:
		m_vPos.x -= 200;
		m_vPos.y -= 725;
		break;
	case 6:
		m_vPos.x -= 280;
		m_vPos.y -= 725;
		break;
	case 7:
		m_vPos.x -= 360;
		m_vPos.y -= 725;
		break;
	case 8:
		m_vPos.x -= 440;
		m_vPos.y -= 725;
		break;
	case 9:
		m_vPos.x -= 520;
		m_vPos.y -= 725;
		break;
	default:
		break;
	}
}

void Card::ReplaceAllCards() {
	if (m_vPos.y < 0)
		m_vPos.y += 515;
}

void Card::RemoveAllCards() {
	if (m_vPos.y > 0)
		m_vPos.y -= 515;
}

void Card::Select()
{
	m_f4Tint = Vector4(0.05, 0.05, 0.7, 1.0); 
	//m_vPos.y += 15;
}

void Card::Unselect()
{
	m_f4Tint = Vector4(0.05, 0.7, 0.05, 1.0); 
	hovered = true;
}

void Card::SetUsed()
{
	m_f4Tint = Vector4(0.05, 0.7, 0.05, 1.0);
	m_vPos.y -= 15;
	hovered = false;
}

void Card::Hover()
{
	if (!hovered)
	{
		m_vPos.y += 15;
		hovered = true;
	}
}

void Card::Unhover()
{
	if (hovered)
	{
		m_vPos.y -= 15;
		hovered = false;
	}
}

void Card::UpgradeAllCards()
{
	if (dmgAmount > 0)
	{
		dmgAmount += 2;
	}
	else if (healthAmount > 0)
	{
		healthAmount += 2;
	}
	else if (shieldAmount > 0)
	{
		shieldAmount += 2;
	}
}

void Card::UpgradeCard()
{
	if (dmgAmount > 0)
	{
		dmgAmount += 1;
	}
	else if (healthAmount > 0)
	{
		healthAmount += 1;
	}
	else if (shieldAmount > 0)
	{
		shieldAmount += 1;
	}
}

void Card::Reset()
{
	//Unselect();
	Unhover();
	m_f4Tint = Vector4(0.05, 0.7, 0.05, 1.0);
}
