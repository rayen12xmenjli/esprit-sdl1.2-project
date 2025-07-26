#include "menu.h"
#include <stdio.h>
#include <stdlib.h>

void initialize_sdl() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }
    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
        fprintf(stderr, "SDL_mixer could not initialize! Mix_Error: %s\n", Mix_GetError());
        exit(1);
    }
    if (TTF_Init() == -1) {
        fprintf(stderr, "SDL_ttf could not initialize! TTF_Error: %s\n", TTF_GetError());
        exit(1);
    }
}

void handle_events(int *running, int *selected, Button buttons[], int button_count, Mix_Chunk *hover_sound, Mix_Chunk *click_sound, int *selected_button) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                *running = 0;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        (*selected_button)--;
                        if (*selected_button < 0) *selected_button = button_count - 1;
                        Mix_PlayChannel(-1, hover_sound, 0);
                        break;
                    case SDLK_DOWN:
                        (*selected_button)++;
                        if (*selected_button >= button_count) *selected_button = 0;
                        Mix_PlayChannel(-1, hover_sound, 0);
                        break;
                    case SDLK_RETURN:
                        *selected = *selected_button;
                        Mix_PlayChannel(-1, click_sound, 0);
                        break;
                    case SDLK_ESCAPE:
                        *running = 0;
                        break;
                    default:
                        break; // Suppress warnings for unhandled keys
                }
                break;
            case SDL_MOUSEMOTION:
                for (int i = 0; i < button_count; i++) {
                    if (event.motion.x >= buttons[i].position.x &&
                        event.motion.x <= buttons[i].position.x + buttons[i].position.w &&
                        event.motion.y >= buttons[i].position.y &&
                        event.motion.y <= buttons[i].position.y + buttons[i].position.h) {
                        if (!buttons[i].hovered) Mix_PlayChannel(-1, hover_sound, 0);
                        buttons[i].hovered = 1;
                        *selected_button = i;
                    } else {
                        buttons[i].hovered = 0;
                    }
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                for (int i = 0; i < button_count; i++) {
                    if (event.button.x >= buttons[i].position.x &&
                        event.button.x <= buttons[i].position.x + buttons[i].position.w &&
                        event.button.y >= buttons[i].position.y &&
                        event.button.y <= buttons[i].position.y + buttons[i].position.h) {
                        Mix_PlayChannel(-1, click_sound, 0);
                        *selected = i;
                    }
                }
                break;
        }
    }
}

void render(SDL_Surface *screen, SDL_Surface *background, Button buttons[], int button_count, TTF_Font *font, SDL_Color textColor, int selected_button) {
    SDL_BlitSurface(background, NULL, screen, NULL);
    for (int i = 0; i < button_count; i++) {
        SDL_Surface *button_surface = (i == selected_button) ? buttons[i].hover_image : buttons[i].image;
        SDL_BlitSurface(button_surface, NULL, screen, &buttons[i].position);
    }
    SDL_Surface *titleSurface = TTF_RenderText_Solid(font, "Shattered Bloodline", textColor);
    SDL_Rect titlePosition = {(SCREEN_WIDTH - titleSurface->w) / 2, 75, 0, 0};
    SDL_BlitSurface(titleSurface, NULL, screen, &titlePosition);
    SDL_FreeSurface(titleSurface);
}

void cleanup(Button buttons[], int button_count, SDL_Surface *background, Mix_Music *music, Mix_Chunk *hover_sound, Mix_Chunk *click_sound, TTF_Font *font) {
    if (buttons) {
        for (int i = 0; i < button_count; i++) {
            if (buttons[i].image) SDL_FreeSurface(buttons[i].image);
            if (buttons[i].hover_image) SDL_FreeSurface(buttons[i].hover_image);
        }
    }
    if (background) SDL_FreeSurface(background);
    if (music) Mix_FreeMusic(music);
    if (hover_sound) Mix_FreeChunk(hover_sound);
    if (click_sound) Mix_FreeChunk(click_sound);
    if (font) TTF_CloseFont(font);
    if (background_static) SDL_FreeSurface(background_static);
    
}
