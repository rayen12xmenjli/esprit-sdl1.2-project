#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "enemy.h"
#include "mouvement.h"
#include "collision.h"
#include "game.h"
#include "utils.h"

// Adjustable constants
static const int MOVE_SPEED = 3;
static const int LOW_HEALTH_THRESHOLD = 30;
static const int NORMAL_DAMAGE = 5;
static const int HIGH_DAMAGE = 10;
static const int NORMAL_COOLDOWN = 60;
static const int FAST_COOLDOWN = 30;
static const float ENEMY_GRAVITY = 0.5f;
static const int FRAME_DELAY = 8;

// Static sprite sheets
static SDL_Surface *attack1Sheet = NULL;
static SDL_Surface *attack1SheetFlipped = NULL;
static SDL_Surface *attack2Sheet = NULL;
static SDL_Surface *attack2SheetFlipped = NULL;
static SDL_Surface *attack3Sheet = NULL;
static SDL_Surface *attack3SheetFlipped = NULL;
static SDL_Surface *deadSheet = NULL;
static SDL_Surface *deadSheetFlipped = NULL;
static SDL_Surface *enablingSheet = NULL;
static SDL_Surface *enablingSheetFlipped = NULL;
static SDL_Surface *hurtSheet = NULL;
static SDL_Surface *hurtSheetFlipped = NULL;
static SDL_Surface *idleSheet = NULL;
static SDL_Surface *idleSheetFlipped = NULL;
static SDL_Surface *walkSheet = NULL;
static SDL_Surface *walkSheetFlipped = NULL;
static SDL_Surface *shutdownSheet = NULL;
static SDL_Surface *shutdownSheetFlipped = NULL;

void initEnemy(Enemy *enemy, int x, int y) {
    if (!enemy) {
        fprintf(stderr, "initEnemy: Enemy pointer is NULL\n");
        exit(1);
    }
    printf("Starting initEnemy at x=%d, y=%d\n", x, y);

    // Load sprite sheets
    if (!attack1Sheet) {
        attack1Sheet = IMG_Load("assets/enemies/Swordsman/attack_1_resized.png");
        if (!attack1Sheet) { fprintf(stderr, "Failed to load attack_1_resized.png: %s\n", SDL_GetError()); exit(1); }
        SDL_SetColorKey(attack1Sheet, SDL_SRCCOLORKEY, SDL_MapRGB(attack1Sheet->format, 0, 0, 0));
        SDL_Surface *optimized = SDL_DisplayFormat(attack1Sheet);
        if (optimized) { SDL_FreeSurface(attack1Sheet); attack1Sheet = optimized; }
        attack1SheetFlipped = flipHorizontally(attack1Sheet);
        if (!attack1SheetFlipped) { fprintf(stderr, "Failed to flip attack_1_resized.png\n"); exit(1); }
    }
    if (!attack2Sheet) {
        attack2Sheet = IMG_Load("assets/enemies/Swordsman/attack_2_resized.png");
        if (!attack2Sheet) { fprintf(stderr, "Failed to load attack_2_resized.png: %s\n", SDL_GetError()); exit(1); }
        SDL_SetColorKey(attack2Sheet, SDL_SRCCOLORKEY, SDL_MapRGB(attack2Sheet->format, 0, 0, 0));
        SDL_Surface *optimized = SDL_DisplayFormat(attack2Sheet);
        if (optimized) { SDL_FreeSurface(attack2Sheet); attack2Sheet = optimized; }
        attack2SheetFlipped = flipHorizontally(attack2Sheet);
        if (!attack2SheetFlipped) { fprintf(stderr, "Failed to flip attack_2_resized.png\n"); exit(1); }
    }
    if (!attack3Sheet) {
        attack3Sheet = IMG_Load("assets/enemies/Swordsman/attack_3_resized.png");
        if (!attack3Sheet) { fprintf(stderr, "Failed to load attack_3_resized.png: %s\n", SDL_GetError()); exit(1); }
        SDL_SetColorKey(attack3Sheet, SDL_SRCCOLORKEY, SDL_MapRGB(attack3Sheet->format, 0, 0, 0));
        SDL_Surface *optimized = SDL_DisplayFormat(attack3Sheet);
        if (optimized) { SDL_FreeSurface(attack3Sheet); attack3Sheet = optimized; }
        attack3SheetFlipped = flipHorizontally(attack3Sheet);
        if (!attack3SheetFlipped) { fprintf(stderr, "Failed to flip attack_3_resized.png\n"); exit(1); }
    }
    if (!deadSheet) {
        deadSheet = IMG_Load("assets/enemies/Swordsman/dead_resized.png");
        if (!deadSheet) { fprintf(stderr, "Failed to load dead_resized.png: %s\n", SDL_GetError()); exit(1); }
        SDL_SetColorKey(deadSheet, SDL_SRCCOLORKEY, SDL_MapRGB(deadSheet->format, 0, 0, 0));
        SDL_Surface *optimized = SDL_DisplayFormat(deadSheet);
        if (optimized) { SDL_FreeSurface(deadSheet); deadSheet = optimized; }
        deadSheetFlipped = flipHorizontally(deadSheet);
        if (!deadSheetFlipped) { fprintf(stderr, "Failed to flip dead_resized.png\n"); exit(1); }
    }
    if (!enablingSheet) {
        enablingSheet = IMG_Load("assets/enemies/Swordsman/enabling_resized.png");
        if (!enablingSheet) { fprintf(stderr, "Failed to load enabling_resized.png: %s\n", SDL_GetError()); exit(1); }
        SDL_SetColorKey(enablingSheet, SDL_SRCCOLORKEY, SDL_MapRGB(enablingSheet->format, 0, 0, 0));
        SDL_Surface *optimized = SDL_DisplayFormat(enablingSheet);
        if (optimized) { SDL_FreeSurface(enablingSheet); enablingSheet = optimized; }
        enablingSheetFlipped = flipHorizontally(enablingSheet);
        if (!enablingSheetFlipped) { fprintf(stderr, "Failed to flip enabling_resized.png\n"); exit(1); }
    }
    if (!hurtSheet) {
        hurtSheet = IMG_Load("assets/enemies/Swordsman/hurt_resized.png");
        if (!hurtSheet) { fprintf(stderr, "Failed to load hurt_resized.png: %s\n", SDL_GetError()); exit(1); }
        SDL_SetColorKey(hurtSheet, SDL_SRCCOLORKEY, SDL_MapRGB(hurtSheet->format, 0, 0, 0));
        SDL_Surface *optimized = SDL_DisplayFormat(hurtSheet);
        if (optimized) { SDL_FreeSurface(hurtSheet); hurtSheet = optimized; }
        hurtSheetFlipped = flipHorizontally(hurtSheet);
        if (!hurtSheetFlipped) { fprintf(stderr, "Failed to flip hurt_resized.png\n"); exit(1); }
    }
    if (!idleSheet) {
        idleSheet = IMG_Load("assets/enemies/Swordsman/idle_resized.png");
        if (!idleSheet) { fprintf(stderr, "Failed to load idle_resized.png: %s\n", SDL_GetError()); exit(1); }
        SDL_SetColorKey(idleSheet, SDL_SRCCOLORKEY, SDL_MapRGB(idleSheet->format, 0, 0, 0));
        SDL_Surface *optimized = SDL_DisplayFormat(idleSheet);
        if (optimized) { SDL_FreeSurface(idleSheet); idleSheet = optimized; }
        idleSheetFlipped = flipHorizontally(idleSheet);
        if (!idleSheetFlipped) { fprintf(stderr, "Failed to flip idle_resized.png\n"); exit(1); }
    }
    if (!walkSheet) {
        walkSheet = IMG_Load("assets/enemies/Swordsman/walk_resized.png");
        if (!walkSheet) { fprintf(stderr, "Failed to load walk_resized.png: %s\n", SDL_GetError()); exit(1); }
        SDL_SetColorKey(walkSheet, SDL_SRCCOLORKEY, SDL_MapRGB(walkSheet->format, 0, 0, 0));
        SDL_Surface *optimized = SDL_DisplayFormat(walkSheet);
        if (optimized) { SDL_FreeSurface(walkSheet); walkSheet = optimized; }
        walkSheetFlipped = flipHorizontally(walkSheet);
        if (!walkSheetFlipped) { fprintf(stderr, "Failed to flip walk_resized.png\n"); exit(1); }
    }
    if (!shutdownSheet) {
        shutdownSheet = IMG_Load("assets/enemies/Swordsman/shutdown_resized.png");
        if (!shutdownSheet) { fprintf(stderr, "Failed to load shutdown_resized.png: %s\n", SDL_GetError()); exit(1); }
        SDL_SetColorKey(shutdownSheet, SDL_SRCCOLORKEY, SDL_MapRGB(shutdownSheet->format, 0, 0, 0));
        SDL_Surface *optimized = SDL_DisplayFormat(shutdownSheet);
        if (optimized) { SDL_FreeSurface(shutdownSheet); shutdownSheet = optimized; }
        shutdownSheetFlipped = flipHorizontally(shutdownSheet);
        if (!shutdownSheetFlipped) { fprintf(stderr, "Failed to flip shutdown_resized.png\n"); exit(1); }
    }

    // Initialize properties
    enemy->world_x = x;
    enemy->position.x = x;
    enemy->position.y = y;
    enemy->position.w = 258;
    enemy->position.h = 258;
    enemy->state = ENEMY_SHUTDOWN;
    enemy->frame = 0;
    enemy->totalFrames = 5;
    enemy->health = 100;
    enemy->maxHealth = 100;
    enemy->speed = MOVE_SPEED;
    enemy->facingLeft = 0;
    enemy->active = 1;
    enemy->yVelocity = 0.0f;
    enemy->onGround = 0;
    enemy->frameDelay = 0;
    enemy->damageCooldown = 0;
    enemy->invulnerabilityTimer = 0;
    enemy->activationZone = 500;
    enemy->attackZone = 100;
    printf("Enemy initialized at x=%d, y=%d\n", x, y);
}

void updateEnemy(Enemy *enemy, SDL_Rect playerPosition, int *playerHealth, int playerMaxHealth, struct GAME *game) {
    if (!enemy || !playerHealth || !game) {
        fprintf(stderr, "updateEnemy: Invalid parameters\n");
        return;
    }
    if (!enemy->active || enemy->state == ENEMY_DEAD) return;

    // Adjust difficulty based on health
    int isLowHealth = enemy->health <= LOW_HEALTH_THRESHOLD;
    int damage = isLowHealth ? HIGH_DAMAGE : NORMAL_DAMAGE;
    int attackCooldown = isLowHealth ? FAST_COOLDOWN : NORMAL_COOLDOWN;
    int moveSpeed = isLowHealth ? MOVE_SPEED + 1 : MOVE_SPEED;

    // Set total frames
    switch (enemy->state) {
        case ENEMY_IDLE: enemy->totalFrames = 5; break;
        case ENEMY_WALKING: enemy->totalFrames = 8; break;
        case ENEMY_ATTACKING:
            enemy->totalFrames = isLowHealth ? 2 : 4; // Use faster attack3 at low health
            break;
        case ENEMY_HURT: enemy->totalFrames = 3; break;
        case ENEMY_DEAD: enemy->totalFrames = 4; break;
        case ENEMY_SHUTDOWN: enemy->totalFrames = 5; break;
        case ENEMY_ENABLING: enemy->totalFrames = 5; break;
    }

    // Apply gravity using onGroundSoldier for consistency with soldier.c
    if (!onGroundSoldier(game, (Soldier*)enemy)) {
        enemy->yVelocity += ENEMY_GRAVITY;
        enemy->position.y += (int)enemy->yVelocity;
    } else {
        enemy->yVelocity = 0;
        placeEnemyOnGround(game, enemy);
    }

    // Update timers
    if (enemy->damageCooldown > 0) enemy->damageCooldown--;
    if (enemy->invulnerabilityTimer > 0) enemy->invulnerabilityTimer--;

    // Calculate distance to player
    int playerWorldX = playerPosition.x + game->background.scroll_x;
    int dx = playerWorldX - enemy->world_x;
    int dy = playerPosition.y - enemy->position.y;
    int distance = (int)sqrt(dx * dx + dy * dy);
    enemy->facingLeft = dx < 0 ? 1 : 0;

    // Handle shutdown and enabling
    if (enemy->state == ENEMY_SHUTDOWN) {
        if (distance <= enemy->activationZone) {
            enemy->state = ENEMY_ENABLING;
            enemy->frame = 0;
            enemy->totalFrames = 5;
        }
    } else if (enemy->state == ENEMY_ENABLING) {
        if (enemy->frame >= enemy->totalFrames - 1) {
            enemy->state = ENEMY_IDLE;
            enemy->frame = 0;
            enemy->totalFrames = 5;
        }
    }

    // Handle hurt state
    if (enemy->state == ENEMY_HURT && enemy->invulnerabilityTimer <= 0) {
        enemy->state = distance <= enemy->activationZone ? ENEMY_WALKING : ENEMY_IDLE;
        enemy->frame = 0;
        enemy->totalFrames = enemy->state == ENEMY_WALKING ? 8 : 5;
    }

    // AI logic
    if (enemy->state != ENEMY_SHUTDOWN && enemy->state != ENEMY_ENABLING && enemy->invulnerabilityTimer <= 0) {
        // Check for attack
        SDL_Rect attackRect = {
            enemy->position.x + (enemy->facingLeft ? -100 : enemy->position.w),
            enemy->position.y,
            100, enemy->position.h
        };
        if (rectIntersect(&attackRect, &playerPosition) && enemy->damageCooldown <= 0) {
            enemy->state = ENEMY_ATTACKING;
            if (enemy->frame >= enemy->totalFrames - 1) {
                *playerHealth -= damage;
                enemy->damageCooldown = attackCooldown;
                if (*playerHealth <= 0) {
                    game->player.state = DEAD;
                    game->player.health = 0;
                } else {
                    game->player.state = HURT;
                }
                enemy->frame = 0;
            }
        } else if (distance <= enemy->activationZone && enemy->state != ENEMY_ATTACKING) {
            // Move towards player
            enemy->state = ENEMY_WALKING;
            int newWorldX = enemy->world_x + (dx > 0 ? moveSpeed : dx < 0 ? -moveSpeed : 0);
            // Removed checkWallCollision call
            enemy->world_x = newWorldX;
            if (enemy->world_x < 0) enemy->world_x = 0;
            if (enemy->world_x > game->background.width - enemy->position.w) {
                enemy->world_x = game->background.width - enemy->position.w;
            }
            enemy->position.x = enemy->world_x - game->background.scroll_x;
        } else if (enemy->state != ENEMY_ATTACKING) {
            enemy->state = ENEMY_IDLE;
        }
    }

    // Handle player attacks
    if (game->player.state == ATTACK_1 && enemy->invulnerabilityTimer <= 0) {
        SDL_Rect playerAttackRect = {
            game->player.position.x + (game->player.facing == RIGHT ? game->player.position.w : -100),
            game->player.position.y,
            100, game->player.position.h
        };
        if (rectIntersect(&playerAttackRect, &enemy->position)) {
            enemy->health -= 20;
            enemy->state = ENEMY_HURT;
            enemy->frame = 0;
            enemy->totalFrames = 3;
            enemy->invulnerabilityTimer = 30;
        }
    }
    if (game->player.bulletActive && enemy->invulnerabilityTimer <= 0) {
        SDL_Rect bulletRect = {game->player.bullet.x, game->player.bullet.y, game->player.bullet.w, game->player.bullet.h};
        if (rectIntersect(&bulletRect, &enemy->position)) {
            enemy->health -= 15;
            enemy->state = ENEMY_HURT;
            enemy->frame = 0;
            enemy->totalFrames = 3;
            enemy->invulnerabilityTimer = 30;
            game->player.bulletActive = 0;
        }
    }

    // Handle death
    if (enemy->health <= 0 && enemy->state != ENEMY_DEAD) {
        enemy->state = ENEMY_DEAD;
        enemy->frame = 0;
        enemy->totalFrames = 4;
        game->player.score += 50;
    }

    // Update animation
    if (++enemy->frameDelay >= FRAME_DELAY) {
        enemy->frame++;
        if (enemy->state == ENEMY_IDLE || enemy->state == ENEMY_WALKING) {
            enemy->frame %= enemy->totalFrames;
        } else if (enemy->state == ENEMY_DEAD && enemy->frame >= enemy->totalFrames) {
            enemy->active = 0;
            enemy->frame = enemy->totalFrames - 1;
        } else if (enemy->frame >= enemy->totalFrames) {
            enemy->frame = 0;
            if (enemy->state == ENEMY_ATTACKING) {
                enemy->state = ENEMY_IDLE;
                enemy->totalFrames = 5;
            }
        }
        enemy->frameDelay = 0;
    }
}

void renderEnemy(SDL_Surface *screen, Enemy *enemy, int scroll_x) {
    if (!screen || !enemy || !enemy->active) return;

    SDL_Surface *sheet = enemy->facingLeft ?
        (enemy->state == ENEMY_IDLE ? idleSheetFlipped :
         enemy->state == ENEMY_WALKING ? walkSheetFlipped :
         enemy->state == ENEMY_ATTACKING ? (enemy->health <= LOW_HEALTH_THRESHOLD ? attack3SheetFlipped : attack1SheetFlipped) :
         enemy->state == ENEMY_HURT ? hurtSheetFlipped :
         enemy->state == ENEMY_DEAD ? deadSheetFlipped :
         enemy->state == ENEMY_SHUTDOWN ? shutdownSheetFlipped :
         enablingSheetFlipped) :
        (enemy->state == ENEMY_IDLE ? idleSheet :
         enemy->state == ENEMY_WALKING ? walkSheet :
         enemy->state == ENEMY_ATTACKING ? (enemy->health <= LOW_HEALTH_THRESHOLD ? attack3Sheet : attack1Sheet) :
         enemy->state == ENEMY_HURT ? hurtSheet :
         enemy->state == ENEMY_DEAD ? deadSheet :
         enemy->state == ENEMY_SHUTDOWN ? shutdownSheet :
         enablingSheet);

    SDL_Rect srcRect = {enemy->frame * 256, 0, 256, 256};
    if (srcRect.x >= sheet->w) srcRect.x = 0;
    SDL_Rect destRect = {enemy->position.x - scroll_x, enemy->position.y, 258, 258};
    SDL_BlitSurface(sheet, &srcRect, screen, &destRect);

    // Render health bar
    if (enemy->state != ENEMY_DEAD && enemy->health > 0) {
        int hb_x = enemy->position.x - scroll_x + (enemy->position.w / 2) - 50;
        int hb_y = enemy->position.y - 20;
        int hb_width = 100;
        int hb_height = 10;
        SDL_Rect bgRect = {hb_x, hb_y, hb_width, hb_height};
        SDL_FillRect(screen, &bgRect, SDL_MapRGB(screen->format, 0, 0, 0));
        int health_width = (int)((enemy->health / (float)enemy->maxHealth) * hb_width);
        SDL_Rect healthRect = {hb_x, hb_y, health_width, hb_height};
        SDL_FillRect(screen, &healthRect, SDL_MapRGB(screen->format, 255, 0, 0));
    }
}

void freeEnemy(Enemy *enemy) {
    if (!enemy) return;
    enemy->active = 0;
}

void freeEnemySprites(void) {
    SDL_FreeSurface(attack1Sheet); attack1Sheet = NULL;
    SDL_FreeSurface(attack1SheetFlipped); attack1SheetFlipped = NULL;
    SDL_FreeSurface(attack2Sheet); attack2Sheet = NULL;
    SDL_FreeSurface(attack2SheetFlipped); attack2SheetFlipped = NULL;
    SDL_FreeSurface(attack3Sheet); attack3Sheet = NULL;
    SDL_FreeSurface(attack3SheetFlipped); attack3SheetFlipped = NULL;
    SDL_FreeSurface(deadSheet); deadSheet = NULL;
    SDL_FreeSurface(deadSheetFlipped); deadSheetFlipped = NULL;
    SDL_FreeSurface(enablingSheet); enablingSheet = NULL;
    SDL_FreeSurface(enablingSheetFlipped); enablingSheetFlipped = NULL;
    SDL_FreeSurface(hurtSheet); hurtSheet = NULL;
    SDL_FreeSurface(hurtSheetFlipped); hurtSheetFlipped = NULL;
    SDL_FreeSurface(idleSheet); idleSheet = NULL;
    SDL_FreeSurface(idleSheetFlipped); idleSheetFlipped = NULL;
    SDL_FreeSurface(walkSheet); walkSheet = NULL;
    SDL_FreeSurface(walkSheetFlipped); walkSheetFlipped = NULL;
    SDL_FreeSurface(shutdownSheet); shutdownSheet = NULL;
    SDL_FreeSurface(shutdownSheetFlipped); shutdownSheetFlipped = NULL;
}
