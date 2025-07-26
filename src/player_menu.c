#include "player_menu.h"
#include "sound.h"
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <stdio.h>

PlayerButton mainButtons[NUM_PLAYER_BUTTONS_MAIN]; 
PlayerButton avatarButtons[NUM_PLAYER_BUTTONS_AVATAR]; 
SDL_Surface *playerBackground = NULL;
Mix_Chunk *hoverSound = NULL;
TTF_Font *font = NULL; 
int playerMenuState = PLAYER_MENU_MAIN; 
int isSelectionMade = 0; 

void PlayerMenu_Init() {
    playerBackground = IMG_Load("assets/menu player/background2.jpg");
    if (!playerBackground) {
        fprintf(stderr, "Erreur : Impossible de charger l'image d'arrière-plan 2.\n");
        exit(1);
    }

    font = TTF_OpenFont("assets/fonts/font.ttf", 24); 
    if (!font) {
        fprintf(stderr, "Erreur : Impossible de charger la police (%s)\n", TTF_GetError());
        exit(1);
    }

    mainButtons[0].normal = IMG_Load("assets/menu player/mono.png");       
    mainButtons[0].hover = IMG_Load("assets/menu player/monohover.png");
    mainButtons[1].normal = IMG_Load("assets/menu player/multijoueur.png"); 
    mainButtons[1].hover = IMG_Load("assets/menu player/multijoueurhover.png");
    mainButtons[2].normal = IMG_Load("assets/menu player/retour.png");      
    mainButtons[2].hover = IMG_Load("assets/menu player/retourhover.png");

    for (int i = 0; i < NUM_PLAYER_BUTTONS_MAIN; i++) {
        if (!mainButtons[i].normal || !mainButtons[i].hover) {
            fprintf(stderr, "Erreur : Impossible de charger les images des boutons.\n");
            exit(1);
        }
        mainButtons[i].state = BTN_NORMAL;
    }

    mainButtons[0].position.x = 200; 
    mainButtons[0].position.y = 300; 
    mainButtons[1].position.x = 800; 
    mainButtons[1].position.y = 300; 
    mainButtons[2].position.x = 900;  
    mainButtons[2].position.y = 600; 

    avatarButtons[0].normal = IMG_Load("assets/menu player/avatar1.png");      
    avatarButtons[0].hover = IMG_Load("assets/menu player/avatar1hover.png");
    avatarButtons[1].normal = IMG_Load("assets/menu player/avatar2.png");       
    avatarButtons[1].hover = IMG_Load("assets/menu player/avatar2hover.png");
    avatarButtons[2].normal = IMG_Load("assets/menu player/input1.png");        
    avatarButtons[2].hover = IMG_Load("assets/menu player/input1hover.png");
    avatarButtons[3].normal = IMG_Load("assets/menu player/input2.png");        
    avatarButtons[3].hover = IMG_Load("assets/menu player/input2hover.png");
    avatarButtons[4].normal = IMG_Load("assets/menu player/retour.png");        
    avatarButtons[4].hover = IMG_Load("assets/menu player/retourhover.png");
    avatarButtons[5].normal = IMG_Load("assets/menu player/valider.png");       
    avatarButtons[5].hover = IMG_Load("assets/menu player/validerhover.png");

    for (int i = 0; i < NUM_PLAYER_BUTTONS_AVATAR; i++) {
        if (!avatarButtons[i].normal || !avatarButtons[i].hover) {
            fprintf(stderr, "Erreur : Impossible de charger les images des boutons.\n");
            exit(1);
        }
        avatarButtons[i].state = BTN_NORMAL;
    }

    avatarButtons[0].position.x = 50; 
    avatarButtons[0].position.y = 50; 
    avatarButtons[1].position.x = 850; 
    avatarButtons[1].position.y = 50; 
    avatarButtons[2].position.x = 200; 
    avatarButtons[2].position.y = 350; 
    avatarButtons[3].position.x = 880; 
    avatarButtons[3].position.y = 350; 
    avatarButtons[4].position.x = 1050;  
    avatarButtons[4].position.y = 600; 
    avatarButtons[5].position.x = 100; 
    avatarButtons[5].position.y = 600; 

    avatarButtons[5].state = BTN_HIDDEN;

    hoverSound = Mix_LoadWAV("assets/sounds/menu player/hover_sound.wav");
    if (!hoverSound) {
        fprintf(stderr, "Erreur : Impossible de charger le son de survol (%s).\n", Mix_GetError());
        exit(1);
    }
}

int PlayerMenu_HandleEvents(SDL_Event *event) {
    if (event->type == SDL_QUIT) {
        return PLAYER_MENU_BACK; 
    }

    if (playerMenuState == PLAYER_MENU_MAIN) {
        if (event->type == SDL_MOUSEMOTION) {
            int x = event->motion.x;
            int y = event->motion.y;
            for (int i = 0; i < NUM_PLAYER_BUTTONS_MAIN; i++) {
                if (x >= mainButtons[i].position.x && x <= mainButtons[i].position.x + mainButtons[i].normal->w &&
                    y >= mainButtons[i].position.y && y <= mainButtons[i].position.y + mainButtons[i].normal->h) {
                    mainButtons[i].state = BTN_HOVER;
                    Mix_PlayChannel(-1, hoverSound, 0);
                } else {
                    mainButtons[i].state = BTN_NORMAL;
                }
            }
        }

        if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) {
            int x = event->button.x;
            int y = event->button.y;
            for (int i = 0; i < NUM_PLAYER_BUTTONS_MAIN; i++) {
                if (x >= mainButtons[i].position.x && x <= mainButtons[i].position.x + mainButtons[i].normal->w &&
                    y >= mainButtons[i].position.y && y <= mainButtons[i].position.y + mainButtons[i].normal->h) {
                    if (i == 0 || i == 1) { 
                        playerMenuState = PLAYER_MENU_AVATAR; 
                        return PLAYER_MENU_CONTINUE;
                    } else if (i == 2) { 
                        return PLAYER_MENU_BACK; 
                    }
                }
            }
        }
    } else if (playerMenuState == PLAYER_MENU_AVATAR) {
        if (event->type == SDL_MOUSEMOTION) {
            int x = event->motion.x;
            int y = event->motion.y;
            for (int i = 0; i < NUM_PLAYER_BUTTONS_AVATAR; i++) {
                if (x >= avatarButtons[i].position.x && x <= avatarButtons[i].position.x + avatarButtons[i].normal->w &&
                    y >= avatarButtons[i].position.y && y <= avatarButtons[i].position.y + avatarButtons[i].normal->h) {
                    if (avatarButtons[i].state != BTN_HIDDEN) {
                        avatarButtons[i].state = BTN_HOVER;
                        Mix_PlayChannel(-1, hoverSound, 0);
                    }
                } else {
                    if (avatarButtons[i].state != BTN_HIDDEN) {
                        avatarButtons[i].state = BTN_NORMAL;
                    }
                }
            }
        }

        if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) {
            int x = event->button.x;
            int y = event->button.y;
            for (int i = 0; i < NUM_PLAYER_BUTTONS_AVATAR; i++) {
                if (x >= avatarButtons[i].position.x && x <= avatarButtons[i].position.x + avatarButtons[i].normal->w &&
                    y >= avatarButtons[i].position.y && y <= avatarButtons[i].position.y + avatarButtons[i].normal->h) {
                    if (i >= 0 && i <= 3) {
                        printf("Bouton %d sélectionné.\n", i); 
                        isSelectionMade = 1; 
                        avatarButtons[5].state = BTN_NORMAL; 
                    } else if (i == 4) { 
                        playerMenuState = PLAYER_MENU_MAIN; 
                        return PLAYER_MENU_CONTINUE;
                    } else if (i == 5 && isSelectionMade) { 
                        printf("Scores validés.\n");
                        isSelectionMade = 0; 
                        avatarButtons[5].state = BTN_HIDDEN;
                        return PLAYER_MENU_PROCEED;
                    }
                    return PLAYER_MENU_CONTINUE;
                }
            }
        }
    }
    return PLAYER_MENU_CONTINUE; 
}

void PlayerMenu_Render(SDL_Surface *screen) {
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    SDL_BlitSurface(playerBackground, NULL, screen, NULL);

    if (playerMenuState == PLAYER_MENU_MAIN) {
        SDL_Color textColor = {255, 255, 255, 0}; 
        SDL_Surface *textSurface = TTF_RenderText_Solid(font, "Choisissez votre mode de jeu", textColor);
        SDL_Rect textPosition = {screen->w / 2 - textSurface->w / 2, 200, 0, 0};
        SDL_BlitSurface(textSurface, NULL, screen, &textPosition);
        SDL_FreeSurface(textSurface);

        for (int i = 0; i < NUM_PLAYER_BUTTONS_MAIN; i++) {
            SDL_Surface *buttonSurface = mainButtons[i].state == BTN_HOVER ? mainButtons[i].hover : mainButtons[i].normal;
            SDL_BlitSurface(buttonSurface, NULL, screen, &mainButtons[i].position);
        }
    } else if (playerMenuState == PLAYER_MENU_AVATAR) {
        SDL_Color textColor = {0, 0, 0, 0}; 
        SDL_Surface *textSurface = TTF_RenderText_Solid(font, "Choisissez votre avatar ou input", textColor);
        SDL_Rect textPosition = {screen->w / 2 - textSurface->w / 2, 100, 0, 0};
        SDL_BlitSurface(textSurface, NULL, screen, &textPosition);
        SDL_FreeSurface(textSurface);

        for (int i = 0; i < NUM_PLAYER_BUTTONS_AVATAR; i++) {
            if (avatarButtons[i].state == BTN_HIDDEN) {
                continue; 
            }
            SDL_Surface *buttonSurface = avatarButtons[i].state == BTN_HOVER ? avatarButtons[i].hover : avatarButtons[i].normal;
            SDL_BlitSurface(buttonSurface, NULL, screen, &avatarButtons[i].position);
        }
    }
}

void PlayerMenu_Cleanup() {
    for (int i = 0; i < NUM_PLAYER_BUTTONS_MAIN; i++) {
        if (mainButtons[i].normal) SDL_FreeSurface(mainButtons[i].normal);
        if (mainButtons[i].hover) SDL_FreeSurface(mainButtons[i].hover);
    }

    for (int i = 0; i < NUM_PLAYER_BUTTONS_AVATAR; i++) {
        if (avatarButtons[i].normal) SDL_FreeSurface(avatarButtons[i].normal);
        if (avatarButtons[i].hover) SDL_FreeSurface(avatarButtons[i].hover);
    }

    if (playerBackground) SDL_FreeSurface(playerBackground);
    if (font) TTF_CloseFont(font);
    if (hoverSound) Mix_FreeChunk(hoverSound);
}
