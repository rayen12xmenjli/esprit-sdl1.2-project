/**
 * @file main.c
 * @brief Main program for game with menu system and integrated game.
 * @author Rayen Menjli
 * @version 0.2
 * @date 06/05/2025
 */

#include "menu.h"
#include "player_menu.h"
#include "game.h"
#include "player.h"
#include "player2.h"
#include "events.h"
#include "ui.h"
#include "inventory.h"
#include "level.h"
#include "jet.h"
#include "soldier.h"
#include "robot.h"
#include "enemy.h"
#include "enigme.h"
#include "enemylvl2.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>

#define FPS 60
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

// Define global variables for menu system
SDL_Surface *opening_layers[OPENING_ANIMATION_FRAMES];
SDL_Surface *game_intro_layers[GAME_INTRO_FRAMES];
SDL_Surface *loading_layers[LOADING_ANIMATION_FRAMES];
SDL_Surface *background_static = NULL;
SDL_Surface *countdown_background = NULL;
Mix_Chunk *countdown_beep = NULL;
int current_frame = 0;
Uint32 last_frame_time = 0;
int animation_complete = 0;

// Game data for the actual game
GAME game_data;

void loadBackgroundAssets() {
    background_static = IMG_Load("assets/images/background1.bmp");
    if (!background_static) {
        fprintf(stderr, "Failed to load static background: %s\n", IMG_GetError());
        exit(1);
    }

    for (int i = 0; i < OPENING_ANIMATION_FRAMES; i++) {
        char path[100];
        snprintf(path, sizeof(path), "assets/images/opening_frames/frame%d.png", i + 1);
        opening_layers[i] = IMG_Load(path);
        if (!opening_layers[i]) {
            fprintf(stderr, "Failed to load opening frame %d: %s\n", i + 1, IMG_GetError());
            exit(1);
        }
    }

    for (int i = 0; i < GAME_INTRO_FRAMES; i++) {
        char path[100];
        snprintf(path, sizeof(path), "assets/images/game_intro_frames/%d.png", i + 1);
        game_intro_layers[i] = IMG_Load(path);
        if (!game_intro_layers[i]) {
            fprintf(stderr, "Failed to load game intro frame %d: %s\n", i + 1, IMG_GetError());
            exit(1);
        }
    }

    for (int i = 0; i < LOADING_ANIMATION_FRAMES; i++) {
        char path[100];
        snprintf(path, sizeof(path), "assets/images/loading_frames/frame%d.png", i + 1);
        loading_layers[i] = IMG_Load(path);
        if (!loading_layers[i]) {
            fprintf(stderr, "Failed to load loading frame %d: %s\n", i + 1, IMG_GetError());
            exit(1);
        }
    }
}

void renderBackground(SDL_Surface *screen, BackgroundState background_state) {
    Uint32 current_time = SDL_GetTicks();
    if (background_state == OPENING_ANIMATION) {
        if (current_time - last_frame_time > 100) {
            current_frame = (current_frame + 1) % OPENING_ANIMATION_FRAMES;
            last_frame_time = current_time;
            if (current_frame == 0) animation_complete = 1;
        }
        SDL_BlitSurface(opening_layers[current_frame], NULL, screen, NULL);
    } else if (background_state == GAME_INTRO_ANIMATION) {
        if (current_time - last_frame_time > 100) {
            current_frame = (current_frame + 1) % GAME_INTRO_FRAMES;
            last_frame_time = current_time;
            if (current_frame == 0) animation_complete = 1;
        }
        SDL_BlitSurface(game_intro_layers[current_frame], NULL, screen, NULL);
    } else if (background_state == LOADING_ANIMATION) {
        if (current_time - last_frame_time > 100) {
            current_frame = (current_frame + 1) % LOADING_ANIMATION_FRAMES;
            last_frame_time = current_time;
            if (current_frame == 0) animation_complete = 1;
        }
        SDL_BlitSurface(loading_layers[current_frame], NULL, screen, NULL);
    } else if (background_state == BACKGROUND_STATIC) {
        SDL_BlitSurface(background_static, NULL, screen, NULL);
    }
}

// Implement initGame and freeGame to resolve undefined references
void initGame(GAME *game) {
    if (!game || !game->screen) {
        fprintf(stderr, "initGame: Game or screen is NULL\n");
        exit(1);
    }
    game->running = 1;
    game->level = 0;
    game->global.currentLevel = 0;
    game->global.showMessage = 0;
    game->global.gameOver = 0;
    game->global.quizActive = 0;
    game->inventoryVisible = 0;
    // Initialize game components
    initPlayer(&game->player, game->screen);
    initPlayer2(&game->player2, 200, 500, game);
    initUI(&game->ui);
    initInventory(&game->inventory);
    initJet(&game->jet);
    // Add more initializations if other components exist in your GAME struct
}

void freeGame(GAME *game) {
    if (!game) return;
    // Free game resources
    freePlayer(&game->player);
    freePlayer2(&game->player2);
    freeUI(&game->ui);
    freeInventory(&game->inventory);
    freeJet(&game->jet);
    freeResources(game); // From level.c
    // Do not free game->screen, as it’s managed by main
}

int main() {
    GameState current_state = STATE_OPENING_ANIMATION;
    BackgroundState background_state = OPENING_ANIMATION;
    int countdown_value = 3;
    Uint32 countdown_start_time = 0;
    Uint32 last_time = 0;

    // Initialize SDL for menu and game
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }
    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        fprintf(stderr, "IMG_Init failed: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }
    if (TTF_Init() < 0) {
        fprintf(stderr, "TTF_Init failed: %s\n", TTF_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    if (!initialiserMusique()) {
        fprintf(stderr, "Failed to initialize audio\n");
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Surface *screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (!screen) {
        fprintf(stderr, "Failed to create screen: %s\n", SDL_GetError());
        libererMusique(NULL);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    game_data.screen = screen; // Assign screen to game_data

    countdown_beep = Mix_LoadWAV("assets/sounds/countdown_beep.wav");
    if (!countdown_beep) {
        fprintf(stderr, "Failed to load countdown beep: %s\n", Mix_GetError());
        libererMusique(NULL);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    loadBackgroundAssets();
    SDL_Surface *background_main = IMG_Load("assets/images/background1.bmp");
    SDL_Surface *background_scores = IMG_Load("assets/images/background3.bmp");
    Mix_Music *music_main = Mix_LoadMUS("assets/sounds/music_main.ogg");
    Mix_Music *music_player = Mix_LoadMUS("assets/sounds/menu player/player_music.ogg");
    Mix_Chunk *hover_sound = Mix_LoadWAV("assets/sounds/hover_sound.wav");
    Mix_Chunk *click_sound = Mix_LoadWAV("assets/sounds/click_sound.wav");
    TTF_Font *font = TTF_OpenFont("assets/fonts/arial.ttf", 30);

    if (!background_main || !background_scores || !music_main || !music_player || !hover_sound || !click_sound || !font) {
        fprintf(stderr, "Failed to load assets!\n");
        cleanup(NULL, 0, background_main, music_main, hover_sound, click_sound, font);
        libererMusique(NULL);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    Button buttons_main[5] = {
        {{(1280 - 300) / 2, 200, 300, 60}, IMG_Load("assets/images/button/play.png"), IMG_Load("assets/images/button/play_hover.png"), 0},
        {{(1280 - 300) / 2, 270, 300, 60}, IMG_Load("assets/images/button/options.png"), IMG_Load("assets/images/button/options_hover.png"), 0},
        {{(1280 - 300) / 2, 340, 300, 60}, IMG_Load("assets/images/button/hs.png"), IMG_Load("assets/images/button/hs_hover.png"), 0},
        {{(1280 - 300) / 2, 410, 300, 60}, IMG_Load("assets/images/button/history.png"), IMG_Load("assets/images/button/history_hover.png"), 0},
        {{(1280 - 300) / 2, 480, 300, 60}, IMG_Load("assets/images/button/quit.png"), IMG_Load("assets/images/button/quit_hover.png"), 0}
    };

    Button button_retour = {{(1280 - 300) / 2, 600, 300, 60}, IMG_Load("assets/images/retour.png"), IMG_Load("assets/images/retour_hover.png"), 0};

    for (int i = 0; i < 5; i++) {
        if (!buttons_main[i].image || !buttons_main[i].hover_image) {
            fprintf(stderr, "Failed to load main button images!\n");
            cleanup(buttons_main, 5, background_main, music_main, hover_sound, click_sound, font);
            libererMusique(NULL);
            TTF_Quit();
            IMG_Quit();
            SDL_Quit();
            return 1;
        }
    }
    if (!button_retour.image || !button_retour.hover_image) {
        fprintf(stderr, "Failed to load retour button images!\n");
        cleanup(buttons_main, 5, background_main, music_main, hover_sound, click_sound, font);
        libererMusique(NULL);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    PlayerMenu_Init();
    Mix_PlayMusic(music_main, -1);

    int running = 1, selected = -1, selected_button = 0;
    static GameState previous_state = STATE_OPENING_ANIMATION;

    while (running) {
        if (current_state != previous_state) {
            if (Mix_PlayingMusic()) Mix_HaltMusic();
            switch (current_state) {
                case STATE_MAIN_MENU:
                    Mix_PlayMusic(music_main, -1);
                    break;
                case STATE_PLAYER:
                    Mix_PlayMusic(music_player, -1);
                    break;
                case STATE_GAME:
                    // Game music will be handled by initGame or load_level
                    break;
                default:
                    break;
            }
            previous_state = current_state;
        }

        switch (current_state) {
            case STATE_OPENING_ANIMATION:
                renderBackground(screen, background_state);
                SDL_Surface *skip_text = TTF_RenderText_Solid(font, "Press Space to Skip", (SDL_Color){255, 255, 255, 255});
                SDL_Rect skip_pos = {(SCREEN_WIDTH - skip_text->w) / 2, SCREEN_HEIGHT - 50, 0, 0};
                SDL_BlitSurface(skip_text, NULL, screen, &skip_pos);
                SDL_FreeSurface(skip_text);
                SDL_Flip(screen);

                SDL_Event event;
                while (SDL_PollEvent(&event)) {
                    if (event.type == SDL_QUIT) running = 0;
                    else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
                        current_state = STATE_MAIN_MENU;
                        background_state = BACKGROUND_STATIC;
                        animation_complete = 0;
                        current_frame = 0;
                    }
                }
                if (animation_complete) {
                    current_state = STATE_MAIN_MENU;
                    background_state = BACKGROUND_STATIC;
                    animation_complete = 0;
                    current_frame = 0;
                }
                break;

            case STATE_MAIN_MENU:
                handle_events(&running, &selected, buttons_main, 5, hover_sound, click_sound, &selected_button);
                render(screen, background_main, buttons_main, 5, font, (SDL_Color){230, 195, 51}, selected_button);
                SDL_Flip(screen);
                if (selected != -1) {
                    switch (selected) {
                        case 0: 
                            current_state = STATE_PLAYER;
                            break;
                        case 1: current_state = STATE_OPTIONS; break;
                        case 2: current_state = STATE_SCORES; break;
                        case 3: printf("History button clicked\n"); selected = -1; break;
                        case 4: running = 0; break;
                    }
                    selected = -1;
                }
                break;

            case STATE_PLAYER:
                {
                    SDL_Event event;
                    while (SDL_PollEvent(&event)) {
                        if (event.type == SDL_QUIT) {
                            running = 0;
                        } else {
                            int result = PlayerMenu_HandleEvents(&event);
                            if (result == PLAYER_MENU_BACK) {
                                current_state = STATE_MAIN_MENU;
                            } else if (result == PLAYER_MENU_PROCEED) {
                                current_state = STATE_GAME_INTRO;
                                background_state = GAME_INTRO_ANIMATION;
                                current_frame = 0;
                                animation_complete = 0;
                            }
                        }
                    }
                    PlayerMenu_Render(screen);
                    SDL_Flip(screen);
                }
                break;

            case STATE_GAME_INTRO:
                renderBackground(screen, background_state);
                SDL_Flip(screen);
                if (animation_complete) {
                    current_state = STATE_LOADING;
                    background_state = LOADING_ANIMATION;
                    animation_complete = 0;
                    current_frame = 0;
                }
                break;

            case STATE_LOADING:
                renderBackground(screen, background_state);
                SDL_Flip(screen);
                if (animation_complete) {
                    background_state = COUNTDOWN_ANIMATION;
                    current_state = STATE_COUNTDOWN;
                    animation_complete = 0;
                    countdown_value = 3;
                    countdown_start_time = 0;
                }
                break;

            case STATE_COUNTDOWN:
                {
                    SDL_Event event;
                    while (SDL_PollEvent(&event)) {
                        if (event.type == SDL_QUIT) {
                            running = 0;
                        }
                    }

                    if (!countdown_background) {
                        countdown_background = IMG_Load("assets/images/countdown_background.bmp");
                        if (!countdown_background) {
                            fprintf(stderr, "Failed to load countdown background: %s\n", IMG_GetError());
                            running = 0;
                            break;
                        }
                    }

                    SDL_BlitSurface(countdown_background, NULL, screen, NULL);
                    char countdown_text[20];
                    snprintf(countdown_text, sizeof(countdown_text), "%d", countdown_value);
                    SDL_Surface *countdown_surface = TTF_RenderText_Solid(font, countdown_text, (SDL_Color){255, 255, 255});
                    SDL_Rect countdown_pos = {(1280 - countdown_surface->w) / 2, (720 - countdown_surface->h) / 2, 0, 0};
                    SDL_BlitSurface(countdown_surface, NULL, screen, &countdown_pos);
                    SDL_FreeSurface(countdown_surface);

                    SDL_Flip(screen);

                    Uint32 current_time = SDL_GetTicks();
                    if (countdown_start_time == 0) {
                        countdown_start_time = current_time;
                    }

                    if (current_time - countdown_start_time >= 1500) {
                        countdown_value--;
                        countdown_start_time = current_time;
                        if (countdown_value > 0) {
                            Mix_PlayChannel(-1, countdown_beep, 0);
                        }
                        if (countdown_value <= 0) {
                            current_state = STATE_GAME;
                            Mix_HaltMusic();
                            countdown_value = 3;
                            countdown_start_time = 0;
                            // Initialize game
                            initGame(&game_data);
                            load_level(&game_data, 1);
                            last_time = SDL_GetTicks();
                        }
                    }
                }
                break;

            case STATE_GAME:
                {
                    Uint32 current_time = SDL_GetTicks();
                    if (current_time - last_time >= 1000 / FPS) {
                        handleEvents(&game_data);
                        playLevel(&game_data);
                        SDL_Flip(screen);
                        last_time = current_time;
                    }
                    if (!game_data.running) {
                        // Game over or player quit
                        current_state = STATE_MAIN_MENU;
                        Mix_HaltMusic();
                        Mix_PlayMusic(music_main, -1);
                        freeGame(&game_data);
                    }
                }
                break;

            case STATE_SCORES:
            case STATE_OPTIONS:
            case STATE_SAVE_LOAD:
                printf("Sub-menu not implemented yet\n");
                current_state = STATE_MAIN_MENU;
                Mix_PlayMusic(music_main, -1);
                break;

            case STATE_ENIGMA:
            case STATE_ENIGMA_QUIZ:
            case STATE_ENIGMA_PUZZLE:
                printf("Enigma states not implemented yet\n");
                current_state = STATE_MAIN_MENU;
                break;

            case STATE_QUIT:
                running = 0;
                break;
        }
        SDL_Delay(16); // ~60 FPS
    }

    // Cleanup animation layers
    for (int i = 0; i < OPENING_ANIMATION_FRAMES; i++) {
        if (opening_layers[i]) SDL_FreeSurface(opening_layers[i]);
    }
    for (int i = 0; i < GAME_INTRO_FRAMES; i++) {
        if (game_intro_layers[i]) SDL_FreeSurface(game_intro_layers[i]);
    }
    for (int i = 0; i < LOADING_ANIMATION_FRAMES; i++) {
        if (loading_layers[i]) SDL_FreeSurface(loading_layers[i]);
    }

    // Cleanup other resources
    if (countdown_beep) Mix_FreeChunk(countdown_beep);
    if (countdown_background) SDL_FreeSurface(countdown_background);
    cleanup(buttons_main, 5, background_main, music_main, hover_sound, click_sound, font);
    if (background_static) SDL_FreeSurface(background_static);
    if (background_scores) SDL_FreeSurface(background_scores);
    if (music_player) Mix_FreeMusic(music_player);
    PlayerMenu_Cleanup();
    freeGame(&game_data); // Ensure game resources are freed
    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}
