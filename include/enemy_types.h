#ifndef ENEMY_TYPES_H
#define ENEMY_TYPES_H

#include <SDL/SDL.h>


#define SPRITE_WIDTH 128
#define SPRITE_HEIGHT 128
#define MAX_FALL_SPEED 10.0f
#define ATTACK_DISTANCE 60
#define CHASE_DISTANCE 500

typedef struct {
    SDL_Rect position;
    int world_x;
    float xVelocity;
    float yVelocity;
    int onGround;
    EnemyState state; // Use EnemyState from enemy.h
    int frame;
    int totalFrames;
    int direction;
    int health;
    int maxHealth;
    int active;
    Uint32 lastFrameTime;
    int attackCooldown;
} Mummy;

typedef struct {
    SDL_Rect position;
    int world_x;
    float xVelocity;
    float yVelocity;
    int onGround;
    EnemyState state;
    int frame;
    int totalFrames;
    int direction;
    int health;
    int maxHealth;
    int active;
    Uint32 lastFrameTime;
    int attackCooldown;
    SDL_Rect projectilePos;
    int projectileActive;
    float projectileDx;
} Deceased;

typedef struct {
    SDL_Rect position;
    int world_x;
    float xVelocity;
    float yVelocity;
    int onGround;
    EnemyState state;
    int frame;
    int totalFrames;
    int direction;
    int health;
    int maxHealth;
    int active;
    Uint32 lastFrameTime;
    int attackCooldown;
    int hasPetrified;
} Gorgon;

typedef struct {
    SDL_Rect position;
    int world_x;
    float xVelocity;
    float yVelocity;
    int onGround;
    EnemyState state;
    int frame;
    int totalFrames;
    int direction;
    int health;
    int maxHealth;
    int active;
    Uint32 lastFrameTime;
    int attackCooldown;
    int hasFallen;
} SkeletonSpearman;

typedef struct {
    SDL_Rect position;
    int world_x;
    float xVelocity;
    float yVelocity;
    int onGround;
    EnemyState state;
    int frame;
    int totalFrames;
    int direction;
    int health;
    int maxHealth;
    int active;
    Uint32 lastFrameTime;
    int attackCooldown;
    int hasFallen;
} SkeletonWarrior;

typedef struct {
    SDL_Rect position;
    int world_x;
    float xVelocity;
    float yVelocity;
    int onGround;
    EnemyState state;
    int frame;
    int totalFrames;
    int direction;
    int health;
    int maxHealth;
    int active;
    Uint32 lastFrameTime;
    int attackCooldown;
} EvilWizard;

typedef struct {
    SDL_Rect position;
    int world_x;
    float xVelocity;
    float yVelocity;
    int onGround;
    EnemyState state;
    int frame;
    int totalFrames;
    int direction;
    int health;
    int maxHealth;
    int active;
    Uint32 lastFrameTime;
    int attackCooldown;
    SDL_Rect projectilePos;
    int projectileActive;
    float projectileDx;
} Necromancer;

#endif
