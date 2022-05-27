/// \file Game.cpp
/// \brief Code for the game class CGame.

#include <fstream>
#include "Game.h"

#include "GameDefines.h"
#include "SpriteRenderer.h"
#include "ComponentIncludes.h"
#include "WindowDesc.h"
#include "Windows.h"
#include "Window.h"

#include "shellapi.h"

/// Delete the object manager. The renderer needs to be deleted before this
/// destructor runs so it will be done elsewhere.

CGame::~CGame(){
  delete m_pObjectManager;
} //destructor

/// Create the renderer and the object manager, load images and sounds, and
/// begin the game.

void CGame::Initialize(){
  m_pRenderer = new LSpriteRenderer(eSpriteMode::Batched2D); 
  m_pRenderer->Initialize(eSprite::Size); 
  LoadImages(); //load images from xml file list

  m_pObjectManager = new CObjectManager; //set up the object manager 
  LoadSounds(); //load the sounds for this game

  BeginGame();
} //Initialize

/// Load the specific images needed for this game. This is where `eSprite`
/// values from `GameDefines.h` get tied to the names of sprite tags in
/// `gamesettings.xml`. Those sprite tags contain the name of the corresponding
/// image file. If the image tag or the image file are missing, then the game
/// should abort from deeper in the Engine code leaving you with an error
/// message in a dialog box.

void CGame::LoadImages(){  
  m_pRenderer->BeginResourceUpload();

  m_pRenderer->Load(eSprite::PlayerSpritesheet, "PlayerSpritesheet");
  m_pRenderer->Load(eSprite::EnemySpritesheet, "EnemySpritesheet");
  m_pRenderer->Load(eSprite::EnemyRunning, "EnemyRunning");
  m_pRenderer->Load(eSprite::Player, "player");
  m_pRenderer->Load(eSprite::PlayerRunning, "PlayerRunning");
  m_pRenderer->Load(eSprite::Enemy, "enemy");
  m_pRenderer->Load(eSprite::Card, "card");
  m_pRenderer->Load(eSprite::Background, "background");
  m_pRenderer->Load(eSprite::Node, "node");
  m_pRenderer->Load(eSprite::Line, "line");
  m_pRenderer->Load(eSprite::DoorClosed, "doorClosed");
  m_pRenderer->Load(eSprite::DoorOpen, "doorOpen");
  m_pRenderer->Load(eSprite::MapBackground, "mapBackground");
  m_pRenderer->Load(eSprite::Checkmark, "checkmark");
  m_pRenderer->Load(eSprite::WinBackground, "winBackground");
  m_pRenderer->Load(eSprite::LoseBackground, "loseBackground");
  m_pRenderer->Load(eSprite::BookSpritesheet, "BookSpritesheet");
  m_pRenderer->Load(eSprite::BookTurning, "BookTurning");
  m_pRenderer->Load(eSprite::Paper, "paper");
  m_pRenderer->Load(eSprite::Boss, "boss");
  m_pRenderer->Load(eSprite::Nerd, "nerd");
  m_pRenderer->Load(eSprite::MenuBackground, "menuBackground");
  m_pRenderer->Load(eSprite::PlayButton, "playButton");
  m_pRenderer->Load(eSprite::CardBackground, "cardBackground");
  m_pRenderer->Load(eSprite::Laptop, "laptop");
  m_pRenderer->Load(eSprite::IntroBackground, "introBackground");
  m_pRenderer->Load(eSprite::PlayAgainButton, "playAgainButton");
  m_pRenderer->Load(eSprite::Calendar, "calendar");
  m_pRenderer->Load(eSprite::CardDamage, "cardDamage");
  m_pRenderer->Load(eSprite::CardHealth, "cardHealth");
  m_pRenderer->Load(eSprite::CardShield, "cardShield");
  m_pRenderer->Load(eSprite::NerdBackground, "nerdBackground");

  m_pRenderer->EndResourceUpload();
} //LoadImages

/// Initialize the audio player and load game sounds.

void CGame::LoadSounds(){
  m_pAudio->Initialize(eSound::Size);
  m_pAudio->Load(eSound::StudyTime, "StudyTime");
  m_pAudio->Load(eSound::EndlessHomework, "EndlessHomework");
  m_pAudio->Load(eSound::Lame, "Lame");
  m_pAudio->Load(eSound::PlayerDamage, "PlayerDamage");
  m_pAudio->Load(eSound::EnemyDamage, "EnemyDamage");
  m_pAudio->Load(eSound::Auto, "auto");
  m_pAudio->Load(eSound::PowerNap, "PowerNap");
  m_pAudio->Load(eSound::Time, "Time");
} //LoadSounds

/// Release all of the DirectX12 objects by deleting the renderer.

void CGame::Release(){
  delete m_pRenderer;
  m_pRenderer = nullptr; //for safety
} //Release

/// Ask the object manager to create the game objects.

void CGame::CreateObjects(){
  rng = new LRandom();
  
  player = (Player*)m_pObjectManager->create(eSprite::Player, Vector2(125, 430));
} //CreateObjects

/// Call this function to start a new game. This should be re-entrant so that
/// you can restart a new game without having to shut down and restart the
/// program. All we need to do is delete any old objects out of the object
/// manager and create some new ones.

void CGame::BeginGame(){  
  m_pObjectManager->ClearEnemies();
  m_pObjectManager->ClearNodes();
  m_pObjectManager->clear(); //clear old objects
  CreateObjects(); //create new objects 
  replaceCards();

  gameOver = false;
  enemyUpdateIndex = -1;
  state = GameState::Menu;

  layers.clear();
  levelAdjacencyLists.clear();
  currentlyUnlockedNodes.clear();

  //Generate levels
  const int numLayers = 5;
  int currID = 0;
  for (int i = 0; i < numLayers; i++)
  {
      std::vector<Node> layer;

      int levelsInLayer = rng->randn(1, 4);
      if (i == 0 || i == numLayers - 1)
         levelsInLayer = 1;
      else
      {
          if (levelsInLayer == 1)
              levelsInLayer += rng->randn(0, 1);
      }
      
      for (int j = 0; j < levelsInLayer; j++)
      {
          Node newNode;
          newNode.id = currID;
          newNode.layer = i;
          currID++;

          //Position the node within map
          int base = 300;
          if (levelsInLayer < 4)
              base += 400 / (levelsInLayer + 1);

          int divisor = levelsInLayer;
          if (levelsInLayer == 4)
              divisor--;

          int multiplier = levelsInLayer * 100;
          if (levelsInLayer == 2)
          {
              multiplier *= 2;
              divisor++;
          }

          newNode.position = Vector2(base + (multiplier / divisor) * j, 125 * i + 150);

          //Determine number of enemies in level
          newNode.numEnemies = rng->randn(1, i + 1);

          if (i == 0 || i == numLayers - 1)
              newNode.numEnemies = 1;

          layer.push_back(newNode);

          auto nodeObj = (NodeObject*)m_pObjectManager->create(eSprite::Node, newNode.position);
          nodeObj->numEnemies = newNode.numEnemies;
      }

      layers.push_back(layer);
  }

  //Set and unlock first level
  layers[0][0].unlocked = true;
  currLevel = 0;
  currLayer = 0;
  m_pObjectManager->UnlockLevel(0);
  currentlyUnlockedNodes.push_back(&layers[0][0]);

  //Generate adjacency lists

  std::vector<AdjacencyListEntry> firstLevelAdjacencyList;

  //Connect first level to all levels in next layer
  for (int i = 0; i < layers[1].size(); i++)
  {
      AdjacencyListEntry entry;
      entry.from = &layers[0][0];
      entry.to = &layers[1][i];
      firstLevelAdjacencyList.push_back(entry);
  }
  levelAdjacencyLists.push_back(firstLevelAdjacencyList);

  //Connect inner layers
  for (int i = 1; i < numLayers - 2; i++)
  {
      int currLayerSize = layers[i].size();
      int nextLayerSize = layers[i + 1].size();

      //If same size, connect like to like
      if (currLayerSize == nextLayerSize)
      {
          for (int j = 0; j < currLayerSize; j++)
          {
              std::vector<AdjacencyListEntry> levelAdjacencyList;
              AdjacencyListEntry entry;
              entry.from = &layers[i][j];
              entry.to = &layers[i + 1][j];

              levelAdjacencyList.push_back(entry);
              levelAdjacencyLists.push_back(levelAdjacencyList);
          }
      }
      else if (currLayerSize == 1)
      {
          std::vector<AdjacencyListEntry> levelAdjacencyList;

          for (int j = 0; j < nextLayerSize; j++)
          {
              AdjacencyListEntry entry;
              entry.from = &layers[i][0];
              entry.to = &layers[i + 1][j];
              levelAdjacencyList.push_back(entry);
          }

          levelAdjacencyLists.push_back(levelAdjacencyList);
      }
      else if (nextLayerSize == 1)
      {
          for (int j = 0; j < currLayerSize; j++)
          {
              std::vector<AdjacencyListEntry> levelAdjacencyList;
              AdjacencyListEntry entry;
              entry.from = &layers[i][j];
              entry.to = &layers[i + 1][0];

              levelAdjacencyList.push_back(entry);
              levelAdjacencyLists.push_back(levelAdjacencyList);
          }
      }
      else if (currLayerSize > nextLayerSize)
      {
          if (currLayerSize == 4)
          {
              if (nextLayerSize == 3)
              {
                  for (int j = 0; j < nextLayerSize; j++)
                  {
                      std::vector<AdjacencyListEntry> levelAdjacencyList;
                      AdjacencyListEntry entry;
                      entry.from = &layers[i][j];
                      entry.to = &layers[i + 1][j];

                      levelAdjacencyList.push_back(entry);
                      levelAdjacencyLists.push_back(levelAdjacencyList);
                  }

                  std::vector<AdjacencyListEntry> levelAdjacencyList;
                  AdjacencyListEntry entry;
                  entry.from = &layers[i][currLayerSize - 1];
                  entry.to = &layers[i + 1][nextLayerSize - 1];

                  levelAdjacencyList.push_back(entry);
                  levelAdjacencyLists.push_back(levelAdjacencyList);
              }
              else if (nextLayerSize == 2)
              {
                  for (int j = 0; j < nextLayerSize; j++)
                  {
                      std::vector<AdjacencyListEntry> levelAdjacencyList1;
                      AdjacencyListEntry entry1;
                      entry1.from = &layers[i][2*j];
                      entry1.to = &layers[i + 1][(j+1)/2];
                      levelAdjacencyList1.push_back(entry1);

                      std::vector<AdjacencyListEntry> levelAdjacencyList2;
                      AdjacencyListEntry entry2;
                      entry2.from = &layers[i][2*j + 1];
                      entry2.to = &layers[i + 1][(2*j+1)/2];
                      levelAdjacencyList2.push_back(entry2);

                      levelAdjacencyLists.push_back(levelAdjacencyList1);
                      levelAdjacencyLists.push_back(levelAdjacencyList2);
                  }
              }
          }
          else if (currLayerSize == 3) 
          {
              //Since case of 1 is handled separately and currLayerSize == 3,
              //that means nextLayerSize == 2

              for (int j = 0; j < nextLayerSize; j++)
              {
                  std::vector<AdjacencyListEntry> levelAdjacencyList;
                  AdjacencyListEntry entry;
                  entry.from = &layers[i][j];
                  entry.to = &layers[i + 1][j];

                  levelAdjacencyList.push_back(entry);
                  levelAdjacencyLists.push_back(levelAdjacencyList);
              }

              std::vector<AdjacencyListEntry> levelAdjacencyList;
              AdjacencyListEntry entry;
              entry.from = &layers[i][currLayerSize - 1];
              entry.to = &layers[i + 1][nextLayerSize - 1];

              levelAdjacencyList.push_back(entry);
              levelAdjacencyLists.push_back(levelAdjacencyList);
          }
      }
      else if (nextLayerSize > currLayerSize)
      {
          if (currLayerSize == 2)
          {
              if (nextLayerSize == 3)
              {
                  for (int j = 0; j < currLayerSize; j++)
                  {
                      std::vector<AdjacencyListEntry> levelAdjacencyList;
                      AdjacencyListEntry entry;
                      entry.from = &layers[i][j];
                      entry.to = &layers[i + 1][j];

                      levelAdjacencyList.push_back(entry);
                      levelAdjacencyLists.push_back(levelAdjacencyList);
                  }

                  AdjacencyListEntry entry;
                  entry.from = &layers[i][currLayerSize - 1];
                  entry.to = &layers[i + 1][nextLayerSize - 1];

                  levelAdjacencyLists[levelAdjacencyLists.size() - 1].push_back(entry);
              }
              else if (nextLayerSize == 4)
              {
                  for (int j = 0; j < currLayerSize; j++)
                  {
                      std::vector<AdjacencyListEntry> levelAdjacencyList;
                      AdjacencyListEntry entry1;
                      entry1.from = &layers[i][j];
                      entry1.to = &layers[i + 1][j*2];

                      AdjacencyListEntry entry2;
                      entry2.from = &layers[i][j];
                      entry2.to = &layers[i + 1][j*2 + 1];

                      levelAdjacencyList.push_back(entry1);
                      levelAdjacencyList.push_back(entry2);
                      levelAdjacencyLists.push_back(levelAdjacencyList);
                  }
              }
          }
          else if (currLayerSize == 3)
          {
              //nextLayerSize must be 4

              for (int j = 0; j < currLayerSize; j++)
              {
                  std::vector<AdjacencyListEntry> levelAdjacencyList;
                  AdjacencyListEntry entry;
                  entry.from = &layers[i][j];
                  entry.to = &layers[i + 1][j];

                  levelAdjacencyList.push_back(entry);
                  levelAdjacencyLists.push_back(levelAdjacencyList);
              }

              AdjacencyListEntry entry;
              entry.from = &layers[i][currLayerSize - 1];
              entry.to = &layers[i + 1][nextLayerSize - 1];
              levelAdjacencyLists[levelAdjacencyLists.size() - 1].push_back(entry);
          }
      }

  }

  //Connect everything in second to last layer to last level
  for (int i = 0; i < layers[numLayers - 2].size(); i++)
  {
      std::vector<AdjacencyListEntry> adjacencyList;
      AdjacencyListEntry entry;
      entry.from = &layers[numLayers - 2][i];
      entry.to = &layers[numLayers - 1][0];

      adjacencyList.push_back(entry);
      levelAdjacencyLists.push_back(adjacencyList);
  }

  //Place a randomized special card node
  int specialLayer = rng->randn(2, layers.size() - 2);
  int specialNode = rng->randn(0, layers[specialLayer].size() - 1);

  int specialID = layers[specialLayer][specialNode].id;
  
  levelAdjacencyLists[specialID][0].from->special = true;

  m_pObjectManager->GetNodes().at(specialID)->SetSpecial();
  
  for (auto adjacencyList : levelAdjacencyLists)
  {
      for (auto entry : adjacencyList)
      {
          if (entry.to->id == specialID)
          {
              entry.from->numEnemies = max(entry.from->numEnemies, 4);
              m_pObjectManager->GetNodes().at(entry.from->id)->numEnemies = entry.from->numEnemies;
          }
      }
  }

} //BeginGame

/// Poll the keyboard state and respond to the key presses that happened since
/// the last frame.

void CGame::KeyboardHandler(){
  m_pKeyboard->GetState(); //get current keyboard state 
  
  if(m_pKeyboard->TriggerDown(VK_F1)) //help
    ShellExecute(0, 0, "https://larc.unt.edu/code/blank/", 0, 0, SW_SHOW);
  
  if(m_pKeyboard->TriggerDown(VK_F2)) //toggle frame rate 
    m_bDrawFrameRate = !m_bDrawFrameRate;

  if (m_pKeyboard->TriggerDown(VK_BACK)) //restart game
      BeginGame(); //restart game

  if (gameOver)
  {
      if (m_pKeyboard->TriggerDown(VK_LBUTTON))
      {
          const Vector2 buttonCenter = Vector2(m_nWinWidth / 2, m_nWinHeight / 2 + 75);
          const float buttonWidth = m_pRenderer->GetWidth(eSprite::PlayAgainButton);
          const float buttonHeight = m_pRenderer->GetHeight(eSprite::PlayAgainButton);

          Vector2 topLeft = buttonCenter + Vector2(-1 * buttonWidth / 2.0f, buttonHeight / 2.0f);
          Vector2 bottomRight = buttonCenter + Vector2(buttonWidth / 2.0f, -1 * buttonHeight / 2.0f);

          findMouse();
          Vector2 mousePos = Vector2(mPoint.x, m_nWinHeight - mPoint.y);

          if (mousePos.x >= topLeft.x && mousePos.x <= bottomRight.x &&
              mousePos.y >= bottomRight.y && mousePos.y <= topLeft.y)
          {
              BeginGame();
          }

      }
      return;
  }
  if (state == GameState::Battle)
  {
      //God mode
      if (m_pKeyboard->TriggerDown('G'))
      {
          //Need to clear current enemies
          for (auto enemy : m_pObjectManager->GetEnemies())
          {
              enemy->Kill();
          }
          m_pObjectManager->ClearEnemies();

          //Then do normal end of level stuff

          //Check if player has won
          if (currLayer == layers.size() - 1)
          {
              gameOver = true;
              state = GameState::GameOver;
              return;
          }

          //Lock levels that are no longer accessible
          for (auto node : currentlyUnlockedNodes)
          {
              m_pObjectManager->LockLevel(node->id);
              node->unlocked = false;
          }
          currentlyUnlockedNodes.clear();

          //Unlock levels adjacent to this one
          for (auto entry : levelAdjacencyLists[currLevel])
          {
              entry.to->unlocked = true;
              state = GameState::NewCard;

              if (!entry.to->special)
                  m_pObjectManager->UnlockLevel(entry.to->id);

              currentlyUnlockedNodes.push_back(entry.to);
          }

          m_pObjectManager->CompleteLevel(levelAdjacencyLists[currLevel][0].from->id);  //Mark current level as completed

          removeCards();        //Remove remaining unused cards
          clearUsed();          //Clear the vector tracking used cards
          player->Reset();

          for (auto card : player->GetDeck())
          {
              card->Reset();
          }

          enemyUpdateIndex = -1;

          //Shuffle the cards 10 times
          if (shuffleTracker == 1) {
              for (int i = 0; i < 10; i++) {
                  player->shuffleCards();
              }
              shuffleTracker = 0;
          }
          else {
              shuffleTracker++;
          }
          replaceCards(); //Replace with 5 new cards
          cardNum = -10; //Reset cardNum for selection

          return;
      }

      if (enemyUpdateIndex == -1)
      {
          if (player->GetState() == PlayerState::WaitingForInput)
          {
              if (cardNum == -10)
              {
                  chooseCard();//Find which card was chosen
              }
              else if (cardNum >= 0 && cardNum <= 4)
              {
                  if (m_pKeyboard->TriggerDown(VK_LBUTTON))
                  {
                      ChooseTarget(m_pObjectManager->GetEnemies().size());
                  }
              }
          }
          else if (player->GetState() == PlayerState::Attacking && player->FinishedAttacking())
          {
              //Where 0 is below is how the enemy taking damage is decided 
              if (player->GetDeck().at(cardNum)->dealDamage() > 0)
              {
                  auto enemy = m_pObjectManager->GetEnemies()[choseEnemy];
                  if (enemy->TakeDamage(player->useCard(cardNum)))
                      m_pObjectManager->RemoveEnemy(choseEnemy);
              }
              else
              {
                  player->useCard(cardNum);
              }

              if (m_pObjectManager->GetEnemies().size() == 0)
              {
                  //Check if player has won
                  if (currLayer == layers.size() - 1)
                  {
                      gameOver = true;
                      state = GameState::GameOver;
                      return;
                  }

                  //Lock levels that are no longer accessible
                  for (auto node : currentlyUnlockedNodes)
                  {
                      m_pObjectManager->LockLevel(node->id);
                      node->unlocked = false;
                  }
                  currentlyUnlockedNodes.clear();

                  //Unlock levels adjacent to this one
                  for (auto entry : levelAdjacencyLists[currLevel])
                  {
                      
                      entry.to->unlocked = true;
                      state = GameState::NewCard;

                      if (!entry.to->special)
                        m_pObjectManager->UnlockLevel(entry.to->id);

                      currentlyUnlockedNodes.push_back(entry.to);
                  }

                  m_pObjectManager->CompleteLevel(levelAdjacencyLists[currLevel][0].from->id);  //Mark current level as completed

                  removeCards();        //Remove remaining unused cards
                  clearUsed();          //Clear the vector tracking used cards
                  player->SetBack();    //Reset the player position and state
                  player->GetDeck().at(cardNum)->SetUsed();
                  //Shuffle the cards 10 times
                  if (shuffleTracker == 1) {
                      for (int i = 0; i < 10; i++) {
                          player->shuffleCards();
                      }
                      shuffleTracker = 0;
                  }
                  else {
                      shuffleTracker++;
                  }
                  replaceCards(); //Replace with 5 new cards
                  cardNum = -10; //Reset cardNum for selection

                  return;
              }

              player->ReturnToPosition();
          }
          else if (player->GetState() == PlayerState::Returned && turnNum == 3)
          {
              removeCards();        //Remove remaining unused cards
              clearUsed();          //Clear the vector tracking used cards
              player->SetBack();    //Reset the player position and state
              player->GetDeck().at(cardNum)->Unselect();
              player->GetDeck().at(cardNum)->Unhover();
              //Shuffle the cards 10 times
              if (shuffleTracker == 1) {
                  for (int i = 0; i < 10; i++) {
                      player->shuffleCards();
                  }
                  shuffleTracker = 0;
              }
              else {
                  shuffleTracker++;
              }
              replaceCards();       //Replace with 5 new cards
              enemyUpdateIndex++;   //Update enemy index to make enemy attack
              cardNum = -10;        //Reset cardNum for selection
              turnNum = 0;          //Reset turnNum to 0 so the player can play 3 more cards next turn
          }
          else if (player->GetState() == PlayerState::Returned)
          {
              player->SetBack();
              player->GetDeck().at(cardNum)->SetUsed();
              cardNum = -10;
              //enemyUpdateIndex++;
          }
      }
      else
      {
          auto enemy = m_pObjectManager->GetEnemies()[enemyUpdateIndex];

          if (enemy->GetState() == EnemyState::InPosition)
              enemy->PlayCard(Vector2(m_nWinWidth / 2.0f, m_nWinHeight / 2.0f));
          else if (enemy->GetState() == EnemyState::PlayingCard && enemy->FinishedAttacking())
          {
              //Take action based on the enemy card's type

              auto enemyCard = enemy->GetCard();
              if (enemyCard.type == EnemyCardType::Attack)
              {
                  player->TakeDamage(enemyCard.value);

                  if (player->IsDead())
                  {
                      gameOver = true;
                      state = GameState::GameOver;
                      return;
                  }
              }
              else if (enemyCard.type == EnemyCardType::Heal)
              {
                  enemy->Heal(enemyCard.value);
              }

              enemy->ReturnToPosition();
          }
          else if (enemy->GetState() == EnemyState::Returned)
          {
              enemy->SetBack();
              enemyUpdateIndex++;

              if (enemyUpdateIndex == m_pObjectManager->GetEnemies().size())
              {
                  player->ResetShield();
                  enemyUpdateIndex = -1;
              }
          }
      }

  }
  else if (state == GameState::Map)
  {
      cardNum = -10;
      turnNum = 0;
      if (m_pKeyboard->TriggerDown(VK_LBUTTON))
      {
          findMouse();
          Vector2 mousePos = Vector2(mPoint.x, m_nWinHeight - mPoint.y);

          for (auto layer : layers)
          {
              for (auto node : layer)
              {
                  if (node.unlocked)
                  {
                      float dist = Vector2::DistanceSquared(mousePos, node.position);
                      const float radSquared = pow(19, 2);

                      const float width = 1430 * 0.05f;
                      const float height = 1604 * 0.05f;

                      Vector2 bottomRight = Vector2(node.position.x + width / 2, node.position.y + height / 2);
                      Vector2 topLeft = Vector2(node.position.x - width / 2, node.position.y - height / 2);

                      if (mousePos.x >= topLeft.x && mousePos.x <= bottomRight.x &&
                          mousePos.y >= topLeft.y && mousePos.y <= bottomRight.y)
                      {
                          if (node.special)
                          {
                              currLevel = node.id;
                              currLayer = node.layer;

                              //Lock levels that are no longer accessible
                              for (auto node : currentlyUnlockedNodes)
                              {
                                  if (!node->special)
                                  {
                                      m_pObjectManager->LockLevel(node->id);
                                  }
                                  node->unlocked = false;
                              }
                              currentlyUnlockedNodes.clear();

                              //Unlock levels adjacent to this one
                              for (auto entry : levelAdjacencyLists[currLevel])
                              {
                                  entry.to->unlocked = true;
                                  state = GameState::Nerd;

                                  m_pObjectManager->UnlockLevel(entry.to->id);

                                  currentlyUnlockedNodes.push_back(entry.to);
                              }

                              m_pObjectManager->CompleteLevel(levelAdjacencyLists[currLevel][0].from->id);  //Mark current level as completed

                          }
                          else
                          {
                              state = GameState::Battle;
                              LoadEnemies(node.numEnemies);
                              numEnemies = node.numEnemies;
                              currLevel = node.id;
                              currLayer = node.layer;

                              if (currLayer == layers.size() - 1)
                              {
                                  m_pObjectManager->GetEnemies().at(0)->SetBoss();
                              }
                          }
                      }
                  }
              }
          }
      }
  }
  else if (state == GameState::Menu)
  {
      if (m_pKeyboard->TriggerDown(VK_LBUTTON))
      {
          Vector2 buttonCenter = Vector2(m_nWinWidth / 2, m_nWinHeight / 2 + 100);
          float buttonWidth = m_pRenderer->GetWidth(eSprite::PlayButton);
          float buttonHeight = m_pRenderer->GetHeight(eSprite::PlayButton);

          Vector2 topLeft = buttonCenter + Vector2(-1 * buttonWidth / 2.0f, buttonHeight / 2.0f);
          Vector2 bottomRight = buttonCenter + Vector2(buttonWidth / 2.0f, -1 * buttonHeight / 2.0f);

          findMouse();
          Vector2 mousePos = Vector2(mPoint.x, m_nWinHeight - mPoint.y);

          if (mousePos.x >= topLeft.x && mousePos.x <= bottomRight.x &&
              mousePos.y >= bottomRight.y && mousePos.y <= topLeft.y)
          {
              state = GameState::Intro;
          }
      }
  }
  else if (state == GameState::NewCard)
  {
      if (cardUpgraded == false) {
          removeCards();
          for (int i = 0; i < player->GetDeck().size(); i++) {
              player->GetDeck().at(i)->AddCardMove(i);
          }
          cardUpgraded = true;

      }
      chooseCard();
 }
  else if (state == GameState::Intro)
  {
    if (m_pKeyboard->TriggerDown(VK_RETURN))
    {
        state = GameState::Map;
    }
  }
  else if (state == GameState::Nerd)
  {
      if (m_pKeyboard->TriggerDown(VK_LBUTTON))
      {
          //Upgrade cards
          for (auto card : player->GetDeck())
          {
              card->UpgradeAllCards();
          }

          state = GameState::Map;
      }
  }

} //KeyboardHandler

/// Draw the current frame rate to a hard-coded position in the window.
/// The frame rate will be drawn in a hard-coded position using the font
/// specified in gamesettings.xml.

void CGame::DrawFrameRateText(){
  const std::string s = std::to_string(m_pTimer->GetFPS()) + " fps"; //frame rate
  const Vector2 pos(m_nWinWidth - 128.0f, 30.0f); //hard-coded position
  m_pRenderer->DrawScreenText(s.c_str(), pos); //draw to screen
} //DrawFrameRateText

void CGame::DrawGameOverText() {
    Vector2 pos(m_nWinWidth / 2.0f - 260.0f, m_nWinHeight / 2.0f - 180.0f);
    std::string text = "You finished school!  Con-grad-ulations!";

    if (player->IsDead())
    {
        text = "You failed school!  Oh no!";
        pos.x += 75;
    }

    
    m_pRenderer->DrawScreenText(text.c_str(), pos, Colors::White); //draw to screen
} //DrawFrameRateText

/// Ask the object manager to draw the game objects. The renderer is notified
/// of the start and end of the frame so that it can let Direct3D do its
/// pipelining jiggery-pokery.

void CGame::RenderFrame(){
  m_pRenderer->BeginFrame(); //required before rendering
  
  if(m_bDrawFrameRate)DrawFrameRateText(); //draw frame rate, if required

  if (gameOver)
  {
      if (currLayer == layers.size() - 1)
        m_pRenderer->Draw(eSprite::WinBackground, Vector2(m_nWinWidth / 2, m_nWinHeight / 2));
      else
        m_pRenderer->Draw(eSprite::LoseBackground, Vector2(m_nWinWidth / 2, m_nWinHeight / 2));

      m_pRenderer->Draw(eSprite::PlayAgainButton, Vector2(m_nWinWidth / 2, m_nWinHeight / 2 + 75));

      DrawGameOverText();
  }
  else if (state == GameState::Map) //Draw lines between nodes
  {
      m_pRenderer->Draw(eSprite::MapBackground, Vector2(m_nWinWidth / 2, m_nWinHeight / 2));

      for (auto adjacencyList : levelAdjacencyLists)
      {
          for (auto entry : adjacencyList)
          {
              m_pRenderer->DrawLine((UINT)eSprite::Line, entry.from->position, entry.to->position);
          }
      }
  }
  else if (state == GameState::Battle)
  {
      m_pRenderer->Draw(eSprite::Background, Vector2(m_nWinWidth / 2, m_nWinHeight / 2));

      //Draw number of cards left to play in this turn
      std::string s2 = "3/3";
      if (turnNum == 0) {
          s2 = "3/3";
      }
      else if (turnNum == 1) {
          s2 = "2/3";
      }
      else if (turnNum == 2) {
          s2 = "1/3";
      }
      else {
          s2 = "0/3";
      }
      m_pRenderer->DrawScreenText(s2.c_str(), Vector2(125, 635), Colors::Black); 
  }
  else if (state == GameState::Menu)
  {
      m_pRenderer->Draw(eSprite::MenuBackground, Vector2(m_nWinWidth / 2, m_nWinHeight / 2));
      m_pRenderer->Draw(eSprite::PlayButton, Vector2(m_nWinWidth / 2, m_nWinHeight / 2 + 100));
  }
  else if (state == GameState::NewCard)
  {
      m_pRenderer->Draw(eSprite::CardBackground, Vector2(m_nWinWidth / 2, m_nWinHeight / 2));
  }
  else if (state == GameState::Intro)
  {
      m_pRenderer->Draw(eSprite::IntroBackground, Vector2(m_nWinWidth / 2, m_nWinHeight / 2));
  }
  else if (state == GameState::Nerd)
  {
      m_pRenderer->Draw(eSprite::NerdBackground, Vector2(m_nWinWidth / 2, m_nWinHeight / 2));
  }

  m_pObjectManager->draw(); //draw objects

  m_pRenderer->EndFrame(); //required after rendering
} //RenderFrame

/// This function will be called regularly to process and render a frame
/// of animation, which involves the following. Handle keyboard input.
/// Notify the  audio player at the start of each frame so that it can prevent
/// multiple copies of a sound from starting on the same frame.  
/// Move the game objects. Render a frame of animation.

void CGame::ProcessFrame(){
  KeyboardHandler(); //handle keyboard input
  m_pAudio->BeginFrame(); //notify audio player that frame has begun

  m_pTimer->Tick([&](){ //all time-dependent function calls should go here
    m_pObjectManager->move(); //move all objects
  });

  RenderFrame(); //render a frame of animation
} //ProcessFrame

//Helper function to find the mouse position inside the game window
void CGame::findMouse() {
    GetCursorPos(&mPoint);
    ScreenToClient(m_Hwnd, &mPoint);
    /*  ///Uncomment below to output mouse position of the last click to a file: test.txt
    std::ofstream of;
    of.open("test.txt");
    of << "Mouse X: " << mPoint.x << "\nMouse Y: " << mPoint.y ;
    of.close();
    */
}

void CGame::clearUsed() {
    for (int i = 0; i < usedCards.size(); i++)
        usedCards.at(i) = 0;
}

void CGame::markUsed(int index) {
    usedCards.at(index) = 99;
}

bool CGame::IsMarked(int index) {
    if (usedCards.at(index) == 99) {
        return true;
    }
    else {
        return false;
    }
}

void CGame::replaceCards() {
    if (shuffleTracker == 0) {
        for (int i = 0; i < 5; i++) {
            player->GetDeck().at(i)->ReplaceCard();
            player->GetDeck().at(i)->RepositionCard(i);
        }
    }
    else {
        for (int i = 5; i < 10; i++) {
            player->GetDeck().at(i)->ReplaceCard();
            player->GetDeck().at(i)->RepositionCard(i);
        }
    }
}

void CGame::removeCards() {
    if (shuffleTracker == 0) {
        for (int i = 0; i < 5; i++) {
            if (!IsMarked(i))
                player->GetDeck().at(i)->RemoveCard(i);
        }
    }
    else {
        for (int i = 5; i < 10; i++) {
            if (!IsMarked(i))
                player->GetDeck().at(i)->RemoveCard(i);
        }
    }
}

void CGame::chooseCard() {
    findMouse(); //Find mouse position when button is clicked
    if (state == GameState::Battle) {
        if (mPoint.x > 314.0f && mPoint.x < 381.0f && mPoint.y < 700.0f && mPoint.y > 581.0f) {
            if (shuffleTracker == 0) {
                player->GetDeck().at(0)->Hover();
            }
            else {
                player->GetDeck().at(5)->Hover();
            }
            if (m_pKeyboard->TriggerDown(VK_LBUTTON))
            {
                if (shuffleTracker == 0) {
                    cardNum = 0;
                }
                else {
                    cardNum = 5;
                }
                player->GetDeck().at(cardNum)->Select();
                player->SetCard(cardNum);

                if (player->GetDeck().at(cardNum)->dealDamage() > 0)
                {
                    player->SetUnavailable();
                }
                else
                {
                    for (auto enemy : m_pObjectManager->GetEnemies())
                    {
                        enemy->SetUnavailable();
                    }
                }

                if (shuffleTracker == 1)
                    cardNum = 0;
            }
        }
        else
        {
            if (shuffleTracker == 0) {
                player->GetDeck().at(0)->Unhover();
            }
            else {
                player->GetDeck().at(5)->Unhover();
            }
        }

        //Check if mouse was clicked on 2nd card
        if (mPoint.x > 394.0f && mPoint.x < 461.0f && mPoint.y < 700.0f && mPoint.y > 581.0f) {
            if (shuffleTracker == 0) {
                player->GetDeck().at(1)->Hover();
            }
            else {
                player->GetDeck().at(6)->Hover();
            }
            if (m_pKeyboard->TriggerDown(VK_LBUTTON))
            {
                if (shuffleTracker == 0) {
                    cardNum = 1;
                }
                else {
                    cardNum = 6;
                }
                player->GetDeck().at(cardNum)->Select();
                player->SetCard(cardNum);

                if (player->GetDeck().at(cardNum)->dealDamage() > 0)
                {
                    player->SetUnavailable();
                }
                else
                {
                    for (auto enemy : m_pObjectManager->GetEnemies())
                    {
                        enemy->SetUnavailable();
                    }
                }

                if (shuffleTracker == 1)
                    cardNum = 1;
            }
        }
        else
        {
            if (shuffleTracker == 0) {
                player->GetDeck().at(1)->Unhover();
            }
            else {
                player->GetDeck().at(6)->Unhover();
            }
        }

        //Check if mouse was clicked on 3rd card
        if (mPoint.x > 474.0f && mPoint.x < 541.0f && mPoint.y < 700.0f && mPoint.y > 581.0f) {
            if (shuffleTracker == 0) {
                player->GetDeck().at(2)->Hover();
            }
            else {
                player->GetDeck().at(7)->Hover();
            }
            if (m_pKeyboard->TriggerDown(VK_LBUTTON))
            {
                if (shuffleTracker == 0) {
                    cardNum = 2;
                }
                else {
                    cardNum = 7;
                }
                player->GetDeck().at(cardNum)->Select();
                player->SetCard(cardNum);

                if (player->GetDeck().at(cardNum)->dealDamage() > 0)
                {
                    player->SetUnavailable();
                }
                else
                {
                    for (auto enemy : m_pObjectManager->GetEnemies())
                    {
                        enemy->SetUnavailable();
                    }
                }

                if (shuffleTracker == 1)
                    cardNum = 2;
            }
        }
        else
        {
            if (shuffleTracker == 0) {
                player->GetDeck().at(2)->Unhover();
            }
            else {
                player->GetDeck().at(7)->Unhover();
            }
        }

        //Check if mouse was clicked on 4th card
        if (mPoint.x > 554.0f && mPoint.x < 621.0f && mPoint.y < 700.0f && mPoint.y > 581.0f) {
            if (shuffleTracker == 0) {
                player->GetDeck().at(3)->Hover();
            }
            else {
                player->GetDeck().at(8)->Hover();
            }
            if (m_pKeyboard->TriggerDown(VK_LBUTTON))
            {
                if (shuffleTracker == 0) {
                    cardNum = 3;
                }
                else {
                    cardNum = 8;
                }
                player->GetDeck().at(cardNum)->Select();
                player->SetCard(cardNum);

                if (player->GetDeck().at(cardNum)->dealDamage() > 0)
                {
                    player->SetUnavailable();
                }
                else
                {
                    for (auto enemy : m_pObjectManager->GetEnemies())
                    {
                        enemy->SetUnavailable();
                    }
                }

                if (shuffleTracker == 1)
                    cardNum = 3;
            }
        }
        else
        {
            if (shuffleTracker == 0) {
                player->GetDeck().at(3)->Unhover();
            }
            else {
                player->GetDeck().at(8)->Unhover();
            }
        }

        //Check if mouse was clicked on 5th card
        if (mPoint.x > 634.0f && mPoint.x < 706.0f && mPoint.y < 700.0f && mPoint.y > 581.0f) {
            if (shuffleTracker == 0) {
                player->GetDeck().at(4)->Hover();
            }
            else {
                player->GetDeck().at(9)->Hover();
            }
            if (m_pKeyboard->TriggerDown(VK_LBUTTON))
            {
                if (shuffleTracker == 0) {
                    cardNum = 4;
                }
                else {
                    cardNum = 9;
                }
                player->GetDeck().at(cardNum)->Select();
                player->SetCard(cardNum);

                if (player->GetDeck().at(cardNum)->dealDamage() > 0)
                {
                    player->SetUnavailable();
                }
                else
                {
                    for (auto enemy : m_pObjectManager->GetEnemies())
                    {
                        enemy->SetUnavailable();
                    }
                }

                if (shuffleTracker == 1)
                    cardNum = 4;
            }
        }
        else
        {
            if (shuffleTracker == 0) {
                player->GetDeck().at(4)->Unhover();
            }
            else {
                player->GetDeck().at(9)->Unhover();
            }
        }
    } else if (state == GameState::NewCard) {
        if (mPoint.x > 112.0f && mPoint.x < 186.0f && mPoint.y < 494.0f && mPoint.y > 370.0f) {
            player->GetDeck().at(0)->Hover();
            if (m_pKeyboard->TriggerDown(VK_LBUTTON))
            {
                cardNum = 0;
                player->GetDeck().at(cardNum)->UpgradeCard();
                player->SetCard(cardNum);
                cardUpgraded = false;
                for (int i = 0; i < player->GetDeck().size(); i++) {
                    player->GetDeck().at(i)->AddCardRemove(i);
                }
                replaceCards();
                state = GameState::Map;
            }
        }
        else
        {
            player->GetDeck().at(0)->Unhover();
        }

        //Check if mouse was clicked on 2nd card
        if (mPoint.x > 192.0f && mPoint.x < 266.0f && mPoint.y < 494.0f && mPoint.y > 370.0f) {
            player->GetDeck().at(1)->Hover();
            if (m_pKeyboard->TriggerDown(VK_LBUTTON))
            {
                cardNum = 1;
                player->GetDeck().at(cardNum)->UpgradeCard();
                player->SetCard(cardNum);
                cardUpgraded = false;
                for (int i = 0; i < player->GetDeck().size(); i++) {
                    player->GetDeck().at(i)->AddCardRemove(i);
                }
                replaceCards();
                state = GameState::Map;
            }
        }
        else
        {
            player->GetDeck().at(1)->Unhover();
        }

        //Check if mouse was clicked on 3rd card
        if (mPoint.x > 272.0f && mPoint.x < 346.0f && mPoint.y < 494.0f && mPoint.y > 370.0f) {
            player->GetDeck().at(2)->Hover();
            if (m_pKeyboard->TriggerDown(VK_LBUTTON))
            {
                cardNum = 2;
                player->GetDeck().at(cardNum)->UpgradeCard();
                player->SetCard(cardNum);
                cardUpgraded = false;
                for (int i = 0; i < player->GetDeck().size(); i++) {
                    player->GetDeck().at(i)->AddCardRemove(i);
                }
                replaceCards();
                state = GameState::Map;
            }
        }
        else
        {
            player->GetDeck().at(2)->Unhover();
        }

        //Check if mouse was clicked on 4th card
        if (mPoint.x > 352.0f && mPoint.x < 426.0f && mPoint.y < 494.0f && mPoint.y > 370.0f) {
            player->GetDeck().at(3)->Hover();
            if (m_pKeyboard->TriggerDown(VK_LBUTTON))
            {
                cardNum = 3;
                player->GetDeck().at(cardNum)->UpgradeCard();
                player->SetCard(cardNum);
                cardUpgraded = false;
                for (int i = 0; i < player->GetDeck().size(); i++) {
                    player->GetDeck().at(i)->AddCardRemove(i);
                }
                replaceCards();
                state = GameState::Map;
            }
        }
        else
        {
            player->GetDeck().at(3)->Unhover();
        }

        //Check if mouse was clicked on 5th card
        if (mPoint.x > 432.0f && mPoint.x < 506.0f && mPoint.y < 494.0f && mPoint.y > 370.0f) {
            player->GetDeck().at(4)->Hover();
            if (m_pKeyboard->TriggerDown(VK_LBUTTON))
            {
                cardNum = 4;
                player->GetDeck().at(cardNum)->UpgradeCard();
                player->SetCard(cardNum);
                cardUpgraded = false;
                for (int i = 0; i < player->GetDeck().size(); i++) {
                    player->GetDeck().at(i)->AddCardRemove(i);
                }
                replaceCards();
                state = GameState::Map;
            }
        }
        else
        {
            player->GetDeck().at(4)->Unhover();
        }

        if (mPoint.x > 512.0f && mPoint.x < 586.0f && mPoint.y < 494.0f && mPoint.y > 370.0f) {
            player->GetDeck().at(5)->Hover();
            if (m_pKeyboard->TriggerDown(VK_LBUTTON))
            {
                cardNum = 5;
                player->GetDeck().at(cardNum)->UpgradeCard();
                player->SetCard(cardNum);
                cardUpgraded = false;
                for (int i = 0; i < player->GetDeck().size(); i++) {
                    player->GetDeck().at(i)->AddCardRemove(i);
                }
                replaceCards();
                state = GameState::Map;
            }
        }
        else
        {
            player->GetDeck().at(5)->Unhover();
        }

        if (mPoint.x > 592.0f && mPoint.x < 666.0f && mPoint.y < 494.0f && mPoint.y > 370.0f) {
            player->GetDeck().at(6)->Hover();
            if (m_pKeyboard->TriggerDown(VK_LBUTTON))
            {
                cardNum = 6;
                player->GetDeck().at(cardNum)->UpgradeCard();
                player->SetCard(cardNum);
                cardUpgraded = false;
                for (int i = 0; i < player->GetDeck().size(); i++) {
                    player->GetDeck().at(i)->AddCardRemove(i);
                }
                replaceCards();
                state = GameState::Map;
            }
        }
        else
        {
            player->GetDeck().at(6)->Unhover();
        }

        if (mPoint.x > 672.0f && mPoint.x < 746.0f && mPoint.y < 494.0f && mPoint.y > 370.0f) {
            player->GetDeck().at(7)->Hover();
            if (m_pKeyboard->TriggerDown(VK_LBUTTON))
            {
                cardNum = 7;
                player->GetDeck().at(cardNum)->UpgradeCard();
                player->SetCard(cardNum);
                cardUpgraded = false;
                for (int i = 0; i < player->GetDeck().size(); i++) {
                    player->GetDeck().at(i)->AddCardRemove(i);
                }
                replaceCards();
                state = GameState::Map;
            }
        }
        else
        {
            player->GetDeck().at(7)->Unhover();
        }

        if (mPoint.x > 752.0f && mPoint.x < 826.0f && mPoint.y < 494.0f && mPoint.y > 370.0f) {
            player->GetDeck().at(8)->Hover();
            if (m_pKeyboard->TriggerDown(VK_LBUTTON))
            {
                cardNum = 8;
                player->GetDeck().at(cardNum)->UpgradeCard();
                player->SetCard(cardNum);
                cardUpgraded = false;
                for (int i = 0; i < player->GetDeck().size(); i++) {
                    player->GetDeck().at(i)->AddCardRemove(i);
                }
                replaceCards();
                state = GameState::Map;
            }
        }
        else
        {
            player->GetDeck().at(8)->Unhover();
        }

        if (mPoint.x > 832.0f && mPoint.x < 906.0f && mPoint.y < 494.0f && mPoint.y > 370.0f) {
            player->GetDeck().at(9)->Hover();
            if (m_pKeyboard->TriggerDown(VK_LBUTTON))
            {
                cardNum = 9;
                player->GetDeck().at(cardNum)->UpgradeCard();
                player->SetCard(cardNum);
                cardUpgraded = false;
                for (int i = 0; i < player->GetDeck().size(); i++) {
                    player->GetDeck().at(i)->AddCardRemove(i);
                }
                replaceCards();
                state = GameState::Map;
            }
        }
        else
        {
            player->GetDeck().at(9)->Unhover();
        }
    }
}

void CGame::ChooseTarget(int numEnemies) {
    findMouse(); //Find mouse position when button is clicked
    Vector2 mousePos = Vector2(mPoint.x, m_nWinHeight - mPoint.y);

    if (shuffleTracker == 1) {
        cardNum = cardNum + 5;
    }

    //If the selected card deals damage, select an enemy
    if (player->GetDeck().at(cardNum)->dealDamage() > 0)
    {
        switch (numEnemies) {
        case 1:
            if (mPoint.x > 855.0f && mPoint.x < 943.0f && mPoint.y < 463.0f && mPoint.y > 305.0f && !IsMarked(cardNum)) {
                choseEnemy = 0;
                turnNum++;
                player->GetDeck().at(cardNum)->RemoveCard(cardNum);
                markUsed(cardNum);
                player->PlayCard(Vector2(m_nWinWidth / 2.0f, m_nWinHeight / 2.0f));
                player->SetNormal();
            }
            else {
                player->GetDeck().at(cardNum)->Unselect();
                cardNum = -10;

                player->SetNormal();
            }
            break;
        case 2:
            if (mPoint.x > 855.0f && mPoint.x < 943.0f && mPoint.y < 351.0f && mPoint.y > 191.0f && !IsMarked(cardNum)) {
                choseEnemy = 0;
                turnNum++;
                player->GetDeck().at(cardNum)->RemoveCard(cardNum);
                markUsed(cardNum);
                player->PlayCard(Vector2(m_nWinWidth / 2.0f, m_nWinHeight / 2.0f));
                player->SetNormal();
            }
            else if (mPoint.x > 855.0f && mPoint.x < 943.0f && mPoint.y < 575.0f && mPoint.y > 415.0f && !IsMarked(cardNum)) {
                choseEnemy = 1;
                turnNum++;
                player->GetDeck().at(cardNum)->RemoveCard(cardNum);
                markUsed(cardNum);
                player->PlayCard(Vector2(m_nWinWidth / 2.0f, m_nWinHeight / 2.0f));
                player->SetNormal();
            }
            else {
                player->GetDeck().at(cardNum)->Unselect();
                cardNum = -10;

                player->SetNormal();
            }
            break;
        case 3:
            if (mPoint.x > 855.0f && mPoint.x < 943.0f && mPoint.y < 239.0f && mPoint.y > 76.0f && !IsMarked(cardNum)) {
                choseEnemy = 0;
                turnNum++;
                player->GetDeck().at(cardNum)->RemoveCard(cardNum);
                markUsed(cardNum);
                player->PlayCard(Vector2(m_nWinWidth / 2.0f, m_nWinHeight / 2.0f));
                player->SetNormal();
            }
            else if (mPoint.x > 855.0f && mPoint.x < 943.0f && mPoint.y < 463.0f && mPoint.y > 303.0f && !IsMarked(cardNum)) {
                choseEnemy = 1;
                turnNum++;
                player->GetDeck().at(cardNum)->RemoveCard(cardNum);
                markUsed(cardNum);
                player->PlayCard(Vector2(m_nWinWidth / 2.0f, m_nWinHeight / 2.0f));
                player->SetNormal();
            }
            else if (mPoint.x > 855.0f && mPoint.x < 943.0f && mPoint.y < 688.0f && mPoint.y > 527.0f && !IsMarked(cardNum)) {
                choseEnemy = 2;
                turnNum++;
                player->GetDeck().at(cardNum)->RemoveCard(cardNum);
                markUsed(cardNum);
                player->PlayCard(Vector2(m_nWinWidth / 2.0f, m_nWinHeight / 2.0f));
                player->SetNormal();
            }
            else {
                player->GetDeck().at(cardNum)->Unselect();
                cardNum = -10;

                player->SetNormal();
            }
            break;
        case 4:
            if (mPoint.x > 855.0f && mPoint.x < 943.0f && mPoint.y < 239.0f && mPoint.y > 76.0f && !IsMarked(cardNum)) {
                choseEnemy = 0;
                turnNum++;
                player->GetDeck().at(cardNum)->RemoveCard(cardNum);
                markUsed(cardNum);
                player->PlayCard(Vector2(m_nWinWidth / 2.0f, m_nWinHeight / 2.0f));
                player->SetNormal();
            }
            else if (mPoint.x > 855.0f && mPoint.x < 943.0f && mPoint.y < 463.0f && mPoint.y > 303.0f && !IsMarked(cardNum)) {
                choseEnemy = 1;
                turnNum++;
                player->GetDeck().at(cardNum)->RemoveCard(cardNum);
                markUsed(cardNum);
                player->PlayCard(Vector2(m_nWinWidth / 2.0f, m_nWinHeight / 2.0f));
                player->SetNormal();
            }
            else if (mPoint.x > 855.0f && mPoint.x < 943.0f && mPoint.y < 688.0f && mPoint.y > 527.0f && !IsMarked(cardNum)) {
                choseEnemy = 2;
                turnNum++;
                player->GetDeck().at(cardNum)->RemoveCard(cardNum);
                markUsed(cardNum);
                player->PlayCard(Vector2(m_nWinWidth / 2.0f, m_nWinHeight / 2.0f));
                player->SetNormal();
            }
            else if (mPoint.x > 706.0f && mPoint.x < 798.0f && mPoint.y < 462.0f && mPoint.y > 302.0f && !IsMarked(cardNum)) {
                choseEnemy = 3;
                turnNum++;
                player->GetDeck().at(cardNum)->RemoveCard(cardNum);
                markUsed(cardNum);
                player->PlayCard(Vector2(m_nWinWidth / 2.0f, m_nWinHeight / 2.0f));
                player->SetNormal();
            }
            else {
                player->GetDeck().at(cardNum)->Unselect();
                cardNum = -10;

                player->SetNormal();
            }
            break;
        case 5:
            if (mPoint.x > 855.0f && mPoint.x < 943.0f && mPoint.y < 239.0f && mPoint.y > 76.0f && !IsMarked(cardNum)) {
                choseEnemy = 0;
                turnNum++;
                player->GetDeck().at(cardNum)->RemoveCard(cardNum);
                markUsed(cardNum);
                player->PlayCard(Vector2(m_nWinWidth / 2.0f, m_nWinHeight / 2.0f));
            }
            else if (mPoint.x > 855.0f && mPoint.x < 943.0f && mPoint.y < 463.0f && mPoint.y > 303.0f && !IsMarked(cardNum)) {
                choseEnemy = 1;
                turnNum++;
                player->GetDeck().at(cardNum)->RemoveCard(cardNum);
                markUsed(cardNum);
                player->PlayCard(Vector2(m_nWinWidth / 2.0f, m_nWinHeight / 2.0f));
                player->SetNormal();
            }
            else if (mPoint.x > 855.0f && mPoint.x < 943.0f && mPoint.y < 688.0f && mPoint.y > 527.0f && !IsMarked(cardNum)) {
                choseEnemy = 2;
                turnNum++;
                player->GetDeck().at(cardNum)->RemoveCard(cardNum);
                markUsed(cardNum);
                player->PlayCard(Vector2(m_nWinWidth / 2.0f, m_nWinHeight / 2.0f));
                player->SetNormal();
            }
            else if (mPoint.x > 706.0f && mPoint.x < 798.0f && mPoint.y < 351.0f && mPoint.y > 189.0f && !IsMarked(cardNum)) {
                choseEnemy = 3;
                turnNum++;
                player->GetDeck().at(cardNum)->RemoveCard(cardNum);
                markUsed(cardNum);
                player->PlayCard(Vector2(m_nWinWidth / 2.0f, m_nWinHeight / 2.0f));
                player->SetNormal();
            }
            else if (mPoint.x > 706.0f && mPoint.x < 798.0f && mPoint.y < 577.0f && mPoint.y > 415.0f && !IsMarked(cardNum)) {
                choseEnemy = 4;
                turnNum++;
                player->GetDeck().at(cardNum)->RemoveCard(cardNum);
                markUsed(cardNum);
                player->PlayCard(Vector2(m_nWinWidth / 2.0f, m_nWinHeight / 2.0f));
                player->SetNormal();
            }
            else {
                player->GetDeck().at(cardNum)->Unselect();
                cardNum = -10;

                player->SetNormal();
            }
            break;
        default:
            break;
        }
    }
    else //Otherwise, select the player
    {
        if (m_pKeyboard->TriggerDown(VK_LBUTTON))
        {
            float playerWidth = m_pRenderer->GetWidth(eSprite::Player);
            float playerHeight = m_pRenderer->GetHeight(eSprite::Player);

            Vector2 bottomRight = Vector2(player->m_vPos.x + playerWidth / 2, player->m_vPos.y + playerHeight / 2);
            Vector2 topLeft = Vector2(player->m_vPos.x - playerWidth / 2, player->m_vPos.y - playerHeight / 2);

            if(mPoint.x > 79.0f && mPoint.x < 174.0f && mPoint.y < 410.0f && mPoint.y > 267.0f /* && !IsMarked(cardNum) */ )
            //if (mousePos.x >= topLeft.x && mousePos.x <= bottomRight.x &&
            //    mousePos.y >= topLeft.y && mousePos.y <= bottomRight.y)
            {
                turnNum++;
                player->GetDeck().at(cardNum)->RemoveCard(cardNum);
                markUsed(cardNum);
                player->PlayCard(Vector2(m_nWinWidth / 2.0f, m_nWinHeight / 2.0f));   

                for (auto enemy : m_pObjectManager->GetEnemies())
                {
                    enemy->SetNormal();
                }
            }
            else
            {
                player->GetDeck().at(cardNum)->Unselect();
                cardNum = -10;

                for (auto enemy : m_pObjectManager->GetEnemies())
                {
                    enemy->SetNormal();
                }
            }
        }
    }
}

void CGame::LoadEnemies(int numEnemies)
{
    for (int i = 0; i < numEnemies; i++)
    {
        m_pObjectManager->create(eSprite::Enemy, Vector2(125, 430));
    }
}
