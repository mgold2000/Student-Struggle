#pragma once

#include "Card.h"
#include "Object.h"
#include "Random.h"
#include "EventTimer.h"

enum class PlayerState { WaitingForInput, MovingTowardsCenter, Attacking, Returning, Returned };

class Player : public CObject, CCommon
{
	public:
		Player(const Vector2& p, float height);

		void TakeDamage(int);
		int useCard(int);
		void ResetShield() { shield = 0; }
		void createCards();
		void CreateStartDeck();
		void shuffleCards();

		bool IsDead() { return health == 0; }
		void draw();
		void move();
		void PlayCard(const Vector2& center);
		void ReturnToPosition();
		PlayerState GetState() { return state; }
		void SetBack();
		std::vector<Card*> GetDeck() { return deck; }

		bool FinishedAttacking();
		void SetCard(int card);

		void SetUnavailable();
		void SetNormal();
		void Reset();

	private:
		int health;
		int shield = 0;

		PlayerState state;
		Vector2 target;
		Vector2 oldPosition;
		const float speed = 460.0f;
		const float diff = 15.0f;
		float height;
		LEventTimer* animationTimer = nullptr;
		int bookIndex;
		LEventTimer* damageTimer = nullptr;
		int currCardIndex;
		float attackingTime;

		std::vector<Card*> deck;

		std::string getHeadText() { return std::to_string(health);  }
		std::string getShieldText() { return std::to_string(shield); }
		void UpdateFrame();
		void UpdateBook();
};