#ifndef NPC_H
#define NPC_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "game.h"

// NPC States
typedef enum {
    NPC_IDLE,
    NPC_IDLE_2,
    NPC_IDLE_3,
    NPC_DIALOGUE,
    NPC_APPROVAL
} NPCState;

// NPC Structure
typedef struct NPC {
    SDL_Rect position;
    int world_x;
    SDL_Surface *idleSheet; // 6 frames
    SDL_Surface *idle2Sheet; // 11 frames
    SDL_Surface *idle3Sheet; // 7 frames
    SDL_Surface *dialogueSheet; // 16 frames
    SDL_Surface *approvalSheet; // 8 frames
    NPCState state;
    int frame; // Current frame in animation
    int frameCount; // Total frames for the current state
    int frameTimer; // Timer for frame switching
    int frameDelay; // Delay between frames (ms)
    char dialogue[256]; // Dialogue text
    int active;
    int dealing; // Flag to indicate if a deal is in progress
    int dealTimer; // Timer for approval state
    SDL_Surface *healthIcon; // Icon for health restoration
    int showHealthIcon; // Flag to show/hide icon
    Uint32 healthIconDisplayTime; // Time when icon was shown
} NPC;

// Function prototypes
void initNPC(NPC *npc, int x, int y, const char *dialogue, GAME *game);
void freeNPC(NPC *npc);
void updateNPC(NPC *npc, float deltaTime);
void renderNPC(SDL_Surface *screen, NPC *npc, int scroll_x, GAME *game);
void placeNPCOnGround(GAME *game, NPC *npc);

#endif
