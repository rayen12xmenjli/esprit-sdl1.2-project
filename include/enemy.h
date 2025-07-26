#ifndef ENEMY_H
#define ENEMY_H

#include <SDL/SDL.h>
#include "utils.h"

struct GAME;

typedef enum {
    ENEMY_IDLE,
    ENEMY_WALKING,
    ENEMY_ATTACKING,
    ENEMY_HURT,
    ENEMY_DEAD,
    ENEMY_SHUTDOWN,
    ENEMY_ENABLING
} EnemyState;

typedef struct {
    SDL_Rect position;
    EnemyState state;
    int frame;
    int totalFrames;
    int health;
    int maxHealth;
    int speed;
    int facingLeft;
    int active;
    float yVelocity;
    int onGround;
    int frameDelay;
    int damageCooldown;
    int invulnerabilityTimer;
    int activationZone;
    int attackZone;
    
    
    int world_x; // Added world_x
    // Sprite sheets
    SDL_Surface* idleSheet;
    SDL_Surface* idleSheetFlipped;
    SDL_Surface* walkSheet;
    SDL_Surface* walkSheetFlipped;
    SDL_Surface* attack1Sheet;
    SDL_Surface* attack1SheetFlipped;
    SDL_Surface* attack2Sheet;
    SDL_Surface* attack2SheetFlipped;
    SDL_Surface* attack3Sheet;
    SDL_Surface* attack3SheetFlipped;
    SDL_Surface* hurtSheet;
    SDL_Surface* hurtSheetFlipped;
    SDL_Surface* deadSheet;
    SDL_Surface* deadSheetFlipped;
    SDL_Surface* enablingSheet;
    SDL_Surface* enablingSheetFlipped;
    SDL_Surface* shutdownSheet;
    SDL_Surface* shutdownSheetFlipped;
} Enemy;

void initEnemy(Enemy* enemy, int x, int y);
void updateEnemy(Enemy* enemy, SDL_Rect playerPosition, int* playerHealth, int playerMaxHealth, struct GAME *game);
void renderEnemy(SDL_Surface* screen, Enemy* enemy, int scroll_x);
void freeEnemy(Enemy* enemy);
void freeEnemySprites(void);

#endif
