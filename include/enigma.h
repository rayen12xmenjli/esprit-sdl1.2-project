#ifndef ENIGMA_H
#define ENIGMA_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include "game.h" // Include the header where GameState is defined

typedef struct {
    SDL_Surface *screen;
    SDL_Surface *background;
    Mix_Music *suspense_music;
    Mix_Chunk *hover_sound;
    Mix_Chunk *click_sound;
    TTF_Font *font;
    SDL_Rect quiz_button;
    SDL_Rect puzzle_button;
    SDL_Rect answer_buttons[3]; // For Quiz mode
    int quiz_active;            // 0 = main enigma menu, 1 = quiz mode
} Enigma;

// Function prototypes
void initEnigma(Enigma *enigma, SDL_Surface *screen, GameState *state, Mix_Chunk *hover_sound, Mix_Chunk *click_sound, TTF_Font *font);
void updateEnigma(Enigma *enigma);
void renderEnigma(Enigma *enigma);
void handleEnigmaEvents(Enigma *enigma, SDL_Event *event);
void cleanupEnigma(Enigma *enigma);

#endif // ENIGMA_H
