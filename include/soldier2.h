#ifndef SOLDIER2_H
#define SOLDIER2_H

#include <SDL/SDL.h>
#include "utils.h"
#include "constants.h"

struct GAME;

typedef enum {
    SOLDIER2_IDLE,
    SOLDIER2_WALK,
    SOLDIER2_ATTACK,
    SOLDIER2_SHOT_2,
    SOLDIER2_GRENADE,
    SOLDIER2_RECHARGE,
    SOLDIER2_HURT,
    SOLDIER2_DEAD
} Soldier2State;

typedef struct {
    SDL_Rect position;
    int frame;
    int totalFrames;
    int active;
} Smoke;

typedef struct {
    SDL_Rect position;
    int world_x;
    float xVelocity;
    float yVelocity;
    int onGround;
    Soldier2State state;
    int frame;
    int totalFrames;
    int health;
    int maxHealth;
    int bullets;
    int damageCooldown;
    int invulnerabilityTimer;
    int facingLeft;
    int active;
    int frameDelay;
    int patrolDirection;
    int patrolTimer;
    int patrolLeft;
    int patrolRight;
    int grenadeCooldown;
    struct {
        SDL_Rect position;
        int frame;
        int totalFrames;
        int active;
    } smoke;
} Soldier2;



void initSoldier2(Soldier2 *soldier, int x, int y, struct GAME *game);
void updateSoldier2(Soldier2 *soldier, SDL_Rect playerPos, int *playerHealth, int playerMaxHealth, struct GAME *game);
void renderSoldier2(SDL_Surface *screen, Soldier2 *soldier, int scroll_x);
void freeSoldier2(Soldier2 *soldier);
void freeSoldier2Sprites(void);

#endif
