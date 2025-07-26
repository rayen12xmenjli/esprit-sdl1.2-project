#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include "game.h"
#include "utils.h"
#include "collision.h"
#include "events.h"
#include <stdio.h>

// Static sound variables for player actions
static Mix_Chunk *jumpSound = NULL;
static Mix_Chunk *attackSound = NULL;
static Mix_Chunk *shootSound = NULL;
static Mix_Chunk *hurtSound = NULL;
static Mix_Chunk *deathSound = NULL;
static Mix_Chunk *rechargeSound = NULL;

// Forward declaration
void freePlayer(Player *player);

void initPlayer(Player *player, SDL_Surface *screen) {
    printf("Starting initPlayer...\n");

    // Load idle sheet
    player->idleSheet = IMG_Load("assets/player/Idle_resized.png");
    if (!player->idleSheet || player->idleSheet->w != 1536 || player->idleSheet->h != 256) {
        fprintf(stderr, "Failed to load or invalid dimensions for Idle_resized.png: %s\n", IMG_GetError());
        freePlayer(player);
        exit(1);
    }
    SDL_SetColorKey(player->idleSheet, SDL_SRCCOLORKEY, SDL_MapRGB(player->idleSheet->format, 255, 0, 255));
    player->idleSheetFlipped = flipHorizontally(player->idleSheet);
    if (!player->idleSheetFlipped) {
        fprintf(stderr, "Failed to flip Idle_resized.png\n");
        freePlayer(player);
        exit(1);
    }
    printf("Idle sheet loaded and flipped.\n");

    // Load walk sheet
    player->walkSheet = IMG_Load("assets/player/Walk_resized.png");
    if (!player->walkSheet || player->walkSheet->w != 2048 || player->walkSheet->h != 256) {
        fprintf(stderr, "Failed to load or invalid dimensions for Walk_resized.png: %s\n", IMG_GetError());
        freePlayer(player);
        exit(1);
    }
    SDL_SetColorKey(player->walkSheet, SDL_SRCCOLORKEY, SDL_MapRGB(player->walkSheet->format, 255, 0, 255));
    player->walkSheetFlipped = flipHorizontally(player->walkSheet);
    if (!player->walkSheetFlipped) {
        fprintf(stderr, "Failed to flip Walk_resized.png\n");
        freePlayer(player);
        exit(1);
    }
    printf("Walk sheet loaded and flipped.\n");

    // Load run sheet
    player->runSheet = IMG_Load("assets/player/Run_resized.png");
    if (!player->runSheet || player->runSheet->w != 2048 || player->runSheet->h != 256) {
        fprintf(stderr, "Failed to load or invalid dimensions for Run_resized.png: %s\n", IMG_GetError());
        freePlayer(player);
        exit(1);
    }
    SDL_SetColorKey(player->runSheet, SDL_SRCCOLORKEY, SDL_MapRGB(player->runSheet->format, 255, 0, 255));
    player->runSheetFlipped = flipHorizontally(player->runSheet);
    if (!player->runSheetFlipped) {
        fprintf(stderr, "Failed to flip Run_resized.png\n");
        freePlayer(player);
        exit(1);
    }
    printf("Run sheet loaded and flipped.\n");

    // Load jump sheet
    player->jumpSheet = IMG_Load("assets/player/Jump_resized.png");
    if (!player->jumpSheet || player->jumpSheet->w != 2816 || player->jumpSheet->h != 256) {
        fprintf(stderr, "Failed to load or invalid dimensions for Jump_resized.png: %s\n", IMG_GetError());
        freePlayer(player);
        exit(1);
    }
    SDL_SetColorKey(player->jumpSheet, SDL_SRCCOLORKEY, SDL_MapRGB(player->jumpSheet->format, 255, 0, 255));
    player->jumpSheetFlipped = flipHorizontally(player->jumpSheet);
    if (!player->jumpSheetFlipped) {
        fprintf(stderr, "Failed to flip Jump_resized.png\n");
        freePlayer(player);
        exit(1);
    }
    printf("Jump sheet loaded and flipped.\n");

    // Load attack1 sheet
    player->attack1Sheet = IMG_Load("assets/player/Attack_1_resized.png");
    if (!player->attack1Sheet || player->attack1Sheet->w != 1536 || player->attack1Sheet->h != 256) {
        fprintf(stderr, "Failed to load or invalid dimensions for Attack_1_resized.png: %s\n", IMG_GetError());
        freePlayer(player);
        exit(1);
    }
    SDL_SetColorKey(player->attack1Sheet, SDL_SRCCOLORKEY, SDL_MapRGB(player->attack1Sheet->format, 255, 0, 255));
    player->attack1SheetFlipped = flipHorizontally(player->attack1Sheet);
    if (!player->attack1SheetFlipped) {
        fprintf(stderr, "Failed to flip Attack_1_resized.png\n");
        freePlayer(player);
        exit(1);
    }
    printf("Attack1 sheet loaded and flipped.\n");

    // Load shot sheet
    player->shotSheet = IMG_Load("assets/player/Shot_resized.png");
    if (!player->shotSheet || player->shotSheet->w != 3072 || player->shotSheet->h != 256) {
        fprintf(stderr, "Failed to load or invalid dimensions for Shot_resized.png: %s\n", IMG_GetError());
        freePlayer(player);
        exit(1);
    }
    SDL_SetColorKey(player->shotSheet, SDL_SRCCOLORKEY, SDL_MapRGB(player->shotSheet->format, 255, 0, 255));
    player->shotSheetFlipped = flipHorizontally(player->shotSheet);
    if (!player->shotSheetFlipped) {
        fprintf(stderr, "Failed to flip Shot_resized.png\n");
        freePlayer(player);
        exit(1);
    }
    printf("Shot sheet loaded and flipped.\n");

    // Load recharge sheet
    player->rechargeSheet = IMG_Load("assets/player/Recharge_resized.png");
    if (!player->rechargeSheet || player->rechargeSheet->w != 3072 || player->rechargeSheet->h != 256) {
        fprintf(stderr, "Failed to load or invalid dimensions for Recharge_resized.png: %s\n", IMG_GetError());
        freePlayer(player);
        exit(1);
    }
    SDL_SetColorKey(player->rechargeSheet, SDL_SRCCOLORKEY, SDL_MapRGB(player->rechargeSheet->format, 255, 0, 255));
    player->rechargeSheetFlipped = flipHorizontally(player->rechargeSheet);
    if (!player->rechargeSheetFlipped) {
        fprintf(stderr, "Failed to flip Recharge_resized.png\n");
        freePlayer(player);
        exit(1);
    }
    printf("Recharge sheet loaded and flipped.\n");

    // Load hurt sheet
    player->hurtSheet = IMG_Load("assets/player/Hurt_resized.png");
    if (!player->hurtSheet || player->hurtSheet->w != 512 || player->hurtSheet->h != 256) {
        fprintf(stderr, "Failed to load or invalid dimensions for Hurt_resized.png: %s\n", IMG_GetError());
        freePlayer(player);
        exit(1);
    }
    SDL_SetColorKey(player->hurtSheet, SDL_SRCCOLORKEY, SDL_MapRGB(player->hurtSheet->format, 255, 0, 255));
    player->hurtSheetFlipped = flipHorizontally(player->hurtSheet);
    if (!player->hurtSheetFlipped) {
        fprintf(stderr, "Failed to flip Hurt_resized.png\n");
        freePlayer(player);
        exit(1);
    }
    printf("Hurt sheet loaded and flipped.\n");

    // Load dead sheet
    player->deadSheet = IMG_Load("assets/player/Dead_resized.png");
    if (!player->deadSheet || player->deadSheet->w != 1024 || player->deadSheet->h != 256) {
        fprintf(stderr, "Failed to load or invalid dimensions for Dead_resized.png: %s\n", IMG_GetError());
        freePlayer(player);
        exit(1);
    }
    SDL_SetColorKey(player->deadSheet, SDL_SRCCOLORKEY, SDL_MapRGB(player->deadSheet->format, 255, 0, 255));
    player->deadSheetFlipped = flipHorizontally(player->deadSheet);
    if (!player->deadSheetFlipped) {
        fprintf(stderr, "Failed to flip Dead_resized.png\n");
        freePlayer(player);
        exit(1);
    }
    printf("Dead sheet loaded and flipped.\n");

    // Load health bar images
    player->healthBarBg = IMG_Load("assets/health/healthbar_bg.png");
    if (!player->healthBarBg || player->healthBarBg->w != 274 || player->healthBarBg->h != 25) {
        fprintf(stderr, "Failed to load or invalid dimensions for healthbar_bg.png: %s\n", IMG_GetError());
        freePlayer(player);
        exit(1);
    }
    SDL_SetColorKey(player->healthBarBg, SDL_SRCCOLORKEY, SDL_MapRGB(player->healthBarBg->format, 255, 0, 255));

    player->healthBarGreen = IMG_Load("assets/health/healthbar_green.png");
    if (!player->healthBarGreen || player->healthBarGreen->w != 271 || player->healthBarGreen->h != 21) {
        fprintf(stderr, "Failed to load or invalid dimensions for healthbar_green.png: %s\n", IMG_GetError());
        freePlayer(player);
        exit(1);
    }
    SDL_SetColorKey(player->healthBarGreen, SDL_SRCCOLORKEY, SDL_MapRGB(player->healthBarGreen->format, 255, 0, 255));
    printf("Health bar images loaded.\n");

    // Load kill icons
    player->killIcon1 = IMG_Load("assets/icons/kill_icon1.png");
    if (!player->killIcon1 || player->killIcon1->w != 88 || player->killIcon1->h != 88) {
        fprintf(stderr, "Failed to load or invalid dimensions for kill_icon1.png: %s\n", IMG_GetError());
        freePlayer(player);
        exit(1);
    }
    SDL_SetColorKey(player->killIcon1, SDL_SRCCOLORKEY, SDL_MapRGB(player->killIcon1->format, 255, 0, 255));

    player->killIcon2 = IMG_Load("assets/icons/kill_icon2.png");
    if (!player->killIcon2 || player->killIcon2->w != 88 || player->killIcon2->h != 88) {
        fprintf(stderr, "Failed to load or invalid dimensions for kill_icon2.png: %s\n", IMG_GetError());
        freePlayer(player);
        exit(1);
    }
    SDL_SetColorKey(player->killIcon2, SDL_SRCCOLORKEY, SDL_MapRGB(player->killIcon2->format, 255, 0, 255));

    player->killIcon3 = IMG_Load("assets/icons/kill_icon3.png");
    if (!player->killIcon3 || player->killIcon3->w != 88 || player->killIcon3->h != 88) {
        fprintf(stderr, "Failed to load or invalid dimensions for kill_icon3.png: %s\n", IMG_GetError());
        freePlayer(player);
        exit(1);
    }
    SDL_SetColorKey(player->killIcon3, SDL_SRCCOLORKEY, SDL_MapRGB(player->killIcon3->format, 255, 0, 255));

    player->killIcon4 = IMG_Load("assets/icons/kill_icon4.png");
    if (!player->killIcon4 || player->killIcon4->w != 88 || player->killIcon4->h != 88) {
        fprintf(stderr, "Failed to load or invalid dimensions for kill_icon4.png: %s\n", IMG_GetError());
        freePlayer(player);
        exit(1);
    }
    SDL_SetColorKey(player->killIcon4, SDL_SRCCOLORKEY, SDL_MapRGB(player->killIcon4->format, 255, 0, 255));
    printf("Kill icons loaded.\n");

    // Load health item
    player->healthItem.image = IMG_Load("assets/items/health_item.png");
    if (!player->healthItem.image || player->healthItem.image->w != 48 || player->healthItem.image->h != 48) {
        fprintf(stderr, "Failed to load or invalid dimensions for health_item.png: %s\n", IMG_GetError());
        freePlayer(player);
        exit(1);
    }
    SDL_SetColorKey(player->healthItem.image, SDL_SRCCOLORKEY, SDL_MapRGB(player->healthItem.image->format, 255, 0, 255));
    printf("Health item loaded.\n");

    // Load bullet sheet with transparency fix
    player->bulletSheet = IMG_Load("assets/player/bullet.png");
    if (!player->bulletSheet || player->bulletSheet->w != 288 || player->bulletSheet->h != 72) {
        fprintf(stderr, "Failed to load or invalid dimensions for bullet.png: %s\n", IMG_GetError());
        freePlayer(player);
        exit(1);
    }
    Uint32 colorkey = SDL_MapRGB(player->bulletSheet->format, 255, 0, 255); // Magenta as transparent color
    SDL_SetColorKey(player->bulletSheet, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
    SDL_Surface *optimized = SDL_DisplayFormatAlpha(player->bulletSheet);
    if (!optimized) {
        fprintf(stderr, "SDL_DisplayFormatAlpha failed for bullet.png: %s\n", SDL_GetError());
        SDL_FreeSurface(player->bulletSheet);
        exit(1);
    }
    SDL_FreeSurface(player->bulletSheet);
    player->bulletSheet = optimized;
    SDL_SetColorKey(player->bulletSheet, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
    player->bulletSheetFlipped = flipHorizontally(player->bulletSheet);
    if (!player->bulletSheetFlipped) {
        fprintf(stderr, "Failed to flip bullet.png\n");
        SDL_FreeSurface(player->bulletSheet);
        exit(1);
    }
    SDL_SetColorKey(player->bulletSheetFlipped, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
    printf("Bullet sheet loaded and flipped with transparency.\n");

    // Load sound effects
    jumpSound = Mix_LoadWAV("assets/sounds/jump.wav");
    if (!jumpSound) {
        fprintf(stderr, "Failed to load jump.wav: %s\n", Mix_GetError());
    }
    attackSound = Mix_LoadWAV("assets/sounds/attack.wav");
    if (!attackSound) {
        fprintf(stderr, "Failed to load attack.wav: %s\n", Mix_GetError());
    }
    shootSound = Mix_LoadWAV("assets/sounds/shoot.wav");
    if (!shootSound) {
        fprintf(stderr, "Failed to load shoot.wav: %s\n", Mix_GetError());
    }
    hurtSound = Mix_LoadWAV("assets/sounds/hurt.wav");
    if (!hurtSound) {
        fprintf(stderr, "Failed to load hurt.wav: %s\n", Mix_GetError());
    }
    deathSound = Mix_LoadWAV("assets/sounds/death.wav");
    if (!deathSound) {
        fprintf(stderr, "Failed to load death.wav: %s\n", Mix_GetError());
    }
    rechargeSound = Mix_LoadWAV("assets/sounds/recharge.wav");
    if (!rechargeSound) {
        fprintf(stderr, "Failed to load recharge.wav: %s\n", Mix_GetError());
    }
    printf("Sound effects loaded.\n");

    // Initialize player attributes
    player->state = IDLE;
    player->frame = 0;
    player->totalFrames = 6;
    player->position.x = 200;
    player->position.y = SCREEN_HEIGHT - 256 - 150;
    player->position.w = 256;
    player->position.h = 256;
    player->world_x = 200;
    player->facing = RIGHT;
    player->yVelocity = 0.0f;
    player->onGround = 0;
    player->freezeYMovement = 0;
    player->health = 100;
    player->maxHealth = 100;
    player->lives = 4;
    player->score = 0;
    player->ammo = 10;
    player->isShooting = 0;
    player->isAttacking = 0;
    player->bulletActive = 0;
    player->bullet_w = 72;
    player->bullet_h = 72;
    player->bullet.w = 72;
    player->bullet.h = 72;
    player->bulletFrame = 0;
    player->bulletDirection = 0;
    player->bulletStartX = 0;
    player->kills = 0;
    player->iconDisplayTime = 0;
    player->showIcon = 0;
    player->healthItem.position.w = 48;
    player->healthItem.position.h = 48;
    player->healthItem.active = 0;
    player->healthItem.spawnTime = 0;
    player->died = 0;
    player->isJumping = 0;
    player->nearDoor = -1;
    player->shootDebounce = 0;         // Initialize shoot debounce
    printf("Player attributes initialized at screen_x=%d, world_x=%d, y=%d, died=%d, isJumping=%d, nearDoor=%d\n",
           player->position.x, player->world_x, player->position.y, player->died, player->isJumping, player->nearDoor);
}

void updatePlayer(Player *player, GAME *game) {
    static int lastState = IDLE;
    static int frameDelay = 0;
    static Uint32 wastedStart = 0;
    static int previousKills = 0;
    static int soldierWasActive[MAX_SOLDIERS] = {0};
    static int soldier2WasActive[MAX_SOLDIERS2] = {0};

    // Play sound effects on state transition
    if (player->state != lastState) {
        player->frame = 0;
        frameDelay = 0;
        switch (player->state) {
            case JUMP:
                if (jumpSound) Mix_PlayChannel(-1, jumpSound, 0);
                break;
            case ATTACK_1:
                if (attackSound) Mix_PlayChannel(-1, attackSound, 0);
                break;
            case SHOT:
                if (shootSound) Mix_PlayChannel(-1, shootSound, 0);
                break;
            case RECHARGE:
                if (rechargeSound) Mix_PlayChannel(-1, rechargeSound, 0);
                break;
            case HURT:
                if (hurtSound) Mix_PlayChannel(-1, hurtSound, 0);
                break;
            case DEAD:
                if (deathSound) Mix_PlayChannel(-1, deathSound, 0);
                break;
            default:
                break;
        }
        lastState = player->state;
    }

    // Handle DEAD state first
    if (player->state == DEAD) {
        player->freezeYMovement = 1;
        if (++frameDelay >= 8) {
            player->frame++;
            if (player->frame >= player->totalFrames) {
                player->frame = player->totalFrames - 1;
                if (player->lives > 0) {
                    player->lives--;
                    player->died = 1;
                    player->state = IDLE;
                    player->frame = 0;
                    frameDelay = 0;
                    player->health = player->maxHealth;
                    player->position.x = 200;
                    player->world_x = 200;
                    player->position.y = SCREEN_HEIGHT - 256 - 100;
                    placePlayerOnGround(game);
                    player->yVelocity = 0.0f;
                    player->onGround = 1;
                    player->freezeYMovement = 0;
                    player->died = 0;
                    player->isJumping = 0;
                    game->background.scroll_x = 0;
                    printf("Player respawned, lives: %d, screen_x=%d, world_x=%d\n",
                           player->lives, player->position.x, player->world_x);
                } else {
                    player->lives = 0;
                    if (wastedStart == 0) {
                        wastedStart = SDL_GetTicks();
                        game->ui.showWasted = 1;
                        game->ui.wastedFrame = 0;
                        printf("No lives left, starting wasted animation\n");
                    }
                    if (SDL_GetTicks() - wastedStart >= 5200) {
                        game->global.gameOver = 1;
                        game->ui.showWasted = 0;
                        wastedStart = 0;
                        printf("Wasted animation complete, game over\n");
                    }
                }
            }
            frameDelay = 0;
        }
        return;
    }

    // Set frame delay and total frames based on state
    int frameDelayMax = 6;
    switch (player->state) {
        case IDLE: player->totalFrames = 6; frameDelayMax = 6; break;
        case WALK: player->totalFrames = 8; frameDelayMax = 6; break;
        case RUN: player->totalFrames = 8; frameDelayMax = 4; break;
        case JUMP: player->totalFrames = 11; frameDelayMax = 5; break;
        case ATTACK_1: player->totalFrames = 6; frameDelayMax = 4; break;
        case SHOT: player->totalFrames = 12; frameDelayMax = 4; break;
        case RECHARGE: player->totalFrames = 12; frameDelayMax = 4; break;
        case HURT: player->totalFrames = 2; frameDelayMax = 6; break;
        case DEAD: player->totalFrames = 4; frameDelayMax = 8; break;
    }

    // Handle jumping state transitions
    if (!player->onGround && player->state != HURT && player->state != DEAD && player->state != JUMP) {
        player->state = JUMP;
        player->frame = 0;
        frameDelay = 0;
        player->isJumping = 1;
    } else if (player->onGround && player->state == JUMP) {
        player->state = IDLE;
        player->frame = 0;
        frameDelay = 0;
        player->isJumping = 0;
    }

    // Handle damage input
    if (game->input.damage) {
        player->health -= 25;
        if (player->health <= 0) {
            player->health = 0;
            player->state = DEAD;
            player->frame = 0;
            frameDelay = 0;
            player->onGround = 0;
            player->freezeYMovement = 1;
            player->died = 1;
            printf("Player died, lives: %d\n", player->lives);
        } else {
            player->state = HURT;
            player->frame = 0;
            frameDelay = 0;
        }
        game->input.damage = 0;
    }

    // Update animation frame
    if (++frameDelay >= frameDelayMax) {
        player->frame++;
        if (player->state == IDLE || player->state == WALK || player->state == RUN) {
            player->frame %= player->totalFrames;
        } else {
            if (player->frame >= player->totalFrames) {
                if (player->state == RECHARGE) player->ammo = 10;
                if (player->state != DEAD && player->state != JUMP) {
                    if (player->onGround) {
                        player->state = IDLE;
                        player->frame = 0;
                        player->isJumping = 0;
                    }
                    player->frame = 0;
                }
            }
        }
        frameDelay = 0;
    }

    // Only handle action inputs if not in HURT state or if HURT animation has completed
    if (player->state != HURT || player->frame >= player->totalFrames) {
        if (game->input.attack && player->state != DEAD) {
            player->state = ATTACK_1;
            player->frame = 0;
            frameDelay = 0;
            player->isAttacking = 1;
            player->score += 10;
        }
        // Handle shoot input with debounce
        if (game->input.shoot && player->ammo > 0 && player->state != DEAD && !player->shootDebounce) {
            player->state = SHOT;
            player->frame = 0;
            frameDelay = 0;
            player->isShooting = 1;
            player->bulletActive = 1;
            player->bullet.x = player->world_x + (player->facing == LEFT ? -72 : 256);
            player->bullet.y = player->position.y + 128 - player->bullet_h / 2;
            player->bulletFrame = 0;
            player->bulletDirection = (player->facing == LEFT) ? -1 : 1;
            player->bulletStartX = player->bullet.x;
            player->ammo--;
            player->score += 20;
            player->shootDebounce = 1; // Set debounce flag
            printf("Shot fired, ammo remaining: %d\n", player->ammo);
        }
        // Reset shoot debounce when shoot input is released
        if (!game->input.shoot) {
            player->shootDebounce = 0;
        }
        if (game->input.recharge && player->ammo < 10 && player->state != DEAD) {
            player->state = RECHARGE;
            player->frame = 0;
            frameDelay = 0;
        }
    }

    // Handle bullet animation and movement
    if (player->bulletActive) {
        player->bullet.x += player->bulletDirection * 15;
        static int bulletFrameDelay = 0;
        if (++bulletFrameDelay >= 4) {
            player->bulletFrame = (player->bulletFrame + 1) % 4;
            bulletFrameDelay = 0;
        }
        if (abs(player->bullet.x - player->bulletStartX) > 1000) {
            player->bulletActive = 0;
        }
    }

    // Handle kill icon and bonus item logic for Soldier enemies
    for (int i = 0; i < game->numSoldiers; i++) {
        Soldier *soldier = &game->soldiers[i];
        if (soldierWasActive[i] && !soldier->active) {
            player->kills++;
            if (!player->healthItem.active && (rand() % 2 == 0)) {
                player->healthItem.active = 1;
                player->healthItem.position.x = soldier->world_x;
                player->healthItem.position.y = SCREEN_HEIGHT - 256 - 150 + (256 - 48); // Adjusted for health item height (48)
                player->healthItem.spawnTime = SDL_GetTicks();
            }
            soldierWasActive[i] = 0;
        }
        if (soldier->active) {
            soldierWasActive[i] = 1;
        }
    }

    // Handle kill icon and bonus item logic for Soldier2 enemies
    for (int i = 0; i < game->numSoldiers2; i++) {
        Soldier2 *soldier2 = &game->soldiers2[i];
        if (soldier2WasActive[i] && !soldier2->active) {
            player->kills++;
            if (!player->healthItem.active && (rand() % 2 == 0)) {
                player->healthItem.active = 1;
                player->healthItem.position.x = soldier2->world_x;
                player->healthItem.position.y = SCREEN_HEIGHT - 256 - 150 + (256 - 48); // Adjusted for health item height (48)
                player->healthItem.spawnTime = SDL_GetTicks();
            }
            soldier2WasActive[i] = 0;
        }
        if (soldier2->active) {
            soldier2WasActive[i] = 1;
        }
    }

    // Check for new kill thresholds
    if (player->kills != previousKills) {
        if ((player->kills >= 1 && player->kills < 2) ||
            (player->kills >= 2 && player->kills < 4) ||
            (player->kills >= 4 && player->kills < 8) ||
            (player->kills >= 8)) {
            player->showIcon = 1;
            player->iconDisplayTime = SDL_GetTicks();
        }
        previousKills = player->kills;
    }

    // Hide kill icon after 2 seconds
    if (player->showIcon && (SDL_GetTicks() - player->iconDisplayTime >= 2000)) {
        player->showIcon = 0;
    }

    // Check for collision with health item
    if (player->healthItem.active && rectIntersect(&player->position, &player->healthItem.position)) {
        player->health += player->maxHealth / 4;
        if (player->health > player->maxHealth) {
            player->health = player->maxHealth;
        }
        player->healthItem.active = 0;
        if (player->itemPickupSound) {
            Mix_PlayChannel(-1, player->itemPickupSound, 0);
        }
    }

    // Hide health item after 10 seconds
    if (player->healthItem.active && (SDL_GetTicks() - player->healthItem.spawnTime >= 10000)) {
        player->healthItem.active = 0;
    }
}

void renderPlayer(Player *player, GAME *game) {
    if (!player || !game || !game->screen) {
        fprintf(stderr, "renderPlayer: Player, game, or screen is NULL\n");
        return;
    }

    printf("renderPlayer: state=%d, facing=%d, frame=%d, position=(%d,%d), world_x=%d\n",
           player->state, player->facing, player->frame, player->position.x, player->position.y, player->world_x);

    SDL_Surface *sheet = NULL;
    if (player->facing == LEFT) {
        switch (player->state) {
            case IDLE: sheet = player->idleSheetFlipped; break;
            case WALK: sheet = player->walkSheetFlipped; break;
            case RUN: sheet = player->runSheetFlipped; break;
            case JUMP: sheet = player->jumpSheetFlipped; break;
            case ATTACK_1: sheet = player->attack1SheetFlipped; break;
            case SHOT: sheet = player->shotSheetFlipped; break;
            case RECHARGE: sheet = player->rechargeSheetFlipped; break;
            case HURT: sheet = player->hurtSheetFlipped; break;
            case DEAD: sheet = player->deadSheetFlipped; break;
        }
    } else {
        switch (player->state) {
            case IDLE: sheet = player->idleSheet; break;
            case WALK: sheet = player->walkSheet; break;
            case RUN: sheet = player->runSheet; break;
            case JUMP: sheet = player->jumpSheet; break;
            case ATTACK_1: sheet = player->attack1Sheet; break;
            case SHOT: sheet = player->shotSheet; break;
            case RECHARGE: sheet = player->rechargeSheet; break;
            case HURT: sheet = player->hurtSheet; break;
            case DEAD: sheet = player->deadSheet; break;
        }
    }

    if (!sheet) {
        fprintf(stderr, "renderPlayer: No valid sheet for state %d\n", player->state);
        return;
    }

    SDL_Rect srcRect = {player->frame * 256, 0, 256, 256};
    if (srcRect.x >= sheet->w || srcRect.w > sheet->w || srcRect.h > sheet->h) {
        fprintf(stderr, "renderPlayer: Invalid srcRect (%d,%d,%d,%d) for sheet (%d,%d)\n",
                srcRect.x, srcRect.y, srcRect.w, srcRect.h, sheet->w, sheet->h);
        return;
    }
    SDL_Rect destRect = {player->position.x, player->position.y, 256, 256};
    if (SDL_BlitSurface(sheet, &srcRect, game->screen, &destRect) < 0) {
        fprintf(stderr, "renderPlayer: SDL_BlitSurface failed for player sprite: %s\n", SDL_GetError());
        return;
    }

    if (player->bulletActive) {
        SDL_Surface *bulletSheet = (player->bulletDirection < 0) ? player->bulletSheetFlipped : player->bulletSheet;
        if (!bulletSheet) {
            fprintf(stderr, "renderPlayer: Bullet sheet is NULL\n");
            return;
        }
        SDL_Rect bulletSrcRect = {player->bulletFrame * 72, 0, 72, 72};
        SDL_Rect bulletDestRect = {player->bullet.x - game->background.scroll_x, player->bullet.y, 72, 72};
        if (bulletSrcRect.x >= bulletSheet->w || bulletSrcRect.w > bulletSheet->w || bulletSrcRect.h > bulletSheet->h) {
            fprintf(stderr, "renderPlayer: Invalid bulletSrcRect (%d,%d,%d,%d) for bulletSheet (%d,%d)\n",
                    bulletSrcRect.x, bulletSrcRect.y, bulletSrcRect.w, bulletSrcRect.h, bulletSheet->w, bulletSheet->h);
            return;
        }
        if (SDL_BlitSurface(bulletSheet, &bulletSrcRect, game->screen, &bulletDestRect) < 0) {
            fprintf(stderr, "renderPlayer: SDL_BlitSurface failed for bullet: %s\n", SDL_GetError());
            return;
        }
    }

    if (player->showIcon) {
        SDL_Surface *iconToDisplay = NULL;
        if (player->kills >= 8) iconToDisplay = player->killIcon4;
        else if (player->kills >= 4) iconToDisplay = player->killIcon3;
        else if (player->kills >= 2) iconToDisplay = player->killIcon2;
        else if (player->kills >= 1) iconToDisplay = player->killIcon1;

        if (iconToDisplay) {
            int iconWidth = iconToDisplay->w;
            int iconHeight = iconToDisplay->h;
            int startX = (SCREEN_WIDTH - iconWidth) / 2;
            int startY = 10;
            SDL_Rect destRect = {startX, startY, iconWidth, iconHeight};
            if (SDL_BlitSurface(iconToDisplay, NULL, game->screen, &destRect) < 0) {
                fprintf(stderr, "renderPlayer: SDL_BlitSurface failed for kill icon: %s\n", SDL_GetError());
                return;
            }
        }
    }

    if (player->healthItem.active) {
        if (!player->healthItem.image) {
            fprintf(stderr, "renderPlayer: Health item image is NULL\n");
            return;
        }
        SDL_Rect healthDestRect = {player->healthItem.position.x - game->background.scroll_x,
                                  player->healthItem.position.y,
                                  player->healthItem.image->w, // Use actual width (48)
                                  player->healthItem.image->h  // Use actual height (48)
        };
        if (SDL_BlitSurface(player->healthItem.image, NULL, game->screen, &healthDestRect) < 0) {
            fprintf(stderr, "renderPlayer: SDL_BlitSurface failed for health item: %s\n", SDL_GetError());
            return;
        }
    }
}

void freePlayer(Player *player) {
    if (player->idleSheet) SDL_FreeSurface(player->idleSheet);
    if (player->idleSheetFlipped) SDL_FreeSurface(player->idleSheetFlipped);
    if (player->walkSheet) SDL_FreeSurface(player->walkSheet);
    if (player->walkSheetFlipped) SDL_FreeSurface(player->walkSheetFlipped);
    if (player->runSheet) SDL_FreeSurface(player->runSheet);
    if (player->runSheetFlipped) SDL_FreeSurface(player->runSheetFlipped);
    if (player->jumpSheet) SDL_FreeSurface(player->jumpSheet);
    if (player->jumpSheetFlipped) SDL_FreeSurface(player->jumpSheetFlipped);
    if (player->attack1Sheet) SDL_FreeSurface(player->attack1Sheet);
    if (player->attack1SheetFlipped) SDL_FreeSurface(player->attack1SheetFlipped);
    if (player->shotSheet) SDL_FreeSurface(player->shotSheet);
    if (player->shotSheetFlipped) SDL_FreeSurface(player->shotSheetFlipped);
    if (player->rechargeSheet) SDL_FreeSurface(player->rechargeSheet);
    if (player->rechargeSheetFlipped) SDL_FreeSurface(player->rechargeSheetFlipped);
    if (player->hurtSheet) SDL_FreeSurface(player->hurtSheet);
    if (player->hurtSheetFlipped) SDL_FreeSurface(player->hurtSheetFlipped);
    if (player->deadSheet) SDL_FreeSurface(player->deadSheet);
    if (player->deadSheetFlipped) SDL_FreeSurface(player->deadSheetFlipped);
    if (player->healthBarBg) SDL_FreeSurface(player->healthBarBg);
    if (player->healthBarGreen) SDL_FreeSurface(player->healthBarGreen);
    if (player->killIcon1) SDL_FreeSurface(player->killIcon1);
    if (player->killIcon2) SDL_FreeSurface(player->killIcon2);
    if (player->killIcon3) SDL_FreeSurface(player->killIcon3);
    if (player->killIcon4) SDL_FreeSurface(player->killIcon4);
    if (player->healthItem.image) SDL_FreeSurface(player->healthItem.image);
    if (player->bulletSheet) SDL_FreeSurface(player->bulletSheet);
    if (player->bulletSheetFlipped) SDL_FreeSurface(player->bulletSheetFlipped);

    // Free sound effects
    if (jumpSound) {
        Mix_FreeChunk(jumpSound);
        jumpSound = NULL;
    }
    if (attackSound) {
        Mix_FreeChunk(attackSound);
        attackSound = NULL;
    }
    if (shootSound) {
        Mix_FreeChunk(shootSound);
        shootSound = NULL;
    }
    if (hurtSound) {
        Mix_FreeChunk(hurtSound);
        hurtSound = NULL;
    }
    if (deathSound) {
        Mix_FreeChunk(deathSound);
        deathSound = NULL;
    }
    if (rechargeSound) {
        Mix_FreeChunk(rechargeSound);
        rechargeSound = NULL;
    }
}
