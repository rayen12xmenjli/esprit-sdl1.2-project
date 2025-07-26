#include "jet.h"
#include <SDL/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

// Debug logging toggle
#define DEBUG_LOG 1
#define LOG(fmt, ...) if (DEBUG_LOG) fprintf(stderr, fmt, ##__VA_ARGS__)

void initJet(Jet* jet) {
    if (!jet) {
        fprintf(stderr, "initJet: Jet pointer is NULL\n");
        exit(1);
    }
    LOG("Initializing jet\n");

    jet->spriteSheet = IMG_Load("assets/enemies/jet/jet.png");
    if (!jet->spriteSheet) {
        fprintf(stderr, "Failed to load jet.png: %s\n", IMG_GetError());
        exit(1);
    }
    Uint32 colorkey = SDL_MapRGB(jet->spriteSheet->format, 255, 255, 255);
    SDL_SetColorKey(jet->spriteSheet, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
    SDL_Surface* optimized = SDL_DisplayFormatAlpha(jet->spriteSheet);
    if (!optimized) {
        fprintf(stderr, "SDL_DisplayFormatAlpha failed for jet.png: %s\n", SDL_GetError());
        SDL_FreeSurface(jet->spriteSheet);
        exit(1);
    }
    SDL_FreeSurface(jet->spriteSheet);
    jet->spriteSheet = optimized;

    jet->bomb.spriteSheet = IMG_Load("assets/enemies/jet/bomb.png");
    if (!jet->bomb.spriteSheet) {
        fprintf(stderr, "Failed to load bomb.png: %s\n", IMG_GetError());
        SDL_FreeSurface(jet->spriteSheet);
        exit(1);
    }
    SDL_SetColorKey(jet->bomb.spriteSheet, SDL_SRCCOLORKEY | SDL_RLEACCEL,
                    SDL_MapRGB(jet->bomb.spriteSheet->format, 255, 255, 255));
    optimized = SDL_DisplayFormatAlpha(jet->bomb.spriteSheet);
    if (!optimized) {
        fprintf(stderr, "SDL_DisplayFormatAlpha failed for bomb.png: %s\n", SDL_GetError());
        SDL_FreeSurface(jet->spriteSheet);
        SDL_FreeSurface(jet->bomb.spriteSheet);
        exit(1);
    }
    SDL_FreeSurface(jet->bomb.spriteSheet);
    jet->bomb.spriteSheet = optimized;

    char filename[50];
    for (int i = 0; i < 27; i++) {
        sprintf(filename, "assets/enemies/jet/fire/%02d.png", i + 1);
        jet->bomb.fire.frames[i] = IMG_Load(filename);
        if (!jet->bomb.fire.frames[i]) {
            fprintf(stderr, "Failed to load %s: %s\n", filename, IMG_GetError());
            for (int j = 0; j < i; j++) {
                SDL_FreeSurface(jet->bomb.fire.frames[j]);
            }
            SDL_FreeSurface(jet->spriteSheet);
            SDL_FreeSurface(jet->bomb.spriteSheet);
            exit(1);
        }
        SDL_SetColorKey(jet->bomb.fire.frames[i], SDL_SRCCOLORKEY | SDL_RLEACCEL,
                        SDL_MapRGB(jet->bomb.fire.frames[i]->format, 255, 255, 255));
        optimized = SDL_DisplayFormatAlpha(jet->bomb.fire.frames[i]);
        if (!optimized) {
            fprintf(stderr, "SDL_DisplayFormatAlpha failed for %s: %s\n", filename, SDL_GetError());
            for (int j = 0; j < i; j++) {
                SDL_FreeSurface(jet->bomb.fire.frames[j]);
            }
            SDL_FreeSurface(jet->bomb.fire.frames[i]);
            SDL_FreeSurface(jet->spriteSheet);
            SDL_FreeSurface(jet->bomb.spriteSheet);
            exit(1);
        }
        SDL_FreeSurface(jet->bomb.fire.frames[i]);
        jet->bomb.fire.frames[i] = optimized;
    }

    jet->position.x = -256;
    jet->position.y = 100;
    jet->position.w = 256;
    jet->position.h = 256;
    jet->state = JET_OFFSCREEN;
    jet->speed = 5;
    jet->frame = 0;
    jet->active = 0;
    jet->lastSpawnTime = 0;

    jet->bomb.position.w = 16;
    jet->bomb.position.h = 16;
    jet->bomb.active = 0;
    jet->bomb.frame = 0;
    jet->bomb.yVelocity = 5.0f;

    jet->bomb.fire.position.w = 547;
    jet->bomb.fire.position.h = 483;
    jet->bomb.fire.active = 0;
    jet->bomb.fire.currentFrame = 0;

    LOG("Jet initialized: spriteSheet=%p, bomb.spriteSheet=%p, colorkey=%u\n",
        (void *)jet->spriteSheet, (void *)jet->bomb.spriteSheet, colorkey);
}

void updateJet(Jet* jet, Player* player, int scroll_x) {
    if (!jet || !player) {
        LOG("updateJet: Invalid parameters\n");
        return;
    }

    Uint32 currentTime = SDL_GetTicks();
    static int hasDroppedBomb = 0;

    if (!jet->active && (currentTime - jet->lastSpawnTime >= 10000)) {
        jet->active = 1;
        jet->state = JET_FLYING;
        jet->position.x = -256;
        jet->position.y = 100;
        jet->bomb.active = 0;
        jet->frame = 0;
        jet->bomb.fire.active = 0;
        hasDroppedBomb = 0;
        LOG("Jet spawned at x=%d, y=%d\n", jet->position.x, jet->position.y);
    }

    if (jet->active && jet->state == JET_FLYING) {
        jet->position.x += jet->speed;

        static int jetFrameDelay = 0;
        if (++jetFrameDelay >= 5) {
            jet->frame = (jet->frame + 1) % 6;
            jetFrameDelay = 0;
        }

        if (!jet->bomb.active && !hasDroppedBomb &&
            jet->position.x >= scroll_x + SCREEN_WIDTH / 4 &&
            currentTime >= jet->lastSpawnTime + 2000) {
            jet->bomb.active = 1;
            hasDroppedBomb = 1;
            jet->bomb.position.x = jet->position.x + jet->position.w / 2 - jet->bomb.position.w / 2;
            jet->bomb.position.y = jet->position.y + jet->position.h;
            jet->bomb.frame = 0;
            LOG("Bomb dropped at x=%d, y=%d\n", jet->bomb.position.x, jet->bomb.position.y);
        }

        if (jet->position.x > scroll_x + SCREEN_WIDTH) {
            jet->state = JET_OFFSCREEN;
            jet->active = 0;
            jet->lastSpawnTime = SDL_GetTicks();
            hasDroppedBomb = 0;
            LOG("Jet despawned\n");
        }
    }

    if (jet->bomb.active) {
        jet->bomb.position.y += (int)jet->bomb.yVelocity;

        static int bombFrameDelay = 0;
        if (++bombFrameDelay >= 5) {
            jet->bomb.frame = (jet->bomb.frame + 1) % 6;
            bombFrameDelay = 0;
        }

        int groundLevel = SCREEN_HEIGHT - 100;
        if (jet->bomb.position.y >= groundLevel) {
            jet->bomb.fire.active = 1;
            jet->bomb.fire.currentFrame = 0;
            jet->bomb.fire.position.x = jet->bomb.position.x - (jet->bomb.fire.position.w / 2 - jet->bomb.position.w / 2);
            jet->bomb.fire.position.y = groundLevel - (jet->bomb.fire.position.h / 2);
            jet->bomb.active = 0;
            LOG("Bomb hit ground at y=%d\n", groundLevel);
        }
    }

    if (jet->bomb.fire.active) {
        static int fireFrameDelay = 0;
        if (++fireFrameDelay >= 5) {
            jet->bomb.fire.currentFrame++;
            if (jet->bomb.fire.currentFrame >= 27) {
                jet->bomb.fire.active = 0;
                jet->bomb.fire.currentFrame = 0;
            }
            fireFrameDelay = 0;
        }
    }

    LOG("Jet updated: x=%d, y=%d, state=%d, frame=%d, active=%d, bomb.active=%d\n",
        jet->position.x, jet->position.y, jet->state, jet->frame, jet->active, jet->bomb.active);
}

void renderJet(SDL_Surface* screen, Jet* jet, int scroll_x) {
    if (!screen || !jet) {
        LOG("renderJet: Invalid parameters\n");
        return;
    }

    if (jet->active && jet->state == JET_FLYING) {
        SDL_Rect srcRect = {jet->frame * 256, 0, 256, 256};
        SDL_Rect destRect = {jet->position.x - scroll_x, jet->position.y, 256, 256};
        SDL_BlitSurface(jet->spriteSheet, &srcRect, screen, &destRect);
    }

    if (jet->bomb.active) {
        SDL_Rect bombSrcRect = {jet->bomb.frame * 16, 0, 16, 16};
        SDL_Rect bombDestRect = {jet->bomb.position.x - scroll_x, jet->bomb.position.y, 16, 16};
        SDL_BlitSurface(jet->bomb.spriteSheet, &bombSrcRect, screen, &bombDestRect);
    }

    if (jet->bomb.fire.active) {
        SDL_Rect fireDestRect = {jet->bomb.fire.position.x - scroll_x, jet->bomb.fire.position.y, 547, 483};
        SDL_BlitSurface(jet->bomb.fire.frames[jet->bomb.fire.currentFrame], NULL, screen, &fireDestRect);
    }

    LOG("Jet rendered: x=%d, y=%d, state=%d, frame=%d, active=%d, bomb.active=%d\n",
        jet->position.x, jet->position.y, jet->state, jet->frame, jet->active, jet->bomb.active);
}

void freeJet(Jet* jet) {
    if (!jet) {
        LOG("freeJet: Jet pointer is NULL\n");
        return;
    }

    if (jet->spriteSheet) {
        SDL_FreeSurface(jet->spriteSheet);
        jet->spriteSheet = NULL;
    }
    if (jet->bomb.spriteSheet) {
        SDL_FreeSurface(jet->bomb.spriteSheet);
        jet->bomb.spriteSheet = NULL;
    }
    for (int i = 0; i < 27; i++) {
        if (jet->bomb.fire.frames[i]) {
            SDL_FreeSurface(jet->bomb.fire.frames[i]);
            jet->bomb.fire.frames[i] = NULL;
        }
    }
    jet->active = 0;
    LOG("Jet resources freed\n");
}
