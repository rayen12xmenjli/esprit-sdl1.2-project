#include <SDL/SDL_image.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "enemylvl2.h"
#include "collision.h"
#include "mouvement.h"
#include "utils.h"
#include "game.h"

#define MOVE_SPEED 2.0f

// New sprite dimensions
#define SPRITE_WIDTH 256
#define SPRITE_HEIGHT 256

// SkeletonWarrior constants
#define WARRIOR_IDLE_FRAMES 4
#define WARRIOR_WALK_FRAMES 6
#define WARRIOR_ATTACK_FRAMES 8
#define WARRIOR_HURT_FRAMES 2
#define WARRIOR_DYING_FRAMES 5
#define WARRIOR_IDLE_DURATION 200
#define WARRIOR_WALK_DURATION 150
#define WARRIOR_ATTACK_DURATION 120
#define WARRIOR_HURT_DURATION 150
#define WARRIOR_DYING_DURATION 180

// Mummy constants
#define MUMMY_IDLE_FRAMES 4
#define MUMMY_WALK_FRAMES 6
#define MUMMY_ATTACK_FRAMES 5
#define MUMMY_HURT_FRAMES 2
#define MUMMY_DYING_FRAMES 5
#define MUMMY_IDLE_DURATION 200
#define MUMMY_WALK_DURATION 150
#define MUMMY_ATTACK_DURATION 120
#define MUMMY_HURT_DURATION 150
#define MUMMY_DYING_DURATION 180

// Deceased constants
#define DECEASED_IDLE_FRAMES 4
#define DECEASED_WALK_FRAMES 6
#define DECEASED_ATTACK_FRAMES 5
#define DECEASED_HURT_FRAMES 2
#define DECEASED_DYING_FRAMES 6
#define DECEASED_IDLE_DURATION 200
#define DECEASED_WALK_DURATION 150
#define DECEASED_ATTACK_DURATION 120
#define DECEASED_HURT_DURATION 150
#define DECEASED_DYING_DURATION 180

// Gorgon constants
#define GORGON_IDLE_FRAMES 5
#define GORGON_WALK_FRAMES 8
#define GORGON_RUN_FRAMES 7
#define GORGON_ATTACK_FRAMES 10
#define GORGON_HURT_FRAMES 3
#define GORGON_DYING_FRAMES 3
#define GORGON_SPECIAL_FRAMES 5
#define GORGON_IDLE_DURATION 200
#define GORGON_WALK_DURATION 150
#define GORGON_RUN_DURATION 100
#define GORGON_ATTACK_DURATION 120
#define GORGON_HURT_DURATION 150
#define GORGON_DYING_DURATION 180
#define GORGON_SPECIAL_DURATION 150

// SkeletonSpearman constants
#define SPEARMAN_IDLE_FRAMES 7
#define SPEARMAN_WALK_FRAMES 7
#define SPEARMAN_RUN_FRAMES 6
#define SPEARMAN_ATTACK_FRAMES 4
#define SPEARMAN_HURT_FRAMES 3
#define SPEARMAN_DYING_FRAMES 5
#define SPEARMAN_FALL_FRAMES 6
#define SPEARMAN_RUN_ATTACK_FRAMES 5
#define SPEARMAN_IDLE_DURATION 200
#define SPEARMAN_WALK_DURATION 150
#define SPEARMAN_RUN_DURATION 100
#define SPEARMAN_ATTACK_DURATION 120
#define SPEARMAN_HURT_DURATION 150
#define SPEARMAN_DYING_DURATION 180
#define SPEARMAN_FALL_DURATION 150
#define SPEARMAN_RUN_ATTACK_DURATION 120

// Sprite sheets
static SDL_Surface *mummyIdleSheet = NULL;
static SDL_Surface *mummyIdleSheetFlipped = NULL;
static SDL_Surface *mummyWalkSheet = NULL;
static SDL_Surface *mummyWalkSheetFlipped = NULL;
static SDL_Surface *mummyAttackSheet = NULL;
static SDL_Surface *mummyAttackSheetFlipped = NULL;
static SDL_Surface *mummyHurtSheet = NULL;
static SDL_Surface *mummyHurtSheetFlipped = NULL;
static SDL_Surface *mummyDyingSheet = NULL;
static SDL_Surface *mummyDyingSheetFlipped = NULL;

static SDL_Surface *deceasedIdleSheet = NULL;
static SDL_Surface *deceasedIdleSheetFlipped = NULL;
static SDL_Surface *deceasedWalkSheet = NULL;
static SDL_Surface *deceasedWalkSheetFlipped = NULL;
static SDL_Surface *deceasedAttackSheet = NULL;
static SDL_Surface *deceasedAttackSheetFlipped = NULL;
static SDL_Surface *deceasedHurtSheet = NULL;
static SDL_Surface *deceasedHurtSheetFlipped = NULL;
static SDL_Surface *deceasedDyingSheet = NULL;
static SDL_Surface *deceasedDyingSheetFlipped = NULL;
static SDL_Surface *deceasedProjectileSheet = NULL;

static SDL_Surface *gorgonIdleSheet = NULL;
static SDL_Surface *gorgonIdleSheetFlipped = NULL;
static SDL_Surface *gorgonWalkSheet = NULL;
static SDL_Surface *gorgonWalkSheetFlipped = NULL;
static SDL_Surface *gorgonRunSheet = NULL;
static SDL_Surface *gorgonRunSheetFlipped = NULL;
static SDL_Surface *gorgonAttackSheet = NULL;
static SDL_Surface *gorgonAttackSheetFlipped = NULL;
static SDL_Surface *gorgonHurtSheet = NULL;
static SDL_Surface *gorgonHurtSheetFlipped = NULL;
static SDL_Surface *gorgonDyingSheet = NULL;
static SDL_Surface *gorgonDyingSheetFlipped = NULL;
static SDL_Surface *gorgonSpecialSheet = NULL;
static SDL_Surface *gorgonSpecialSheetFlipped = NULL;

static SDL_Surface *spearmanIdleSheet = NULL;
static SDL_Surface *spearmanIdleSheetFlipped = NULL;
static SDL_Surface *spearmanWalkSheet = NULL;
static SDL_Surface *spearmanWalkSheetFlipped = NULL;
static SDL_Surface *spearmanRunSheet = NULL;
static SDL_Surface *spearmanRunSheetFlipped = NULL;
static SDL_Surface *spearmanAttackSheet = NULL;
static SDL_Surface *spearmanAttackSheetFlipped = NULL;
static SDL_Surface *spearmanHurtSheet = NULL;
static SDL_Surface *spearmanHurtSheetFlipped = NULL;
static SDL_Surface *spearmanDyingSheet = NULL;
static SDL_Surface *spearmanDyingSheetFlipped = NULL;
static SDL_Surface *spearmanFallSheet = NULL;
static SDL_Surface *spearmanFallSheetFlipped = NULL;
static SDL_Surface *spearmanRunAttackSheet = NULL;
static SDL_Surface *spearmanRunAttackSheetFlipped = NULL;

// Utility functions
static SDL_Surface* loadSprite(const char* path) {
    SDL_Surface *surface = IMG_Load(path);
    if (!surface) {
        fprintf(stderr, "Failed to load %s: %s\n", path, IMG_GetError());
        return NULL;
    }
    Uint32 colorkey = SDL_MapRGB(surface->format, 255, 255, 255);
    SDL_SetColorKey(surface, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
    return surface;
}

static void initParticles(Particle* particles, int count, float x, float y, SDL_Color color) {
    for (int i = 0; i < count; i++) {
        particles[i].x = x;
        particles[i].y = y;
        particles[i].dx = ((float)rand() / RAND_MAX - 0.5f) * 2.0f;
        particles[i].dy = ((float)rand() / RAND_MAX - 0.5f) * 2.0f;
        particles[i].lifetime = 500 + rand() % 500;
        particles[i].active = 1;
        particles[i].color = color;
    }
}

static void updateParticles(Particle* particles, int count, Uint32 deltaTime) {
    for (int i = 0; i < count; i++) {
        if (!particles[i].active) continue;
        particles[i].x += particles[i].dx * (deltaTime / 1000.0f) * 50;
        particles[i].y += particles[i].dy * (deltaTime / 1000.0f) * 50;
        particles[i].lifetime -= deltaTime;
        if (particles[i].lifetime <= 0) {
            particles[i].active = 0;
        }
    }
}

static void renderParticles(SDL_Surface* screen, Particle* particles, int count, int scroll_x) {
    for (int i = 0; i < count; i++) {
        if (!particles[i].active) continue;
        SDL_Rect rect = {(int)particles[i].x - scroll_x, (int)particles[i].y, 2, 2};
        SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, particles[i].color.r, particles[i].color.g, particles[i].color.b));
    }
}

// --- Mummy Functions ---
void initMummy(Mummy *mummy, int x, int y, struct GAME *game) {
    if (!mummy || !game) return;

    if (!mummyIdleSheet) {
        mummyIdleSheet = loadSprite("assets/characters/5Mummy/mummy_idle.png");
        mummyIdleSheetFlipped = flipHorizontally(mummyIdleSheet);
        mummyWalkSheet = loadSprite("assets/characters/5Mummy/mummy_walk.png");
        mummyWalkSheetFlipped = flipHorizontally(mummyWalkSheet);
        mummyAttackSheet = loadSprite("assets/characters/5Mummy/mummy_attack.png");
        mummyAttackSheetFlipped = flipHorizontally(mummyAttackSheet);
        mummyHurtSheet = loadSprite("assets/characters/5Mummy/mummy_hurt.png");
        mummyHurtSheetFlipped = flipHorizontally(mummyHurtSheet);
        mummyDyingSheet = loadSprite("assets/characters/5Mummy/mummy_death.png");
        mummyDyingSheetFlipped = flipHorizontally(mummyDyingSheet);
    }

    mummy->position = (SDL_Rect){x, y, SPRITE_WIDTH, SPRITE_HEIGHT};
    mummy->world_x = x;
    mummy->xVelocity = 0.0f;
    mummy->yVelocity = 0.0f;
    mummy->onGround = 0;
    mummy->state = ENEMY2_IDLE;
    mummy->frame = 0;
    mummy->totalFrames = MUMMY_IDLE_FRAMES;
    mummy->direction = -1; // Inverse facing: start facing left
    mummy->health = 50;
    mummy->maxHealth = 50;
    mummy->active = 1;
    mummy->lastFrameTime = SDL_GetTicks();
    mummy->attackCooldown = 0;

    placeEnemyLvl2OnGround(game, &mummy->position, mummy->world_x);
}

void updateMummy(Mummy *mummy, int *playerHealth, int playerMaxHealth, struct GAME *game, Uint32 currentTime) {
    if (!mummy || !mummy->active || !game || !playerHealth) return;

    // Check activation zone
    float dx = game->player2.world_x - mummy->world_x;
    float distance = fabs(dx);
    if (distance > ACTIVATION_ZONE) return;

    // Update gravity and ground status
    mummy->onGround = onGroundEnemyLvl2(game, &mummy->position, mummy->world_x);
    if (!mummy->onGround) {
        mummy->yVelocity += GRAVITY;
        if (mummy->yVelocity > MAX_FALL_SPEED) mummy->yVelocity = MAX_FALL_SPEED;
    } else {
        mummy->yVelocity = 0.0f;
    }
    mummy->position.y += (int)mummy->yVelocity;
    if (mummy->onGround) placeEnemyLvl2OnGround(game, &mummy->position, mummy->world_x);

    // Update facing direction
    mummy->direction = dx > 0 ? 1 : -1;

    // State transitions
    if (mummy->health <= 0 && mummy->state != ENEMY2_DYING && mummy->state != ENEMY2_DEAD) {
        mummy->state = ENEMY2_DYING;
        mummy->frame = 0;
        mummy->lastFrameTime = currentTime;
    } else if (mummy->state == ENEMY2_HURT || mummy->state == ENEMY2_ATTACKING || mummy->state == ENEMY2_DYING) {
        // Stay in these states
    } else if (distance < ATTACK_DISTANCE && mummy->attackCooldown == 0) {
        mummy->state = ENEMY2_ATTACKING;
        mummy->frame = 0;
        mummy->xVelocity = 0.0f;
    } else if (distance < CHASE_DISTANCE) {
        mummy->state = ENEMY2_WALKING;
        mummy->xVelocity = mummy->direction * MOVE_SPEED;
    } else {
        mummy->state = ENEMY2_IDLE;
        mummy->xVelocity = 0.0f;
    }

    // Update position
    mummy->world_x += (int)mummy->xVelocity;
    if (mummy->world_x < 0) mummy->world_x = 0;
    if (mummy->world_x > game->background.width - mummy->position.w) mummy->world_x = game->background.width - mummy->position.w;
    mummy->position.x = mummy->world_x - game->background.scroll_x;

    // Pixel-perfect collision for player2 attack
    if (game->player2.etat == P2_ATTACK_1 && mummy->state != ENEMY2_HURT) {
        SDL_Rect attackRect = {game->player2.world_x + (game->player2.lookingRight ? 50 : -50), game->player2.position.y, 100, 100};
        SDL_Rect mummyRect = {mummy->world_x, mummy->position.y, mummy->position.w, mummy->position.h};
        SDL_Surface *playerSheet = game->player2.lookingRight ? game->player2.spriteSheet : game->player2.spriteSheetFlipped;
        SDL_Rect srcRectPlayer = game->player2.frame;
        if (!game->player2.lookingRight) {
            srcRectPlayer.x = (playerSheet->w - srcRectPlayer.x) - srcRectPlayer.w;
        }
        SDL_Surface *mummySheet = mummy->state == ENEMY2_IDLE ? (mummy->direction > 0 ? mummyIdleSheet : mummyIdleSheetFlipped) :
                                  mummy->state == ENEMY2_WALKING ? (mummy->direction > 0 ? mummyWalkSheet : mummyWalkSheetFlipped) :
                                  mummy->state == ENEMY2_ATTACKING ? (mummy->direction > 0 ? mummyAttackSheet : mummyAttackSheetFlipped) :
                                  (mummy->direction > 0 ? mummyHurtSheet : mummyHurtSheetFlipped);
        SDL_Rect srcRectMummy = {mummy->frame * SPRITE_WIDTH, 0, SPRITE_WIDTH, SPRITE_HEIGHT};

        if (pixelPerfectCollision(playerSheet, &attackRect, &srcRectPlayer, mummySheet, &mummyRect, &srcRectMummy)) {
            mummy->health -= 10;
            mummy->state = ENEMY2_HURT;
            mummy->frame = 0;
            mummy->lastFrameTime = currentTime;
        }
    }

    // Pixel-perfect collision for mummy attack
    if (mummy->state == ENEMY2_ATTACKING && mummy->frame == 2 && mummy->attackCooldown == 0) {
        SDL_Rect mummyRect = {mummy->world_x, mummy->position.y, mummy->position.w, mummy->position.h};
        SDL_Rect playerRect = {game->player2.world_x, game->player2.position.y, game->player2.position.w, game->player2.position.h};
        SDL_Surface *mummySheet = mummy->direction > 0 ? mummyAttackSheet : mummyAttackSheetFlipped;
        SDL_Surface *playerSheet = game->player2.lookingRight ? game->player2.spriteSheet : game->player2.spriteSheetFlipped;
        SDL_Rect srcRectMummy = {mummy->frame * SPRITE_WIDTH, 0, SPRITE_WIDTH, SPRITE_HEIGHT};
        SDL_Rect srcRectPlayer = game->player2.frame;
        if (!game->player2.lookingRight) {
            srcRectPlayer.x = (playerSheet->w - srcRectPlayer.x) - srcRectPlayer.w;
        }

        if (pixelPerfectCollision(mummySheet, &mummyRect, &srcRectMummy, playerSheet, &playerRect, &srcRectPlayer)) {
            *playerHealth -= 5;
            mummy->attackCooldown = 1000;
            if (*playerHealth <= 0) game->player2.etat = P2_DEAD;
            else game->player2.etat = P2_HURT;
        }
    }

    // Update attack cooldown
    Uint32 deltaTime = currentTime - mummy->lastFrameTime;
    if (mummy->attackCooldown > 0) mummy->attackCooldown = mummy->attackCooldown > deltaTime ? mummy->attackCooldown - deltaTime : 0;

    // Animation timing
    int frameDuration;
    switch (mummy->state) {
        case ENEMY2_IDLE: frameDuration = MUMMY_IDLE_DURATION; mummy->totalFrames = MUMMY_IDLE_FRAMES; break;
        case ENEMY2_WALKING: frameDuration = MUMMY_WALK_DURATION; mummy->totalFrames = MUMMY_WALK_FRAMES; break;
        case ENEMY2_ATTACKING: frameDuration = MUMMY_ATTACK_DURATION; mummy->totalFrames = MUMMY_ATTACK_FRAMES; break;
        case ENEMY2_HURT: frameDuration = MUMMY_HURT_DURATION; mummy->totalFrames = MUMMY_HURT_FRAMES; break;
        case ENEMY2_DYING: frameDuration = MUMMY_DYING_DURATION; mummy->totalFrames = MUMMY_DYING_FRAMES; break;
        case ENEMY2_DEAD: frameDuration = MUMMY_DYING_DURATION; mummy->totalFrames = 1; break;
        default: frameDuration = MUMMY_IDLE_DURATION; mummy->totalFrames = MUMMY_IDLE_FRAMES; break;
    }

    if (currentTime - mummy->lastFrameTime >= frameDuration) {
        if (mummy->state == ENEMY2_HURT || mummy->state == ENEMY2_ATTACKING || mummy->state == ENEMY2_DYING) {
            if (mummy->frame < mummy->totalFrames - 1) {
                mummy->frame++;
            } else {
                if (mummy->state == ENEMY2_DYING) {
                    mummy->state = ENEMY2_DEAD;
                    mummy->frame = 0;
                    mummy->active = 0;
                    game->player2.score += 50;
                } else {
                    mummy->state = ENEMY2_IDLE;
                    mummy->frame = 0;
                }
            }
        } else {
            mummy->frame = (mummy->frame + 1) % mummy->totalFrames;
        }
        mummy->lastFrameTime = currentTime;
    }
}

void renderMummy(SDL_Surface *screen, Mummy *mummy, int scroll_x) {
    if (!screen || !mummy || !mummy->active) return;

    SDL_Surface *sheet = mummy->direction > 0 ?
        (mummy->state == ENEMY2_IDLE ? mummyIdleSheet :
         mummy->state == ENEMY2_WALKING ? mummyWalkSheet :
         mummy->state == ENEMY2_ATTACKING ? mummyAttackSheet :
         mummy->state == ENEMY2_HURT ? mummyHurtSheet :
         mummyDyingSheet) :
        (mummy->state == ENEMY2_IDLE ? mummyIdleSheetFlipped :
         mummy->state == ENEMY2_WALKING ? mummyWalkSheetFlipped :
         mummy->state == ENEMY2_ATTACKING ? mummyAttackSheetFlipped :
         mummy->state == ENEMY2_HURT ? mummyHurtSheetFlipped :
         mummyDyingSheetFlipped);

    SDL_Rect srcRect = {mummy->frame * SPRITE_WIDTH, 0, SPRITE_WIDTH, SPRITE_HEIGHT};
    SDL_Rect destRect = {mummy->world_x - scroll_x, mummy->position.y, SPRITE_WIDTH, SPRITE_HEIGHT};
    SDL_BlitSurface(sheet, &srcRect, screen, &destRect);

    if (mummy->health > 0) {
        SDL_Rect healthBarBg = {mummy->world_x - scroll_x, mummy->position.y - 20, 50, 5};
        SDL_FillRect(screen, &healthBarBg, SDL_MapRGB(screen->format, 255, 0, 0));
        SDL_Rect healthBar = {mummy->world_x - scroll_x, mummy->position.y - 20, (50 * mummy->health) / mummy->maxHealth, 5};
        SDL_FillRect(screen, &healthBar, SDL_MapRGB(screen->format, 0, 255, 0));
    }
}

void freeMummy(Mummy *mummy) {
    if (mummy) mummy->active = 0;
}

// --- Deceased Functions ---
void initDeceased(Deceased *deceased, int x, int y, struct GAME *game) {
    if (!deceased || !game) return;

    if (!deceasedIdleSheet) {
        deceasedIdleSheet = loadSprite("assets/characters/6Deceased/deceased_idle.png");
        deceasedIdleSheetFlipped = flipHorizontally(deceasedIdleSheet);
        deceasedWalkSheet = loadSprite("assets/characters/6Deceased/deceased_walk.png");
        deceasedWalkSheetFlipped = flipHorizontally(deceasedWalkSheet);
        deceasedAttackSheet = loadSprite("assets/characters/6Deceased/deceased_attack.png");
        deceasedAttackSheetFlipped = flipHorizontally(deceasedAttackSheet);
        deceasedHurtSheet = loadSprite("assets/characters/6Deceased/deceased_hurt.png");
        deceasedHurtSheetFlipped = flipHorizontally(deceasedHurtSheet);
        deceasedDyingSheet = loadSprite("assets/characters/6Deceased/deceased_death.png");
        deceasedDyingSheetFlipped = flipHorizontally(deceasedDyingSheet);
        deceasedProjectileSheet = loadSprite("assets/characters/6Deceased/fireball.png");
    }

    deceased->position = (SDL_Rect){x, y, SPRITE_WIDTH, SPRITE_HEIGHT};
    deceased->world_x = x;
    deceased->xVelocity = 0.0f;
    deceased->yVelocity = 0.0f;
    deceased->onGround = 0;
    deceased->state = ENEMY2_IDLE;
    deceased->frame = 0;
    deceased->totalFrames = DECEASED_IDLE_FRAMES;
    deceased->direction = -1; // Inverse facing: start facing left
    deceased->health = 40;
    deceased->maxHealth = 40;
    deceased->active = 1;
    deceased->lastFrameTime = SDL_GetTicks();
    deceased->attackCooldown = 0;
    deceased->projectilePos = (SDL_Rect){0, 0, 32, 32};
    deceased->projectileActive = 0;
    deceased->projectileDx = 0.0f;
    deceased->projectileDy = 0.0f;
    deceased->projectileDistance = 0.0f;

    placeEnemyLvl2OnGround(game, &deceased->position, deceased->world_x);
}

void updateDeceased(Deceased *deceased, int *playerHealth, int playerMaxHealth, struct GAME *game, Uint32 currentTime) {
    if (!deceased || !deceased->active || !game || !playerHealth) return;

    // Check activation zone
    float dx = game->player2.world_x - deceased->world_x;
    float dy = game->player2.position.y - deceased->position.y;
    float distance = sqrt(dx * dx + dy * dy);
    if (distance > ACTIVATION_ZONE) return;

    // Update gravity and ground status
    deceased->onGround = onGroundEnemyLvl2(game, &deceased->position, deceased->world_x);
    if (!deceased->onGround) {
        deceased->yVelocity += GRAVITY;
        if (deceased->yVelocity > MAX_FALL_SPEED) deceased->yVelocity = MAX_FALL_SPEED;
    } else {
        deceased->yVelocity = 0.0f;
    }
    deceased->position.y += (int)deceased->yVelocity;
    if (deceased->onGround) placeEnemyLvl2OnGround(game, &deceased->position, deceased->world_x);

    // Update facing direction
    deceased->direction = dx > 0 ? 1 : -1;

    // State transitions
    if (deceased->health <= 0 && deceased->state != ENEMY2_DYING && deceased->state != ENEMY2_DEAD) {
        deceased->state = ENEMY2_DYING;
        deceased->frame = 0;
        deceased->lastFrameTime = currentTime;
    } else if (deceased->state == ENEMY2_HURT || deceased->state == ENEMY2_ATTACKING || deceased->state == ENEMY2_DYING) {
        // Stay in these states
    } else if (distance < 350 && deceased->attackCooldown == 0) {
        deceased->state = ENEMY2_ATTACKING;
        deceased->frame = 0;
        deceased->xVelocity = 0.0f;
        deceased->projectilePos.x = deceased->world_x + SPRITE_WIDTH / 2;
        deceased->projectilePos.y = deceased->position.y + SPRITE_HEIGHT / 2;
        deceased->projectileDx = (dx / distance) * 5.0f;
        deceased->projectileDy = (dy / distance) * 5.0f;
        deceased->projectileActive = 1;
        deceased->projectileDistance = 0.0f;
        deceased->attackCooldown = 1000;
    } else if (distance < CHASE_DISTANCE) {
        deceased->state = ENEMY2_WALKING;
        deceased->xVelocity = (dx / distance) * MOVE_SPEED;
        if (deceased->attackCooldown > 0) deceased->xVelocity = -deceased->xVelocity; // Retreat after attack
    } else {
        deceased->state = ENEMY2_IDLE;
        deceased->xVelocity = 0.0f;
    }

    // Update position
    deceased->world_x += (int)deceased->xVelocity;
    if (deceased->world_x < 0) deceased->world_x = 0;
    if (deceased->world_x > game->background.width - deceased->position.w) deceased->world_x = game->background.width - deceased->position.w;
    deceased->position.x = deceased->world_x - game->background.scroll_x;

    // Update projectile
    if (deceased->projectileActive) {
        deceased->projectilePos.x += (int)deceased->projectileDx;
        deceased->projectilePos.y += (int)deceased->projectileDy;
        deceased->projectileDistance += 5.0f;
        SDL_Rect playerRect = {game->player2.world_x, game->player2.position.y, game->player2.position.w, game->player2.position.h};
        SDL_Surface *playerSheet = game->player2.lookingRight ? game->player2.spriteSheet : game->player2.spriteSheetFlipped;
        SDL_Rect srcRectPlayer = game->player2.frame;
        if (!game->player2.lookingRight) {
            srcRectPlayer.x = (playerSheet->w - srcRectPlayer.x) - srcRectPlayer.w;
        }
        SDL_Rect projSrc = {0, 0, deceasedProjectileSheet->w, deceasedProjectileSheet->h};
        if (pixelPerfectCollision(deceasedProjectileSheet, &deceased->projectilePos, &projSrc, playerSheet, &playerRect, &srcRectPlayer)) {
            *playerHealth -= 8;
            deceased->projectileActive = 0;
            if (*playerHealth <= 0) game->player2.etat = P2_DEAD;
            else game->player2.etat = P2_HURT;
        } else if (deceased->projectileDistance > 500) {
            deceased->projectileActive = 0;
        }
    }

    // Pixel-perfect collision for player2 attack
    if (game->player2.etat == P2_ATTACK_1 && deceased->state != ENEMY2_HURT) {
        SDL_Rect attackRect = {game->player2.world_x + (game->player2.lookingRight ? 50 : -50), game->player2.position.y, 100, 100};
        SDL_Rect deceasedRect = {deceased->world_x, deceased->position.y, deceased->position.w, deceased->position.h};
        SDL_Surface *playerSheet = game->player2.lookingRight ? game->player2.spriteSheet : game->player2.spriteSheetFlipped;
        SDL_Rect srcRectPlayer = game->player2.frame;
        if (!game->player2.lookingRight) {
            srcRectPlayer.x = (playerSheet->w - srcRectPlayer.x) - srcRectPlayer.w;
        }
        SDL_Surface *deceasedSheet = deceased->state == ENEMY2_IDLE ? (deceased->direction > 0 ? deceasedIdleSheet : deceasedIdleSheetFlipped) :
                                    deceased->state == ENEMY2_WALKING ? (deceased->direction > 0 ? deceasedWalkSheet : deceasedWalkSheetFlipped) :
                                    deceased->state == ENEMY2_ATTACKING ? (deceased->direction > 0 ? deceasedAttackSheet : deceasedAttackSheetFlipped) :
                                    (deceased->direction > 0 ? deceasedHurtSheet : deceasedHurtSheetFlipped);
        SDL_Rect srcRectDeceased = {deceased->frame * SPRITE_WIDTH, 0, SPRITE_WIDTH, SPRITE_HEIGHT};

        if (pixelPerfectCollision(playerSheet, &attackRect, &srcRectPlayer, deceasedSheet, &deceasedRect, &srcRectDeceased)) {
            deceased->health -= 10;
            deceased->state = ENEMY2_HURT;
            deceased->frame = 0;
            deceased->lastFrameTime = currentTime;
        }
    }

    // Update attack cooldown
    Uint32 deltaTime = currentTime - deceased->lastFrameTime;
    if (deceased->attackCooldown > 0) deceased->attackCooldown = deceased->attackCooldown > deltaTime ? deceased->attackCooldown - deltaTime : 0;

    // Animation timing
    int frameDuration;
    switch (deceased->state) {
        case ENEMY2_IDLE: frameDuration = DECEASED_IDLE_DURATION; deceased->totalFrames = DECEASED_IDLE_FRAMES; break;
        case ENEMY2_WALKING: frameDuration = DECEASED_WALK_DURATION; deceased->totalFrames = DECEASED_WALK_FRAMES; break;
        case ENEMY2_ATTACKING: frameDuration = DECEASED_ATTACK_DURATION; deceased->totalFrames = DECEASED_ATTACK_FRAMES; break;
        case ENEMY2_HURT: frameDuration = DECEASED_HURT_DURATION; deceased->totalFrames = DECEASED_HURT_FRAMES; break;
        case ENEMY2_DYING: frameDuration = DECEASED_DYING_DURATION; deceased->totalFrames = DECEASED_DYING_FRAMES; break;
        case ENEMY2_DEAD: frameDuration = DECEASED_DYING_DURATION; deceased->totalFrames = 1; break;
        default: frameDuration = DECEASED_IDLE_DURATION; deceased->totalFrames = DECEASED_IDLE_FRAMES; break;
    }

    if (currentTime - deceased->lastFrameTime >= frameDuration) {
        if (deceased->state == ENEMY2_HURT || deceased->state == ENEMY2_ATTACKING || deceased->state == ENEMY2_DYING) {
            if (deceased->frame < deceased->totalFrames - 1) {
                deceased->frame++;
            } else {
                if (deceased->state == ENEMY2_DYING) {
                    deceased->state = ENEMY2_DEAD;
                    deceased->frame = 0;
                    deceased->active = 0;
                    game->player2.score += 50;
                } else {
                    deceased->state = ENEMY2_IDLE;
                    deceased->frame = 0;
                }
            }
        } else {
            deceased->frame = (deceased->frame + 1) % deceased->totalFrames;
        }
        deceased->lastFrameTime = currentTime;
    }
}

void renderDeceased(SDL_Surface *screen, Deceased *deceased, int scroll_x) {
    if (!screen || !deceased || !deceased->active) return;

    SDL_Surface *sheet = deceased->direction > 0 ?
        (deceased->state == ENEMY2_IDLE ? deceasedIdleSheet :
         deceased->state == ENEMY2_WALKING ? deceasedWalkSheet :
         deceased->state == ENEMY2_ATTACKING ? deceasedAttackSheet :
         deceased->state == ENEMY2_HURT ? deceasedHurtSheet :
         deceasedDyingSheet) :
        (deceased->state == ENEMY2_IDLE ? deceasedIdleSheetFlipped :
         deceased->state == ENEMY2_WALKING ? deceasedWalkSheetFlipped :
         deceased->state == ENEMY2_ATTACKING ? deceasedAttackSheetFlipped :
         deceased->state == ENEMY2_HURT ? deceasedHurtSheetFlipped :
         deceasedDyingSheetFlipped);

    SDL_Rect srcRect = {deceased->frame * SPRITE_WIDTH, 0, SPRITE_WIDTH, SPRITE_HEIGHT};
    SDL_Rect destRect = {deceased->world_x - scroll_x, deceased->position.y, SPRITE_WIDTH, SPRITE_HEIGHT};
    SDL_BlitSurface(sheet, &srcRect, screen, &destRect);

    if (deceased->projectileActive) {
        SDL_Rect projDest = {deceased->projectilePos.x - scroll_x, deceased->projectilePos.y, 32, 32};
        SDL_BlitSurface(deceasedProjectileSheet, NULL, screen, &projDest);
    }

    if (deceased->health > 0) {
        SDL_Rect healthBarBg = {deceased->world_x - scroll_x, deceased->position.y - 20, 50, 5};
        SDL_FillRect(screen, &healthBarBg, SDL_MapRGB(screen->format, 255, 0, 0));
        SDL_Rect healthBar = {deceased->world_x - scroll_x, deceased->position.y - 20, (50 * deceased->health) / deceased->maxHealth, 5};
        SDL_FillRect(screen, &healthBar, SDL_MapRGB(screen->format, 0, 255, 0));
    }
}

void freeDeceased(Deceased *deceased) {
    if (deceased) deceased->active = 0;
}

// --- Gorgon Functions ---
void initGorgon(Gorgon *gorgon, int x, int y, struct GAME *game) {
    if (!gorgon || !game) return;

    if (!gorgonIdleSheet) {
        gorgonIdleSheet = loadSprite("assets/characters/Gorgon_3/idle.png");
        gorgonIdleSheetFlipped = flipHorizontally(gorgonIdleSheet);
        gorgonWalkSheet = loadSprite("assets/characters/Gorgon_3/walk.png");
        gorgonWalkSheetFlipped = flipHorizontally(gorgonWalkSheet);
        gorgonRunSheet = loadSprite("assets/characters/Gorgon_3/run.png");
        gorgonRunSheetFlipped = flipHorizontally(gorgonRunSheet);
        gorgonAttackSheet = loadSprite("assets/characters/Gorgon_3/attack_1.png");
        gorgonAttackSheetFlipped = flipHorizontally(gorgonAttackSheet);
        gorgonHurtSheet = loadSprite("assets/characters/Gorgon_3/hurt.png");
        gorgonHurtSheetFlipped = flipHorizontally(gorgonHurtSheet);
        gorgonDyingSheet = loadSprite("assets/characters/Gorgon_3/dead.png");
        gorgonDyingSheetFlipped = flipHorizontally(gorgonDyingSheet);
        gorgonSpecialSheet = loadSprite("assets/characters/Gorgon_3/special.png");
        gorgonSpecialSheetFlipped = flipHorizontally(gorgonSpecialSheet);
    }

    gorgon->position = (SDL_Rect){x, y, SPRITE_WIDTH, SPRITE_HEIGHT};
    gorgon->world_x = x;
    gorgon->xVelocity = 0.0f;
    gorgon->yVelocity = 0.0f;
    gorgon->onGround = 0;
    gorgon->state = ENEMY2_IDLE;
    gorgon->frame = 0;
    gorgon->totalFrames = GORGON_IDLE_FRAMES;
    gorgon->direction = 1;
    gorgon->health = 60;
    gorgon->maxHealth = 60;
    gorgon->active = 1;
    gorgon->lastFrameTime = SDL_GetTicks();
    gorgon->attackCooldown = 0;
    gorgon->hasPetrified = 0;
    for (int i = 0; i < MAX_PARTICLES; i++) {
        gorgon->particles[i].active = 0;
    }

    placeEnemyLvl2OnGround(game, &gorgon->position, gorgon->world_x);
}

void updateGorgon(Gorgon *gorgon, int *playerHealth, int playerMaxHealth, struct GAME *game, Uint32 currentTime) {
    if (!gorgon || !gorgon->active || !game || !playerHealth) return;

    // Check activation zone
    float dx = game->player2.world_x - gorgon->world_x;
    float dy = game->player2.position.y - gorgon->position.y;
    float distance = sqrt(dx * dx + dy * dy);
    if (distance > ACTIVATION_ZONE) return;

    // Update gravity and ground status
    gorgon->onGround = onGroundEnemyLvl2(game, &gorgon->position, gorgon->world_x);
    if (!gorgon->onGround) {
        gorgon->yVelocity += GRAVITY;
        if (gorgon->yVelocity > MAX_FALL_SPEED) gorgon->yVelocity = MAX_FALL_SPEED;
    } else {
        gorgon->yVelocity = 0.0f;
    }
    gorgon->position.y += (int)gorgon->yVelocity;
    if (gorgon->onGround) placeEnemyLvl2OnGround(game, &gorgon->position, gorgon->world_x);

    // Update facing direction
    gorgon->direction = dx > 0 ? 1 : -1;

    // State transitions
    if (gorgon->health <= 0 && gorgon->state != ENEMY2_DYING && gorgon->state != ENEMY2_DEAD) {
        gorgon->state = ENEMY2_DYING;
        gorgon->frame = 0;
        gorgon->lastFrameTime = currentTime;
    } else if (gorgon->state == ENEMY2_HURT || gorgon->state == ENEMY2_ATTACKING || gorgon->state == ENEMY2_SPECIAL || gorgon->state == ENEMY2_DYING) {
        // Stay in these states
    } else if (distance < 250 && !gorgon->hasPetrified && gorgon->attackCooldown == 0) {
        gorgon->state = ENEMY2_SPECIAL;
        gorgon->frame = 0;
        gorgon->xVelocity = 0.0f;
        gorgon->hasPetrified = 1;
        game->player2.freezeYMovement = 1;
        SDL_Color petrifyColor = {255, 215, 0, 255};
        initParticles(gorgon->particles, MAX_PARTICLES, game->player2.world_x + game->player2.position.w / 2, game->player2.position.y + game->player2.position.h / 2, petrifyColor);
        gorgon->attackCooldown = 3000;
    } else if (distance < ATTACK_DISTANCE && gorgon->attackCooldown == 0) {
        gorgon->state = ENEMY2_ATTACKING;
        gorgon->frame = 0;
        gorgon->xVelocity = 0.0f;
    } else if (distance < CHASE_DISTANCE) {
        float speed = MOVE_SPEED;
        if (distance < 300) {
            gorgon->state = ENEMY2_RUNNING;
            speed *= 1.5f;
        } else {
            gorgon->state = ENEMY2_WALKING;
        }
        gorgon->xVelocity = (dx / distance) * speed;
    } else {
        gorgon->state = ENEMY2_IDLE;
        Uint32 patrolTime = currentTime / 2000;
        gorgon->direction = (patrolTime % 2 == 0) ? 1 : -1;
        gorgon->xVelocity = gorgon->direction * MOVE_SPEED * 0.5f;
    }

    // Update position
    gorgon->world_x += (int)gorgon->xVelocity;
    if (gorgon->world_x < 0) gorgon->world_x = 0;
    if (gorgon->world_x > game->background.width - gorgon->position.w) gorgon->world_x = game->background.width - gorgon->position.w;
    gorgon->position.x = gorgon->world_x - game->background.scroll_x;

    // Pixel-perfect collision for player2 attack
    if (game->player2.etat == P2_ATTACK_1 && gorgon->state != ENEMY2_HURT) {
        SDL_Rect attackRect = {game->player2.world_x + (game->player2.lookingRight ? 50 : -50), game->player2.position.y, 100, 100};
        SDL_Rect gorgonRect = {gorgon->world_x, gorgon->position.y, gorgon->position.w, gorgon->position.h};
        SDL_Surface *playerSheet = game->player2.lookingRight ? game->player2.spriteSheet : game->player2.spriteSheetFlipped;
        SDL_Rect srcRectPlayer = game->player2.frame;
        if (!game->player2.lookingRight) {
            srcRectPlayer.x = (playerSheet->w - srcRectPlayer.x) - srcRectPlayer.w;
        }
        SDL_Surface *gorgonSheet = gorgon->state == ENEMY2_IDLE ? (gorgon->direction > 0 ? gorgonIdleSheet : gorgonIdleSheetFlipped) :
                                   gorgon->state == ENEMY2_WALKING ? (gorgon->direction > 0 ? gorgonWalkSheet : gorgonWalkSheetFlipped) :
                                   gorgon->state == ENEMY2_RUNNING ? (gorgon->direction > 0 ? gorgonRunSheet : gorgonRunSheetFlipped) :
                                   gorgon->state == ENEMY2_ATTACKING ? (gorgon->direction > 0 ? gorgonAttackSheet : gorgonAttackSheetFlipped) :
                                   gorgon->state == ENEMY2_SPECIAL ? (gorgon->direction > 0 ? gorgonSpecialSheet : gorgonSpecialSheetFlipped) :
                                   (gorgon->direction > 0 ? gorgonHurtSheet : gorgonHurtSheetFlipped);
        SDL_Rect srcRectGorgon = {gorgon->frame * SPRITE_WIDTH, 0, SPRITE_WIDTH, SPRITE_HEIGHT};

        if (pixelPerfectCollision(playerSheet, &attackRect, &srcRectPlayer, gorgonSheet, &gorgonRect, &srcRectGorgon)) {
            gorgon->health -= 10;
            gorgon->state = ENEMY2_HURT;
            gorgon->frame = 0;
            gorgon->lastFrameTime = currentTime;
        }
    }

    // Pixel-perfect collision for gorgon attack
    if (gorgon->state == ENEMY2_ATTACKING && gorgon->frame == 2 && gorgon->attackCooldown == 0) {
        SDL_Rect gorgonRect = {gorgon->world_x, gorgon->position.y, gorgon->position.w, gorgon->position.h};
        SDL_Rect playerRect = {game->player2.world_x, game->player2.position.y, game->player2.position.w, game->player2.position.h};
        SDL_Surface *gorgonSheet = gorgon->direction > 0 ? gorgonAttackSheet : gorgonAttackSheetFlipped;
        SDL_Surface *playerSheet = game->player2.lookingRight ? game->player2.spriteSheet : game->player2.spriteSheetFlipped;
        SDL_Rect srcRectGorgon = {gorgon->frame * SPRITE_WIDTH, 0, SPRITE_WIDTH, SPRITE_HEIGHT};
        SDL_Rect srcRectPlayer = game->player2.frame;
        if (!game->player2.lookingRight) {
            srcRectPlayer.x = (playerSheet->w - srcRectPlayer.x) - srcRectPlayer.w;
        }

        if (pixelPerfectCollision(gorgonSheet, &gorgonRect, &srcRectGorgon, playerSheet, &playerRect, &srcRectPlayer)) {
            *playerHealth -= 10;
            gorgon->attackCooldown = 1000;
            if (*playerHealth <= 0) game->player2.etat = P2_DEAD;
            else game->player2.etat = P2_HURT;
        }
    }

    // Update attack cooldown and particles
    Uint32 deltaTime = currentTime - gorgon->lastFrameTime;
    if (gorgon->attackCooldown > 0) gorgon->attackCooldown = gorgon->attackCooldown > deltaTime ? gorgon->attackCooldown - deltaTime : 0;
    updateParticles(gorgon->particles, MAX_PARTICLES, deltaTime);

    // Animation timing
    int frameDuration;
    switch (gorgon->state) {
        case ENEMY2_IDLE: frameDuration = GORGON_IDLE_DURATION; gorgon->totalFrames = GORGON_IDLE_FRAMES; break;
        case ENEMY2_WALKING: frameDuration = GORGON_WALK_DURATION; gorgon->totalFrames = GORGON_WALK_FRAMES; break;
        case ENEMY2_RUNNING: frameDuration = GORGON_RUN_DURATION; gorgon->totalFrames = GORGON_RUN_FRAMES; break;
        case ENEMY2_ATTACKING: frameDuration = GORGON_ATTACK_DURATION; gorgon->totalFrames = GORGON_ATTACK_FRAMES; break;
        case ENEMY2_HURT: frameDuration = GORGON_HURT_DURATION; gorgon->totalFrames = GORGON_HURT_FRAMES; break;
        case ENEMY2_DYING: frameDuration = GORGON_DYING_DURATION; gorgon->totalFrames = GORGON_DYING_FRAMES; break;
        case ENEMY2_SPECIAL: frameDuration = GORGON_SPECIAL_DURATION; gorgon->totalFrames = GORGON_SPECIAL_FRAMES; break;
        case ENEMY2_DEAD: frameDuration = GORGON_DYING_DURATION; gorgon->totalFrames = 1; break;
        default: frameDuration = GORGON_IDLE_DURATION; gorgon->totalFrames = GORGON_IDLE_FRAMES; break;
    }

    if (currentTime - gorgon->lastFrameTime >= frameDuration) {
        if (gorgon->state == ENEMY2_HURT || gorgon->state == ENEMY2_ATTACKING || gorgon->state == ENEMY2_DYING || gorgon->state == ENEMY2_SPECIAL) {
            if (gorgon->frame < gorgon->totalFrames - 1) {
                gorgon->frame++;
            } else {
                if (gorgon->state == ENEMY2_DYING) {
                    gorgon->state = ENEMY2_DEAD;
                    gorgon->frame = 0;
                    gorgon->active = 0;
                    game->player2.score += 50;
                } else {
                    gorgon->state = ENEMY2_IDLE;
                    gorgon->frame = 0;
                    if (gorgon->state == ENEMY2_SPECIAL) game->player2.freezeYMovement = 0;
                }
            }
        } else {
            gorgon->frame = (gorgon->frame + 1) % gorgon->totalFrames;
        }
        gorgon->lastFrameTime = currentTime;
    }
}

void renderGorgon(SDL_Surface *screen, Gorgon *gorgon, int scroll_x) {
    if (!screen || !gorgon || !gorgon->active) return;

    SDL_Surface *sheet = gorgon->direction > 0 ?
        (gorgon->state == ENEMY2_IDLE ? gorgonIdleSheet :
         gorgon->state == ENEMY2_WALKING ? gorgonWalkSheet :
         gorgon->state == ENEMY2_RUNNING ? gorgonRunSheet :
         gorgon->state == ENEMY2_ATTACKING ? gorgonAttackSheet :
         gorgon->state == ENEMY2_HURT ? gorgonHurtSheet :
         gorgon->state == ENEMY2_SPECIAL ? gorgonSpecialSheet :
         gorgonDyingSheet) :
        (gorgon->state == ENEMY2_IDLE ? gorgonIdleSheetFlipped :
         gorgon->state == ENEMY2_WALKING ? gorgonWalkSheetFlipped :
         gorgon->state == ENEMY2_RUNNING ? gorgonRunSheetFlipped :
         gorgon->state == ENEMY2_ATTACKING ? gorgonAttackSheetFlipped :
         gorgon->state == ENEMY2_HURT ? gorgonHurtSheetFlipped :
         gorgon->state == ENEMY2_SPECIAL ? gorgonSpecialSheetFlipped :
         gorgonDyingSheetFlipped);

    SDL_Rect srcRect = {gorgon->frame * SPRITE_WIDTH, 0, SPRITE_WIDTH, SPRITE_HEIGHT};
    SDL_Rect destRect = {gorgon->world_x - scroll_x, gorgon->position.y, SPRITE_WIDTH, SPRITE_HEIGHT};
    SDL_BlitSurface(sheet, &srcRect, screen, &destRect);

    renderParticles(screen, gorgon->particles, MAX_PARTICLES, scroll_x);

    if (gorgon->health > 0) {
        SDL_Rect healthBarBg = {gorgon->world_x - scroll_x, gorgon->position.y - 20, 50, 5};
        SDL_FillRect(screen, &healthBarBg, SDL_MapRGB(screen->format, 255, 0, 0));
        SDL_Rect healthBar = {gorgon->world_x - scroll_x, gorgon->position.y - 20, (50 * gorgon->health) / gorgon->maxHealth, 5};
        SDL_FillRect(screen, &healthBar, SDL_MapRGB(screen->format, 0, 255, 0));
    }
}

void freeGorgon(Gorgon *gorgon) {
    if (gorgon) gorgon->active = 0;
}

// --- SkeletonSpearman Functions ---
void initSkeletonSpearman(SkeletonSpearman *spearman, int x, int y, struct GAME *game) {
    if (!spearman || !game) return;

    if (!spearmanIdleSheet) {
        spearmanIdleSheet = loadSprite("assets/characters/Skeleton_Spearman/idle.png");
        spearmanIdleSheetFlipped = flipHorizontally(spearmanIdleSheet);
        spearmanWalkSheet = loadSprite("assets/characters/Skeleton_Spearman/walk.png");
        spearmanWalkSheetFlipped = flipHorizontally(spearmanWalkSheet);
        spearmanRunSheet = loadSprite("assets/characters/Skeleton_Spearman/run.png");
        spearmanRunSheetFlipped = flipHorizontally(spearmanRunSheet);
        spearmanAttackSheet = loadSprite("assets/characters/Skeleton_Spearman/attack_1.png");
        spearmanAttackSheetFlipped = flipHorizontally(spearmanAttackSheet);
        spearmanHurtSheet = loadSprite("assets/characters/Skeleton_Spearman/hurt.png");
        spearmanHurtSheetFlipped = flipHorizontally(spearmanHurtSheet);
        spearmanDyingSheet = loadSprite("assets/characters/Skeleton_Spearman/dead.png");
        spearmanDyingSheetFlipped = flipHorizontally(spearmanDyingSheet);
        spearmanFallSheet = loadSprite("assets/characters/Skeleton_Spearman/fall.png");
        spearmanFallSheetFlipped = flipHorizontally(spearmanFallSheet);
        spearmanRunAttackSheet = loadSprite("assets/characters/Skeleton_Spearman/run_attack.png");
        spearmanRunAttackSheetFlipped = flipHorizontally(spearmanRunAttackSheet);
    }

    spearman->position = (SDL_Rect){x, y, SPRITE_WIDTH, SPRITE_HEIGHT};
    spearman->world_x = x;
    spearman->xVelocity = 0.0f;
    spearman->yVelocity = 0.0f;
    spearman->onGround = 0;
    spearman->state = ENEMY2_IDLE;
    spearman->frame = 0;
    spearman->totalFrames = SPEARMAN_IDLE_FRAMES;
    spearman->direction = 1;
    spearman->health = 45;
    spearman->maxHealth = 45;
    spearman->active = 1;
    spearman->lastFrameTime = SDL_GetTicks();
    spearman->attackCooldown = 0;
    spearman->hasFallen = 0;

    placeEnemyLvl2OnGround(game, &spearman->position, spearman->world_x);
}

void updateSkeletonSpearman(SkeletonSpearman *spearman, int *playerHealth, int playerMaxHealth, struct GAME *game, Uint32 currentTime) {
    if (!spearman || !spearman->active || !game || !playerHealth) return;

    // Check activation zone
    float dx = game->player2.world_x - spearman->world_x;
    float dy = game->player2.position.y - spearman->position.y;
    float distance = sqrt(dx * dx + dy * dy);
    if (distance > ACTIVATION_ZONE) return;

    // Update gravity and ground status
    spearman->onGround = onGroundEnemyLvl2(game, &spearman->position, spearman->world_x);
    if (!spearman->onGround) {
        spearman->yVelocity += GRAVITY;
        if (spearman->yVelocity > MAX_FALL_SPEED) spearman->yVelocity = MAX_FALL_SPEED;
    } else {
        spearman->yVelocity = 0.0f;
    }
    spearman->position.y += (int)spearman->yVelocity;
    if (spearman->onGround) placeEnemyLvl2OnGround(game, &spearman->position, spearman->world_x);

    // Update facing direction
    spearman->direction = dx > 0 ? 1 : -1;

    // State transitions
    if (spearman->health <= 0 && spearman->state != ENEMY2_DYING && spearman->state != ENEMY2_DEAD) {
        spearman->state = ENEMY2_DYING;
        spearman->frame = 0;
        spearman->lastFrameTime = currentTime;
    } else if (spearman->health <= 22 && !spearman->hasFallen && spearman->state != ENEMY2_DYING) {
        spearman->state = ENEMY2_FALL;
        spearman->frame = 0;
        spearman->hasFallen = 1;
    } else if (spearman->state == ENEMY2_HURT || spearman->state == ENEMY2_ATTACKING || spearman->state == ENEMY2_RUN_ATTACK || spearman->state == ENEMY2_FALL || spearman->state == ENEMY2_DYING) {
        // Stay in these states
    } else if (distance < ATTACK_DISTANCE && spearman->state == ENEMY2_RUNNING && spearman->attackCooldown == 0) {
        spearman->state = ENEMY2_RUN_ATTACK;
        spearman->frame = 0;
        spearman->xVelocity = 0.0f;
    } else if (distance < ATTACK_DISTANCE && spearman->attackCooldown == 0) {
        spearman->state = ENEMY2_ATTACKING;
        spearman->frame = 0;
        spearman->xVelocity = 0.0f;
    } else if (distance < CHASE_DISTANCE) {
        float speed = MOVE_SPEED;
        if (distance < 300) {
            spearman->state = ENEMY2_RUNNING;
            speed *= 1.5f;
        } else {
            spearman->state = ENEMY2_WALKING;
        }
        spearman->xVelocity = (dx / distance) * speed;
    } else {
        spearman->state = ENEMY2_IDLE;
        Uint32 patrolTime = currentTime / 2000;
        spearman->direction = (patrolTime % 2 == 0) ? 1 : -1;
        spearman->xVelocity = spearman->direction * MOVE_SPEED * 0.5f;
    }

    // Update position
    spearman->world_x += (int)spearman->xVelocity;
    if (spearman->world_x < 0) spearman->world_x = 0;
    if (spearman->world_x > game->background.width - spearman->position.w) spearman->world_x = game->background.width - spearman->position.w;
    spearman->position.x = spearman->world_x - game->background.scroll_x;

    // Pixel-perfect collision for player2 attack
    if (game->player2.etat == P2_ATTACK_1 && spearman->state != ENEMY2_HURT) {
        SDL_Rect attackRect = {game->player2.world_x + (game->player2.lookingRight ? 50 : -50), game->player2.position.y, 100, 100};
        SDL_Rect spearmanRect = {spearman->world_x, spearman->position.y, spearman->position.w, spearman->position.h};
        SDL_Surface *playerSheet = game->player2.lookingRight ? game->player2.spriteSheet : game->player2.spriteSheetFlipped;
        SDL_Rect srcRectPlayer = game->player2.frame;
        if (!game->player2.lookingRight) {
            srcRectPlayer.x = (playerSheet->w - srcRectPlayer.x) - srcRectPlayer.w;
        }
        SDL_Surface *spearmanSheet = spearman->state == ENEMY2_IDLE ? (spearman->direction > 0 ? spearmanIdleSheet : spearmanIdleSheetFlipped) :
                                     spearman->state == ENEMY2_WALKING ? (spearman->direction > 0 ? spearmanWalkSheet : spearmanWalkSheetFlipped) :
                                     spearman->state == ENEMY2_RUNNING ? (spearman->direction > 0 ? spearmanRunSheet : spearmanRunSheetFlipped) :
                                     spearman->state == ENEMY2_ATTACKING ? (spearman->direction > 0 ? spearmanAttackSheet : spearmanAttackSheetFlipped) :
                                     spearman->state == ENEMY2_RUN_ATTACK ? (spearman->direction > 0 ? spearmanRunAttackSheet : spearmanRunAttackSheetFlipped) :
                                     spearman->state == ENEMY2_FALL ? (spearman->direction > 0 ? spearmanFallSheet : spearmanFallSheetFlipped) :
                                     (spearman->direction > 0 ? spearmanHurtSheet : spearmanHurtSheetFlipped);
        SDL_Rect srcRectSpearman = {spearman->frame * SPRITE_WIDTH, 0, SPRITE_WIDTH, SPRITE_HEIGHT};

        if (pixelPerfectCollision(playerSheet, &attackRect, &srcRectPlayer, spearmanSheet, &spearmanRect, &srcRectSpearman)) {
            spearman->health -= 10;
            spearman->state = ENEMY2_HURT;
            spearman->frame = 0;
            spearman->lastFrameTime = currentTime;
        }
    }

    // Pixel-perfect collision for spearman attack
    if (spearman->state == ENEMY2_ATTACKING && spearman->frame == 2 && spearman->attackCooldown == 0) {
        SDL_Rect spearmanRect = {spearman->world_x, spearman->position.y, spearman->position.w, spearman->position.h};
        SDL_Rect playerRect = {game->player2.world_x, game->player2.position.y, game->player2.position.w, game->player2.position.h};
        SDL_Surface *spearmanSheet = spearman->direction > 0 ? spearmanAttackSheet : spearmanAttackSheetFlipped;
        SDL_Surface *playerSheet = game->player2.lookingRight ? game->player2.spriteSheet : game->player2.spriteSheetFlipped;
        SDL_Rect srcRectSpearman = {spearman->frame * SPRITE_WIDTH, 0, SPRITE_WIDTH, SPRITE_HEIGHT};
        SDL_Rect srcRectPlayer = game->player2.frame;
        if (!game->player2.lookingRight) {
            srcRectPlayer.x = (playerSheet->w - srcRectPlayer.x) - srcRectPlayer.w;
        }

        if (pixelPerfectCollision(spearmanSheet, &spearmanRect, &srcRectSpearman, playerSheet, &playerRect, &srcRectPlayer)) {
            *playerHealth -= 7;
            spearman->attackCooldown = 1000;
            if (*playerHealth <= 0) game->player2.etat = P2_DEAD;
            else game->player2.etat = P2_HURT;
        }
    } else if (spearman->state == ENEMY2_RUN_ATTACK && spearman->frame == 2 && spearman->attackCooldown == 0) {
        SDL_Rect spearmanRect = {spearman->world_x, spearman->position.y, spearman->position.w, spearman->position.h};
        SDL_Rect playerRect = {game->player2.world_x, game->player2.position.y, game->player2.position.w, game->player2.position.h};
        SDL_Surface *spearmanSheet = spearman->direction > 0 ? spearmanRunAttackSheet : spearmanRunAttackSheetFlipped;
        SDL_Surface *playerSheet = game->player2.lookingRight ? game->player2.spriteSheet : game->player2.spriteSheetFlipped;
        SDL_Rect srcRectSpearman = {spearman->frame * SPRITE_WIDTH, 0, SPRITE_WIDTH, SPRITE_HEIGHT};
        SDL_Rect srcRectPlayer = game->player2.frame;
        if (!game->player2.lookingRight) {
            srcRectPlayer.x = (playerSheet->w - srcRectPlayer.x) - srcRectPlayer.w;
        }

        if (pixelPerfectCollision(spearmanSheet, &spearmanRect, &srcRectSpearman, playerSheet, &playerRect, &srcRectPlayer)) {
            *playerHealth -= 10;
            spearman->attackCooldown = 1000;
            if (*playerHealth <= 0) game->player2.etat = P2_DEAD;
            else game->player2.etat = P2_HURT;
        }
    }

    // Update attack cooldown
    Uint32 deltaTime = currentTime - spearman->lastFrameTime;
    if (spearman->attackCooldown > 0) spearman->attackCooldown = spearman->attackCooldown > deltaTime ? spearman->attackCooldown - deltaTime : 0;

    // Animation timing
    int frameDuration;
    switch (spearman->state) {
        case ENEMY2_IDLE: frameDuration = SPEARMAN_IDLE_DURATION; spearman->totalFrames = SPEARMAN_IDLE_FRAMES; break;
        case ENEMY2_WALKING: frameDuration = SPEARMAN_WALK_DURATION; spearman->totalFrames = SPEARMAN_WALK_FRAMES; break;
        case ENEMY2_RUNNING: frameDuration = SPEARMAN_RUN_DURATION; spearman->totalFrames = SPEARMAN_RUN_FRAMES; break;
        case ENEMY2_ATTACKING: frameDuration = SPEARMAN_ATTACK_DURATION; spearman->totalFrames = SPEARMAN_ATTACK_FRAMES; break;
        case ENEMY2_HURT: frameDuration = SPEARMAN_HURT_DURATION; spearman->totalFrames = SPEARMAN_HURT_FRAMES; break;
        case ENEMY2_DYING: frameDuration = SPEARMAN_DYING_DURATION; spearman->totalFrames = SPEARMAN_DYING_FRAMES; break;
        case ENEMY2_FALL: frameDuration = SPEARMAN_FALL_DURATION; spearman->totalFrames = SPEARMAN_FALL_FRAMES; break;
        case ENEMY2_RUN_ATTACK: frameDuration = SPEARMAN_RUN_ATTACK_DURATION; spearman->totalFrames = SPEARMAN_RUN_ATTACK_FRAMES; break;
        case ENEMY2_DEAD: frameDuration = SPEARMAN_DYING_DURATION; spearman->totalFrames = 1; break;
        default: frameDuration = SPEARMAN_IDLE_DURATION; spearman->totalFrames = SPEARMAN_IDLE_FRAMES; break;
    }

    if (currentTime - spearman->lastFrameTime >= frameDuration) {
        if (spearman->state == ENEMY2_HURT || spearman->state == ENEMY2_ATTACKING || spearman->state == ENEMY2_DYING || spearman->state == ENEMY2_FALL || spearman->state == ENEMY2_RUN_ATTACK) {
            if (spearman->frame < spearman->totalFrames - 1) {
                spearman->frame++;
            } else {
                if (spearman->state == ENEMY2_DYING) {
                    spearman->state = ENEMY2_DEAD;
                    spearman->frame = 0;
                    spearman->active = 0;
                    game->player2.score += 50;
                } else {
                    spearman->state = ENEMY2_IDLE;
                    spearman->frame = 0;
                }
            }
        } else {
            spearman->frame = (spearman->frame + 1) % spearman->totalFrames;
        }
        spearman->lastFrameTime = currentTime;
    }
}

void renderSkeletonSpearman(SDL_Surface *screen, SkeletonSpearman *spearman, int scroll_x) {
    if (!screen || !spearman || !spearman->active) return;

    SDL_Surface *sheet = spearman->direction > 0 ?
        (spearman->state == ENEMY2_IDLE ? spearmanIdleSheet :
         spearman->state == ENEMY2_WALKING ? spearmanWalkSheet :
         spearman->state == ENEMY2_RUNNING ? spearmanRunSheet :
         spearman->state == ENEMY2_ATTACKING ? spearmanAttackSheet :
         spearman->state == ENEMY2_HURT ? spearmanHurtSheet :
         spearman->state == ENEMY2_FALL ? spearmanFallSheet :
         spearman->state == ENEMY2_RUN_ATTACK ? spearmanRunAttackSheet :
         spearmanDyingSheet) :
        (spearman->state == ENEMY2_IDLE ? spearmanIdleSheetFlipped :
         spearman->state == ENEMY2_WALKING ? spearmanWalkSheetFlipped :
         spearman->state == ENEMY2_RUNNING ? spearmanRunSheetFlipped :
         spearman->state == ENEMY2_ATTACKING ? spearmanAttackSheetFlipped :
         spearman->state == ENEMY2_HURT ? spearmanHurtSheetFlipped :
         spearman->state == ENEMY2_FALL ? spearmanFallSheetFlipped :
         spearman->state == ENEMY2_RUN_ATTACK ? spearmanRunAttackSheetFlipped :
         spearmanDyingSheetFlipped);

    SDL_Rect srcRect = {spearman->frame * SPRITE_WIDTH, 0, SPRITE_WIDTH, SPRITE_HEIGHT};
    SDL_Rect destRect = {spearman->world_x - scroll_x, spearman->position.y, SPRITE_WIDTH, SPRITE_HEIGHT};
    SDL_BlitSurface(sheet, &srcRect, screen, &destRect);

    if (spearman->health > 0) {
        SDL_Rect healthBarBg = {spearman->world_x - scroll_x, spearman->position.y - 20, 50, 5};
        SDL_FillRect(screen, &healthBarBg, SDL_MapRGB(screen->format, 255, 0, 0));
        SDL_Rect healthBar = {spearman->world_x - scroll_x, spearman->position.y - 20, (50 * spearman->health) / spearman->maxHealth, 5};
        SDL_FillRect(screen, &healthBar, SDL_MapRGB(screen->format, 0, 255, 0));
    }
}

void freeSkeletonSpearman(SkeletonSpearman *spearman) {
    if (spearman) spearman->active = 0;
}

// --- Free all enemy sprites ---
void freeEnemyLvl2Sprites() {
    if (mummyIdleSheet) SDL_FreeSurface(mummyIdleSheet);
    if (mummyIdleSheetFlipped) SDL_FreeSurface(mummyIdleSheetFlipped);
    if (mummyWalkSheet) SDL_FreeSurface(mummyWalkSheet);
    if (mummyWalkSheetFlipped) SDL_FreeSurface(mummyWalkSheetFlipped);
    if (mummyAttackSheet) SDL_FreeSurface(mummyAttackSheet);
    if (mummyAttackSheetFlipped) SDL_FreeSurface(mummyAttackSheetFlipped);
    if (mummyHurtSheet) SDL_FreeSurface(mummyHurtSheet);
    if (mummyHurtSheetFlipped) SDL_FreeSurface(mummyHurtSheetFlipped);
    if (mummyDyingSheet) SDL_FreeSurface(mummyDyingSheet);
    if (mummyDyingSheetFlipped) SDL_FreeSurface(mummyDyingSheetFlipped);

    if (deceasedIdleSheet) SDL_FreeSurface(deceasedIdleSheet);
    if (deceasedIdleSheetFlipped) SDL_FreeSurface(deceasedIdleSheetFlipped);
    if (deceasedWalkSheet) SDL_FreeSurface(deceasedWalkSheet);
    if (deceasedWalkSheetFlipped) SDL_FreeSurface(deceasedWalkSheetFlipped);
    if (deceasedAttackSheet) SDL_FreeSurface(deceasedAttackSheet);
    if (deceasedAttackSheetFlipped) SDL_FreeSurface(deceasedAttackSheetFlipped);
    if (deceasedHurtSheet) SDL_FreeSurface(deceasedHurtSheet);
    if (deceasedHurtSheetFlipped) SDL_FreeSurface(deceasedHurtSheetFlipped);
    if (deceasedDyingSheet) SDL_FreeSurface(deceasedDyingSheet);
    if (deceasedDyingSheetFlipped) SDL_FreeSurface(deceasedDyingSheetFlipped);
    if (deceasedProjectileSheet) SDL_FreeSurface(deceasedProjectileSheet);

    if (gorgonIdleSheet) SDL_FreeSurface(gorgonIdleSheet);
    if (gorgonIdleSheetFlipped) SDL_FreeSurface(gorgonIdleSheetFlipped);
    if (gorgonWalkSheet) SDL_FreeSurface(gorgonWalkSheet);
    if (gorgonWalkSheetFlipped) SDL_FreeSurface(gorgonWalkSheetFlipped);
    if (gorgonRunSheet) SDL_FreeSurface(gorgonRunSheet);
    if (gorgonRunSheetFlipped) SDL_FreeSurface(gorgonRunSheetFlipped);
    if (gorgonAttackSheet) SDL_FreeSurface(gorgonAttackSheet);
    if (gorgonAttackSheetFlipped) SDL_FreeSurface(gorgonAttackSheetFlipped);
    if (gorgonHurtSheet) SDL_FreeSurface(gorgonHurtSheet);
    if (gorgonHurtSheetFlipped) SDL_FreeSurface(gorgonHurtSheetFlipped);
    if (gorgonDyingSheet) SDL_FreeSurface(gorgonDyingSheet);
    if (gorgonDyingSheetFlipped) SDL_FreeSurface(gorgonDyingSheetFlipped);
    if (gorgonSpecialSheet) SDL_FreeSurface(gorgonSpecialSheet);
    if (gorgonSpecialSheetFlipped) SDL_FreeSurface(gorgonSpecialSheetFlipped);

    if (spearmanIdleSheet) SDL_FreeSurface(spearmanIdleSheet);
    if (spearmanIdleSheetFlipped) SDL_FreeSurface(spearmanIdleSheetFlipped);
    if (spearmanWalkSheet) SDL_FreeSurface(spearmanWalkSheet);
    if (spearmanWalkSheetFlipped) SDL_FreeSurface(spearmanWalkSheetFlipped);
    if (spearmanRunSheet) SDL_FreeSurface(spearmanRunSheet);
    if (spearmanRunSheetFlipped) SDL_FreeSurface(spearmanRunSheetFlipped);
    if (spearmanAttackSheet) SDL_FreeSurface(spearmanAttackSheet);
    if (spearmanAttackSheetFlipped) SDL_FreeSurface(spearmanAttackSheetFlipped);
    if (spearmanHurtSheet) SDL_FreeSurface(spearmanHurtSheet);
    if (spearmanHurtSheetFlipped) SDL_FreeSurface(spearmanHurtSheetFlipped);
    if (spearmanDyingSheet) SDL_FreeSurface(spearmanDyingSheet);
    if (spearmanDyingSheetFlipped) SDL_FreeSurface(spearmanDyingSheetFlipped);
    if (spearmanFallSheet) SDL_FreeSurface(spearmanFallSheet);
    if (spearmanFallSheetFlipped) SDL_FreeSurface(spearmanFallSheetFlipped);
    if (spearmanRunAttackSheet) SDL_FreeSurface(spearmanRunAttackSheet);
    if (spearmanRunAttackSheetFlipped) SDL_FreeSurface(spearmanRunAttackSheetFlipped);

    // Reset pointers to NULL
    mummyIdleSheet = mummyIdleSheetFlipped = mummyWalkSheet = mummyWalkSheetFlipped = NULL;
    mummyAttackSheet = mummyAttackSheetFlipped = mummyHurtSheet = mummyHurtSheetFlipped = NULL;
    mummyDyingSheet = mummyDyingSheetFlipped = NULL;
    deceasedIdleSheet = deceasedIdleSheetFlipped = deceasedWalkSheet = deceasedWalkSheetFlipped = NULL;
    deceasedAttackSheet = deceasedAttackSheetFlipped = deceasedHurtSheet = deceasedHurtSheetFlipped = NULL;
    deceasedDyingSheet = deceasedDyingSheetFlipped = deceasedProjectileSheet = NULL;
    gorgonIdleSheet = gorgonIdleSheetFlipped = gorgonWalkSheet = gorgonWalkSheetFlipped = NULL;
    gorgonRunSheet = gorgonRunSheetFlipped = gorgonAttackSheet = gorgonAttackSheetFlipped = NULL;
    gorgonHurtSheet = gorgonHurtSheetFlipped = gorgonDyingSheet = gorgonDyingSheetFlipped = NULL;
    gorgonSpecialSheet = gorgonSpecialSheetFlipped = NULL;
    spearmanIdleSheet = spearmanIdleSheetFlipped = spearmanWalkSheet = spearmanWalkSheetFlipped = NULL;
    spearmanRunSheet = spearmanRunSheetFlipped = spearmanAttackSheet = spearmanAttackSheetFlipped = NULL;
    spearmanHurtSheet = spearmanHurtSheetFlipped = spearmanDyingSheet = spearmanDyingSheetFlipped = NULL;
    spearmanFallSheet = spearmanFallSheetFlipped = spearmanRunAttackSheet = spearmanRunAttackSheetFlipped = NULL;
}
