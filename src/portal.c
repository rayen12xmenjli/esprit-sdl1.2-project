#include "portal.h"
#include <SDL/SDL_image.h>
#include <stdlib.h>
#include <stdio.h>

void initPortal(Portal* portal, int x, int y) {
    char filename[64];
    for (int i = 0; i < 7; i++) {
        snprintf(filename, sizeof(filename), "assets/portal/portal1_frame_%d.png", i + 1);
        portal->frames[i] = IMG_Load(filename);
        if (!portal->frames[i]) {
            printf("Failed to load %s: %s\n", filename, SDL_GetError());
            for (int j = 0; j < i; j++) {
                SDL_FreeSurface(portal->frames[j]);
            }
            exit(1);
        }
        SDL_SetColorKey(portal->frames[i], SDL_SRCCOLORKEY, SDL_MapRGB(portal->frames[i]->format, 255, 0, 255));
    }
    portal->position.x = x;
    portal->position.y = y;
    portal->position.w = 256;
    portal->position.h = 256;
    portal->frame = 0;
    portal->totalFrames = 7;
    portal->active = 1;
}

void updatePortal(Portal* portal) {
    if (!portal->active) return;
    static int frameDelay = 0;
    if (++frameDelay >= 10) {
        portal->frame = (portal->frame + 1) % portal->totalFrames;
        frameDelay = 0;
    }
}

void renderPortal(SDL_Surface* screen, Portal* portal, int scroll_x) {
    if (!portal->active) return;
    SDL_Rect adjustedPos = {portal->position.x - scroll_x, portal->position.y, portal->position.w, portal->position.h};
    SDL_BlitSurface(portal->frames[portal->frame], NULL, screen, &adjustedPos);
}

void freePortal(Portal* portal) {
    for (int i = 0; i < 7; i++) {
        if (portal->frames[i]) {
            SDL_FreeSurface(portal->frames[i]);
            portal->frames[i] = NULL;
        }
    }
    portal->active = 0;
}
