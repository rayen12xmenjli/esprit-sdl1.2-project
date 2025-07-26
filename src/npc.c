#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <stdio.h>
#include <string.h>
#include "game.h"
#include "npc.h"
#include "collision.h"
#include "mouvement.h"

void initNPC(NPC *npc, int x, int y, const char *dialogue, GAME *game) {
    if (!npc || !game) {
        fprintf(stderr, "initNPC: NPC or game pointer is NULL\n");
        return;
    }

    npc->position.x = x;
    npc->position.y = y;
    npc->position.w = 256; // Updated to 256
    npc->position.h = 256; // Updated to 256
    npc->world_x = x;

    char path[50];
    sprintf(path, "assets/npc/npc_idle.png");
    npc->idleSheet = IMG_Load(path);
    if (!npc->idleSheet) fprintf(stderr, "Failed to load %s: %s\n", path, IMG_GetError());
    else SDL_SetColorKey(npc->idleSheet, SDL_SRCCOLORKEY, SDL_MapRGB(npc->idleSheet->format, 255, 0, 255));

    sprintf(path, "assets/npc/npc_idle2.png");
    npc->idle2Sheet = IMG_Load(path);
    if (!npc->idle2Sheet) fprintf(stderr, "Failed to load %s: %s\n", path, IMG_GetError());
    else SDL_SetColorKey(npc->idle2Sheet, SDL_SRCCOLORKEY, SDL_MapRGB(npc->idle2Sheet->format, 255, 0, 255));

    sprintf(path, "assets/npc/npc_idle3.png");
    npc->idle3Sheet = IMG_Load(path);
    if (!npc->idle3Sheet) fprintf(stderr, "Failed to load %s: %s\n", path, IMG_GetError());
    else SDL_SetColorKey(npc->idle3Sheet, SDL_SRCCOLORKEY, SDL_MapRGB(npc->idle3Sheet->format, 255, 0, 255));

    sprintf(path, "assets/npc/npc_dialogue.png");
    npc->dialogueSheet = IMG_Load(path);
    if (!npc->dialogueSheet) fprintf(stderr, "Failed to load %s: %s\n", path, IMG_GetError());
    else SDL_SetColorKey(npc->dialogueSheet, SDL_SRCCOLORKEY, SDL_MapRGB(npc->dialogueSheet->format, 255, 0, 255));

    sprintf(path, "assets/npc/npc_approval.png");
    npc->approvalSheet = IMG_Load(path);
    if (!npc->approvalSheet) fprintf(stderr, "Failed to load %s: %s\n", path, IMG_GetError());
    else SDL_SetColorKey(npc->approvalSheet, SDL_SRCCOLORKEY, SDL_MapRGB(npc->approvalSheet->format, 255, 0, 255));

    // Load health icon for NPC 1 (health restoration NPC)
    if (game->level == 1 && strcmp(dialogue, "I restore health!") == 0) {
        npc->healthIcon = IMG_Load("assets/icons/icon28.png");
        if (!npc->healthIcon || npc->healthIcon->w != 88 || npc->healthIcon->h != 88) {
            fprintf(stderr, "Failed to load or invalid dimensions for icon28.png: %s\n", IMG_GetError());
            if (npc->healthIcon) SDL_FreeSurface(npc->healthIcon);
            npc->healthIcon = NULL;
        } else {
            SDL_SetColorKey(npc->healthIcon, SDL_SRCCOLORKEY, SDL_MapRGB(npc->healthIcon->format, 255, 0, 255));
            printf("Health icon loaded for NPC\n");
        }
    } else {
        npc->healthIcon = NULL;
    }

    npc->state = NPC_IDLE;
    npc->frame = 0;
    npc->frameCount = 6;
    npc->frameTimer = 0;
    npc->frameDelay = 500;
    strncpy(npc->dialogue, dialogue, sizeof(npc->dialogue) - 1);
    npc->dialogue[sizeof(npc->dialogue) - 1] = '\0';
    npc->active = 1;
    npc->dealing = 0;
    npc->dealTimer = 0;
    npc->showHealthIcon = 0;
    npc->healthIconDisplayTime = 0;

    placeNPCOnGround(game, npc);
    printf("NPC initialized at x=%d, y=%d, dialogue='%s'\n", x, y, npc->dialogue);
}

void freeNPC(NPC *npc) {
    if (!npc) {
        fprintf(stderr, "freeNPC: NPC pointer is NULL\n");
        return;
    }

    if (npc->idleSheet) {
        SDL_FreeSurface(npc->idleSheet);
        npc->idleSheet = NULL;
    }
    if (npc->idle2Sheet) {
        SDL_FreeSurface(npc->idle2Sheet);
        npc->idle2Sheet = NULL;
    }
    if (npc->idle3Sheet) {
        SDL_FreeSurface(npc->idle3Sheet);
        npc->idle3Sheet = NULL;
    }
    if (npc->dialogueSheet) {
        SDL_FreeSurface(npc->dialogueSheet);
        npc->dialogueSheet = NULL;
    }
    if (npc->approvalSheet) {
        SDL_FreeSurface(npc->approvalSheet);
        npc->approvalSheet = NULL;
    }
    if (npc->healthIcon) {
        SDL_FreeSurface(npc->healthIcon);
        npc->healthIcon = NULL;
    }
    npc->active = 0;
}

void updateNPC(NPC *npc, float deltaTime) {
    if (!npc || !npc->active) return;

    npc->frameTimer += (int)(deltaTime * 1000);
    if (npc->frameTimer >= npc->frameDelay) {
        npc->frameTimer = 0;

        npc->frame++;
        switch (npc->state) {
            case NPC_IDLE:
                npc->frameCount = 6;
                if (npc->frame >= 6) npc->frame = 0;
                break;
            case NPC_IDLE_2:
                npc->frameCount = 11;
                if (npc->frame >= 11) npc->frame = 0;
                break;
            case NPC_IDLE_3:
                npc->frameCount = 7;
                if (npc->frame >= 7) npc->frame = 0;
                break;
            case NPC_DIALOGUE:
                npc->frameCount = 16;
                if (npc->frame >= 16) npc->frame = 0;
                break;
            case NPC_APPROVAL:
                npc->frameCount = 8;
                if (npc->frame >= 8) npc->frame = 0;
                npc->dealTimer += npc->frameDelay;
                if (npc->dealTimer >= 3000) {
                    npc->state = NPC_IDLE;
                    npc->dealing = 0;
                    npc->dealTimer = 0;
                    npc->frame = 0;
                }
                break;
        }

        if (npc->state == NPC_IDLE && npc->frame == 0) {
            npc->state = NPC_IDLE_2;
        } else if (npc->state == NPC_IDLE_2 && npc->frame == 0) {
            npc->state = NPC_IDLE_3;
        } else if (npc->state == NPC_IDLE_3 && npc->frame == 0) {
            npc->state = NPC_IDLE;
        }
    }

    // Hide health icon after 2 seconds
    if (npc->showHealthIcon && (SDL_GetTicks() - npc->healthIconDisplayTime >= 2000)) {
        npc->showHealthIcon = 0;
        printf("Health icon hidden for NPC\n");
    }
}

void renderNPC(SDL_Surface *screen, NPC *npc, int scroll_x, GAME *game) {
    if (!npc || !npc->active || !screen || !game) return;

    SDL_Surface *sheet = NULL;
    switch (npc->state) {
        case NPC_IDLE:
            sheet = npc->idleSheet;
            break;
        case NPC_IDLE_2:
            sheet = npc->idle2Sheet;
            break;
        case NPC_IDLE_3:
            sheet = npc->idle3Sheet;
            break;
        case NPC_DIALOGUE:
            sheet = npc->dialogueSheet;
            break;
        case NPC_APPROVAL:
            sheet = npc->approvalSheet;
            break;
    }

    if (!sheet) {
        fprintf(stderr, "renderNPC: No valid sheet for state %d\n", npc->state);
        return;
    }

    SDL_Rect src = {npc->frame * 256, 0, 256, 256}; // Updated to 256x256
    SDL_Rect dest = {npc->world_x - scroll_x, npc->position.y, 256, 256}; // Updated to 256x256
    if (SDL_BlitSurface(sheet, &src, screen, &dest) < 0) {
        fprintf(stderr, "renderNPC: SDL_BlitSurface failed: %s\n", SDL_GetError());
    }

    // Render health icon if active
    if (npc->showHealthIcon && npc->healthIcon) {
        int iconWidth = npc->healthIcon->w; // 88 pixels
        int iconHeight = npc->healthIcon->h; // 88 pixels
        int startX = (npc->world_x - scroll_x) + (npc->position.w / 2) - (iconWidth / 2); // Center horizontally
        int startY = npc->position.y - iconHeight - 10; // 10 pixels above NPC
        SDL_Rect iconDest = {startX, startY, iconWidth, iconHeight};
        if (SDL_BlitSurface(npc->healthIcon, NULL, screen, &iconDest) < 0) {
            fprintf(stderr, "renderNPC: SDL_BlitSurface failed for health icon: %s\n", SDL_GetError());
        }
    }
}


