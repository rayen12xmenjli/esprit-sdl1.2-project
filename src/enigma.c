#include "enigma.h"
#include <stdio.h>
#include <string.h>

// Initialize the Enigma Sub-Menu
void initEnigma(Enigma *enigma, SDL_Surface *screen, GameState *state, Mix_Chunk *hover_sound, Mix_Chunk *click_sound, TTF_Font *font) {
    enigma->screen = screen;
    enigma->background = IMG_Load("assets/images/background4.bmp");
    enigma->suspense_music = Mix_LoadMUS("assets/sounds/suspense_music.ogg");
    enigma->hover_sound = hover_sound;
    enigma->click_sound = click_sound;
    enigma->font = font;
    enigma->quiz_active = 0;

    // Initialize button positions
    enigma->quiz_button = (SDL_Rect){(1280 - 250) / 2, 200, 250, 50};
    enigma->puzzle_button = (SDL_Rect){(1280 - 250) / 2, 270, 250, 50};

    for (int i = 0; i < 3; i++) {
        enigma->answer_buttons[i] = (SDL_Rect){(1280 - 250) / 2, 340 + i * 70, 250, 50};
    }

    if (!enigma->background || !enigma->suspense_music) {
        fprintf(stderr, "Failed to load enigma assets!\n");
        exit(1);
    }
}

// Update the Enigma Sub-Menu
void updateEnigma(Enigma *enigma) {
    // No continuous updates needed for now
}

// Render the Enigma Sub-Menu
void renderEnigma(Enigma *enigma) {
    SDL_BlitSurface(enigma->background, NULL, enigma->screen, NULL);

    if (enigma->quiz_active) {
        // Render Quiz mode
        SDL_Color textColor = {255, 255, 255};
        SDL_Surface *quiz_text = TTF_RenderText_Solid(enigma->font, "Quiz", textColor);
        SDL_Rect quiz_pos = {(1280 - quiz_text->w) / 2, 100, 0, 0};
        SDL_BlitSurface(quiz_text, NULL, enigma->screen, &quiz_pos);
        SDL_FreeSurface(quiz_text);

        SDL_Surface *question = TTF_RenderText_Solid(enigma->font, "Sample Question?", textColor);
        SDL_Rect q_pos = {(1280 - question->w) / 2, 200, 0, 0};
        SDL_BlitSurface(question, NULL, enigma->screen, &q_pos);
        SDL_FreeSurface(question);

        for (int i = 0; i < 3; i++) {
            char answer_text[20];
            sprintf(answer_text, "Answer %d", i + 1);
            SDL_Surface *answer_surface = TTF_RenderText_Solid(enigma->font, answer_text, textColor);
            SDL_BlitSurface(answer_surface, NULL, enigma->screen, &enigma->answer_buttons[i]);
            SDL_FreeSurface(answer_surface);
        }
    } else {
        // Render main Enigma menu
        SDL_Color textColor = {255, 255, 255};
        SDL_Surface *quiz_button_text = TTF_RenderText_Solid(enigma->font, "Quiz", textColor);
        SDL_Surface *puzzle_button_text = TTF_RenderText_Solid(enigma->font, "Puzzle", textColor);

        SDL_BlitSurface(quiz_button_text, NULL, enigma->screen, &enigma->quiz_button);
        SDL_BlitSurface(puzzle_button_text, NULL, enigma->screen, &enigma->puzzle_button);

        SDL_FreeSurface(quiz_button_text);
        SDL_FreeSurface(puzzle_button_text);
    }

    SDL_Flip(enigma->screen);
}

// Handle events for the Enigma Sub-Menu
void handleEnigmaEvents(Enigma *enigma, SDL_Event *event) {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    if (event->type == SDL_MOUSEMOTION) {
        if (enigma->quiz_active) {
            for (int i = 0; i < 3; i++) {
                SDL_Rect *pos = &enigma->answer_buttons[i];
                if (mouseX >= pos->x && mouseX <= pos->x + pos->w &&
                    mouseY >= pos->y && mouseY <= pos->y + pos->h) {
                    Mix_PlayChannel(-1, enigma->hover_sound, 0);
                }
            }
        } else {
            SDL_Rect *quiz_pos = &enigma->quiz_button;
            SDL_Rect *puzzle_pos = &enigma->puzzle_button;

            if (mouseX >= quiz_pos->x && mouseX <= quiz_pos->x + quiz_pos->w &&
                mouseY >= quiz_pos->y && mouseY <= quiz_pos->y + quiz_pos->h) {
                Mix_PlayChannel(-1, enigma->hover_sound, 0);
            }

            if (mouseX >= puzzle_pos->x && mouseX <= puzzle_pos->x + puzzle_pos->w &&
                mouseY >= puzzle_pos->y && mouseY <= puzzle_pos->y + puzzle_pos->h) {
                Mix_PlayChannel(-1, enigma->hover_sound, 0);
            }
        }
    }

    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) {
        if (enigma->quiz_active) {
            for (int i = 0; i < 3; i++) {
                SDL_Rect *pos = &enigma->answer_buttons[i];
                if (mouseX >= pos->x && mouseX <= pos->x + pos->w &&
                    mouseY >= pos->y && mouseY <= pos->y + pos->h) {
                    Mix_PlayChannel(-1, enigma->click_sound, 0);
                    printf("Answer %d selected\n", i + 1); // Placeholder action
                }
            }
        } else {
            SDL_Rect *quiz_pos = &enigma->quiz_button;
            SDL_Rect *puzzle_pos = &enigma->puzzle_button;

            if (mouseX >= quiz_pos->x && mouseX <= quiz_pos->x + quiz_pos->w &&
                mouseY >= quiz_pos->y && mouseY <= quiz_pos->y + quiz_pos->h) {
                Mix_PlayChannel(-1, enigma->click_sound, 0);
                enigma->quiz_active = 1;
                Mix_PlayMusic(enigma->suspense_music, -1);
            }

            if (mouseX >= puzzle_pos->x && mouseX <= puzzle_pos->x + puzzle_pos->w &&
                mouseY >= puzzle_pos->y && mouseY <= puzzle_pos->y + puzzle_pos->h) {
                Mix_PlayChannel(-1, enigma->click_sound, 0);
                printf("Puzzle button clicked\n"); // Placeholder action
            }
        }
    }

    if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_ESCAPE) {
        SDL_Quit();
        exit(0);
    }
}

// Cleanup resources for the Enigma Sub-Menu
void cleanupEnigma(Enigma *enigma) {
    SDL_FreeSurface(enigma->background);
    Mix_FreeMusic(enigma->suspense_music);
}
