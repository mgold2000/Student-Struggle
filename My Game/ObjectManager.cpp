/// \file ObjectManager.cpp
/// \brief Code for the the object manager class CObjectManager.

#include "ObjectManager.h"
#include "ComponentIncludes.h"
#include "Player.h"
#include "Enemy.h"
#include "NodeObject.h"

/// Create an object and put a pointer to it at the back of the object list
/// `m_stdObjectList`, which it inherits from `LBaseObjectManager`.
/// \param t Sprite type.
/// \param pos Initial position.
/// \return Pointer to the object created.

CObject* CObjectManager::create(eSprite t, const Vector2& pos){
    CObject* pObj = nullptr;

    switch (t) { //create object of type t
    case eSprite::Player:  
        {
            Player* temp = new Player(pos, m_nWinHeight);

            for (int i = 0; i < temp->GetDeck().size(); i++)
            {
                m_stdObjectList.push_back(temp->GetDeck().at(i));
            }
            pObj = temp;
        }
        break;
    case eSprite::Enemy:  
        pObj = new Enemy(pos, m_nWinHeight); 
        enemies.push_back((Enemy*)pObj);
        PositionEnemies();
        break;
    case eSprite::Card:
        pObj = new Card(pos);
        break;
    case eSprite::Node:
        pObj = new NodeObject(pos);
        nodes.push_back((NodeObject*)pObj);
        break;
    default: pObj = new CObject(t, pos);
    } //switch

    m_stdObjectList.push_back(pObj); //push pointer onto object list
    return pObj; //return pointer to created object
} //create

//Position enemies into rows in a visually-pleasing way
//Currently supports a max of 6 enemies, 2 rows of 3
void CObjectManager::PositionEnemies()
{
    const float enemyWidth = 100;
    const float backRowX = 900.0f;
    const float frontRowX = backRowX - enemyWidth - 50;

    int numEnemies = enemies.size();

    if (numEnemies > MaxEnemiesPerRow)
    {
        int numFrontRow = numEnemies - MaxEnemiesPerRow;

        std::vector<Enemy*> backRow;
        for (int i = 0; i < MaxEnemiesPerRow; i++)
        {
            backRow.push_back(enemies.at(i));
        }

        SetEquallySpaced(backRow, backRowX);

        std::vector<Enemy*> frontRow;
        for (int i = 0; i < numFrontRow; i++)
        {
            frontRow.push_back(enemies.at(i + MaxEnemiesPerRow));
        }

        SetEquallySpaced(frontRow, frontRowX);
    }
    else
    {
        SetEquallySpaced(enemies, backRowX);
    }
}

//Set a row of enemies to be equally spaced and centered
void CObjectManager::SetEquallySpaced(std::vector<Enemy*> enemies, float x)
{
    const float diffY = -75;
    const int enemyHeight = 150;

    int numEnemies = enemies.size();
    //Below is how enemies are spaced
    float totalDist = numEnemies * enemyHeight + (numEnemies - 1) * abs(diffY);
    float startY = m_nWinHeight - (m_nWinHeight - totalDist) / 2 - enemyHeight / 2;

    Vector2 currPosition = Vector2(x, startY);

    for (int i = 0; i < numEnemies; i++)
    {
        enemies.at(i)->SetPosition(currPosition);
        currPosition += Vector2(0, diffY - enemyHeight);
    }
}

void CObjectManager::ClearEnemies()
{
    enemies.clear();
}

void CObjectManager::RemoveEnemy(int index)
{
    enemies.erase(enemies.begin() + index);
    PositionEnemies();
}

void CObjectManager::UnlockLevel(int id)
{
    //nodes[id]->m_f4Tint = Vector4(0.05, 0.85, 0.0, 1.0);
    nodes[id]->m_nSpriteIndex = (UINT)eSprite::DoorOpen;
}

void CObjectManager::CompleteLevel(int id)
{
    //nodes[id]->m_f4Tint = Vector4(0.05, 0.05, 0.85, 1.0);
    nodes[id]->complete = true;
}

void CObjectManager::LockLevel(int id)
{
    //nodes[id]->m_f4Tint = Vector4(0.75, 0.25, 0.0, 1.0);
    nodes[id]->m_nSpriteIndex = (UINT)eSprite::DoorClosed;
}
