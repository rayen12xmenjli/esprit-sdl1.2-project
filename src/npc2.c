#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "npc2.h"
#include "collision.h"
#include "utils.h"

void initNPC2(NPC2* npc, int x, int y, GAME* game) {
    if (!npc || !game) {
        fprintf(stderr, "initNPC2: Invalid pointers\n");
        return;
    }

    // Initialize position and world coordinates
    npc->position.x = x;
    npc->position.y = y; // Adjusted later by placeNPCOnGround
    npc->position.w = 128;
    npc->position.h = 128;
    npc->world_x = x;
    npc->initialX = x;
    npc->active = 1;

    // Load sprite sheets and bust image
    char path[50];
    sprintf(path, "assets/npc/scientist_movement.png");
    npc->movementSheet = IMG_Load(path);
    if (!npc->movementSheet) {
        fprintf(stderr, "initNPC2: Failed to load %s: %s\n", path, IMG_GetError());
    } else {
        SDL_SetColorKey(npc->movementSheet, SDL_SRCCOLORKEY, SDL_MapRGB(npc->movementSheet->format, 255, 0, 255));
        printf("initNPC2: Loaded movementSheet (%dx%d)\n", npc->movementSheet->w, npc->movementSheet->h);
    }

    sprintf(path, "assets/npc/scientist_dialogue.png");
    npc->dialogueSheet = IMG_Load(path);
    if (!npc->dialogueSheet) {
        fprintf(stderr, "initNPC2: Failed to load %s: %s\n", path, IMG_GetError());
    } else {
        SDL_SetColorKey(npc->dialogueSheet, SDL_SRCCOLORKEY, SDL_MapRGB(npc->dialogueSheet->format, 255, 0, 255));
        printf("initNPC2: Loaded dialogueSheet (%dx%d)\n", npc->dialogueSheet->w, npc->dialogueSheet->h);
    }

    sprintf(path, "assets/npc/mad_scientist_bust.png");
    npc->bustImage = IMG_Load(path);
    if (!npc->bustImage) {
        fprintf(stderr, "initNPC2: Failed to load %s: %s\n", path, IMG_GetError());
    } else {
        SDL_SetColorKey(npc->bustImage, SDL_SRCCOLORKEY, SDL_MapRGB(npc->bustImage->format, 255, 0, 255));
        printf("initNPC2: Loaded bustImage (%dx%d)\n", npc->bustImage->w, npc->bustImage->h);
    }

    // Use the game's font
    npc->font = game->ui.font;
    if (!npc->font) {
        fprintf(stderr, "initNPC2: Warning: No font provided by GAME structure\n");
    } else {
        printf("initNPC2: Font loaded successfully\n");
    }

    // Set dialogue text
    npc->dialogueText[0] = "Welcome, hero! The city is under attack!";
    npc->dialogueText[1] = "Jets are dropping bombs everywhere!";
    npc->dialogueText[2] = "You must stop them to save us all!";
    npc->dialogueText[3] = "Beware the explosions when bombs land!";
    npc->dialogueText[4] = "Collect bonuses to restore your health!";
    npc->dialogueText[5] = "Good luck, and stay safe out there!";

    // Initialize state and properties
    npc->state = NPC2_IDLE;
    npc->frame = 0;
    npc->speed = 1;
    npc->dialogueActive = 0;
    npc->dialogueLine = 0;
    npc->dialogueFrame = 0;
    npc->lastMoveTime = SDL_GetTicks();

    // Place NPC on ground
    placeNPC2OnGround(game, (NPC2*)npc);
    printf("initNPC2: NPC2 initialized at world_x=%d, y=%d\n", npc->world_x, npc->position.y);
}

void updateNPC2(NPC2* npc, float deltaTime, SDL_Rect* playerPos) {
    if (!npc || !npc->active || !playerPos) return;

    if (!npc->dialogueActive) {
        // Handle movement state changes (every 3 seconds)
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - npc->lastMoveTime >= 3000) {
            int randomState = rand() % 3;
            npc->state = (randomState == 0) ? NPC2_IDLE : (randomState == 1) ? NPC2_MOVING_LEFT : NPC2_MOVING_RIGHT;
            npc->lastMoveTime = currentTime;
            npc->frame = 0;
            printf("updateNPC2: State changed to %d\n", npc->state);
        }

        // Update world_x based on movement
        if (npc->state == NPC2_MOVING_LEFT) {
            npc->world_x -= npc->speed;
            if (npc->world_x < npc->initialX - 50) npc->world_x = npc->initialX - 50;
        } else if (npc->state == NPC2_MOVING_RIGHT) {
            npc->world_x += npc->speed;
            if (npc->world_x > npc->initialX + 50) npc->world_x = npc->initialX + 50;
        }

        // Animate movement (frame update every 0.083 seconds, ~5 frames at 60 FPS)
        static float moveFrameTimer = 0;
        moveFrameTimer += deltaTime;
        if (moveFrameTimer >= 0.083f) {
            npc->frame = (npc->frame + 1) % 3;
            moveFrameTimer = 0;
        }
    } else {
        npc->state = NPC2_IDLE;
        npc->frame = 0;
    }

    // Collision check with player
    SDL_Rect npcWorldPos = {npc->world_x, npc->position.y, npc->position.w, npc->position.h};
    if (rectIntersect(playerPos, &npcWorldPos)) {
        Uint8* keystate = SDL_GetKeyState(NULL);
        if (keystate[SDLK_e]) {
            if (!npc->dialogueActive) {
                npc->dialogueActive = 1;
                npc->dialogueLine = 0;
                npc->dialogueFrame = 0;
                printf("updateNPC2: Dialogue started\n");
            } else {
                npc->dialogueLine++;
                npc->dialogueFrame = 0;
                if (npc->dialogueLine >= 6) {
                    npc->dialogueActive = 0;
                    npc->dialogueLine = 0;
                    printf("updateNPC2: Dialogue ended\n");
                } else {
                    printf("updateNPC2: Dialogue line %d\n", npc->dialogueLine);
                }
            }
            SDL_Delay(200); // Debounce
        }
    } else {
        if (npc->dialogueActive) {
            npc->dialogueActive = 0;
            npc->dialogueLine = 0;
            npc->dialogueFrame = 0;
            printf("updateNPC2: Dialogue cancelled (player moved away)\n");
        }
    }

    // Animate dialogue
    if (npc->dialogueActive) {
        static float dialogueFrameTimer = 0;
        dialogueFrameTimer += deltaTime;
        if (dialogueFrameTimer >= 0.166f) {
            npc->dialogueFrame = (npc->dialogueFrame + 1) % 9;
            dialogueFrameTimer = 0;
        }
    }
}

void renderNPC2(SDL_Surface* screen, NPC2* npc, int scroll_x, GAME* game) {
    if (!npc || !npc->active || !screen || !game) {
        printf("renderNPC2: Invalid parameters\n");
        return;
    }

    if (!npc->movementSheet || !npc->dialogueSheet || !npc->bustImage) {
        printf("renderNPC2: Missing sprite sheets\n");
        return;
    }

    int screenX = npc->world_x - scroll_x;
    // Use npc->position.y directly, as set by placeNPCOnGround
    int yPos = npc->position.y;

    printf("renderNPC2: Rendering at screenX=%d, yPos=%d, dialogueActive=%d\n", screenX, yPos, npc->dialogueActive);

    if (!npc->dialogueActive) {
        // Render movement/idle animation
        SDL_Rect srcRect = {npc->frame * 128, 0, 128, 128};
        if (npc->state == NPC2_IDLE) {
            srcRect.y = 128; // Left-facing for idle
        } else if (npc->state == NPC2_MOVING_LEFT) {
            srcRect.y = 128;
        } else if (npc->state == NPC2_MOVING_RIGHT) {
            srcRect.y = 96;
        }
        SDL_Rect destRect = {screenX, yPos, 128, 128};
        if (SDL_BlitSurface(npc->movementSheet, &srcRect, screen, &destRect) != 0) {
            printf("renderNPC2: Failed to blit movementSheet: %s\n", SDL_GetError());
        }
    } else {
        // Render dialogue animation
        SDL_Rect dialogueSrcRect = {npc->dialogueFrame * 128, npc->dialogueLine * 128, 128, 128};
        SDL_Rect dialogueDestRect = {screenX + 128 - 32, yPos - 16, 128, 128}; // Adjusted to align above NPC
        if (SDL_BlitSurface(npc->dialogueSheet, &dialogueSrcRect, screen, &dialogueDestRect) != 0) {
            printf("renderNPC2: Failed to blit dialogueSheet: %s\n", SDL_GetError());
        }

        // Render dialogue box
        SDL_Rect dialogueBox = {screenX + 128 + 10, yPos - 10, 300, 80};
        SDL_FillRect(screen, &dialogueBox, SDL_MapRGB(screen->format, 50, 50, 50));

        // Render dialogue text
        if (npc->font) {
            SDL_Color textColor = {255, 255, 255};
            SDL_Surface* textSurface = TTF_RenderText_Solid(npc->font, npc->dialogueText[npc->dialogueLine], textColor);
            if (textSurface) {
                SDL_Rect textDest = {dialogueBox.x + 10, dialogueBox.y + 10, textSurface->w, textSurface->h};
                if (SDL_BlitSurface(textSurface, NULL, screen, &textDest) != 0) {
                    printf("renderNPC2: Failed to blit textSurface: %s\n", SDL_GetError());
                }
                SDL_FreeSurface(textSurface);
            } else {
                printf("renderNPC2: Failed to render text: %s\n", TTF_GetError());
            }
        }

        // Render bust image
        SDL_Rect bustDestRect = {screenX - npc->bustImage->w - 10, yPos + 128 - npc->bustImage->h, npc->bustImage->w, npc->bustImage->h};
        if (SDL_BlitSurface(npc->bustImage, NULL, screen, &bustDestRect) != 0) {
            printf("renderNPC2: Failed to blit bustImage: %s\n", SDL_GetError());
        }
    }
}

void freeNPC2(NPC2* npc) {
    if (!npc) return;

    if (npc->movementSheet) SDL_FreeSurface(npc->movementSheet);
    if (npc->dialogueSheet) SDL_FreeSurface(npc->dialogueSheet);
    if (npc->bustImage) SDL_FreeSurface(npc->bustImage);
    npc->active = 0;
}
