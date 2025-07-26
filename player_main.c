#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include "player_menu.h"
#include "sound.h"
#include <stdio.h>

int main(int ac, char *av[]) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "Erreur lors de l'initialisation de SDL : %s\n", SDL_GetError());
        return -1;
    }

    if (TTF_Init() == -1) {
        fprintf(stderr, "Erreur lors de l'initialisation de SDL_ttf : %s\n", TTF_GetError());
        SDL_Quit();
        return -1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "Erreur lors de l'initialisation de SDL_mixer : %s\n", Mix_GetError());
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    SDL_Surface *ac_screen = SDL_SetVideoMode(1280, 768, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (!ac_screen) {
        fprintf(stderr, "Erreur lors de la création de la fenêtre : %s\n", SDL_GetError());
        Mix_CloseAudio();
        TTF_Quit();
        SDL_Quit();
        return -1;
    }
    SDL_WM_SetCaption("Sous-Menu Joueur", NULL);

    PlayerMenu_Init();

    int ac_running = 1;
    SDL_Event ac_event;

    while (ac_running) {
        while (SDL_PollEvent(&ac_event)) {
            ac_running = PlayerMenu_HandleEvents(&ac_event);
        }

        PlayerMenu_Render(ac_screen);
    }

    PlayerMenu_Cleanup();
    Mix_CloseAudio();
    TTF_Quit();
    SDL_Quit();

    return 0;
}

