#include "Player.h"
#include "ComponentIncludes.h"
#include "Helpers.h"
#include <iostream>

Player::Player(const Vector2& p, float height) : CObject(eSprite::Player, p)
{
	health = 15;
	m_fXScale = .2;
	m_fYScale = .2;
	this->height = height;
	state = PlayerState::WaitingForInput;
	createCards();
	CreateStartDeck();
	shuffleCards();

	currCardIndex = -10;
	attackingTime = 0;

	animationTimer = new LEventTimer(0.1f);

	bookIndex = 0;
	damageTimer = new LEventTimer(0.3f);
}

void Player::TakeDamage(int amount)
{
	if (shield > 0) {
		shield -= amount;
		if (shield < 0)
			health = std::max(0, health - abs(shield));
		ResetShield();
	}
	else {
		health = std::max(0, health - amount);
	}
	m_f4Tint = Vector4(0.9f, 0.4f, 0.4f, 1.0f);
	damageTimer = new LEventTimer(0.3f);
	m_pAudio->play(eSound::PlayerDamage);
}

int Player::useCard(int cardNum)
{
	if (deck[cardNum]->giveShield() > 0)
		shield += deck[cardNum]->giveShield();

	if (deck[cardNum]->giveHealth() > 0)
		health += deck[cardNum]->giveHealth();
	
	return deck.at(cardNum)->dealDamage();
}

void Player::createCards() {
	for (int i = 0; i < 10; i++) {
		deck.push_back(new Card(Vector2(350, -390)));
	}
}

void Player::CreateStartDeck() {
	//First num in createCard = Dmg, 2nd num = Shield, 3rd num = health
	deck[0]->createCard(4, 0, 0);
	deck[1]->createCard(4, 0, 0);
	deck[2]->createCard(4, 0, 0);
	deck[3]->createCard(4, 0, 0);
	deck[4]->createCard(4, 0, 0);
	deck[5]->createCard(0, 2, 0);
	deck[6]->createCard(0, 2, 0);
	deck[7]->createCard(0, 2, 0);
	deck[8]->createCard(0, 2, 0);
	deck[9]->createCard(0, 0, 1);
}

int myrandom(int i) { return rand() % i; }

void Player::shuffleCards() {
	srand(unsigned(time(0)));

	std::random_shuffle(deck.begin(), deck.end());// , myrandom);
}

void Player::PlayCard(const Vector2& center)
{
	state = PlayerState::MovingTowardsCenter;
	target = center;
	oldPosition = m_vPos;
	m_nSpriteIndex = (UINT)eSprite::PlayerRunning;
}

void Player::ReturnToPosition()
{
	state = PlayerState::Returning;
	target = oldPosition;
}

void Player::draw()
{
	if (CCommon::state == GameState::Battle)
	{
		CObject::draw();

		const std::string s = this->getHeadText();
		m_pRenderer->DrawScreenText(s.c_str(), Vector2(m_vPos.x - 25, height - m_vPos.y - 125), Colors::White); //draw to screen

		const std::string s2 = this->getShieldText();
		m_pRenderer->DrawScreenText(s2.c_str(), Vector2(m_vPos.x - 25, height - m_vPos.y - 155), Colors::Blue); //draw to screen

		if (state == PlayerState::Attacking)
		{
			auto currCard = deck.at(currCardIndex);
			if (currCard->dealDamage() > 0)
			{
				LSpriteDesc2D bookDesc;
				bookDesc.m_nSpriteIndex = (UINT)eSprite::BookTurning;
				bookDesc.m_vPos = m_vPos + Vector2(0, 175);
				bookDesc.m_fXScale = 3.0f;
				bookDesc.m_fYScale = 3.0f;
				bookDesc.m_nCurrentFrame = bookIndex;

				m_pRenderer->Draw(&bookDesc);
			}
			else if (currCard->giveHealth() > 0)
			{
				float yBase = m_vPos.y - attackingTime * 25;

				Vector2 basePos1 = Vector2(m_vPos.x, yBase - 150);
				Vector2 basePos2 = Vector2(m_vPos.x + 90, yBase - 125);
				Vector2 basePos3 = Vector2(m_vPos.x - 90, yBase - 125);

				std::cout << basePos1.x << ", " << basePos1.y << std::endl;

				const std::string text = "Z";
				m_pRenderer->DrawScreenText(text.c_str(), basePos1, Colors::DarkBlue);
				m_pRenderer->DrawScreenText(text.c_str(), basePos2, Colors::DarkBlue);
				m_pRenderer->DrawScreenText(text.c_str(), basePos3, Colors::DarkBlue);
			}
			else if (currCard->giveShield() > 0)
			{
				Vector2 basePos1 = Vector2(m_vPos.x, m_vPos.y + 150);
				Vector2 basePos2 = Vector2(m_vPos.x + 90, m_vPos.y + 125);
				Vector2 basePos3 = Vector2(m_vPos.x - 90, m_vPos.y + 125);

				float speed = 30.0f;

				LSpriteDesc2D desc1;
				desc1.m_nSpriteIndex = (UINT)eSprite::Calendar;
				desc1.m_vPos = basePos1 + Vector2(0, attackingTime * speed);
				desc1.m_fXScale = 2.0f;
				desc1.m_fYScale = 2.0f;

				float dirSpeed = speed * cosf(3.141592 / 4);

				LSpriteDesc2D desc2;
				desc2.m_nSpriteIndex = (UINT)eSprite::Calendar;
				desc2.m_vPos = basePos2 + Vector2(attackingTime * dirSpeed, attackingTime * dirSpeed);
				desc2.m_fXScale = 2.0f;
				desc2.m_fYScale = 2.0f;

				LSpriteDesc2D desc3;
				desc3.m_nSpriteIndex = (UINT)eSprite::Calendar;
				desc3.m_vPos = basePos3 + Vector2(attackingTime * -dirSpeed, attackingTime * dirSpeed);
				desc3.m_fXScale = 2.0f;
				desc3.m_fYScale = 2.0f;

				m_pRenderer->Draw(&desc1);
				m_pRenderer->Draw(&desc2);
				m_pRenderer->Draw(&desc3);
			}
		}
	}
}

void Player::move()
{
	const float t = m_pTimer->GetFrameTime();

	Vector2 direction;

	switch (state)
	{
		case PlayerState::MovingTowardsCenter:
			UpdateFrame();
			direction = (target - m_vPos);
			direction.Normalize();
			m_vPos += direction * speed * t;

			if ((m_vPos - target).LengthSquared() < diff)
			{
				state = PlayerState::Attacking;
				bookIndex = 0;
				attackingTime = 0;

				auto currCard = deck.at(currCardIndex);
				if (currCard->dealDamage() > 0)
				{
					m_pAudio->play(eSound::StudyTime);
				}
				else if (currCard->giveHealth() > 0)
				{
					m_pAudio->play(eSound::PowerNap);
				}
				else if (currCard->giveShield() > 0)
				{
					m_pAudio->play(eSound::Time);
				}
			}
			break;
		case PlayerState::Attacking:
			UpdateBook();
			attackingTime += t;
			break;
		case PlayerState::Returning:
			UpdateFrame();
			direction = (target - m_vPos);
			direction.Normalize();
			m_vPos += direction * speed * t;

			if ((m_vPos - target).LengthSquared() < diff)
			{
				state = PlayerState::Returned;
				m_nSpriteIndex = (UINT)eSprite::Player;
				m_nCurrentFrame = 0;
			}
			break;
	}

	if (damageTimer->Triggered())
		m_f4Tint = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
}

void Player::UpdateFrame()
{
	const size_t numFrames = m_pRenderer->GetNumFrames(m_nSpriteIndex);

	if (animationTimer->Triggered())
	{
		m_nCurrentFrame++;
		m_nCurrentFrame %= numFrames;
	}
}

void Player::UpdateBook()
{
	const size_t numBookFrames = m_pRenderer->GetNumFrames((UINT)eSprite::BookTurning);

	if (animationTimer->Triggered())
	{
		bookIndex++;
	}
}

void Player::SetBack()
{
	state = PlayerState::WaitingForInput;
	m_vPos = oldPosition;
}

bool Player::FinishedAttacking()
{
	const size_t numBookFrames = m_pRenderer->GetNumFrames(eSprite::BookTurning);

	return bookIndex == numBookFrames;
}

void Player::SetCard(int card)
{
	currCardIndex = card;
}

void Player::SetUnavailable()
{
	m_fAlpha = 0.5f;
}

void Player::SetNormal()
{
	m_fAlpha = 1.0f;
}

void Player::Reset()
{
	m_vPos = Vector2(125, 430);   //Manually reset player to right position no matter when god mode activated
	state = PlayerState::WaitingForInput;
	bookIndex = 0;
	currCardIndex = -10;
	attackingTime = 0;
	m_nSpriteIndex = (UINT)eSprite::Player;
	m_nCurrentFrame = 0;
}
