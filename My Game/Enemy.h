#pragma once

#include "Object.h"
#include "Random.h"
#include "EventTimer.h"

enum class EnemyState { InPosition, MovingTowardsCenter, PlayingCard, Returning, Returned };
enum class EnemyAttack { EndlessHomework, Lame };

enum class EnemyCardType { Attack, Heal };

struct EnemyCard
{
	EnemyCardType type;
	int value;
};

class Enemy : public CObject
{
	public:
		Enemy(const Vector2& p, float height);
		bool TakeDamage(int);
		EnemyCard GetCard();
		void draw();
		void move();
		void PlayCard(const Vector2& center);
		void ReturnToPosition();
		EnemyState GetState() { return state; }
		void SetBack() { state = EnemyState::InPosition; }

		int health;
		std::string getHeadText() { return std::to_string(health); }

		bool FinishedAttacking();
		void SetBoss();
		void Heal(int amount);

		void SetUnavailable();
		void SetNormal();
		void Kill();

	private:
		EnemyState state;
		const float speed = 460.0f;
		const float diff = 15.0f;
		Vector2 target;
		Vector2 oldPosition;
		float height;
		LEventTimer* animationTimer = nullptr;
		float attackingTime;
		const float attackEnd = 2.5f;
		EnemyAttack attack;
		LEventTimer* damageTimer = nullptr;
		EnemyCard nextCard;

		void UpdateFrame();
};
