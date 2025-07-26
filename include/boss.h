#ifndef BOSS_H
#define BOSS_H

#include <SDL/SDL.h>

#define MAX_BOSS_FRAMES 16 // Max frames for any boss state
#define MAX_VFX_FRAMES 14  // Max frames for any VFX

// Forward declaration of GAME to avoid circular dependency
typedef struct GAME GAME;

typedef enum {
    BOSS_IDLE,
    BOSS_WALK,
    BOSS_ATTACK,
    BOSS_TAKEHIT,
    BOSS_DEATH
} BossState;

typedef struct {
    int active;
    int frame, totalFrames;
    int x, y;
    int frameWidth, frameHeight;
    SDL_Surface** frames;
    int frameDelay;
    int frameDelayThreshold;
} VFX;

typedef struct {
    int world_x, y;
    SDL_Rect position;
    int health, maxHealth;
    BossState state;
    int frame, totalFrames;
    int frameDelay;
    int invulnTimer;
    int facingLeft;
    int active;
    int attackCooldown;
    int angry;
    int hasDamagedThisAttack;
    SDL_Surface** idleFrames;
    int idleFrameCount;
    SDL_Surface** walkFrames;
    int walkFrameCount;
    SDL_Surface** attackFrames;
    int attackFrameCount;
    SDL_Surface** takehitFrames;
    int takehitFrameCount;
    SDL_Surface** deathFrames;
    int deathFrameCount;
    VFX iceSlashVFX;
    VFX frostExplosionVFX[3];
    VFX umbrellaVFX;
    SDL_Surface* levelUpIcon;
    SDL_Rect levelUpIconPosition;
    int levelUpIconActive;
    Uint32 levelUpIconSpawnTime;
    int levelUpFlickerCounter;
} Boss;

void initBoss(Boss* boss, int x, int y);
void updateBoss(Boss* boss, GAME* game);
void renderBoss(SDL_Surface* screen, Boss* boss, int scroll_x, GAME* game);
void freeBoss(Boss* boss);
void placeBossOnGround(GAME* game, Boss* boss);

#endif
