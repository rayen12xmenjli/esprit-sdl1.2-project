#ifndef ENEMYLVL2_H
#define ENEMYLVL2_H

#include <SDL/SDL.h>
#include "utils.h"


#define MAX_FALL_SPEED 10.0f
#define ATTACK_DISTANCE 60
#define CHASE_DISTANCE 500
#define MAX_PARTICLES 100


struct GAME;

// Enum for level 2 enemy states
typedef enum {
    ENEMY2_IDLE,
    ENEMY2_WALKING,
    ENEMY2_ATTACKING,
    ENEMY2_HURT,
    ENEMY2_DYING,
    ENEMY2_DEAD,
    ENEMY2_RUNNING,
    ENEMY2_SPECIAL,
    ENEMY2_FALL,
    ENEMY2_PROTECT,
    ENEMY2_RUN_ATTACK
} Enemy2State;

// Particle structure for Gorgon
typedef struct {
    float x, y;
    float dx, dy;
    int lifetime;
    int active;
    SDL_Color color;
} Particle;

// Struct definitions for level 2 enemies
typedef struct {
    SDL_Rect position;
    int world_x;
    float xVelocity;
    float yVelocity;
    int onGround;
    Enemy2State state;
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
    Enemy2State state;
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
    float projectileDy;
    float projectileDistance;
} Deceased;

typedef struct {
    SDL_Rect position;
    int world_x;
    float xVelocity;
    float yVelocity;
    int onGround;
    Enemy2State state;
    int frame;
    int totalFrames;
    int direction;
    int health;
    int maxHealth;
    int active;
    Uint32 lastFrameTime;
    int attackCooldown;
    int hasPetrified;
    Particle particles[MAX_PARTICLES];
} Gorgon;

typedef struct {
    SDL_Rect position;
    int world_x;
    float xVelocity;
    float yVelocity;
    int onGround;
    Enemy2State state;
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

// Function prototypes
// Mummy
void initMummy(Mummy *mummy, int x, int y, struct GAME *game);
void updateMummy(Mummy *mummy, int *playerHealth, int playerMaxHealth, struct GAME *game, Uint32 currentTime);
void renderMummy(SDL_Surface *screen, Mummy *mummy, int scroll_x);
void freeMummy(Mummy *mummy);

// Deceased
void initDeceased(Deceased *deceased, int x, int y, struct GAME *game);
void updateDeceased(Deceased *deceased, int *playerHealth, int playerMaxHealth, struct GAME *game, Uint32 currentTime);
void renderDeceased(SDL_Surface *screen, Deceased *deceased, int scroll_x);
void freeDeceased(Deceased *deceased);

// Gorgon
void initGorgon(Gorgon *gorgon, int x, int y, struct GAME *game);
void updateGorgon(Gorgon *gorgon, int *playerHealth, int playerMaxHealth, struct GAME *game, Uint32 currentTime);
void renderGorgon(SDL_Surface *screen, Gorgon *gorgon, int scroll_x);
void freeGorgon(Gorgon *gorgon);

// SkeletonSpearman
void initSkeletonSpearman(SkeletonSpearman *spearman, int x, int y, struct GAME *game);
void updateSkeletonSpearman(SkeletonSpearman *spearman, int *playerHealth, int playerMaxHealth, struct GAME *game, Uint32 currentTime);
void renderSkeletonSpearman(SDL_Surface *screen, SkeletonSpearman *spearman, int scroll_x);
void freeSkeletonSpearman(SkeletonSpearman *spearman);

// Free all level 2 enemy sprites
void freeEnemyLvl2Sprites(void);

#endif
