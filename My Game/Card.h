#pragma once

#include "Object.h"

class Card : public CObject, LSettings
{
private:
	int dmgAmount;
	int shieldAmount;
	int healthAmount;

	bool hovered;

public:
	Card(const Vector2& p);
	void createCard(int, int, int);

	int dealDamage();
	int giveShield();
	int giveHealth();

	void Select();
	void Unselect();
	
	void draw();
	void RemoveCard(int pos);
	void RemoveAllCards();
	void ReplaceCard();
	void RepositionCard(int pos);
	void ReplaceAllCards();
	void AddCardMove(int);
	void AddCardRemove(int);

	void Hover();
	void Unhover();
	void SetUsed();
	void UpgradeAllCards();
	void UpgradeCard();
	void Reset();
};
