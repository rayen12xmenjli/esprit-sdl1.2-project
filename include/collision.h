#ifndef COLLISION_H
#define COLLISION_H

#include <SDL/SDL.h>
#include "game.h"
#include "robot.h"
#include "enemy.h"
#include "npc.h"
#include "npc2.h"
#include "enemylvl2.h" 
#include "player2.h" // Added for Player2

void PutPixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
void placePlayerOnGround(GAME *game);
void placeSoldierOnGround(GAME *game, Soldier *soldier);
void placeSoldier2OnGround(GAME *game, Soldier2 *soldier);
void placeRobotOnGround(GAME *game, Robot *robot);
void placeEnemyOnGround(GAME *game, Enemy *enemy);
void placeEnemyLvl2OnGround(GAME *game, SDL_Rect *position, int world_x);
void placeNPCOnGround(GAME *game, NPC *npc);
void placeNPC2OnGround(GAME *game, NPC2 *npc);
Color collision_color(GAME *game, int x, int y);
void placePlayer2OnGround(GAME *game); // Added for Player2

#endif
