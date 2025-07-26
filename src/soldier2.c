#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "soldier2.h"
#include "mouvement.h"
#include "game.h"
#include "utils.h"

// Debug logging toggle
#define DEBUG_LOG 1
#define LOG(fmt, ...) if (DEBUG_LOG) fprintf(stderr, fmt, ##__VA_ARGS__)

// Constants aligned with soldier.c
#define UPDATE_DISTANCE 1500
#define FIRE_DISTANCE 500
#define MELEE_DISTANCE 50
#define MOVE_SPEED 4.0f
#define PATROL_SPEED 2.0f
#define MAX_FALL_SPEED 10.0f
#define FRAME_DELAY 8
#define DAMAGE_COOLDOWN 60
#define GRENADE_COOLDOWN 300
#define INVULNERABILITY_TIME 30
#define DEATH_ANIMATION_FRAMES 4 // Explicitly set to 4 frames for death animation

static SDL_Surface *idleSheet = NULL;
static SDL_Surface *idleSheetFlipped = NULL;
static SDL_Surface *walkSheet = NULL;
static SDL_Surface *walkSheetFlipped = NULL;
static SDL_Surface *attackSheet = NULL;
static SDL_Surface *attackSheetFlipped = NULL;
static SDL_Surface *shot2Sheet = NULL;
static SDL_Surface *shot2SheetFlipped = NULL;
static SDL_Surface *grenadeSheet = NULL;
static SDL_Surface *grenadeSheetFlipped = NULL;
static SDL_Surface *rechargeSheet = NULL;
static SDL_Surface *rechargeSheetFlipped = NULL;
static SDL_Surface *hurtSheet = NULL;
static SDL_Surface *hurtSheetFlipped = NULL;
static SDL_Surface *deadSheet = NULL;
static SDL_Surface *deadSheetFlipped = NULL;

void initSoldier2(Soldier2 *soldier, int x, int y, struct GAME *game) {
    if (!soldier || !game) {
        fprintf(stderr, "initSoldier2: Soldier or game pointer is NULL\n");
        exit(1);
    }
    if (x < 0 || x > game->background.width - 256) {
        fprintf(stderr, "initSoldier2: Invalid x coordinate %d\n", x);
        soldier->active = 0;
        return;
    }
    LOG("Initializing Soldier2 at x=%d, y=%d\n", x, y);

    // Load sprites, matching soldier.c's approach
    if (!idleSheet) {
        idleSheet = IMG_Load("assets/enemies/Soldier/Soldier_2/Idle.png");
        if (!idleSheet) {
            fprintf(stderr, "Failed to load Soldier_2/Idle.png: %s\n", IMG_GetError());
            exit(1);
        }
        Uint32 colorkey = SDL_MapRGB(idleSheet->format, 255, 255, 255);
        SDL_SetColorKey(idleSheet, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
        SDL_Surface *optimized = SDL_DisplayFormatAlpha(idleSheet);
        if (!optimized) {
            fprintf(stderr, "SDL_DisplayFormatAlpha failed for Idle.png: %s\n", SDL_GetError());
            SDL_FreeSurface(idleSheet);
            exit(1);
        }
        SDL_FreeSurface(idleSheet);
        idleSheet = optimized;
        idleSheetFlipped = flipHorizontally(idleSheet);
        if (!idleSheetFlipped) {
            fprintf(stderr, "Failed to flip Idle.png\n");
            SDL_FreeSurface(idleSheet);
            exit(1);
        }
        SDL_SetColorKey(idleSheetFlipped, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
    }
    if (!walkSheet) {
        walkSheet = IMG_Load("assets/enemies/Soldier/Soldier_2/Walk.png");
        if (!walkSheet) {
            fprintf(stderr, "Failed to load Soldier_2/Walk.png: %s\n", IMG_GetError());
            exit(1);
        }
        Uint32 colorkey = SDL_MapRGB(walkSheet->format, 255, 255, 255);
        SDL_SetColorKey(walkSheet, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
        SDL_Surface *optimized = SDL_DisplayFormatAlpha(walkSheet);
        if (!optimized) {
            fprintf(stderr, "SDL_DisplayFormatAlpha failed for Walk.png: %s\n", SDL_GetError());
            SDL_FreeSurface(walkSheet);
            exit(1);
        }
        SDL_FreeSurface(walkSheet);
        walkSheet = optimized;
        walkSheetFlipped = flipHorizontally(walkSheet);
        if (!walkSheetFlipped) {
            fprintf(stderr, "Failed to flip Walk.png\n");
            SDL_FreeSurface(walkSheet);
            exit(1);
        }
        SDL_SetColorKey(walkSheetFlipped, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
    }
    if (!attackSheet) {
        attackSheet = IMG_Load("assets/enemies/Soldier/Soldier_2/Attack.png");
        if (!attackSheet) {
            fprintf(stderr, "Failed to load Soldier_2/Attack.png: %s\n", IMG_GetError());
            exit(1);
        }
        Uint32 colorkey = SDL_MapRGB(attackSheet->format, 255, 255, 255);
        SDL_SetColorKey(attackSheet, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
        SDL_Surface *optimized = SDL_DisplayFormatAlpha(attackSheet);
        if (!optimized) {
            fprintf(stderr, "SDL_DisplayFormatAlpha failed for Attack.png: %s\n", SDL_GetError());
            SDL_FreeSurface(attackSheet);
            exit(1);
        }
        SDL_FreeSurface(attackSheet);
        attackSheet = optimized;
        attackSheetFlipped = flipHorizontally(attackSheet);
        if (!attackSheetFlipped) {
            fprintf(stderr, "Failed to flip Attack.png\n");
            SDL_FreeSurface(attackSheet);
            exit(1);
        }
        SDL_SetColorKey(attackSheetFlipped, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
    }
    if (!shot2Sheet) {
        shot2Sheet = IMG_Load("assets/enemies/Soldier/Soldier_2/Shot_2.png");
        if (!shot2Sheet) {
            fprintf(stderr, "Failed to load Soldier_2/Shot_2.png: %s\n", IMG_GetError());
            exit(1);
        }
        Uint32 colorkey = SDL_MapRGB(shot2Sheet->format, 255, 255, 255);
        SDL_SetColorKey(shot2Sheet, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
        SDL_Surface *optimized = SDL_DisplayFormatAlpha(shot2Sheet);
        if (!optimized) {
            fprintf(stderr, "SDL_DisplayFormatAlpha failed for Shot_2.png: %s\n", SDL_GetError());
            SDL_FreeSurface(shot2Sheet);
            exit(1);
        }
        SDL_FreeSurface(shot2Sheet);
        shot2Sheet = optimized;
        shot2SheetFlipped = flipHorizontally(shot2Sheet);
        if (!shot2SheetFlipped) {
            fprintf(stderr, "Failed to flip Shot_2.png\n");
            SDL_FreeSurface(shot2Sheet);
            exit(1);
        }
        SDL_SetColorKey(shot2SheetFlipped, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
    }
    if (!grenadeSheet) {
        grenadeSheet = IMG_Load("assets/enemies/Soldier/Soldier_2/Grenade.png");
        if (!grenadeSheet) {
            fprintf(stderr, "Failed to load Soldier_2/Grenade.png: %s\n", IMG_GetError());
            exit(1);
        }
        Uint32 colorkey = SDL_MapRGB(grenadeSheet->format, 255, 255, 255);
        SDL_SetColorKey(grenadeSheet, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
        SDL_Surface *optimized = SDL_DisplayFormatAlpha(grenadeSheet);
        if (!optimized) {
            fprintf(stderr, "SDL_DisplayFormatAlpha failed for Grenade.png: %s\n", SDL_GetError());
            SDL_FreeSurface(grenadeSheet);
            exit(1);
        }
        SDL_FreeSurface(grenadeSheet);
        grenadeSheet = optimized;
        grenadeSheetFlipped = flipHorizontally(grenadeSheet);
        if (!grenadeSheetFlipped) {
            fprintf(stderr, "Failed to flip Grenade.png\n");
            SDL_FreeSurface(grenadeSheet);
            exit(1);
        }
        SDL_SetColorKey(grenadeSheetFlipped, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
    }
    if (!rechargeSheet) {
        rechargeSheet = IMG_Load("assets/enemies/Soldier/Soldier_2/Recharge.png");
        if (!rechargeSheet) {
            fprintf(stderr, "Failed to load Soldier_2/Recharge.png: %s\n", IMG_GetError());
            exit(1);
        }
        Uint32 colorkey = SDL_MapRGB(rechargeSheet->format, 255, 255, 255);
        SDL_SetColorKey(rechargeSheet, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
        SDL_Surface *optimized = SDL_DisplayFormatAlpha(rechargeSheet);
        if (!optimized) {
            fprintf(stderr, "SDL_DisplayFormatAlpha failed for Recharge.png: %s\n", SDL_GetError());
            SDL_FreeSurface(rechargeSheet);
            exit(1);
        }
        SDL_FreeSurface(rechargeSheet);
        rechargeSheet = optimized;
        rechargeSheetFlipped = flipHorizontally(rechargeSheet);
        if (!rechargeSheetFlipped) {
            fprintf(stderr, "Failed to flip Recharge.png\n");
            SDL_FreeSurface(rechargeSheet);
            exit(1);
        }
        SDL_SetColorKey(rechargeSheetFlipped, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
    }
    if (!hurtSheet) {
        hurtSheet = IMG_Load("assets/enemies/Soldier/Soldier_2/Hurt.png");
        if (!hurtSheet) {
            fprintf(stderr, "Failed to load Soldier_2/Hurt.png: %s\n", IMG_GetError());
            exit(1);
        }
        Uint32 colorkey = SDL_MapRGB(hurtSheet->format, 255, 255, 255);
        SDL_SetColorKey(hurtSheet, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
        SDL_Surface *optimized = SDL_DisplayFormatAlpha(hurtSheet);
        if (!optimized) {
            fprintf(stderr, "SDL_DisplayFormatAlpha failed for Hurt.png: %s\n", SDL_GetError());
            SDL_FreeSurface(hurtSheet);
            exit(1);
        }
        SDL_FreeSurface(hurtSheet);
        hurtSheet = optimized;
        hurtSheetFlipped = flipHorizontally(hurtSheet);
        if (!hurtSheetFlipped) {
            fprintf(stderr, "Failed to flip Hurt.png\n");
            SDL_FreeSurface(hurtSheet);
            exit(1);
        }
        SDL_SetColorKey(hurtSheetFlipped, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
    }
    if (!deadSheet) {
        deadSheet = IMG_Load("assets/enemies/Soldier/Soldier_2/Dead.png");
        if (!deadSheet) {
            fprintf(stderr, "Failed to load Soldier_2/Dead.png: %s\n", IMG_GetError());
            exit(1);
        }
        Uint32 colorkey = SDL_MapRGB(deadSheet->format, 255, 255, 255);
        SDL_SetColorKey(deadSheet, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
        SDL_Surface *optimized = SDL_DisplayFormatAlpha(deadSheet);
        if (!optimized) {
            fprintf(stderr, "SDL_DisplayFormatAlpha failed for Dead.png: %s\n", SDL_GetError());
            SDL_FreeSurface(deadSheet);
            exit(1);
        }
        SDL_FreeSurface(deadSheet);
        deadSheet = optimized;
        deadSheetFlipped = flipHorizontally(deadSheet);
        if (!deadSheetFlipped) {
            fprintf(stderr, "Failed to flip Dead.png\n");
            SDL_FreeSurface(deadSheet);
            exit(1);
        }
        SDL_SetColorKey(deadSheetFlipped, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
    }

    // Initialize soldier fields, matching soldier.c
    soldier->world_x = x;
    soldier->position.x = x;
    soldier->position.y = y;
    soldier->position.w = 256;
    soldier->position.h = 256;
    soldier->xVelocity = 0.0f;
    soldier->yVelocity = 0.0f;
    soldier->onGround = 0;
    soldier->state = SOLDIER2_IDLE;
    soldier->frame = 0;
    soldier->totalFrames = idleSheet->w / 256;
    soldier->health = 15;
    soldier->maxHealth = 15;
    soldier->bullets = 5;
    soldier->damageCooldown = 0;
    soldier->invulnerabilityTimer = 0;
    soldier->facingLeft = 0;
    soldier->active = 1;
    soldier->frameDelay = 0;
    soldier->patrolDirection = 1;
    soldier->patrolTimer = 60 + rand() % 120;
    soldier->patrolLeft = x - 200;
    soldier->patrolRight = x + 200;
    soldier->grenadeCooldown = 0;
    soldier->smoke.position.w = 256;
    soldier->smoke.position.h = 256;
    soldier->smoke.frame = 0;
    soldier->smoke.totalFrames = grenadeSheet->w / 256;
    soldier->smoke.active = 0;

    // Snap to ground, matching soldier.c
    placeSoldier2OnGround(game, soldier);
    soldier->onGround = onGroundSoldier2(game, soldier);
    soldier->yVelocity = 0.0f;
    LOG("Soldier2 initialized: x=%d, y=%d, onGround=%d\n", soldier->world_x, soldier->position.y, soldier->onGround);
}

void updateSoldier2(Soldier2 *soldier, SDL_Rect playerPos, int *playerHealth, int playerMaxHealth, struct GAME *game) {
    if (!soldier || !playerHealth || !game) {
        LOG("updateSoldier2: Invalid parameters\n");
        return;
    }
    if (!soldier->active) {
        LOG("updateSoldier2: Soldier inactive, skipping update\n");
        return;
    }

    // Apply gravity and ground snapping, matching soldier.c
    soldier->onGround = onGroundSoldier2(game, soldier);
    if (!soldier->onGround) {
        soldier->yVelocity += GRAVITY;
        if (soldier->yVelocity > MAX_FALL_SPEED) soldier->yVelocity = MAX_FALL_SPEED;
    } else {
        soldier->yVelocity = 0.0f;
    }
    soldier->position.y += (int)soldier->yVelocity;
    if (soldier->onGround && soldier->yVelocity >= 0) {
        placeSoldier2OnGround(game, soldier);
    }

    // Boundary checks, matching soldier.c
    if (soldier->position.y < 0) {
        soldier->position.y = 0;
        soldier->yVelocity = 0.0f;
    }
    if (soldier->position.y > game->background.height - soldier->position.h) {
        soldier->position.y = game->background.height - soldier->position.h;
        soldier->yVelocity = 0.0f;
        soldier->onGround = 1;
    }
    if (soldier->world_x < 0) soldier->world_x = 0;
    if (soldier->world_x > game->background.width - soldier->position.w) {
        soldier->world_x = game->background.width - soldier->position.w;
    }

    // Update cooldowns
    if (soldier->damageCooldown > 0) soldier->damageCooldown--;
    if (soldier->invulnerabilityTimer > 0) soldier->invulnerabilityTimer--;
    if (soldier->grenadeCooldown > 0) soldier->grenadeCooldown--;

    // Update total frames based on state, explicitly set death animation to 4 frames
    switch (soldier->state) {
        case SOLDIER2_IDLE: soldier->totalFrames = idleSheet->w / 256; break;
        case SOLDIER2_WALK: soldier->totalFrames = walkSheet->w / 256; break;
        case SOLDIER2_ATTACK: soldier->totalFrames = attackSheet->w / 256; break;
        case SOLDIER2_SHOT_2: soldier->totalFrames = shot2Sheet->w / 256; break;
        case SOLDIER2_GRENADE: soldier->totalFrames = grenadeSheet->w / 256; break;
        case SOLDIER2_RECHARGE: soldier->totalFrames = rechargeSheet->w / 256; break;
        case SOLDIER2_HURT: soldier->totalFrames = hurtSheet->w / 256; break;
        case SOLDIER2_DEAD: soldier->totalFrames = DEATH_ANIMATION_FRAMES; break; // Fixed to 4 frames
    }

    // Player detection and movement, matching soldier.c
    int dx = game->player.world_x - soldier->world_x;
    int dy = (game->player.position.y + game->player.position.h / 2) - (soldier->position.y + soldier->position.h / 2);
    int distance = (int)sqrt(dx * dx + dy * dy);
    soldier->facingLeft = dx < 0 ? 1 : 0;

    // State machine, matching soldier.c
    if (soldier->state != SOLDIER2_HURT && soldier->state != SOLDIER2_DEAD) {
        if (soldier->health <= 0) {
            soldier->state = SOLDIER2_DEAD;
            soldier->frame = 0;
            soldier->xVelocity = 0.0f; // Stop movement
            soldier->yVelocity = 0.0f;
            game->player.score += 50;
            LOG("Soldier2 transitioned to DEAD state: frame=%d, totalFrames=%d\n", soldier->frame, soldier->totalFrames);
        } else if (distance < MELEE_DISTANCE) {
            soldier->xVelocity = 0.0f;
            soldier->state = SOLDIER2_ATTACK;
            if (soldier->frame >= soldier->totalFrames - 1 && soldier->damageCooldown <= 0) {
                *playerHealth -= 5;
                if (*playerHealth <= 0) {
                    game->player.state = DEAD;
                    game->player.health = 0;
                } else {
                    game->player.state = HURT;
                }
                soldier->damageCooldown = DAMAGE_COOLDOWN;
                soldier->frame = 0;
            }
        } else if (distance < FIRE_DISTANCE && soldier->bullets > 0) {
            soldier->xVelocity = 0.0f;
            soldier->state = SOLDIER2_SHOT_2;
            if (soldier->frame >= soldier->totalFrames - 1 && soldier->damageCooldown <= 0) {
                soldier->bullets--;
                *playerHealth -= 3;
                if (*playerHealth <= 0) {
                    game->player.state = DEAD;
                    game->player.health = 0;
                } else {
                    game->player.state = HURT;
                }
                soldier->damageCooldown = DAMAGE_COOLDOWN;
                soldier->frame = 0;
            }
        } else if (distance < FIRE_DISTANCE && soldier->bullets <= 0 && soldier->grenadeCooldown <= 0) {
            soldier->xVelocity = 0.0f;
            soldier->state = SOLDIER2_GRENADE;
            if (soldier->frame >= soldier->totalFrames - 1) {
                soldier->grenadeCooldown = GRENADE_COOLDOWN;
                soldier->smoke.position.x = game->player.world_x + game->player.position.w / 2 - 128;
                soldier->smoke.position.y = game->player.position.y + game->player.position.h / 2 - 128;
                soldier->smoke.active = 1;
                soldier->smoke.frame = 0;
                soldier->frame = 0;
            }
        } else if (soldier->bullets <= 0 && soldier->state != SOLDIER2_GRENADE) {
            soldier->xVelocity = 0.0f;
            soldier->state = SOLDIER2_RECHARGE;
            if (soldier->frame >= soldier->totalFrames - 1) {
                soldier->bullets = 10;
                soldier->state = SOLDIER2_IDLE;
                soldier->frame = 0;
            }
        } else if (distance < UPDATE_DISTANCE) {
            soldier->state = SOLDIER2_WALK;
            soldier->xVelocity = dx > 0 ? MOVE_SPEED : -MOVE_SPEED;
            soldier->world_x += (int)soldier->xVelocity;
        } else {
            soldier->state = SOLDIER2_WALK;
            soldier->xVelocity = soldier->patrolDirection * PATROL_SPEED;
            soldier->world_x += (int)soldier->xVelocity;
            if (--soldier->patrolTimer <= 0) {
                soldier->patrolDirection = -soldier->patrolDirection;
                soldier->patrolTimer = 60 + rand() % 120;
            }
            if (soldier->world_x < soldier->patrolLeft) {
                soldier->world_x = soldier->patrolLeft;
                soldier->patrolDirection = 1;
            } else if (soldier->world_x > soldier->patrolRight) {
                soldier->world_x = soldier->patrolRight;
                soldier->patrolDirection = -1;
            }
            soldier->facingLeft = soldier->patrolDirection < 0 ? 1 : 0;
        }
    } else if (soldier->state == SOLDIER2_HURT && soldier->invulnerabilityTimer <= 0) {
        soldier->state = SOLDIER2_IDLE;
        soldier->frame = 0;
    }

    // Handle grenade smoke damage
    if (soldier->smoke.active) {
        soldier->smoke.frame++;
        if (soldier->smoke.frame >= soldier->smoke.totalFrames) {
            soldier->smoke.active = 0;
            soldier->smoke.frame = 0;
        }
        if (soldier->smoke.frame % 4 == 0 && abs(soldier->smoke.position.x - game->player.world_x) < 200 &&
            abs(soldier->smoke.position.y - game->player.position.y) < 200) {
            *playerHealth -= 2;
            if (*playerHealth <= 0) {
                game->player.state = DEAD;
                game->player.health = 0;
            } else {
                game->player.state = HURT;
            }
        }
    }

    // Handle player attacks, matching soldier.c
    if (game->player.state == ATTACK_1 && soldier->invulnerabilityTimer <= 0) {
        SDL_Rect attackRect = {
            game->player.world_x + (game->player.facing == RIGHT ? 50 : -50),
            game->player.position.y, 100, 100
        };
        SDL_Rect soldierWorldRect = {soldier->world_x, soldier->position.y, soldier->position.w, soldier->position.h};
        if (rectIntersect(&attackRect, &soldierWorldRect)) {
            soldier->health -= 10;
            soldier->state = SOLDIER2_HURT;
            soldier->frame = 0;
            soldier->invulnerabilityTimer = INVULNERABILITY_TIME;
            LOG("Soldier2 hit by player attack: health=%d\n", soldier->health);
        }
    }
    if (game->player.bulletActive && soldier->invulnerabilityTimer <= 0) {
        SDL_Rect bulletRect = {game->player.bullet.x, game->player.bullet.y, game->player.bullet.w, game->player.bullet.h};
        SDL_Rect soldierWorldRect = {soldier->world_x, soldier->position.y, soldier->position.w, soldier->position.h};
        if (rectIntersect(&bulletRect, &soldierWorldRect)) {
            soldier->health -= 5;
            soldier->state = SOLDIER2_HURT;
            soldier->frame = 0;
            soldier->invulnerabilityTimer = INVULNERABILITY_TIME;
            game->player.bulletActive = 0;
            LOG("Soldier2 hit by player bullet: health=%d\n", soldier->health);
        }
    }

    // Update animation
    if (++soldier->frameDelay >= FRAME_DELAY) {
        soldier->frame++;
        if (soldier->state == SOLDIER2_IDLE || soldier->state == SOLDIER2_WALK) {
            soldier->frame %= soldier->totalFrames; // Loop for idle and walk
        } else if (soldier->state == SOLDIER2_DEAD) {
            if (soldier->frame >= soldier->totalFrames) {
                soldier->frame = soldier->totalFrames - 1; // Clamp to last frame
                soldier->active = 0; // Deactivate after death animation
                LOG("Soldier2 death animation complete: frame=%d, totalFrames=%d, active=%d\n",
                    soldier->frame, soldier->totalFrames, soldier->active);
            }
        } else if (soldier->frame >= soldier->totalFrames) {
            soldier->frame = 0; // Reset for non-looping animations
            if (soldier->state != SOLDIER2_HURT && soldier->state != SOLDIER2_DEAD) {
                soldier->state = SOLDIER2_IDLE;
            }
        }
        soldier->frameDelay = 0;
    }

    // Update screen position for scrolling
    soldier->position.x = soldier->world_x - game->background.scroll_x;
    LOG("Soldier2 updated: x=%d, y=%d, state=%d, onGround=%d, yVelocity=%.2f, frame=%d, totalFrames=%d, active=%d\n",
        soldier->position.x, soldier->position.y, soldier->state, soldier->onGround, soldier->yVelocity,
        soldier->frame, soldier->totalFrames, soldier->active);
}

void renderSoldier2(SDL_Surface *screen, Soldier2 *soldier, int scroll_x) {
    if (!screen || !soldier || !soldier->active) {
        LOG("renderSoldier2: Invalid parameters or soldier inactive\n");
        return;
    }

    // Select sprite sheet, matching soldier.c
    SDL_Surface *sheet = soldier->facingLeft ?
        (soldier->state == SOLDIER2_IDLE ? idleSheetFlipped :
         soldier->state == SOLDIER2_WALK ? walkSheetFlipped :
         soldier->state == SOLDIER2_ATTACK ? attackSheetFlipped :
         soldier->state == SOLDIER2_SHOT_2 ? shot2SheetFlipped :
         soldier->state == SOLDIER2_GRENADE ? grenadeSheetFlipped :
         soldier->state == SOLDIER2_RECHARGE ? rechargeSheetFlipped :
         soldier->state == SOLDIER2_HURT ? hurtSheetFlipped :
         soldier->state == SOLDIER2_DEAD ? deadSheetFlipped : idleSheetFlipped) :
        (soldier->state == SOLDIER2_IDLE ? idleSheet :
         soldier->state == SOLDIER2_WALK ? walkSheet :
         soldier->state == SOLDIER2_ATTACK ? attackSheet :
         soldier->state == SOLDIER2_SHOT_2 ? shot2Sheet :
         soldier->state == SOLDIER2_GRENADE ? grenadeSheet :
         soldier->state == SOLDIER2_RECHARGE ? rechargeSheet :
         soldier->state == SOLDIER2_HURT ? hurtSheet :
         soldier->state == SOLDIER2_DEAD ? deadSheet : idleSheet);

    if (!sheet) {
        sheet = soldier->facingLeft ? idleSheetFlipped : idleSheet;
        LOG("renderSoldier2: Fallback to idle sheet for state=%d\n", soldier->state);
    }

    // Ensure frame is within bounds for death animation
    SDL_Rect srcRect = {soldier->frame * 256, 0, 256, 256};
    if (srcRect.x >= sheet->w || soldier->frame >= soldier->totalFrames) {
        srcRect.x = (soldier->totalFrames - 1) * 256; // Clamp to last frame
        soldier->frame = soldier->totalFrames - 1;
        LOG("renderSoldier2: Frame clamped for state=%d, frame=%d, totalFrames=%d\n",
            soldier->state, soldier->frame, soldier->totalFrames);
    }
    SDL_Rect destRect = {soldier->position.x, soldier->position.y, 256, 256};
    SDL_BlitSurface(sheet, &srcRect, screen, &destRect);

    // Render health bar, matching soldier.c
    if (soldier->health > 0 && soldier->state != SOLDIER2_DEAD) {
        SDL_Rect healthBarBg = {soldier->position.x, soldier->position.y - 20, 50, 5};
        SDL_FillRect(screen, &healthBarBg, SDL_MapRGB(screen->format, 255, 0, 0));
        SDL_Rect healthBar = {soldier->position.x, soldier->position.y - 20, (50 * soldier->health) / soldier->maxHealth, 5};
        SDL_FillRect(screen, &healthBar, SDL_MapRGB(screen->format, 0, 255, 0));
    }

    // Render grenade smoke
    if (soldier->smoke.active) {
        SDL_Surface *smokeSheet = soldier->facingLeft ? grenadeSheetFlipped : grenadeSheet;
        SDL_Rect smokeSrcRect = {soldier->smoke.frame * 256, 0, 256, 256};
        if (smokeSrcRect.x < smokeSheet->w) {
            SDL_Rect smokeDestRect = {soldier->smoke.position.x - scroll_x, soldier->smoke.position.y, 256, 256};
            SDL_BlitSurface(smokeSheet, &smokeSrcRect, screen, &smokeDestRect);
        }
    }

    LOG("Soldier2 rendered: x=%d, y=%d, state=%d, frame=%d, totalFrames=%d, active=%d\n",
        soldier->position.x, soldier->position.y, soldier->state, soldier->frame, soldier->totalFrames, soldier->active);
}

void freeSoldier2(Soldier2 *soldier) {
    if (soldier) {
        soldier->active = 0;
        soldier->health = 0;
        soldier->state = SOLDIER2_DEAD;
        LOG("Soldier2 freed: x=%d, y=%d\n", soldier->position.x, soldier->position.y);
    }
}

void freeSoldier2Sprites() {
    LOG("Freeing Soldier2 sprites...\n");
    SDL_Surface *sheets[] = {
        idleSheet, idleSheetFlipped, walkSheet, walkSheetFlipped,
        attackSheet, attackSheetFlipped, shot2Sheet, shot2SheetFlipped,
        grenadeSheet, grenadeSheetFlipped, rechargeSheet, rechargeSheetFlipped,
        hurtSheet, hurtSheetFlipped, deadSheet, deadSheetFlipped
    };
    for (int i = 0; i < 16; i++) {
        if (sheets[i]) {
            SDL_FreeSurface(sheets[i]);
            sheets[i] = NULL;
        }
    }
    LOG("Soldier2 sprites freed\n");
}
