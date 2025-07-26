#ifndef NPC2_H
#define NPC2_H

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include "game.h"

typedef enum { NPC2_IDLE, NPC2_MOVING_LEFT, NPC2_MOVING_RIGHT } NPC2State;

typedef struct {
    SDL_Surface* movementSheet;
    SDL_Surface* dialogueSheet;
    SDL_Surface* bustImage;
    TTF_Font* font;
    SDL_Rect position;
    int world_x;
    int initialX;
    int active;
    NPC2State state;
    int frame;
    int speed;
    int dialogueActive;
    int dialogueLine;
    int dialogueFrame;
    Uint32 lastMoveTime;
    char* dialogueText[6];
    // New fields for dynamic sprite sizes
    int moveFrameWidth;
    int moveFrameHeight;
    int dialogueFrameWidth;
    int dialogueFrameHeight;
} NPC2;

void initNPC2(NPC2* npc, int x, int y, GAME* game);
void updateNPC2(NPC2* npc, float deltaTime, SDL_Rect* playerPos);
void renderNPC2(SDL_Surface* screen, NPC2* npc, int scroll_x, GAME* game);
void freeNPC2(NPC2* npc);

#endif
