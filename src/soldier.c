#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include "soldier.h"
#include "game.h"
#include "collision.h"
#include "mouvement.h"
#include "utils.h"


#define DEBUG_LOG 1
#define LOG(fmt, ...) if (DEBUG_LOG) fprintf(stderr, fmt, ##__VA_ARGS__)


static SDL_Surface *idleSheet = NULL;
static SDL_Surface *idleSheetFlipped = NULL;
static SDL_Surface *walkSheet = NULL;
static SDL_Surface *walkSheetFlipped = NULL;
static SDL_Surface *attackSheet = NULL;
static SDL_Surface *attackSheetFlipped = NULL;
static SDL_Surface *shotSheet = NULL;
static SDL_Surface *shotSheetFlipped = NULL;
static SDL_Surface *shot2Sheet = NULL;
static SDL_Surface *shot2SheetFlipped = NULL;
static SDL_Surface *grenadeSheet = NULL;
static SDL_Surface *grenadeSheetFlipped = NULL;
static SDL_Surface *rechargeSheet = NULL;
static SDL_Surface *rechargeSheetFlipped = NULL;
static SDL_Surface *throwSheet = NULL;
static SDL_Surface *throwSheetFlipped = NULL;
static SDL_Surface *hurtSheet = NULL;
static SDL_Surface *hurtSheetFlipped = NULL;
static SDL_Surface *deadSheet = NULL;
static SDL_Surface *deadSheetFlipped = NULL;
static SDL_Surface *explosionSheet = NULL;
static SDL_Surface *explosionSheetFlipped = NULL;

static SoldierSprites soldierSprites;


#define FRAME_DELAY 12 
#define MOVE_SPEED 3.0f 
#define PATROL_SPEED 1.5f 

#define MAX_FALL_SPEED 10.0f
#define JUMP_HEIGHT 8.0f
#define MELEE_DISTANCE 100 
#define FIRE_DISTANCE 500
#define ACTIVATION_DISTANCE 1500
#define DAMAGE_COOLDOWN 90 
#define GRENADE_COOLDOWN 360 
#define INVULNERABILITY_TIME 45 
#define STATE_TRANSITION_DELAY 45 

void initSoldierSprites() {
    
}

void freeSoldierSprites() {
    SDL_Surface *sheets[] = {
        idleSheet, idleSheetFlipped, walkSheet, walkSheetFlipped,
        attackSheet, attackSheetFlipped, shotSheet, shotSheetFlipped,
        shot2Sheet, shot2SheetFlipped, grenadeSheet, grenadeSheetFlipped,
        rechargeSheet, rechargeSheetFlipped, throwSheet, throwSheetFlipped,
        hurtSheet, hurtSheetFlipped, deadSheet, deadSheetFlipped,
        explosionSheet, explosionSheetFlipped
    };
    for (int i = 0; i < 22; i++) {
        if (sheets[i]) {
            SDL_FreeSurface(sheets[i]);
            sheets[i] = NULL;
        }
    }
    LOG("Soldier sprites freed\n");
}

void initSoldier(Soldier *soldier, int x, int y, struct GAME *game) {
    if (!soldier || !game) {
        fprintf(stderr, "initSoldier: Soldier or game pointer is NULL\n");
        exit(1);
    }
    if (x < 0 || x > game->background.width - 256) {
        fprintf(stderr, "initSoldier: Invalid x coordinate %d\n", x);
        soldier->active = 0;
        return;
    }
    LOG("Starting initSoldier for soldier at x=%d, y=%d\n", x, y);

    
    if (!idleSheet) {
        idleSheet = IMG_Load("assets/enemies/Soldier/Soldier_1/Idle.png");
        if (!idleSheet) {
            fprintf(stderr, "Failed to load Soldier_1/Idle.png: %s\n", IMG_GetError());
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
        SDL_SetColorKey(idleSheet, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
        idleSheetFlipped = flipHorizontally(idleSheet);
        if (!idleSheetFlipped) {
            fprintf(stderr, "Failed to flip Idle.png\n");
            SDL_FreeSurface(idleSheet);
            exit(1);
        }
        SDL_SetColorKey(idleSheetFlipped, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
        soldierSprites.frameCounts[SOLDIER_IDLE] = idleSheet->w / 256;
    }
    if (!walkSheet) {
        walkSheet = IMG_Load("assets/enemies/Soldier/Soldier_1/Walk.png");
        if (!walkSheet) {
            fprintf(stderr, "Failed to load Soldier_1/Walk.png: %s\n", IMG_GetError());
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
        SDL_SetColorKey(walkSheet, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
        walkSheetFlipped = flipHorizontally(walkSheet);
        if (!walkSheetFlipped) {
            fprintf(stderr, "Failed to flip Walk.png\n");
            SDL_FreeSurface(walkSheet);
            exit(1);
        }
        SDL_SetColorKey(walkSheetFlipped, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
        soldierSprites.frameCounts[SOLDIER_WALK] = walkSheet->w / 256;
    }
    if (!attackSheet) {
        attackSheet = IMG_Load("assets/enemies/Soldier/Soldier_1/Attack.png");
        if (!attackSheet) {
            fprintf(stderr, "Failed to load Soldier_1/Attack.png: %s\n", IMG_GetError());
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
        SDL_SetColorKey(attackSheet, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
        attackSheetFlipped = flipHorizontally(attackSheet);
        if (!attackSheetFlipped) {
            fprintf(stderr, "Failed to flip Attack.png\n");
            SDL_FreeSurface(attackSheet);
            exit(1);
        }
        SDL_SetColorKey(attackSheetFlipped, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
        soldierSprites.frameCounts[SOLDIER_ATTACK] = attackSheet->w / 256;
    }
    if (!shot2Sheet) {
        shot2Sheet = IMG_Load("assets/enemies/Soldier/Soldier_1/Shot_2.png");
        if (!shot2Sheet) {
            fprintf(stderr, "Failed to load Soldier_1/Shot_2.png: %s\n", IMG_GetError());
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
        SDL_SetColorKey(shot2Sheet, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
        shot2SheetFlipped = flipHorizontally(shot2Sheet);
        if (!shot2SheetFlipped) {
            fprintf(stderr, "Failed to flip Shot_2.png\n");
            SDL_FreeSurface(shot2Sheet);
            exit(1);
        }
        SDL_SetColorKey(shot2SheetFlipped, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
        soldierSprites.frameCounts[SOLDIER_SHOT_2] = shot2Sheet->w / 256;
    }
    if (!grenadeSheet) {
        grenadeSheet = IMG_Load("assets/enemies/Soldier/Soldier_1/Grenade.png");
        if (!grenadeSheet) {
            fprintf(stderr, "Failed to load Soldier_1/Grenade.png: %s\n", IMG_GetError());
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
        SDL_SetColorKey(grenadeSheet, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
        grenadeSheetFlipped = flipHorizontally(grenadeSheet);
        if (!grenadeSheetFlipped) {
            fprintf(stderr, "Failed to flip Grenade.png\n");
            SDL_FreeSurface(grenadeSheet);
            exit(1);
        }
        SDL_SetColorKey(grenadeSheetFlipped, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
        soldierSprites.frameCounts[SOLDIER_GRENADE] = grenadeSheet->w / 256;
    }
    if (!rechargeSheet) {
        rechargeSheet = IMG_Load("assets/enemies/Soldier/Soldier_1/Recharge.png");
        if (!rechargeSheet) {
            fprintf(stderr, "Failed to load Soldier_1/Recharge.png: %s\n", IMG_GetError());
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
        SDL_SetColorKey(rechargeSheet, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
        rechargeSheetFlipped = flipHorizontally(rechargeSheet);
        if (!rechargeSheetFlipped) {
            fprintf(stderr, "Failed to flip Recharge.png\n");
            SDL_FreeSurface(rechargeSheet);
            exit(1);
        }
        SDL_SetColorKey(rechargeSheetFlipped, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
        soldierSprites.frameCounts[SOLDIER_RECHARGE] = rechargeSheet->w / 256;
    }
    if (!hurtSheet) {
        hurtSheet = IMG_Load("assets/enemies/Soldier/Soldier_1/Hurt.png");
        if (!hurtSheet) {
            fprintf(stderr, "Failed to load Soldier_1/Hurt.png: %s\n", IMG_GetError());
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
        SDL_SetColorKey(hurtSheet, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
        hurtSheetFlipped = flipHorizontally(hurtSheet);
        if (!hurtSheetFlipped) {
            fprintf(stderr, "Failed to flip Hurt.png\n");
            SDL_FreeSurface(hurtSheet);
            exit(1);
        }
        SDL_SetColorKey(hurtSheetFlipped, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
        soldierSprites.frameCounts[SOLDIER_HURT] = hurtSheet->w / 256;
    }
    if (!deadSheet) {
        deadSheet = IMG_Load("assets/enemies/Soldier/Soldier_1/Dead.png");
        if (!deadSheet) {
            fprintf(stderr, "Failed to load Soldier_1/Dead.png: %s\n", IMG_GetError());
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
        SDL_SetColorKey(deadSheet, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
        deadSheetFlipped = flipHorizontally(deadSheet);
        if (!deadSheetFlipped) {
            fprintf(stderr, "Failed to flip Dead.png\n");
            SDL_FreeSurface(deadSheet);
            exit(1);
        }
        SDL_SetColorKey(deadSheetFlipped, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
        soldierSprites.frameCounts[SOLDIER_DEAD] = deadSheet->w / 256;
    }
    if (!explosionSheet) {
        explosionSheet = IMG_Load("assets/enemies/Soldier/Soldier_1/Explosion.png");
        if (!explosionSheet) {
            fprintf(stderr, "Failed to load Soldier_1/Explosion.png: %s\n", IMG_GetError());
            exit(1);
        }
        Uint32 colorkey = SDL_MapRGB(explosionSheet->format, 255, 255, 255);
        SDL_SetColorKey(explosionSheet, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
        SDL_Surface *optimized = SDL_DisplayFormatAlpha(explosionSheet);
        if (!optimized) {
            fprintf(stderr, "SDL_DisplayFormatAlpha failed for Explosion.png: %s\n", SDL_GetError());
            SDL_FreeSurface(explosionSheet);
            exit(1);
        }
        SDL_FreeSurface(explosionSheet);
        explosionSheet = optimized;
        SDL_SetColorKey(explosionSheet, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
        explosionSheetFlipped = flipHorizontally(explosionSheet);
        if (!explosionSheetFlipped) {
            fprintf(stderr, "Failed to flip Explosion.png\n");
            SDL_FreeSurface(explosionSheet);
            exit(1);
        }
        SDL_SetColorKey(explosionSheetFlipped, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
    }

    
    soldier->position.x = x;
    soldier->position.y = y;
    soldier->position.w = 256;
    soldier->position.h = 256;
    soldier->world_x = x;
    soldier->xVelocity = 0.0f;
    soldier->yVelocity = 0.0f;
    soldier->onGround = 0;
    soldier->state = SOLDIER_IDLE;
    soldier->frame = 0;
    soldier->totalFrames = soldierSprites.frameCounts[SOLDIER_IDLE];
    soldier->direction = 1;
    soldier->facingLeft = 0;
    soldier->health = 15;
    soldier->maxHealth = 15;
    soldier->active = 1;
    soldier->patrolLeft = x - 200;
    soldier->patrolRight = x + 200;
    soldier->lastAttackTime = 0;
    soldier->lastShotTime = 0;
    soldier->lastThrowTime = 0;
    soldier->attackCooldown = 0;
    soldier->shootCooldown = 0;
    soldier->throwCooldown = 0;
    soldier->stateTransitionDelay = 0;
    soldier->bullets = 5;
    soldier->grenadeCooldown = 0;
    soldier->grenadeExplosionDelay = 0;
    soldier->damageCooldown = 0;
    soldier->invulnerabilityTimer = 0;
    soldier->frameDelay = 0;
    soldier->patrolDirection = 1;
    soldier->patrolTimer = 60 + rand() % 120;
    soldier->explosion.position.w = 256;
    soldier->explosion.position.h = 256;
    soldier->explosion.frame = 0;
    soldier->explosion.totalFrames = explosionSheet->w / 256;
    soldier->explosion.active = 0;

    
    soldier->onGround = onGroundSoldier(game, soldier);
    if (!soldier->onGround) {
        soldier->yVelocity = 0.0f;
        placeSoldierOnGround(game, soldier);
    }
    LOG("Soldier initialized at x=%d, y=%d, onGround=%d\n", soldier->position.x, soldier->position.y, soldier->onGround);
}

void updateSoldier(Soldier *soldier, SDL_Rect playerPos, int *playerHealth, int playerMaxHealth, struct GAME *game) {
    if (!soldier || !soldier->active || !game || !playerHealth) {
        LOG("updateSoldier: Invalid parameters\n");
        return;
    }

    
    soldier->onGround = onGroundSoldier(game, soldier);
    if (!soldier->onGround) {
        soldier->yVelocity += GRAVITY;
        if (soldier->yVelocity > MAX_FALL_SPEED) {
            soldier->yVelocity = MAX_FALL_SPEED;
        }
    } else {
        soldier->yVelocity = 0.0f;
    }
    soldier->position.y += (int)soldier->yVelocity;
    if (soldier->onGround && soldier->yVelocity >= 0) {
        placeSoldierOnGround(game, soldier);
    }

    
    if (soldier->position.y < 0) {
        soldier->position.y = 0;
        soldier->yVelocity = 0.0f;
    }
    if (soldier->position.y > game->background.height - soldier->position.h) {
        soldier->position.y = game->background.height - soldier->position.h;
        soldier->yVelocity = 0.0f;
        soldier->onGround = 1;
    }
    if (soldier->world_x < 0) {
        soldier->world_x = 0;
    }
    if (soldier->world_x > game->background.width - soldier->position.w) {
        soldier->world_x = game->background.width - soldier->position.w;
    }

   
    switch (soldier->state) {
        case SOLDIER_IDLE: soldier->totalFrames = idleSheet->w / 256; break;
        case SOLDIER_WALK: soldier->totalFrames = walkSheet->w / 256; break;
        case SOLDIER_ATTACK: soldier->totalFrames = attackSheet->w / 256; break;
        case SOLDIER_SHOOT: soldier->totalFrames = shotSheet ? shotSheet->w / 256 : shot2Sheet->w / 256; break;
        case SOLDIER_SHOT_2: soldier->totalFrames = shot2Sheet->w / 256; break;
        case SOLDIER_GRENADE: soldier->totalFrames = grenadeSheet->w / 256; break;
        case SOLDIER_RECHARGE: soldier->totalFrames = rechargeSheet ? rechargeSheet->w / 256 : idleSheet->w / 256; break;
        case SOLDIER_THROW: soldier->totalFrames = throwSheet ? throwSheet->w / 256 : grenadeSheet->w / 256; break;
        case SOLDIER_HURT: soldier->totalFrames = hurtSheet->w / 256; break;
        case SOLDIER_DEAD: soldier->totalFrames = deadSheet->w / 256; break;
    }

    if (soldier->state != SOLDIER_DEAD) {
     
        if (soldier->damageCooldown > 0) soldier->damageCooldown--;
        if (soldier->invulnerabilityTimer > 0) soldier->invulnerabilityTimer--;
        if (soldier->grenadeCooldown > 0) soldier->grenadeCooldown--;
        if (soldier->stateTransitionDelay > 0) soldier->stateTransitionDelay--;

     
        int dx = game->player.world_x - soldier->world_x;
        int distance = abs(dx);
        soldier->facingLeft = dx < 0 ? 1 : 0;

        if (soldier->state == SOLDIER_HURT && soldier->invulnerabilityTimer <= 0) {
            soldier->state = SOLDIER_IDLE;
            soldier->frame = 0;
            soldier->stateTransitionDelay = STATE_TRANSITION_DELAY;
        }

        if (soldier->stateTransitionDelay <= 0) {
            if (distance < MELEE_DISTANCE) {
                soldier->xVelocity = 0.0f;
                soldier->state = SOLDIER_ATTACK;
                if (soldier->frame >= soldier->totalFrames - 1 && soldier->damageCooldown <= 0) {
                    *playerHealth -= 5;
                    soldier->damageCooldown = DAMAGE_COOLDOWN;
                    soldier->stateTransitionDelay = STATE_TRANSITION_DELAY;
                    soldier->frame = 0;
                    if (*playerHealth <= 0) {
                        game->player.state = DEAD;
                        game->player.health = 0;
                    } else {
                        game->player.state = HURT;
                    }
                }
            } else if (distance < FIRE_DISTANCE && soldier->bullets > 0) {
                soldier->xVelocity = 0.0f;
                soldier->state = SOLDIER_SHOT_2; 
                if (soldier->frame >= soldier->totalFrames - 1 && soldier->damageCooldown <= 0) {
                    soldier->bullets--;
                    *playerHealth -= 10;
                    soldier->damageCooldown = DAMAGE_COOLDOWN;
                    soldier->stateTransitionDelay = STATE_TRANSITION_DELAY;
                    soldier->frame = 0;
                    if (*playerHealth <= 0) {
                        game->player.state = DEAD;
                        game->player.health = 0;
                    } else {
                        game->player.state = HURT;
                    }
                }
            } else if (distance < FIRE_DISTANCE && soldier->bullets <= 0 && soldier->grenadeCooldown <= 0) {
                soldier->xVelocity = 0.0f;
                soldier->state = SOLDIER_GRENADE;
                if (soldier->frame >= soldier->totalFrames - 1) {
                    soldier->grenadeCooldown = GRENADE_COOLDOWN;
                    soldier->grenadeExplosionDelay = 60; 
                    soldier->explosion.position.x = game->player.world_x + game->player.position.w / 2 - 128;
                    soldier->explosion.position.y = game->player.position.y + game->player.position.h / 2 - 128;
                    soldier->explosion.active = 1;
                    soldier->explosion.frame = 0;
                    soldier->stateTransitionDelay = STATE_TRANSITION_DELAY;
                    soldier->frame = 0;
                    soldier->state = SOLDIER_IDLE;
                }
            } else if (distance < ACTIVATION_DISTANCE) {
                soldier->state = SOLDIER_WALK;
                soldier->xVelocity = dx > 0 ? MOVE_SPEED : -MOVE_SPEED;
                soldier->world_x += (int)soldier->xVelocity;
                soldier->position.x = soldier->world_x;
            } else {
                soldier->state = SOLDIER_WALK;
                soldier->xVelocity = soldier->patrolDirection * PATROL_SPEED;
                soldier->world_x += (int)soldier->xVelocity;
                soldier->position.x = soldier->world_x;
                soldier->facingLeft = soldier->patrolDirection < 0 ? 1 : 0;
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
            }

            if (soldier->bullets <= 0 && soldier->state != SOLDIER_GRENADE && soldier->state != SOLDIER_HURT) {
                soldier->state = SOLDIER_RECHARGE;
                if (soldier->frame >= soldier->totalFrames - 1) {
                    soldier->bullets = 10;
                    soldier->state = SOLDIER_IDLE;
                    soldier->frame = 0;
                    soldier->stateTransitionDelay = STATE_TRANSITION_DELAY;
                }
            }
        }

        if (soldier->grenadeExplosionDelay > 0) {
            soldier->grenadeExplosionDelay--;
            if (soldier->grenadeExplosionDelay <= 0 && soldier->explosion.active) {
                if (abs(soldier->explosion.position.x - game->player.world_x) < 200 &&
                    abs(soldier->explosion.position.y - game->player.position.y) < 200) {
                    *playerHealth -= 20;
                    if (*playerHealth <= 0) {
                        game->player.state = DEAD;
                        game->player.health = 0;
                    } else {
                        game->player.state = HURT;
                    }
                }
            }
        }

        if (soldier->explosion.active) {
            soldier->explosion.frame++;
            if (soldier->explosion.frame >= soldier->explosion.totalFrames) {
                soldier->explosion.active = 0;
                soldier->explosion.frame = 0;
            }
        }

        if (game->player.state == ATTACK_1 && soldier->invulnerabilityTimer <= 0) {
            SDL_Rect attackRect = {
                game->player.world_x + (game->player.facing == RIGHT ? 50 : -50),
                game->player.position.y, 100, 100
            };
            SDL_Rect soldierRect = {soldier->world_x, soldier->position.y, soldier->position.w, soldier->position.h};
            if (rectIntersect(&soldierRect, &attackRect)) {
                soldier->health -= 10;
                soldier->state = SOLDIER_HURT;
                soldier->frame = 0;
                soldier->invulnerabilityTimer = INVULNERABILITY_TIME;
                soldier->stateTransitionDelay = STATE_TRANSITION_DELAY;
            }
        }

        if (game->player.bulletActive && soldier->invulnerabilityTimer <= 0) {
            SDL_Rect bulletRect = {game->player.bullet.x, game->player.bullet.y, game->player.bullet_w, game->player.bullet_h};
            SDL_Rect soldierRect = {soldier->world_x, soldier->position.y, soldier->position.w, soldier->position.h};
            if (rectIntersect(&soldierRect, &bulletRect)) {
                soldier->health -= 5;
                soldier->state = SOLDIER_HURT;
                soldier->frame = 0;
                soldier->invulnerabilityTimer = INVULNERABILITY_TIME;
                soldier->stateTransitionDelay = STATE_TRANSITION_DELAY;
                game->player.bulletActive = 0;
            }
        }
    }

    if (soldier->health <= 0 && soldier->state != SOLDIER_DEAD) {
        soldier->state = SOLDIER_DEAD;
        soldier->frame = 0;
        soldier->stateTransitionDelay = STATE_TRANSITION_DELAY;
        game->player.score += 50;
    }

    if (++soldier->frameDelay >= FRAME_DELAY) {
        soldier->frame++;
        if (soldier->state == SOLDIER_IDLE || soldier->state == SOLDIER_WALK) {
            soldier->frame %= soldier->totalFrames;
        } else if (soldier->frame >= soldier->totalFrames) {
            soldier->frame = 0;
            if (soldier->state == SOLDIER_DEAD) {
                soldier->active = 0;
            }
        }
        soldier->frameDelay = 0;
    }

  
    soldier->position.x = soldier->world_x - game->background.scroll_x;
    LOG("Soldier updated: x=%d, y=%d, state=%d, onGround=%d, yVelocity=%.2f\n",
        soldier->position.x, soldier->position.y, soldier->state, soldier->onGround, soldier->yVelocity);
}

void renderSoldier(SDL_Surface *screen, Soldier *soldier, int scroll_x) {
    if (!screen || !soldier || !soldier->active) {
        LOG("renderSoldier: Invalid parameters or soldier inactive\n");
        return;
    }

    SDL_Surface *sheet = soldier->facingLeft ?
        (soldier->state == SOLDIER_IDLE ? idleSheetFlipped :
         soldier->state == SOLDIER_WALK ? walkSheetFlipped :
         soldier->state == SOLDIER_ATTACK ? attackSheetFlipped :
         soldier->state == SOLDIER_SHOOT ? shotSheetFlipped :
         soldier->state == SOLDIER_SHOT_2 ? shot2SheetFlipped :
         soldier->state == SOLDIER_GRENADE ? grenadeSheetFlipped :
         soldier->state == SOLDIER_RECHARGE ? rechargeSheetFlipped :
         soldier->state == SOLDIER_THROW ? throwSheetFlipped :
         soldier->state == SOLDIER_HURT ? hurtSheetFlipped :
         deadSheetFlipped) :
        (soldier->state == SOLDIER_IDLE ? idleSheet :
         soldier->state == SOLDIER_WALK ? walkSheet :
         soldier->state == SOLDIER_ATTACK ? attackSheet :
         soldier->state == SOLDIER_SHOOT ? shotSheet :
         soldier->state == SOLDIER_SHOT_2 ? shot2Sheet :
         soldier->state == SOLDIER_GRENADE ? grenadeSheet :
         soldier->state == SOLDIER_RECHARGE ? rechargeSheet :
         soldier->state == SOLDIER_THROW ? throwSheet :
         soldier->state == SOLDIER_HURT ? hurtSheet :
         deadSheet);

    if (!sheet) {
        sheet = soldier->facingLeft ? idleSheetFlipped : idleSheet;
        LOG("renderSoldier: Fallback to idle sheet for state=%d\n", soldier->state);
    }

    SDL_Rect srcRect = {soldier->frame * 256, 0, 256, 256};
    if (srcRect.x >= sheet->w) {
        srcRect.x = 0;
        soldier->frame = 0;
        LOG("renderSoldier: Frame reset for state=%d, frame=%d\n", soldier->state, soldier->frame);
    }
    SDL_Rect destRect = {soldier->position.x, soldier->position.y, 256, 256};
    SDL_BlitSurface(sheet, &srcRect, screen, &destRect);

    if (soldier->health > 0 && soldier->state != SOLDIER_DEAD) {
        SDL_Rect healthBarBg = {soldier->position.x, soldier->position.y - 20, 50, 5};
        SDL_FillRect(screen, &healthBarBg, SDL_MapRGB(screen->format, 255, 0, 0));
        SDL_Rect healthBar = {soldier->position.x, soldier->position.y - 20, (50 * soldier->health) / soldier->maxHealth, 5};
        SDL_FillRect(screen, &healthBar, SDL_MapRGB(screen->format, 0, 255, 0));
    }

    if (soldier->explosion.active) {
        SDL_Surface *expSheet = soldier->facingLeft ? explosionSheetFlipped : explosionSheet;
        SDL_Rect expSrcRect = {soldier->explosion.frame * 256, 0, 256, 256};
        if (expSrcRect.x < expSheet->w) {
            SDL_Rect expDestRect = {soldier->explosion.position.x - scroll_x, soldier->explosion.position.y, 256, 256};
            SDL_BlitSurface(expSheet, &expSrcRect, screen, &expDestRect);
        }
    }

    LOG("Soldier rendered: x=%d, y=%d, state=%d, frame=%d\n",
        soldier->position.x, soldier->position.y, soldier->state, soldier->frame);
}

void freeSoldier(Soldier *soldier) {
    if (soldier) {
        soldier->active = 0;
        soldier->health = 0;
        soldier->state = SOLDIER_DEAD;
        LOG("Soldier freed: x=%d, y=%d\n", soldier->position.x, soldier->position.y);
    }
}
