#include "boss.h"
#include "game.h"
#include "collision.h"
#include "mouvement.h"
#include "utils.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// Constants for VFX frame sizes
#define UMBRELLA_FRAME_WIDTH 256
#define UMBRELLA_FRAME_HEIGHT 128
#define SLASH_FRAME_WIDTH 128
#define SLASH_FRAME_HEIGHT 128
#define ICE_FRAME_WIDTH 256
#define ICE_FRAME_HEIGHT 128

// Level 2 width from level.c
#define LEVEL2_WIDTH 9651

// Utility function to clamp values
int clamp(int value, int min, int max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

// Get ground Y position at a given x coordinate
int getGroundY(GAME* game, int x) {
    if (!game || !game->background.levelCollision) {
        fprintf(stderr, "getGroundY: Game or levelCollision is NULL\n");
        return SCREEN_HEIGHT - 512; // Fallback to boss height
    }

    int y_offset = (SCREEN_HEIGHT - game->background.levelCollision->h) / 2;
    int y_start = 0; // Start from top of collision map
    int max_y = game->background.levelCollision->h;

    if (x < 0 || x >= game->background.levelCollision->w) {
        printf("getGroundY: Invalid x=%d for levelCollision width=%d\n", x, game->background.levelCollision->w);
        return SCREEN_HEIGHT - 512; // Fallback
    }

    for (int y = y_start; y < max_y; y++) {
        SDL_Color color = get_pixel(game->background.levelCollision, x, y);
        if (color.r == 255 && color.g == 255 && color.b == 255) {
            int ground_y = y + y_offset - 512;
            printf("getGroundY: x=%d, ground_y=%d\n", x, ground_y);
            return ground_y;
        }
    }

    int fallback_y = max_y + y_offset - 512;
    printf("getGroundY: Fallback x=%d, y=%d\n", x, fallback_y);
    return fallback_y; // Fallback to bottom
}

void initVFX(VFX* vfx, const char* folder, int x, int y, int totalFrames, int frameWidth, int frameHeight, int frameDelayThreshold) {
    if (!vfx) {
        fprintf(stderr, "initVFX: VFX pointer is NULL\n");
        return;
    }
    SDL_Surface** frames = malloc(totalFrames * sizeof(SDL_Surface*));
    if (!frames) {
        fprintf(stderr, "initVFX: Failed to allocate memory for VFX frames\n");
        exit(1);
    }
    for (int i = 0; i < totalFrames; i++) {
        char path[100];
        sprintf(path, "%s/frame%d.png", folder, i + 1);
        frames[i] = IMG_Load(path);
        if (!frames[i]) {
            fprintf(stderr, "initVFX: Failed to load %s: %s\n", path, IMG_GetError());
            for (int j = 0; j < i; j++) {
                SDL_FreeSurface(frames[j]);
            }
            free(frames);
            vfx->frames = NULL;
            vfx->totalFrames = 0;
            return;
        }
        SDL_Surface* optimized = SDL_DisplayFormatAlpha(frames[i]);
        SDL_FreeSurface(frames[i]);
        frames[i] = optimized;
    }
    vfx->frames = frames;
    vfx->totalFrames = totalFrames;
    vfx->frame = 0;
    vfx->x = x;
    vfx->y = y;
    vfx->active = 0;
    vfx->frameWidth = frameWidth;
    vfx->frameHeight = frameHeight;
    vfx->frameDelay = 0;
    vfx->frameDelayThreshold = frameDelayThreshold;
}

void updateVFX(VFX* vfx) {
    if (!vfx || !vfx->active) return;
    if (++vfx->frameDelay >= vfx->frameDelayThreshold) {
        vfx->frame++;
        if (vfx->frame >= vfx->totalFrames) {
            vfx->active = 0;
            vfx->frame = 0;
        }
        vfx->frameDelay = 0;
    }
}

void renderVFX(SDL_Surface* screen, VFX* vfx, int scroll_x) {
    if (!screen || !vfx || !vfx->active || !vfx->frames[vfx->frame]) return;
    SDL_Rect src = {0, 0, vfx->frameWidth, vfx->frameHeight};
    SDL_Rect dest = {vfx->x - scroll_x, vfx->y, vfx->frameWidth, vfx->frameHeight};
    if (SDL_BlitSurface(vfx->frames[vfx->frame], &src, screen, &dest) < 0) {
        fprintf(stderr, "renderVFX: SDL_BlitSurface failed: %s\n", SDL_GetError());
    }
}

void freeVFX(VFX* vfx) {
    if (!vfx) return;
    for (int i = 0; i < vfx->totalFrames; i++) {
        if (vfx->frames[i]) SDL_FreeSurface(vfx->frames[i]);
    }
    free(vfx->frames);
    vfx->frames = NULL;
    vfx->active = 0;
}

void initBoss(Boss* boss, int x, int y) {
    if (!boss) {
        fprintf(stderr, "initBoss: Boss pointer is NULL\n");
        exit(1);
    }
    printf("Starting initBoss at x=%d, y=%d\n", x, y);
    boss->idleFrameCount = 6;
    boss->walkFrameCount = 10;
    boss->attackFrameCount = 14;
    boss->takehitFrameCount = 7;
    boss->deathFrameCount = 16;
    boss->idleFrames = malloc(boss->idleFrameCount * sizeof(SDL_Surface*));
    boss->walkFrames = malloc(boss->walkFrameCount * sizeof(SDL_Surface*));
    boss->attackFrames = malloc(boss->attackFrameCount * sizeof(SDL_Surface*));
    boss->takehitFrames = malloc(boss->takehitFrameCount * sizeof(SDL_Surface*));
    boss->deathFrames = malloc(boss->deathFrameCount * sizeof(SDL_Surface*));
    if (!boss->idleFrames || !boss->walkFrames || !boss->attackFrames || 
        !boss->takehitFrames || !boss->deathFrames) {
        fprintf(stderr, "initBoss: Failed to allocate memory for boss frames\n");
        exit(1);
    }
    for (int i = 0; i < boss->idleFrameCount; i++) {
        char path[100];
        sprintf(path, "assets/boss/idle/idle_%d.png", i + 1);
        boss->idleFrames[i] = IMG_Load(path);
        if (!boss->idleFrames[i]) {
            fprintf(stderr, "initBoss: Failed to load %s: %s\n", path, IMG_GetError());
            boss->idleFrameCount = i;
            break;
        }
        SDL_Surface* optimized = SDL_DisplayFormatAlpha(boss->idleFrames[i]);
        SDL_FreeSurface(boss->idleFrames[i]);
        boss->idleFrames[i] = optimized;
    }
    for (int i = 0; i < boss->walkFrameCount; i++) {
        char path[100];
        sprintf(path, "assets/boss/walk/walk_%d.png", i + 1);
        boss->walkFrames[i] = IMG_Load(path);
        if (!boss->walkFrames[i]) {
            fprintf(stderr, "initBoss: Failed to load %s: %s\n", path, IMG_GetError());
            boss->walkFrameCount = i;
            break;
        }
        SDL_Surface* optimized = SDL_DisplayFormatAlpha(boss->walkFrames[i]);
        SDL_FreeSurface(boss->walkFrames[i]);
        boss->walkFrames[i] = optimized;
    }
    for (int i = 0; i < boss->attackFrameCount; i++) {
        char path[100];
        sprintf(path, "assets/boss/1_atk/1_atk_%d.png", i + 1);
        boss->attackFrames[i] = IMG_Load(path);
        if (!boss->attackFrames[i]) {
            fprintf(stderr, "initBoss: Failed to load %s: %s\n", path, IMG_GetError());
            boss->attackFrameCount = i;
            break;
        }
        SDL_Surface* optimized = SDL_DisplayFormatAlpha(boss->attackFrames[i]);
        SDL_FreeSurface(boss->attackFrames[i]);
        boss->attackFrames[i] = optimized;
    }
    for (int i = 0; i < boss->takehitFrameCount; i++) {
        char path[100];
        sprintf(path, "assets/boss/take_hit/take_hit_%d.png", i + 1);
        boss->takehitFrames[i] = IMG_Load(path);
        if (!boss->takehitFrames[i]) {
            fprintf(stderr, "initBoss: Failed to load %s: %s\n", path, IMG_GetError());
            boss->takehitFrameCount = i;
            break;
        }
        SDL_Surface* optimized = SDL_DisplayFormatAlpha(boss->takehitFrames[i]);
        SDL_FreeSurface(boss->takehitFrames[i]);
        boss->takehitFrames[i] = optimized;
    }
    for (int i = 0; i < boss->deathFrameCount; i++) {
        char path[100];
        sprintf(path, "assets/boss/death/death_%d.png", i + 1);
        boss->deathFrames[i] = IMG_Load(path);
        if (!boss->deathFrames[i]) {
            fprintf(stderr, "initBoss: Failed to load %s: %s\n", path, IMG_GetError());
            boss->deathFrameCount = i;
            break;
        }
        SDL_Surface* optimized = SDL_DisplayFormatAlpha(boss->deathFrames[i]);
        SDL_FreeSurface(boss->deathFrames[i]);
        boss->deathFrames[i] = optimized;
    }
    // Load level-up icon
    boss->levelUpIcon = IMG_Load("assets/items/Jump_Bonus_01.png");
    if (!boss->levelUpIcon) {
        fprintf(stderr, "initBoss: Failed to load Jump_Bonus_01.png: %s\n", IMG_GetError());
        for (int i = 0; i < boss->idleFrameCount; i++) if (boss->idleFrames[i]) SDL_FreeSurface(boss->idleFrames[i]);
        for (int i = 0; i < boss->walkFrameCount; i++) if (boss->walkFrames[i]) SDL_FreeSurface(boss->walkFrames[i]);
        for (int i = 0; i < boss->attackFrameCount; i++) if (boss->attackFrames[i]) SDL_FreeSurface(boss->attackFrames[i]);
        for (int i = 0; i < boss->takehitFrameCount; i++) if (boss->takehitFrames[i]) SDL_FreeSurface(boss->takehitFrames[i]);
        for (int i = 0; i < boss->deathFrameCount; i++) if (boss->deathFrames[i]) SDL_FreeSurface(boss->deathFrames[i]);
        free(boss->idleFrames);
        free(boss->walkFrames);
        free(boss->attackFrames);
        free(boss->takehitFrames);
        free(boss->deathFrames);
        exit(1);
    }
    printf("Level-up icon loaded: w=%d, h=%d\n", boss->levelUpIcon->w, boss->levelUpIcon->h);
    SDL_SetColorKey(boss->levelUpIcon, SDL_SRCCOLORKEY, SDL_MapRGB(boss->levelUpIcon->format, 255, 0, 255));

    boss->world_x = x;
    boss->y = y;
    boss->position = (SDL_Rect){x, y, 512, 512};
    boss->health = 500;
    boss->maxHealth = 500;
    boss->state = BOSS_IDLE;
    boss->frame = 0;
    boss->totalFrames = boss->idleFrameCount;
    boss->frameDelay = 0;
    boss->invulnTimer = 0;
    boss->facingLeft = 0;
    boss->active = 1;
    boss->attackCooldown = 0;
    boss->angry = 0;
    boss->hasDamagedThisAttack = 0;
    boss->levelUpIconActive = 0;
    boss->levelUpIconSpawnTime = 0;
    boss->levelUpFlickerCounter = 0;
    initVFX(&boss->umbrellaVFX, "assets/vfx/Umbrella", x, y, 14, UMBRELLA_FRAME_WIDTH, UMBRELLA_FRAME_HEIGHT, 5);
    initVFX(&boss->iceSlashVFX, "assets/vfx/IceSlash", x, y + 256, 9, SLASH_FRAME_WIDTH, SLASH_FRAME_HEIGHT, 5);
    for (int i = 0; i < 3; i++) {
        initVFX(&boss->frostExplosionVFX[i], "assets/vfx/FrostExplosion", x, y + 256, 11, ICE_FRAME_WIDTH, ICE_FRAME_HEIGHT, 55);
        boss->frostExplosionVFX[i].active = 0;
    }
    boss->umbrellaVFX.active = 0;
    boss->iceSlashVFX.active = 0;
    printf("Boss initialized at x=%d, y=%d\n", x, y);
}

void updateBoss(Boss* boss, GAME* game) {
    if (!boss || !game) return;

    // Update level-up icon regardless of boss->active
    if (boss->levelUpIconActive) {
        Uint32 currentTime = SDL_GetTicks();
        Uint32 elapsedTime = currentTime - boss->levelUpIconSpawnTime;
        if (elapsedTime >= 5000) { // Deactivate after 5 seconds
            boss->levelUpIconActive = 0;
            printf("Level-up icon deactivated after 5 seconds\n");
        }
        // Allow collection after 500ms to prevent instant pickup
        SDL_Rect playerHitbox = {game->player.world_x, game->player.position.y, 258, 258};
        if (elapsedTime > 500 && rectIntersect(&boss->levelUpIconPosition, &playerHitbox)) { // Changed: 500ms delay
            game->player.score += 100;
            boss->levelUpIconActive = 0;
            printf("Level-up icon collected, score=%d\n", game->player.score);
        }
    }

    if (!boss->active && !boss->levelUpIconActive) return;

    if (boss->state == BOSS_DEATH) {
        if (++boss->frameDelay >= 5) {
            boss->frame++;
            if (boss->frame >= boss->deathFrameCount && !boss->levelUpIconActive) {
                boss->active = 0;
                boss->frame = boss->deathFrameCount - 1;
                boss->levelUpIconActive = 1;
                boss->levelUpIconPosition.x = boss->world_x + 100; // Center icon slightly offset from boss
                // Ensure icon bottom aligns with ground
                int groundY = getGroundY(game, boss->world_x + 100);
                boss->levelUpIconPosition.y = groundY + 512 - 392; // 392 is icon height
                boss->levelUpIconPosition.w = 314;
                boss->levelUpIconPosition.h = 392;
                boss->levelUpIconSpawnTime = SDL_GetTicks();
                boss->levelUpFlickerCounter = 0;
                printf("Level-up icon spawned at x=%d, y=%d (groundY=%d, boss_x=%d)\n", 
                       boss->levelUpIconPosition.x, boss->levelUpIconPosition.y, groundY, boss->world_x);
            }
            boss->frameDelay = 0;
        }
        return;
    }

    if (!onGroundBoss(game, boss)) {
        boss->y += (int)GRAVITY;
        boss->position.y = boss->y;
        placeBossOnGround(game, boss);
    }
    if (boss->invulnTimer > 0) boss->invulnTimer--;
    if (boss->attackCooldown > 0) boss->attackCooldown--;
    boss->angry = (boss->health < boss->maxHealth / 2);
    switch (boss->state) {
        case BOSS_IDLE: boss->totalFrames = boss->idleFrameCount; break;
        case BOSS_WALK: boss->totalFrames = boss->walkFrameCount; break;
        case BOSS_ATTACK: boss->totalFrames = boss->attackFrameCount; break;
        case BOSS_TAKEHIT: boss->totalFrames = boss->takehitFrameCount; break;
        case BOSS_DEATH: boss->totalFrames = boss->deathFrameCount; break;
    }
    int playerWorldX = game->player.world_x;
    int dx = playerWorldX - boss->world_x;
    int distance = abs(dx);
    boss->facingLeft = dx < 0 ? 0 : 1;
    printf("Player world_x: %d, Boss world_x: %d, dx: %d, facingLeft: %d\n",
           playerWorldX, boss->world_x, dx, boss->facingLeft);

    // Umbrella VFX as projectile
    if (boss->state == BOSS_ATTACK && boss->frame == 0 && boss->attackCooldown <= 0) {
        boss->umbrellaVFX.x = boss->world_x + (boss->facingLeft ? 512 : -256);
        boss->umbrellaVFX.y = boss->y + 100;
        boss->umbrellaVFX.active = 1;
        boss->umbrellaVFX.frame = 0;
        boss->attackCooldown = 60;
    }
    if (boss->umbrellaVFX.active) {
        int target_x = game->player.world_x;
        int move_dx = (target_x > boss->umbrellaVFX.x) ? 5 : -5;
        boss->umbrellaVFX.x += move_dx;
        boss->umbrellaVFX.x = clamp(boss->umbrellaVFX.x, game->background.scroll_x, 
                                    game->background.scroll_x + SCREEN_WIDTH - UMBRELLA_FRAME_WIDTH);
        boss->umbrellaVFX.y = clamp(boss->umbrellaVFX.y, 0, SCREEN_HEIGHT - UMBRELLA_FRAME_HEIGHT);
        SDL_Rect umbrellaHitbox = {boss->umbrellaVFX.x, boss->umbrellaVFX.y, UMBRELLA_FRAME_WIDTH, UMBRELLA_FRAME_HEIGHT};
        SDL_Rect playerHitbox = {game->player.world_x, game->player.position.y, 258, 258};
        if (rectIntersect(&umbrellaHitbox, &playerHitbox)) {
            game->player.health -= 5;
            if (game->player.health <= 0) game->player.state = DEAD;
            else game->player.state = HURT;
            boss->umbrellaVFX.active = 0;
        }
        if (abs(boss->umbrellaVFX.x - playerWorldX) > 1000) {
            boss->umbrellaVFX.active = 0;
        }
    }

    // FrostExplosion VFX spawning
    static Uint32 lastFrostSpawn = 0;
    Uint32 currentTime = SDL_GetTicks();
    if (boss->state != BOSS_DEATH && currentTime - lastFrostSpawn >= 500 && rand() % 100 < 50) {
        int max_explosions = boss->angry ? 3 : 2;
        int active_count = 0;
        for (int i = 0; i < 3; i++) {
            if (boss->frostExplosionVFX[i].active) active_count++;
        }
        if (active_count < max_explosions) {
            for (int i = 0; i < 3; i++) {
                if (!boss->frostExplosionVFX[i].active) {
                    int vfx_x = game->player.world_x + (rand() % 600 - 300) + (i * 100 - 100);
                    vfx_x = clamp(vfx_x, 0, LEVEL2_WIDTH - ICE_FRAME_WIDTH);
                    int ground_y = getGroundY(game, vfx_x);
                    boss->frostExplosionVFX[i].x = vfx_x;
                    boss->frostExplosionVFX[i].y = ground_y + 512 - boss->frostExplosionVFX[i].frameHeight;
                    boss->frostExplosionVFX[i].active = 1;
                    boss->frostExplosionVFX[i].frame = 0;
                    printf("FrostExplosion %d triggered at x=%d, y=%d\n", i, vfx_x, boss->frostExplosionVFX[i].y);
                    active_count++;
                    if (active_count >= max_explosions) break;
                }
            }
            lastFrostSpawn = currentTime;
        }
    }

    // Update all VFX and handle player freezing
    updateVFX(&boss->umbrellaVFX);
    updateVFX(&boss->iceSlashVFX);
    for (int i = 0; i < 3; i++) {
        updateVFX(&boss->frostExplosionVFX[i]);
        if (boss->frostExplosionVFX[i].active) {
            SDL_Rect iceHitbox = {boss->frostExplosionVFX[i].x, boss->frostExplosionVFX[i].y, ICE_FRAME_WIDTH, ICE_FRAME_HEIGHT};
            SDL_Rect playerHitbox = {game->player.world_x, game->player.position.y, 258, 258};
            if (rectIntersect(&iceHitbox, &playerHitbox)) {
                game->player.freezeYMovement = 1;
                boss->frostExplosionVFX[i].active = 0;
                printf("Player frozen by FrostExplosion %d\n", i);
            }
        }
    }
    // Unfreeze only if no VFX is active
    int anyActive = 0;
    for (int i = 0; i < 3; i++) {
        if (boss->frostExplosionVFX[i].active) anyActive = 1;
    }
    if (!anyActive && game->player.freezeYMovement) {
        game->player.freezeYMovement = 0;
        printf("Player unfrozen\n");
    }

    if (boss->health <= 0) {
        printf("Boss health <= 0, setting BOSS_DEATH\n");
        boss->state = BOSS_DEATH;
    } else if (boss->state == BOSS_TAKEHIT && boss->invulnTimer <= 0) {
        boss->state = BOSS_IDLE;
        boss->frame = 0;
    } else if (boss->invulnTimer <= 0) {
        if (distance > 2000) {
            boss->state = BOSS_IDLE;
        } else if (distance > 300) {
            boss->state = BOSS_WALK;
            int move_dx = (dx > 0) ? 2 : -2;
            boss->world_x += move_dx;
            boss->position.x = boss->world_x;
            if (boss->world_x < 0) boss->world_x = 0;
            if (boss->world_x > LEVEL2_WIDTH - boss->position.w) {
                boss->world_x = LEVEL2_WIDTH - boss->position.w;
            }
        } else if (distance <= 300 && boss->attackCooldown <= 0) {
            boss->state = BOSS_ATTACK;
            if (boss->frame >= 5 && boss->frame <= 7) {
                boss->iceSlashVFX.x = boss->world_x + (boss->facingLeft ? 100 : -100);
                boss->iceSlashVFX.y = boss->y + 150;
                boss->iceSlashVFX.active = 1;
            }
            SDL_Rect attackHitbox;
            int attackFrame = 7;
            if (boss->frame == attackFrame && !boss->hasDamagedThisAttack) {
                if (boss->facingLeft) {
                    attackHitbox = (SDL_Rect){boss->world_x + boss->position.w - 100, boss->y + 150, 100, 100};
                } else {
                    attackHitbox = (SDL_Rect){boss->world_x - 100, boss->y + 150, 100, 100};
                }
                SDL_Rect playerHitbox = {game->player.world_x, game->player.position.y, 258, 258};
                if (rectIntersect(&attackHitbox, &playerHitbox)) {
                    game->player.health -= 5;
                    if (game->player.health <= 0) game->player.state = DEAD;
                    else game->player.state = HURT;
                    boss->hasDamagedThisAttack = 1;
                }
            }
            if (boss->frame >= boss->totalFrames - 1) {
                boss->frame = 0;
                boss->state = BOSS_IDLE;
                boss->hasDamagedThisAttack = 0;
                boss->attackCooldown = boss->angry ? 60 : 120;
            }
        } else {
            boss->state = BOSS_IDLE;
        }
    }
    if (game->player.state == ATTACK_1 && boss->invulnTimer <= 0) {
        SDL_Rect attackRect = {
            game->player.position.x + (game->player.facing == RIGHT ? 258 : -100),
            game->player.position.y,
            100, 258
        };
        if (rectIntersect(&attackRect, &boss->position)) {
            boss->health -= 20;
            boss->state = BOSS_TAKEHIT;
            boss->frame = 0;
            boss->invulnTimer = 30;
        }
    }
    if (game->player.bulletActive && boss->invulnTimer <= 0) {
        SDL_Rect bulletRect = {game->player.bullet.x, game->player.bullet.y, game->player.bullet.w, game->player.bullet.h};
        if (rectIntersect(&bulletRect, &boss->position)) {
            boss->health -= 15;
            boss->state = BOSS_TAKEHIT;
            boss->frame = 0;
            boss->invulnTimer = 30;
            game->player.bulletActive = 0;
        }
    }
    if (++boss->frameDelay >= 5) {
        boss->frame++;
        if (boss->state == BOSS_IDLE || boss->state == BOSS_WALK) {
            boss->frame %= boss->totalFrames;
        } else if (boss->frame >= boss->totalFrames) {
            boss->frame = 0;
        }
        boss->frameDelay = 0;
    }
}

void renderBoss(SDL_Surface* screen, Boss* boss, int scroll_x, GAME* game) {
    if (!screen || !boss) return;

    // Render level-up icon regardless of boss->active
    if (boss->levelUpIconActive) {
        Uint32 currentTime = SDL_GetTicks();
        Uint32 elapsedTime = currentTime - boss->levelUpIconSpawnTime;
        // Render icon unless in flicker-off state during last 500ms
        if (elapsedTime < 4500 || (elapsedTime >= 4500 && (elapsedTime / 200) % 2 == 0)) { // Changed: Flicker starts at 4500ms, toggles every 200ms
            SDL_Rect iconDestRect = {boss->levelUpIconPosition.x - scroll_x, boss->levelUpIconPosition.y, 314, 392};
            // Clamp to screen bounds to ensure visibility
            iconDestRect.x = clamp(iconDestRect.x, scroll_x, scroll_x + SCREEN_WIDTH - 314);
            iconDestRect.y = clamp(iconDestRect.y, 0, SCREEN_HEIGHT - 392);
            if (SDL_BlitSurface(boss->levelUpIcon, NULL, screen, &iconDestRect) < 0) {
                fprintf(stderr, "renderBoss: SDL_BlitSurface failed for level-up icon: %s\n", SDL_GetError());
            } else {
                printf("Rendering level-up icon at screen_x=%d, screen_y=%d (world_x=%d, scroll_x=%d, elapsed=%u)\n", 
                       iconDestRect.x, iconDestRect.y, boss->levelUpIconPosition.x, scroll_x, elapsedTime);
            }
        }
    }

    if (!boss->active) return;

    SDL_Surface* frame = NULL;
    switch (boss->state) {
        case BOSS_IDLE: 
            if (boss->frame < boss->idleFrameCount) frame = boss->idleFrames[boss->frame]; 
            break;
        case BOSS_WALK: 
            if (boss->frame < boss->walkFrameCount) frame = boss->walkFrames[boss->frame]; 
            break;
        case BOSS_ATTACK: 
            if (boss->frame < boss->attackFrameCount) frame = boss->attackFrames[boss->frame]; 
            break;
        case BOSS_TAKEHIT: 
            if (boss->frame < boss->takehitFrameCount) frame = boss->takehitFrames[boss->frame]; 
            break;
        case BOSS_DEATH: 
            if (boss->frame < boss->deathFrameCount) frame = boss->deathFrames[boss->frame]; 
            break;
    }
    if (frame) {
        SDL_Rect dest = {boss->world_x - scroll_x, boss->y, 0, 0};
        if (boss->facingLeft) {
            SDL_Surface* flipped = flipHorizontally(frame);
            SDL_BlitSurface(flipped, NULL, screen, &dest);
            SDL_FreeSurface(flipped);
        } else {
            SDL_BlitSurface(frame, NULL, screen, &dest);
        }
    }
    if (boss->state != BOSS_DEATH) {
        int boss_hb_x = SCREEN_WIDTH / 2 - 100;
        int boss_hb_y = 10;
        int boss_hb_width = 200;
        int boss_hb_height = 20;
        SDL_Rect boss_bgRect = {boss_hb_x, boss_hb_y, boss_hb_width, boss_hb_height};
        SDL_FillRect(screen, &boss_bgRect, SDL_MapRGB(screen->format, 0, 0, 0));
        int boss_health_width = (int)((boss->health / (float)boss->maxHealth) * boss_hb_width);
        SDL_Rect boss_healthRect = {boss_hb_x, boss_hb_y, boss_health_width, boss_hb_height};
        SDL_FillRect(screen, &boss_healthRect, SDL_MapRGB(screen->format, 0, 0, 255));
    }
    renderVFX(screen, &boss->iceSlashVFX, scroll_x);
    for (int i = 0; i < 3; i++) {
        renderVFX(screen, &boss->frostExplosionVFX[i], scroll_x);
    }
    renderVFX(screen, &boss->umbrellaVFX, scroll_x);
}

void freeBoss(Boss* boss) {
    if (!boss) return;
    for (int i = 0; i < boss->idleFrameCount; i++) {
        if (boss->idleFrames[i]) SDL_FreeSurface(boss->idleFrames[i]);
    }
    free(boss->idleFrames);
    for (int i = 0; i < boss->walkFrameCount; i++) {
        if (boss->walkFrames[i]) SDL_FreeSurface(boss->walkFrames[i]);
    }
    free(boss->walkFrames);
    for (int i = 0; i < boss->attackFrameCount; i++) {
        if (boss->attackFrames[i]) SDL_FreeSurface(boss->attackFrames[i]);
    }
    free(boss->attackFrames);
    for (int i = 0; i < boss->takehitFrameCount; i++) {
        if (boss->takehitFrames[i]) SDL_FreeSurface(boss->takehitFrames[i]);
    }
    free(boss->takehitFrames);
    for (int i = 0; i < boss->deathFrameCount; i++) {
        if (boss->deathFrames[i]) SDL_FreeSurface(boss->deathFrames[i]);
    }
    free(boss->deathFrames);
    if (boss->levelUpIcon) SDL_FreeSurface(boss->levelUpIcon);
    freeVFX(&boss->iceSlashVFX);
    for (int i = 0; i < 3; i++) {
        freeVFX(&boss->frostExplosionVFX[i]);
    }
    freeVFX(&boss->umbrellaVFX);
    boss->active = 0;
}

void placeBossOnGround(GAME* game, Boss* boss) {
    if (!game || !game->background.levelCollision || !boss) {
        fprintf(stderr, "placeBossOnGround: Game, boss, or levelCollision is NULL\n");
        return;
    }
    int x = boss->world_x + 256;
    int y_offset = (SCREEN_HEIGHT - game->background.levelCollision->h) / 2;
    int y_start = boss->y - y_offset;
    if (x < 0 || x >= game->background.levelCollision->w || y_start < -50 || y_start >= game->background.levelCollision->h) {
        printf("placeBossOnGround: Invalid coordinates x=%d, y_start=%d\n", x, y_start);
        return;
    }
    for (int y = y_start - 40; y <= y_start + 40 && y < game->background.levelCollision->h; y++) {
        if (y < 0) continue;
        SDL_Color color = get_pixel(game->background.levelCollision, x, y);
        if (color.r == 255 && color.g == 255 && color.b == 255) {
            boss->y = y - 512 + y_offset;
            boss->position.y = boss->y;
            return;
        }
    }
    boss->y = (game->background.levelCollision->h - 512) + y_offset;
    boss->position.y = boss->y;
}
