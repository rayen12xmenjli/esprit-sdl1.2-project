#include "game.h"
#include "collision.h"
#include "mouvement.h"
#include "utils.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

// Debug logging toggle
#define DEBUG_LOG 1
#define LOG(fmt, ...) if (DEBUG_LOG) fprintf(stderr, fmt, ##__VA_ARGS__)

// Constants aligned with soldier2.c
#define MOVE_SPEED 4.0f
#define PATROL_SPEED 2.0f
#define MAX_FALL_SPEED 10.0f
#define FRAME_DELAY 8
#define DAMAGE_COOLDOWN 60
#define SPECIAL_COOLDOWN 300
#define INVULNERABILITY_TIME 30

// Global sprite sheets for robot animations
SDL_Surface* robot_idleSheet = NULL;
SDL_Surface* robot_idleSheetFlipped = NULL;
SDL_Surface* robot_walkSheet = NULL;
SDL_Surface* robot_walkSheetFlipped = NULL;
SDL_Surface* robot_attack1Sheet = NULL;
SDL_Surface* robot_attack1SheetFlipped = NULL;
SDL_Surface* robot_attack3Sheet = NULL;
SDL_Surface* robot_attack3SheetFlipped = NULL;
SDL_Surface* robot_specialSheet = NULL;
SDL_Surface* robot_specialSheetFlipped = NULL;
SDL_Surface* robot_hurtSheet = NULL;
SDL_Surface* robot_hurtSheetFlipped = NULL;
SDL_Surface* robot_deathSheet = NULL;
SDL_Surface* robot_deathSheetFlipped = NULL;
SDL_Surface* robot_projectileSheet = NULL;
SDL_Surface* robot_projectileSheetFlipped = NULL;

void initRobot(Robot* robot, int x, int y) {
    if (!robot) {
        fprintf(stderr, "initRobot: Robot pointer is NULL\n");
        exit(1);
    }
    LOG("Initializing robot at x=%d, y=%d\n", x, y);

    // Load sprite sheets only once
    if (!robot_idleSheet) {
        robot_idleSheet = IMG_Load("assets/enemies/robot/idle_resized.png"); // 1024x256, 4 frames
        if (!robot_idleSheet) {
            fprintf(stderr, "Failed to load idle_resized.png: %s\n", IMG_GetError());
            exit(1);
        }
        Uint32 colorkey = SDL_MapRGB(robot_idleSheet->format, 255, 255, 255);
        SDL_SetColorKey(robot_idleSheet, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
        SDL_Surface* optimized = SDL_DisplayFormatAlpha(robot_idleSheet);
        if (!optimized) {
            fprintf(stderr, "SDL_DisplayFormatAlpha failed for idle_resized.png: %s\n", SDL_GetError());
            SDL_FreeSurface(robot_idleSheet);
            exit(1);
        }
        SDL_FreeSurface(robot_idleSheet);
        robot_idleSheet = optimized;
        robot_idleSheetFlipped = flipHorizontally(robot_idleSheet);
        if (!robot_idleSheetFlipped) {
            fprintf(stderr, "Failed to flip idle_resized.png\n");
            SDL_FreeSurface(robot_idleSheet);
            robot_idleSheet = NULL;
            exit(1);
        }
        SDL_SetColorKey(robot_idleSheetFlipped, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
    }
    if (!robot_walkSheet) {
        robot_walkSheet = IMG_Load("assets/enemies/robot/walk_resized.png"); // 1536x256, 6 frames
        if (!robot_walkSheet) {
            fprintf(stderr, "Failed to load walk_resized.png: %s\n", IMG_GetError());
            exit(1);
        }
        Uint32 colorkey = SDL_MapRGB(robot_walkSheet->format, 255, 255, 255);
        SDL_SetColorKey(robot_walkSheet, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
        SDL_Surface* optimized = SDL_DisplayFormatAlpha(robot_walkSheet);
        if (!optimized) {
            fprintf(stderr, "SDL_DisplayFormatAlpha failed for walk_resized.png: %s\n", SDL_GetError());
            SDL_FreeSurface(robot_walkSheet);
            exit(1);
        }
        SDL_FreeSurface(robot_walkSheet);
        robot_walkSheet = optimized;
        robot_walkSheetFlipped = flipHorizontally(robot_walkSheet);
        if (!robot_walkSheetFlipped) {
            fprintf(stderr, "Failed to flip walk_resized.png\n");
            SDL_FreeSurface(robot_walkSheet);
            robot_walkSheet = NULL;
            exit(1);
        }
        SDL_SetColorKey(robot_walkSheetFlipped, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
    }
    if (!robot_attack1Sheet) {
        robot_attack1Sheet = IMG_Load("assets/enemies/robot/attack1_resized.png"); // 1536x256, 6 frames
        if (!robot_attack1Sheet) {
            fprintf(stderr, "Failed to load attack1_resized.png: %s\n", IMG_GetError());
            exit(1);
        }
        Uint32 colorkey = SDL_MapRGB(robot_attack1Sheet->format, 255, 255, 255);
        SDL_SetColorKey(robot_attack1Sheet, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
        SDL_Surface* optimized = SDL_DisplayFormatAlpha(robot_attack1Sheet);
        if (!optimized) {
            fprintf(stderr, "SDL_DisplayFormatAlpha failed for attack1_resized.png: %s\n", SDL_GetError());
            SDL_FreeSurface(robot_attack1Sheet);
            exit(1);
        }
        SDL_FreeSurface(robot_attack1Sheet);
        robot_attack1Sheet = optimized;
        robot_attack1SheetFlipped = flipHorizontally(robot_attack1Sheet);
        if (!robot_attack1SheetFlipped) {
            fprintf(stderr, "Failed to flip attack1_resized.png\n");
            SDL_FreeSurface(robot_attack1Sheet);
            robot_attack1Sheet = NULL;
            exit(1);
        }
        SDL_SetColorKey(robot_attack1SheetFlipped, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
    }
    if (!robot_attack3Sheet) {
        robot_attack3Sheet = IMG_Load("assets/enemies/robot/attack3_resized.png"); // 1536x256, 6 frames
        if (!robot_attack3Sheet) {
            fprintf(stderr, "Failed to load attack3_resized.png: %s\n", IMG_GetError());
            exit(1);
        }
        Uint32 colorkey = SDL_MapRGB(robot_attack3Sheet->format, 255, 255, 255);
        SDL_SetColorKey(robot_attack3Sheet, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
        SDL_Surface* optimized = SDL_DisplayFormatAlpha(robot_attack3Sheet);
        if (!optimized) {
            fprintf(stderr, "SDL_DisplayFormatAlpha failed for attack3_resized.png: %s\n", SDL_GetError());
            SDL_FreeSurface(robot_attack3Sheet);
            exit(1);
        }
        SDL_FreeSurface(robot_attack3Sheet);
        robot_attack3Sheet = optimized;
        robot_attack3SheetFlipped = flipHorizontally(robot_attack3Sheet);
        if (!robot_attack3SheetFlipped) {
            fprintf(stderr, "Failed to flip attack3_resized.png\n");
            SDL_FreeSurface(robot_attack3Sheet);
            robot_attack3Sheet = NULL;
            exit(1);
        }
        SDL_SetColorKey(robot_attack3SheetFlipped, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
    }
    if (!robot_specialSheet) {
        robot_specialSheet = IMG_Load("assets/enemies/robot/special_resized.png"); // 1536x256, 6 frames
        if (!robot_specialSheet) {
            fprintf(stderr, "Failed to load special_resized.png: %s\n", IMG_GetError());
            exit(1);
        }
        Uint32 colorkey = SDL_MapRGB(robot_specialSheet->format, 255, 255, 255);
        SDL_SetColorKey(robot_specialSheet, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
        SDL_Surface* optimized = SDL_DisplayFormatAlpha(robot_specialSheet);
        if (!optimized) {
            fprintf(stderr, "SDL_DisplayFormatAlpha failed for special_resized.png: %s\n", SDL_GetError());
            SDL_FreeSurface(robot_specialSheet);
            exit(1);
        }
        SDL_FreeSurface(robot_specialSheet);
        robot_specialSheet = optimized;
        robot_specialSheetFlipped = flipHorizontally(robot_specialSheet);
        if (!robot_specialSheetFlipped) {
            fprintf(stderr, "Failed to flip special_resized.png\n");
            SDL_FreeSurface(robot_specialSheet);
            robot_specialSheet = NULL;
            exit(1);
        }
        SDL_SetColorKey(robot_specialSheetFlipped, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
    }
    if (!robot_hurtSheet) {
        robot_hurtSheet = IMG_Load("assets/enemies/robot/hurt_resized.png"); // 512x256, 2 frames
        if (!robot_hurtSheet) {
            fprintf(stderr, "Failed to load hurt_resized.png: %s\n", IMG_GetError());
            exit(1);
        }
        Uint32 colorkey = SDL_MapRGB(robot_hurtSheet->format, 255, 255, 255);
        SDL_SetColorKey(robot_hurtSheet, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
        SDL_Surface* optimized = SDL_DisplayFormatAlpha(robot_hurtSheet);
        if (!optimized) {
            fprintf(stderr, "SDL_DisplayFormatAlpha failed for hurt_resized.png: %s\n", SDL_GetError());
            SDL_FreeSurface(robot_hurtSheet);
            exit(1);
        }
        SDL_FreeSurface(robot_hurtSheet);
        robot_hurtSheet = optimized;
        robot_hurtSheetFlipped = flipHorizontally(robot_hurtSheet);
        if (!robot_hurtSheetFlipped) {
            fprintf(stderr, "Failed to flip hurt_resized.png\n");
            SDL_FreeSurface(robot_hurtSheet);
            robot_hurtSheet = NULL;
            exit(1);
        }
        SDL_SetColorKey(robot_hurtSheetFlipped, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
    }
    if (!robot_deathSheet) {
        robot_deathSheet = IMG_Load("assets/enemies/robot/death_resized.png"); // 1536x256, 6 frames
        if (!robot_deathSheet) {
            fprintf(stderr, "Failed to load death_resized.png: %s\n", IMG_GetError());
            exit(1);
        }
        Uint32 colorkey = SDL_MapRGB(robot_deathSheet->format, 255, 255, 255);
        SDL_SetColorKey(robot_deathSheet, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
        SDL_Surface* optimized = SDL_DisplayFormatAlpha(robot_deathSheet);
        if (!optimized) {
            fprintf(stderr, "SDL_DisplayFormatAlpha failed for death_resized.png: %s\n", SDL_GetError());
            SDL_FreeSurface(robot_deathSheet);
            exit(1);
        }
        SDL_FreeSurface(robot_deathSheet);
        robot_deathSheet = optimized;
        robot_deathSheetFlipped = flipHorizontally(robot_deathSheet);
        if (!robot_deathSheetFlipped) {
            fprintf(stderr, "Failed to flip death_resized.png\n");
            SDL_FreeSurface(robot_deathSheet);
            robot_deathSheet = NULL;
            exit(1);
        }
        SDL_SetColorKey(robot_deathSheetFlipped, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
    }
    if (!robot_projectileSheet) {
        robot_projectileSheet = IMG_Load("assets/enemies/robot/projectile_resized.png"); // 256x256, 1 frame
        if (!robot_projectileSheet) {
            fprintf(stderr, "Failed to load projectile_resized.png: %s\n", IMG_GetError());
            exit(1);
        }
        Uint32 colorkey = SDL_MapRGB(robot_projectileSheet->format, 255, 255, 255);
        SDL_SetColorKey(robot_projectileSheet, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
        SDL_Surface* optimized = SDL_DisplayFormatAlpha(robot_projectileSheet);
        if (!optimized) {
            fprintf(stderr, "SDL_DisplayFormatAlpha failed for projectile_resized.png: %s\n", SDL_GetError());
            SDL_FreeSurface(robot_projectileSheet);
            exit(1);
        }
        SDL_FreeSurface(robot_projectileSheet);
        robot_projectileSheet = optimized;
        robot_projectileSheetFlipped = flipHorizontally(robot_projectileSheet);
        if (!robot_projectileSheetFlipped) {
            fprintf(stderr, "Failed to flip projectile_resized.png\n");
            SDL_FreeSurface(robot_projectileSheet);
            robot_projectileSheet = NULL;
            exit(1);
        }
        SDL_SetColorKey(robot_projectileSheetFlipped, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
    }

    // Initialize robot attributes
    robot->position.x = x;
    robot->position.y = y;
    robot->position.w = 256;
    robot->position.h = 256;
    robot->state = ROBOT_IDLE;
    robot->frame = 0;
    robot->totalFrames = robot_idleSheet->w / 256;
    robot->health = 100;
    robot->maxHealth = 100;
    robot->speed = MOVE_SPEED;
    robot->facingLeft = 0;
    robot->lastMeleeTime = 0;
    robot->lastRangedTime = 0;
    robot->lastSpecialTime = 0;
    robot->projectileActive = 0;
    robot->projectilePosition.x = 0;
    robot->projectilePosition.y = 0;
    robot->projectilePosition.w = 256;
    robot->projectilePosition.h = 256;
    robot->projectileDirection = 0;
    robot->invulnerabilityTimer = 0;
    robot->active = 1;
    robot->yVelocity = 0.0f;
    robot->onGround = 0;
    robot->frameDelay = 0;
    robot->world_x = x;
    robot->shieldActive = 0;
    robot->shieldTimer = 0;
    robot->bullets = 10;
    robot->damageCooldown = 0;
    robot->activationZone = 1500;
    robot->fireZone = 500;
    robot->patrolDirection = 1;
    robot->patrolTimer = 60 + rand() % 120;
    robot->patrolLeft = x - 200;
    robot->patrolRight = x + 200;
    robot->specialCooldown = 0;
    robot->stateCooldown = 0;
    LOG("Robot initialized: x=%d, y=%d\n", x, y);
}

void updateRobot(Robot* robot, SDL_Rect playerPos, int* playerHealth, int playerMaxHealth, struct GAME* game) {
    if (!robot || !playerHealth || !game) {
        LOG("updateRobot: Invalid parameters\n");
        return;
    }
    if (!robot->active) {
        LOG("updateRobot: Robot inactive, skipping update\n");
        return;
    }

    // Apply gravity and ensure grounding
    robot->onGround = onGroundEntity(game, &robot->position, robot->world_x);
    if (!robot->onGround) {
        robot->yVelocity += GRAVITY;
        if (robot->yVelocity > MAX_FALL_SPEED) robot->yVelocity = MAX_FALL_SPEED;
    } else {
        robot->yVelocity = 0.0f;
        placeRobotOnGround(game, robot);
    }
    robot->position.y += (int)robot->yVelocity;
    if (robot->position.y < 0) {
        robot->position.y = 0;
        robot->yVelocity = 0.0f;
    }
    if (robot->position.y > game->background.height - robot->position.h) {
        robot->position.y = game->background.height - robot->position.h;
        robot->yVelocity = 0.0f;
        robot->onGround = 1;
    }

    // Update cooldowns
    if (robot->damageCooldown > 0) robot->damageCooldown--;
    if (robot->invulnerabilityTimer > 0) robot->invulnerabilityTimer--;
    if (robot->specialCooldown > 0) robot->specialCooldown--;
    if (robot->stateCooldown > 0) robot->stateCooldown--;

    // Set totalFrames based on state
    switch (robot->state) {
        case ROBOT_IDLE: robot->totalFrames = robot_idleSheet->w / 256; break; // 4 frames
        case ROBOT_WALKING: robot->totalFrames = robot_walkSheet->w / 256; break; // 6 frames
        case ROBOT_ATTACK1: robot->totalFrames = robot_attack1Sheet->w / 256; break; // 6 frames
        case ROBOT_ATTACK3: robot->totalFrames = robot_attack3Sheet->w / 256; break; // 6 frames
        case ROBOT_SPECIAL: robot->totalFrames = robot_specialSheet->w / 256; break; // 6 frames
        case ROBOT_HURT: robot->totalFrames = robot_hurtSheet->w / 256; break; // 2 frames
        case ROBOT_DEAD: robot->totalFrames = robot_deathSheet->w / 256; break; // 6 frames
    }

    // Calculate distance to player in world coordinates
    int playerWorldX = playerPos.x + game->background.scroll_x;
    int dx = playerWorldX - robot->world_x;
    int dy = (playerPos.y + playerPos.h / 2) - (robot->position.y + robot->position.h / 2);
    int distance = (int)sqrt(dx * dx + dy * dy);
    robot->facingLeft = dx < 0 ? 1 : 0;

    // State machine
    if (robot->state != ROBOT_HURT && robot->state != ROBOT_DEAD && robot->stateCooldown <= 0) {
        if (robot->health <= 0) {
            robot->state = ROBOT_DEAD;
            robot->frame = 0;
            robot->yVelocity = 0.0f;
            game->player.score += 50;
            LOG("Robot transitioned to DEAD state: health=%d\n", robot->health);
        } else if (distance < 100 && robot->damageCooldown <= 0) {
            robot->state = ROBOT_ATTACK3;
            robot->yVelocity = 0.0f;
            if (robot->frame >= robot->totalFrames - 1) {
                *playerHealth -= 15;
                if (*playerHealth <= 0) {
                    game->player.state = DEAD;
                    game->player.health = 0;
                } else {
                    game->player.state = HURT;
                }
                robot->damageCooldown = DAMAGE_COOLDOWN;
                robot->frame = 0;
                robot->stateCooldown = FRAME_DELAY;
            }
        } else if (distance < robot->fireZone && robot->bullets > 0 && robot->damageCooldown <= 0) {
            robot->state = ROBOT_ATTACK1;
            robot->yVelocity = 0.0f;
            if (robot->frame >= robot->totalFrames - 1) {
                robot->bullets--;
                robot->projectileActive = 1;
                robot->projectilePosition.x = robot->world_x + (robot->facingLeft ? -robot->projectilePosition.w : robot->position.w);
                robot->projectilePosition.y = robot->position.y + robot->position.h / 2 - robot->projectilePosition.h / 2;
                robot->projectileDirection = robot->facingLeft ? -1 : 1;
                robot->damageCooldown = DAMAGE_COOLDOWN;
                robot->frame = 0;
                robot->stateCooldown = FRAME_DELAY;
            }
        } else if (distance < robot->fireZone && robot->bullets <= 0 && robot->specialCooldown <= 0) {
            robot->state = ROBOT_SPECIAL;
            robot->yVelocity = 0.0f;
            if (robot->frame >= robot->totalFrames - 1) {
                robot->specialCooldown = SPECIAL_COOLDOWN;
                *playerHealth -= 20;
                if (*playerHealth <= 0) {
                    game->player.state = DEAD;
                    game->player.health = 0;
                } else {
                    game->player.state = HURT;
                }
                robot->frame = 0;
                robot->stateCooldown = FRAME_DELAY;
            }
        } else if (robot->bullets <= 0 && robot->state != ROBOT_SPECIAL) {
            robot->state = ROBOT_IDLE;
            robot->yVelocity = 0.0f;
            if (robot->frame >= robot->totalFrames - 1) {
                robot->bullets = 10;
                robot->frame = 0;
            }
        } else if (distance < robot->activationZone) {
            robot->state = ROBOT_WALKING;
            robot->yVelocity = 0.0f;
            float speed = MOVE_SPEED * (1.0f + (robot->activationZone - distance) / (float)robot->activationZone);
            robot->world_x += (dx > 0 ? speed : -speed);
            robot->position.x = robot->world_x - game->background.scroll_x;
        } else {
            robot->state = ROBOT_WALKING;
            robot->yVelocity = 0.0f;
            robot->world_x += robot->patrolDirection * PATROL_SPEED;
            robot->position.x = robot->world_x - game->background.scroll_x;
            if (--robot->patrolTimer <= 0) {
                robot->patrolDirection = -robot->patrolDirection;
                robot->patrolTimer = 60 + rand() % 120;
            }
            if (robot->world_x < robot->patrolLeft) {
                robot->world_x = robot->patrolLeft;
                robot->patrolDirection = 1;
            } else if (robot->world_x > robot->patrolRight) {
                robot->world_x = robot->patrolRight;
                robot->patrolDirection = -1;
            }
            robot->facingLeft = robot->patrolDirection < 0 ? 1 : 0;
        }
    } else if (robot->state == ROBOT_HURT && robot->invulnerabilityTimer <= 0) {
        robot->state = ROBOT_IDLE;
        robot->frame = 0;
        robot->stateCooldown = FRAME_DELAY;
    }

    // Update projectile
    if (robot->projectileActive) {
        robot->projectilePosition.x += robot->projectileDirection * 5;
        SDL_Rect projWorldRect = {robot->projectilePosition.x, robot->projectilePosition.y, robot->projectilePosition.w, robot->projectilePosition.h};
        SDL_Rect playerWorldRect = {playerWorldX, playerPos.y, playerPos.w, playerPos.h};
        if (rectIntersect(&projWorldRect, &playerWorldRect)) {
            *playerHealth -= 10;
            if (*playerHealth <= 0) {
                game->player.state = DEAD;
                game->player.health = 0;
            } else {
                game->player.state = HURT;
            }
            robot->projectileActive = 0;
        } else if (robot->projectilePosition.x < 0 || robot->projectilePosition.x > game->background.width) {
            robot->projectileActive = 0;
        }
    }

    // Player attacks robot
    if (game->player.state == ATTACK_1 && robot->invulnerabilityTimer <= 0) {
        SDL_Rect attackRect = {
            game->player.world_x + (game->player.facing == RIGHT ? 50 : -50),
            game->player.position.y, 100, 100
        };
        SDL_Rect robotWorldRect = {robot->world_x, robot->position.y, robot->position.w, robot->position.h};
        if (rectIntersect(&attackRect, &robotWorldRect)) {
            robot->health -= 20;
            robot->state = ROBOT_HURT;
            robot->frame = 0;
            robot->invulnerabilityTimer = INVULNERABILITY_TIME;
            LOG("Robot hit by player attack: health=%d\n", robot->health);
        }
    }
    if (game->player.bulletActive && robot->invulnerabilityTimer <= 0) {
        SDL_Rect bulletRect = {game->player.bullet.x, game->player.bullet.y, game->player.bullet_w, game->player.bullet_h};
        SDL_Rect robotWorldRect = {robot->world_x, robot->position.y, robot->position.w, robot->position.h};
        if (rectIntersect(&bulletRect, &robotWorldRect)) {
            robot->health -= 15;
            robot->state = ROBOT_HURT;
            robot->frame = 0;
            robot->invulnerabilityTimer = INVULNERABILITY_TIME;
            game->player.bulletActive = 0;
            LOG("Robot hit by player bullet: health=%d\n", robot->health);
        }
    }

    // Animation update
    if (++robot->frameDelay >= FRAME_DELAY) {
        robot->frame++;
        if (robot->state == ROBOT_IDLE || robot->state == ROBOT_WALKING) {
            robot->frame %= robot->totalFrames;
        } else if (robot->state == ROBOT_DEAD) {
            if (robot->frame >= robot->totalFrames) {
                robot->frame = robot->totalFrames - 1;
                robot->active = 0;
                LOG("Robot death animation complete: active=%d\n", robot->active);
            }
        } else if (robot->frame >= robot->totalFrames) {
            robot->frame = 0;
            if (robot->state != ROBOT_HURT) {
                robot->state = ROBOT_IDLE;
            }
        }
        robot->frameDelay = 0;
    }

    // Boundary checks
    if (robot->world_x < 0) robot->world_x = 0;
    if (robot->world_x > game->background.width - robot->position.w) {
        robot->world_x = game->background.width - robot->position.w;
    }
    robot->position.x = robot->world_x - game->background.scroll_x;

    LOG("Robot updated: x=%d, y=%d, state=%d, health=%d, frame=%d, active=%d\n",
        robot->position.x, robot->position.y, robot->state, robot->health, robot->frame, robot->active);
}

void renderRobot(SDL_Surface* screen, Robot* robot, int scroll_x) {
    if (!screen || !robot || !robot->active) {
        LOG("renderRobot: Invalid parameters or robot inactive\n");
        return;
    }

    SDL_Surface* sheet = robot->facingLeft ?
        (robot->state == ROBOT_IDLE ? robot_idleSheetFlipped :
         robot->state == ROBOT_WALKING ? robot_walkSheetFlipped :
         robot->state == ROBOT_ATTACK1 ? robot_attack1SheetFlipped :
         robot->state == ROBOT_ATTACK3 ? robot_attack3SheetFlipped :
         robot->state == ROBOT_SPECIAL ? robot_specialSheetFlipped :
         robot->state == ROBOT_HURT ? robot_hurtSheetFlipped :
         robot->state == ROBOT_DEAD ? robot_deathSheetFlipped : robot_idleSheetFlipped) :
        (robot->state == ROBOT_IDLE ? robot_idleSheet :
         robot->state == ROBOT_WALKING ? robot_walkSheet :
         robot->state == ROBOT_ATTACK1 ? robot_attack1Sheet :
         robot->state == ROBOT_ATTACK3 ? robot_attack3Sheet :
         robot->state == ROBOT_SPECIAL ? robot_specialSheet :
         robot->state == ROBOT_HURT ? robot_hurtSheet :
         robot->state == ROBOT_DEAD ? robot_deathSheet : robot_idleSheet);

    if (!sheet) {
        sheet = robot->facingLeft ? robot_idleSheetFlipped : robot_idleSheet;
        LOG("renderRobot: Fallback to idle sheet for state=%d\n", robot->state);
    }

    SDL_Rect srcRect = {robot->frame * 256, 0, 256, 256};
    if (srcRect.x >= sheet->w || robot->frame >= robot->totalFrames) {
        srcRect.x = (robot->totalFrames - 1) * 256;
        robot->frame = robot->totalFrames - 1;
        LOG("renderRobot: Frame clamped for state=%d, frame=%d, totalFrames=%d\n",
            robot->state, robot->frame, robot->totalFrames);
    }
    SDL_Rect destRect = {robot->position.x, robot->position.y, 256, 256};
    SDL_BlitSurface(sheet, &srcRect, screen, &destRect);

    // Render projectile
    if (robot->projectileActive) {
        SDL_Surface* projSheet = robot->projectileDirection < 0 ? robot_projectileSheetFlipped : robot_projectileSheet;
        SDL_Rect projSrcRect = {0, 0, 256, 256};
        SDL_Rect projDestRect = {robot->projectilePosition.x - scroll_x, robot->projectilePosition.y, 256, 256};
        SDL_BlitSurface(projSheet, &projSrcRect, screen, &projDestRect);
    }

    // Render health bar, matching soldier.c and soldier2.c
    if (robot->health > 0 && robot->state != ROBOT_DEAD) {
        SDL_Rect healthBarBg = {robot->position.x, robot->position.y - 20, 50, 5};
        SDL_FillRect(screen, &healthBarBg, SDL_MapRGB(screen->format, 255, 0, 0));
        SDL_Rect healthBar = {robot->position.x, robot->position.y - 20, (50 * robot->health) / robot->maxHealth, 5};
        SDL_FillRect(screen, &healthBar, SDL_MapRGB(screen->format, 0, 255, 0));
    }

    LOG("Robot rendered: x=%d, y=%d, state=%d, frame=%d, totalFrames=%d, active=%d\n",
        robot->position.x, robot->position.y, robot->state, robot->frame, robot->totalFrames, robot->active);
}

void freeRobot(Robot* robot) {
    if (robot) {
        robot->active = 0;
        robot->health = 0;
        robot->state = ROBOT_DEAD;
        LOG("Robot freed: x=%d, y=%d\n", robot->position.x, robot->position.y);
    }
}

void freeRobotSprites(void) {
    LOG("Freeing robot sprites...\n");
    SDL_Surface* sheets[] = {
        robot_idleSheet, robot_idleSheetFlipped, robot_walkSheet, robot_walkSheetFlipped,
        robot_attack1Sheet, robot_attack1SheetFlipped, robot_attack3Sheet, robot_attack3SheetFlipped,
        robot_specialSheet, robot_specialSheetFlipped, robot_hurtSheet, robot_hurtSheetFlipped,
        robot_deathSheet, robot_deathSheetFlipped, robot_projectileSheet, robot_projectileSheetFlipped
    };
    for (int i = 0; i < 16; i++) {
        if (sheets[i]) {
            SDL_FreeSurface(sheets[i]);
            sheets[i] = NULL;
        }
    }
    LOG("Robot sprites freed\n");
}
