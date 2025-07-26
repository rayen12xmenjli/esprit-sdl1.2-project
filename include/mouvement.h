#ifndef MOUVEMENT_H
#define MOUVEMENT_H

#include "game.h"
#include "collision.h"
#include "soldier.h"
#include "soldier2.h"
#include "robot.h"
#include "enemy.h"
#include "boss.h"
#include "npc.h"
#include "player2.h" // Added for Player2

int onGround(GAME *game, Player *player);
int onGroundSoldier(GAME *game, Soldier *soldier);
int onGroundSoldier2(GAME *game, Soldier2 *soldier);
int onGroundEntity(GAME *game, SDL_Rect *position, int world_x);
int onGroundBoss(GAME *game, Boss *boss);
int onGroundNPC(GAME *game, NPC *npc);
int onGroundEnemyLvl2(GAME *game, SDL_Rect *position, int world_x);
void movement(GAME *game);
int onGroundPlayer2(GAME *game, Player2 *player2); // Added for Player2 ground detection
void movementPlayer2(GAME *game); // Added for Player2 movement

#endif
