#include <SDL/SDL.h>
#include "game.h"
#include "events.h"
#include <stdio.h>

void handleEvents(GAME *game) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                game->running = 0;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        game->running = 0;
                        if (game->global.showInstructions) {
                            game->global.showInstructions = 0; // Close instructions
                        }
                        break;
                    case SDLK_LEFT:
                        game->input.left = 1;
                        break;
                    case SDLK_RIGHT:
                        game->input.right = 1;
                        break;
                    case SDLK_UP:
                        game->input.up = 1;
                        break;
                    case SDLK_SPACE:
                        game->input.jump = 1;
                        game->input.jumpHeld = 1;
                        break;
                    case SDLK_e:
                        game->input.enter = 1;
                        if (game->global.showInstructions) {
                            game->global.showInstructions = 0; // Close instructions
                        }
                        break;
                    case SDLK_p:
                        game->input.pickup = 1;
                        break;
                    case SDLK_a:
                        game->input.attack = 1;
                        break;
                    case SDLK_s:
                        game->input.shoot = 1;
                        break;
                    case SDLK_r:
                        game->input.recharge = 1;
                        game->input.restart = 1;
                        break;
                    case SDLK_d:
                        game->input.damage = 1;
                        break;
                    case SDLK_i:
                        game->inventoryVisible = !game->inventoryVisible; // Toggle inventory visibility
                        printf("Inventory %s\n", game->inventoryVisible ? "shown" : "hidden");
                        break;
                    case SDLK_LSHIFT:
                        game->input.run = 1;
                        break;
                    case SDLK_t:
                        game->input.scroll = 1;
                        break;
                    case SDLK_k:
                        game->input.skip = 1;
                        break;
                    case SDLK_f:
                        game->inventory.selectedSlot = (game->inventory.selectedSlot + 1) % MAX_ITEMS; // Cycle selected slot
                        printf("Selected slot %d\n", game->inventory.selectedSlot);
                        break;
                    case SDLK_1:
                        game->input.useItem[0] = 1; // Trigger use of item 0
                        break;
                    default:
                        break;
                }
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        game->input.left = 0;
                        break;
                    case SDLK_RIGHT:
                        game->input.right = 0;
                        break;
                    case SDLK_UP:
                        game->input.up = 0;
                        break;
                    case SDLK_SPACE:
                        game->input.jump = 0;
                        game->input.jumpHeld = 0;
                        break;
                    case SDLK_e:
                        game->input.enter = 0;
                        break;
                    case SDLK_p:
                        game->input.pickup = 0;
                        break;
                    case SDLK_a:
                        game->input.attack = 0;
                        break;
                    case SDLK_s:
                        game->input.shoot = 0;
                        break;
                    case SDLK_r:
                        game->input.recharge = 0;
                        game->input.restart = 0;
                        break;
                    case SDLK_d:
                        game->input.damage = 0;
                        break;
                    case SDLK_LSHIFT:
                        game->input.run = 0;
                        break;
                    case SDLK_t:
                        game->input.scroll = 0;
                        break;
                    case SDLK_k:
                        game->input.skip = 0;
                        break;
                    case SDLK_1:
                        game->input.useItem[0] = 0; // Reset useItem[0] on key release
                        break;
                    default:
                        break;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    game->input.mouseLeft = 1;
                    game->input.mouseX = event.button.x;
                    game->input.mouseY = event.button.y;
                    fprintf(stderr, "DEBUG: Mouse left button down at (%d, %d)\n", game->input.mouseX, game->input.mouseY);
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    game->input.mouseLeft = 0;
                    fprintf(stderr, "DEBUG: Mouse left button up\n");
                }
                break;
            case SDL_MOUSEMOTION:
                game->input.mouseX = event.motion.x;
                game->input.mouseY = event.motion.y;
                break;
            case SDL_JOYAXISMOTION:
                if (event.jaxis.axis == 0) {
                    if (event.jaxis.value < -8000) {
                        game->input.left = 1;
                        game->input.right = 0;
                    } else if (event.jaxis.value > 8000) {
                        game->input.left = 0;
                        game->input.right = 1;
                    } else {
                        game->input.left = 0;
                        game->input.right = 0;
                    }
                }
                break;
            case SDL_JOYBUTTONDOWN:
                if (event.jbutton.button == 0) {
                    game->input.jump = 1;
                }
                break;
            case SDL_JOYBUTTONUP:
                if (event.jbutton.button == 0) {
                    game->input.jump = 0;
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
