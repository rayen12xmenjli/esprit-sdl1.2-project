#include <SDL/SDL.h>
#include "game.h"
#include <stdio.h>

void handleEventsPlayer2(GAME *game) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                game->running = 0;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_a:
                        game->input.p2_left = 1;
                        break;
                    case SDLK_d:
                        game->input.p2_right = 1;
                        break;
                    case SDLK_w:
                        game->input.p2_jump = 1;
                        game->input.p2_jumpHeld = 1;
                        break;
                    case SDLK_q:
                        game->input.p2_attack = 1;
                        break;
                    case SDLK_e:
                        game->input.p2_damage = 1;
                        break;
                    case SDLK_LSHIFT:
                        game->input.p2_run = 1;
                        break;
                    case SDLK_r:
                        game->input.restart = 1;
                        break;
                    case SDLK_ESCAPE:
                        game->running = 0;
                        break;
                    default:
                        break;
                }
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                    case SDLK_a:
                        game->input.p2_left = 0;
                        break;
                    case SDLK_d:
                        game->input.p2_right = 0;
                        break;
                    case SDLK_w:
                        game->input.p2_jump = 0;
                        game->input.p2_jumpHeld = 0;
                        break;
                    case SDLK_q:
                        game->input.p2_attack = 0;
                        break;
                    case SDLK_e:
                        game->input.p2_damage = 0;
                        break;
                    case SDLK_LSHIFT:
                        game->input.p2_run = 0;
                        break;
                    case SDLK_r:
                        game->input.restart = 0;
                        break;
                    default:
                        break;
                }
                break;
        }

        if (game->global.quizActive) {
            int result = updateEnigme(game, &event);
            if (result == 1) {
                game->global.quizActive = 0;
                SDL_Delay(2000);
                freeEnigme(game);
                load_level(game, 3);
                fprintf(stderr, "DEBUG: Enigma solved, loaded Level 3\n");
            } else if (result == 0) {
                game->global.quizActive = 0;
                SDL_Delay(2000);
                freeEnigme(game);
                load_level(game, 2);
                fprintf(stderr, "DEBUG: Enigma failed, reloaded Level 2\n");
            }
        }
    }
}
