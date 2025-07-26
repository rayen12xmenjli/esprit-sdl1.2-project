#ifndef PORTAL_H
#define PORTAL_H

#include <SDL/SDL.h>
#include "utils.h"

typedef struct {
    SDL_Rect position;      
    SDL_Surface* frames[7]; 
    int frame;              
    int totalFrames;        
    int active;             
} Portal;

void initPortal(Portal* portal, int x, int y);
void updatePortal(Portal* portal);
void renderPortal(SDL_Surface* screen, Portal* portal, int scroll_x);
void freePortal(Portal* portal);

#endif
