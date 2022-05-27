/// \file ObjectManager.h
/// \brief Interface for the object manager CObjectManager.

#ifndef __L4RC_GAME_OBJECTMANAGER_H__
#define __L4RC_GAME_OBJECTMANAGER_H__

#include "BaseObjectManager.h"
#include "Object.h"
#include "Common.h"
#include "Enemy.h"
#include "NodeObject.h"

/// \brief The object manager.
///
/// A collection of all of the game objects.

class CObjectManager: 
  public LBaseObjectManager<CObject>,
  public CCommon{
  public:
    CObject* create(eSprite, const Vector2&); ///< Create new object.
    std::vector<Enemy*> GetEnemies() { return enemies;  }
    void ClearEnemies();
    void ClearNodes() { nodes.clear(); }
    void RemoveEnemy(int index);

    void UnlockLevel(int id);
    void CompleteLevel(int id);
    void LockLevel(int id);
    std::vector<NodeObject*> GetNodes() { return nodes; }

    private:
        std::vector<Enemy*> enemies;
        const int MaxEnemiesPerRow = 3;

        std::vector<NodeObject*> nodes;

        void PositionEnemies();
        void SetEquallySpaced(std::vector<Enemy*> enemies, float x);
}; //CObjectManager

#endif //__L4RC_GAME_OBJECTMANAGER_H__
