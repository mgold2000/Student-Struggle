/// \file GameDefines.h
/// \brief Game specific defines.

#ifndef __L4RC_GAME_GAMEDEFINES_H__
#define __L4RC_GAME_GAMEDEFINES_H__

#include "Defines.h"
#include "Sound.h"

/// \brief Sprite enumerated type.
///
/// An enumerated type for the sprites, which will be cast to an unsigned
/// integer and used for the index of the corresponding texture in graphics
/// memory. `Size` must be last.

enum class eSprite: UINT{
  PlayerRunning, Player, Enemy, Background, Card, PlayerSpritesheet, EnemySpritesheet, EnemyRunning, 
  Node, Line, DoorClosed, DoorOpen, MapBackground, Checkmark, WinBackground, LoseBackground,
  BookSpritesheet, BookTurning, Paper, Boss, Nerd, MenuBackground, PlayButton, CardBackground,
  Laptop, IntroBackground, PlayAgainButton, Calendar, CardDamage, CardShield, CardHealth,
  NerdBackground,
  Size  //MUST BE LAST
}; //eSprite

/// \brief Sound enumerated type.
///
/// An enumerated type for the sounds, which will be cast to an unsigned
/// integer and used for the index of the corresponding sample. `Size` must 
/// be last.

enum class eSound: UINT{
  StudyTime, EndlessHomework, Lame, PlayerDamage, EnemyDamage, Auto, PowerNap, Time, Size  //MUST BE LAST
}; //eSound

#endif //__L4RC_GAME_GAMEDEFINES_H__
