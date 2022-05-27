#include "Enemy.h"
#include "ComponentIncludes.h"
#include "Helpers.h"

Enemy::Enemy(const Vector2& p, float height) : CObject(eSprite::Enemy, p)
{
	health = 10;
	state = EnemyState::InPosition;
	this->height = height;
	animationTimer = new LEventTimer(0.1f);
	attack = EnemyAttack::EndlessHomework;
	damageTimer = new LEventTimer(0.3f);
}

bool Enemy::TakeDamage(int amount)
{
	health = std::max(0, health - amount);

	if (health == 0)
		m_bDead = true;

	m_f4Tint = Vector4(0.9f, 0.4f, 0.4f, 1.0f);
	damageTimer = new LEventTimer(0.3f);
	m_pAudio->play(eSound::EnemyDamage);

	return m_bDead;
}

EnemyCard Enemy::GetCard()
{
	return nextCard;
}

void Enemy::PlayCard(const Vector2& center)
{
	state = EnemyState::MovingTowardsCenter;
	target = center;
	oldPosition = m_vPos;


	if (m_nSpriteIndex == (UINT)eSprite::Enemy)
		m_nSpriteIndex = (UINT)eSprite::EnemyRunning;
}

void Enemy::ReturnToPosition()
{
	state = EnemyState::Returning;
	target = oldPosition;
}

void Enemy::draw()
{
	if (CCommon::state == GameState::Battle)
	{
		CObject::draw();

		const std::string s = this->getHeadText();
		m_pRenderer->DrawScreenText(s.c_str(), Vector2(m_vPos.x - 25, height - m_vPos.y - 125), Colors::White); //draw to screen

		if (state == EnemyState::PlayingCard)
		{
			if (nextCard.type == EnemyCardType::Attack)
			{
				if (attack == EnemyAttack::EndlessHomework)
				{
					LSpriteDesc2D paperDesc;
					paperDesc.m_nSpriteIndex = (UINT)eSprite::Paper;
					paperDesc.m_fXScale = 0.35f;
					paperDesc.m_fYScale = 0.35f;

					Vector2 base = m_vPos;// +Vector2(m_pRenderer->GetWidth(eSprite::EnemyRunning) * cosf(attackingTime), m_pRenderer->GetHeight(eSprite::EnemyRunning) * sinf(attackingTime));

					const float width = m_pRenderer->GetWidth(m_nSpriteIndex);
					const float height = m_pRenderer->GetHeight(m_nSpriteIndex);

					Vector2 rotation = Vector2(width * cosf(attackingTime), height * sinf(attackingTime));

					paperDesc.m_vPos = base + rotation;
					m_pRenderer->Draw(&paperDesc);

					rotation = Vector2(width * cosf((attackingTime + 3.14 / 2.0f)), height * sinf((attackingTime + 3.14 / 2.0f)));
					paperDesc.m_vPos = base + rotation;
					m_pRenderer->Draw(&paperDesc);

					rotation = Vector2(width * cosf((attackingTime + 3.14)), height * sinf((attackingTime + 3.14)));
					paperDesc.m_vPos = base + rotation;
					m_pRenderer->Draw(&paperDesc);

					rotation = Vector2(width * cosf((attackingTime + 3.0f * 3.14 / 2.0f)), height * sinf((attackingTime + 3.0f * 3.14 / 2.0f)));
					paperDesc.m_vPos = base + rotation;
					m_pRenderer->Draw(&paperDesc);
				}
				else if (attack == EnemyAttack::Lame)
				{
					const std::string lameText = "LAME!!!";
					Vector2 lamePosition = Vector2(m_vPos.x - 100 * attackingTime, height - m_vPos.y - 80);
					m_pRenderer->DrawScreenText(lameText.c_str(), lamePosition, Colors::White);
				}
			}
			else if (nextCard.type == EnemyCardType::Heal)
			{
				LSpriteDesc2D laptop;
				laptop.m_nSpriteIndex = (UINT)eSprite::Laptop;
				laptop.m_fXScale = 0.18f;
				laptop.m_fYScale = 0.18f;
				laptop.m_vPos = m_vPos + Vector2(0, 120 + 60 * attackingTime);

				m_pRenderer->Draw(&laptop);
			}
		}
	}
}

void Enemy::move()
{
	const float t = m_pTimer->GetFrameTime();

	Vector2 direction;

	switch (state)
	{
		case EnemyState::MovingTowardsCenter:
			UpdateFrame();
			direction = (target - m_vPos);
			direction.Normalize();
			m_vPos += direction * speed * t;

			if ((m_vPos - target).LengthSquared() < diff)
			{
				state = EnemyState::PlayingCard;
				attackingTime = 0;

				//If health is low, heal with random chance; otherwise, attack
				if (health < 3 && rng->randn(1, 10) <= 7)
				{
					nextCard.type = EnemyCardType::Heal;
					nextCard.value = 1 + rng->randn(0, 2);
					m_pAudio->play(eSound::Auto);
				}
				else
				{
					if (attack == EnemyAttack::EndlessHomework)
					{
						m_pAudio->play(eSound::EndlessHomework);
						nextCard.value = 2 + rng->randn(-1, 1);
					}
					else if (attack == EnemyAttack::Lame)
					{
						m_pAudio->play(eSound::Lame);
						nextCard.value = 3 + rng->randn(0, 1);
					}

					nextCard.type = EnemyCardType::Attack;
				}
			}
			break;
		case EnemyState::PlayingCard:
			attackingTime += t;
			break;
		case EnemyState::Returning:
			UpdateFrame();
			direction = (target - m_vPos);
			direction.Normalize();
			m_vPos += direction * speed * t;

			if ((m_vPos - target).LengthSquared() < diff)
			{
				state = EnemyState::Returned;

				if (m_nSpriteIndex == (UINT)eSprite::EnemyRunning)
				{
					m_nSpriteIndex = (UINT)eSprite::Enemy;
					m_nCurrentFrame = 0;
				}
			}
			break;
	}

	if (damageTimer->Triggered())
		m_f4Tint = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
}

void Enemy::UpdateFrame()
{
	if (m_nSpriteIndex == (UINT)eSprite::Boss)
		return;

	const size_t numFrames = m_pRenderer->GetNumFrames(m_nSpriteIndex);

	if (animationTimer->Triggered())
	{
		m_nCurrentFrame++;
		m_nCurrentFrame %= numFrames;
	}
}

bool Enemy::FinishedAttacking()
{
	return attackingTime >= attackEnd;
}

void Enemy::SetBoss()
{
	attack = EnemyAttack::Lame;
	m_nSpriteIndex = (UINT)eSprite::Boss;
	m_fXScale = 0.75;
	m_fYScale = 0.75;
	health = 20;
}

void Enemy::Heal(int amount)
{
	health += amount;
}

void Enemy::SetUnavailable()
{
	m_fAlpha = 0.5f;
}

void Enemy::SetNormal()
{
	m_fAlpha = 1.0f;
}

void Enemy::Kill()
{
	m_bDead = true;
}
