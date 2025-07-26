#ifndef SOLDIER_H
#define SOLDIER_H

#include <SDL/SDL.h>
#include "utils.h"
#include "constants.h"


struct GAME;



typedef enum {
    SOLDIER_IDLE,
    SOLDIER_WALK,
    SOLDIER_ATTACK,
    SOLDIER_SHOOT,
    SOLDIER_SHOT_2,
    SOLDIER_GRENADE,
    SOLDIER_RECHARGE,
    SOLDIER_THROW,
    SOLDIER_HURT,
    SOLDIER_DEAD
} SoldierState;

typedef struct {
    int frameCounts[10];
} SoldierSprites;

typedef struct {
    SDL_Rect position;
    int frame;
    int totalFrames;
    int active;
} Explosion;

typedef struct {
    SDL_Rect position;
    int world_x;
    float xVelocity;
    float yVelocity;
    int onGround;
    SoldierState state;
    int frame;
    int totalFrames;
    int direction;
    int facingLeft;
    int health;
    int maxHealth;
    int active;
    int patrolLeft;
    int patrolRight;
    Uint32 lastAttackTime;
    Uint32 lastShotTime;
    Uint32 lastThrowTime;
    int attackCooldown;
    int shootCooldown;
    int throwCooldown;
    int stateTransitionDelay;
    int bullets;
    int grenadeCooldown;
    int grenadeExplosionDelay;
    int damageCooldown;
    int invulnerabilityTimer;
    int frameDelay;
    int activationZone;
    int fireZone;
    int patrolDirection;
    int patrolTimer;
    Explosion explosion;
} Soldier;

void initSoldierSprites(void);
void freeSoldierSprites(void);
void initSoldier(Soldier *soldier, int x, int y, struct GAME *game);
void updateSoldier(Soldier *soldier, SDL_Rect playerPos, int *playerHealth, int playerMaxHealth, struct GAME *game);
void renderSoldier(SDL_Surface *screen, Soldier *soldier, int scroll_x);
void freeSoldier(Soldier *soldier);

#endif
