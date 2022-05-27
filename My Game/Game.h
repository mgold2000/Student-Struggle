/// \file Game.h
/// \brief Interface for the game class CGame.

#ifndef __L4RC_GAME_GAME_H__
#define __L4RC_GAME_GAME_H__

#include "Component.h"
#include "Common.h"
#include "ObjectManager.h"
#include "Settings.h"
#include "Random.h"
#include "Player.h"
#include "Card.h"
#include "Node.h"
#include "AdjacencyListEntry.h"

/// \brief The game class.
///
/// The game class is the object-oriented implementation of the game. This class
/// must contain the following public member functions. `Initialize()` does
/// initialization and will be run exactly once at the start of the game.
/// `ProcessFrame()` will be called once per frame to create and render the
/// next animation frame. `Release()` will be called at game exit but before
/// any destructors are run.

class CGame: 
  public LComponent, 
  public LWindow,
  public CCommon{ 

  private:
    bool m_bDrawFrameRate = false; ///< Draw the frame rate.
    bool gameOver = false;
    bool cardUpgraded = false;
    std::vector<std::vector<Node>> layers;
    std::vector<std::vector<AdjacencyListEntry>> levelAdjacencyLists;
    std::vector<Node*> currentlyUnlockedNodes;
    std::vector<int> usedCards{ 0,0,0,0,0,0,0,0,0,0 };

    POINT mPoint;

    int cardNum = -10;
    int currLevel;
    int currLayer;
    int tempInt = 0;
    int numEnemies, choseEnemy = 0, turnNum = 0, shuffleTracker = 0;
    
    void LoadImages(); ///< Load images.
    void LoadSounds(); ///< Load sounds.
    void BeginGame(); ///< Begin playing the game.
    void CreateObjects(); ///< Create game objects.
    void KeyboardHandler(); ///< The keyboard handler.
    void RenderFrame(); ///< Render an animation frame.
    void DrawFrameRateText(); ///< Draw frame rate text to screen.
    void DrawGameOverText();
    void LoadEnemies(int numEnemies);
    void findMouse();
    void chooseCard();
    void markUsed(int);
    bool IsMarked(int);
    void replaceCards();
    void removeCards();
    void drawCards();
    void clearUsed();
    void ChooseTarget(int numEnemies);

  public:
    ~CGame(); ///< Destructor.

    void Initialize(); ///< Initialize the game.
    void ProcessFrame(); ///< Process an animation frame.
    void Release(); ///< Release the renderer.

}; //CGame

#endif //__L4RC_GAME_GAME_H__