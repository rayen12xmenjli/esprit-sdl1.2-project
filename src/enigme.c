#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_rotozoom.h>
#include "enigme.h"

SDL_Color color_correct = {0, 255, 0, 255};
SDL_Color color_incorrect = {255, 0, 0, 255};

int initialiserBackg(Bg *b) {
    fprintf(stderr, "DEBUG: Initializing background\n");
    b->Bg = IMG_Load("assets/enigma/image.jpeg");
    if (!b->Bg) {
        fprintf(stderr, "ERROR: Failed to load background image: %s\n", IMG_GetError());
        return 0;
    }
    b->posAff.x = 0;
    b->posAff.y = 0;
    b->posAff.w = SCREEN_WIDTH;
    b->posAff.h = SCREEN_HEIGHT;
    fprintf(stderr, "DEBUG: Background loaded (%dx%d)\n", b->Bg->w, b->Bg->h);
    return 1;
}

void afficherBackg(Bg b, SDL_Surface *ecran) {
    if (!b.Bg || !ecran) {
        fprintf(stderr, "ERROR: Invalid background or screen surface\n");
        return;
    }
    SDL_BlitSurface(b.Bg, NULL, ecran, &b.posAff);
    fprintf(stderr, "DEBUG: Background blitted\n");
}

void libererBackg(Bg *b) {
    if (b->Bg) {
        SDL_FreeSurface(b->Bg);
        b->Bg = NULL;
        fprintf(stderr, "DEBUG: Freed background\n");
    }
}

int initialiserA(A *a) {
    fprintf(stderr, "DEBUG: Initializing button A\n");
    a->choixA = IMG_Load("assets/enigma/a.png");
    a->choixAGlow = IMG_Load("assets/enigma/Aglow.png");
    if (!a->choixA || !a->choixAGlow) {
        fprintf(stderr, "ERROR: Failed to load A images: %s\n", IMG_GetError());
        return 0;
    }
    a->posA.x = 50;
    a->posA.y = 160;
    a->posA.w = 250;
    a->posA.h = 50;
    fprintf(stderr, "DEBUG: Button A loaded (%dx%d)\n", a->choixA->w, a->choixA->h);
    return 1;
}

void afficherA(A a, SDL_Surface *ecran, int isHovered) {
    if (!ecran) {
        fprintf(stderr, "ERROR: Invalid screen surface\n");
        return;
    }
    if (isHovered && a.choixAGlow) {
        SDL_BlitSurface(a.choixAGlow, NULL, ecran, &a.posA);
        fprintf(stderr, "DEBUG: Button A glow blitted\n");
    } else if (a.choixA) {
        SDL_BlitSurface(a.choixA, NULL, ecran, &a.posA);
        fprintf(stderr, "DEBUG: Button A blitted\n");
    }
}

void libererA(A *a) {
    if (a->choixA) {
        SDL_FreeSurface(a->choixA);
        a->choixA = NULL;
        fprintf(stderr, "DEBUG: Freed button A\n");
    }
    if (a->choixAGlow) {
        SDL_FreeSurface(a->choixAGlow);
        a->choixAGlow = NULL;
        fprintf(stderr, "DEBUG: Freed button A glow\n");
    }
}

int initialiserB(B *b) {
    fprintf(stderr, "DEBUG: Initializing button B\n");
    b->choixB = IMG_Load("assets/enigma/b.png");
    b->choixBGlow = IMG_Load("assets/enigma/Bglow.png");
    if (!b->choixB || !b->choixBGlow) {
        fprintf(stderr, "ERROR: Failed to load B images: %s\n", IMG_GetError());
        return 0;
    }
    b->posB.x = 50;
    b->posB.y = 320;
    b->posB.w = 250;
    b->posB.h = 50;
    fprintf(stderr, "DEBUG: Button B loaded (%dx%d)\n", b->choixB->w, b->choixB->h);
    return 1;
}

void afficherB(B b, SDL_Surface *ecran, int isHovered) {
    if (!ecran) {
        fprintf(stderr, "ERROR: Invalid screen surface\n");
        return;
    }
    if (isHovered && b.choixBGlow) {
        SDL_BlitSurface(b.choixBGlow, NULL, ecran, &b.posB);
        fprintf(stderr, "DEBUG: Button B glow blitted\n");
    } else if (b.choixB) {
        SDL_BlitSurface(b.choixB, NULL, ecran, &b.posB);
        fprintf(stderr, "DEBUG: Button B blitted\n");
    }
}

void libererB(B *b) {
    if (b->choixB) {
        SDL_FreeSurface(b->choixB);
        b->choixB = NULL;
        fprintf(stderr, "DEBUG: Freed button B\n");
    }
    if (b->choixBGlow) {
        SDL_FreeSurface(b->choixBGlow);
        b->choixBGlow = NULL;
        fprintf(stderr, "DEBUG: Freed button B glow\n");
    }
}

int initialiserC(C *c) {
    fprintf(stderr, "DEBUG: Initializing button C\n");
    c->choixC = IMG_Load("assets/enigma/c.png");
    c->choixCGlow = IMG_Load("assets/enigma/Cglow.png");
    if (!c->choixC || !c->choixCGlow) {
        fprintf(stderr, "ERROR: Failed to load C images: %s\n", IMG_GetError());
        return 0;
    }
    c->posC.x = 50;
    c->posC.y = 480;
    c->posC.w = 250;
    c->posC.h = 50;
    fprintf(stderr, "DEBUG: Button C loaded (%dx%d)\n", c->choixC->w, c->choixC->h);
    return 1;
}

void afficherC(C c, SDL_Surface *ecran, int isHovered) {
    if (!ecran) {
        fprintf(stderr, "ERROR: Invalid screen surface\n");
        return;
    }
    if (isHovered && c.choixCGlow) {
        SDL_BlitSurface(c.choixCGlow, NULL, ecran, &c.posC);
        fprintf(stderr, "DEBUG: Button C glow blitted\n");
    } else if (c.choixC) {
        SDL_BlitSurface(c.choixC, NULL, ecran, &c.posC);
        fprintf(stderr, "DEBUG: Button C blitted\n");
    }
}

void libererC(C *c) {
    if (c->choixC) {
        SDL_FreeSurface(c->choixC);
        c->choixC = NULL;
        fprintf(stderr, "DEBUG: Freed button C\n");
    }
    if (c->choixCGlow) {
        SDL_FreeSurface(c->choixCGlow);
        c->choixCGlow = NULL;
        fprintf(stderr, "DEBUG: Freed button C glow\n");
    }
}

int initialiserMusique() {
    fprintf(stderr, "DEBUG: Initializing SDL_mixer\n");
    if (Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 2, 1024) < 0) {
        fprintf(stderr, "ERROR: Failed to initialize SDL_mixer: %s\n", Mix_GetError());
        return 0;
    }
    fprintf(stderr, "DEBUG: SDL_mixer initialized\n");
    return 1;
}

void libererMusique(Mix_Music *music) {
    if (music) {
        Mix_FreeMusic(music);
        fprintf(stderr, "DEBUG: Freed music\n");
    }
    Mix_CloseAudio();
    fprintf(stderr, "DEBUG: Closed SDL_mixer\n");
}

Enigme initialiserEnigme(char *filename) {
    Enigme e = {0};
    fprintf(stderr, "DEBUG: Initializing enigme from %s\n", filename);
    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "ERROR: Cannot open file: %s\n", filename);
        return e;
    }

    char line[QUESTION_LENGTH + ANSWER_LENGTH * 3 + 4];
    int n = 0;
    while (fgets(line, sizeof(line), f)) n++;
    if (n == 0) {
        fprintf(stderr, "ERROR: Empty enigmes file: %s\n", filename);
        fclose(f);
        return e;
    }
    srand(time(NULL));
    int r = rand() % n;
    rewind(f);
    for (int i = 0; i < r; i++) fgets(line, sizeof(line), f);

    char *p = strtok(line, ";");
    if (p) strncpy(e.question, p, QUESTION_LENGTH - 1);
    p = strtok(NULL, ";");
    if (p) strncpy(e.answers[0], p, ANSWER_LENGTH - 1);
    p = strtok(NULL, ";");
    if (p) strncpy(e.answers[1], p, ANSWER_LENGTH - 1);
    p = strtok(NULL, ";");
    if (p) strncpy(e.answers[2], p, ANSWER_LENGTH - 1);
    p = strtok(NULL, ";");
    if (p) e.correctAnswer = atoi(p);
    fclose(f);
    fprintf(stderr, "DEBUG: Loaded enigme: %s (correct: %d)\n", e.question, e.correctAnswer);
    return e;
}

void afficherEnigme(SDL_Surface *screen, Enigme *e) {
    if (!screen || !e) {
        fprintf(stderr, "ERROR: Invalid screen or enigme\n");
        return;
    }
    fprintf(stderr, "DEBUG: Displaying enigme\n");
    TTF_Font *font = TTF_OpenFont("assets/enigma/arial.ttf", 28);
    if (!font) {
        fprintf(stderr, "ERROR: Failed to load font: %s\n", TTF_GetError());
        return;
    }
    SDL_Color color = {255, 255, 255, 255};

    SDL_Surface *texte_question = TTF_RenderUTF8_Blended(font, e->question, color);
    SDL_Surface *texte_reponse1 = TTF_RenderUTF8_Blended(font, e->answers[0], color);
    SDL_Surface *texte_reponse2 = TTF_RenderUTF8_Blended(font, e->answers[1], color);
    SDL_Surface *texte_reponse3 = TTF_RenderUTF8_Blended(font, e->answers[2], color);

    SDL_Rect position_question = {100, 100, 0, 0};
    SDL_Rect position_reponse1 = {400, 180, 0, 0};
    SDL_Rect position_reponse2 = {400, 340, 0, 0};
    SDL_Rect position_reponse3 = {400, 500, 0, 0};

    if (texte_question) {
        SDL_BlitSurface(texte_question, NULL, screen, &position_question);
        fprintf(stderr, "DEBUG: Question blitted\n");
    }
    if (texte_reponse1) {
        SDL_BlitSurface(texte_reponse1, NULL, screen, &position_reponse1);
        fprintf(stderr, "DEBUG: Answer 1 blitted\n");
    }
    if (texte_reponse2) {
        SDL_BlitSurface(texte_reponse2, NULL, screen, &position_reponse2);
        fprintf(stderr, "DEBUG: Answer 2 blitted\n");
    }
    if (texte_reponse3) {
        SDL_BlitSurface(texte_reponse3, NULL, screen, &position_reponse3);
        fprintf(stderr, "DEBUG: Answer 3 blitted\n");
    }

    SDL_FreeSurface(texte_question);
    SDL_FreeSurface(texte_reponse1);
    SDL_FreeSurface(texte_reponse2);
    SDL_FreeSurface(texte_reponse3);
    TTF_CloseFont(font);
}

int checkEnigme(SDL_Event *event, Enigme *e) {
    if (!event || !e) {
        fprintf(stderr, "ERROR: Invalid event or enigme\n");
        return -1;
    }
    int x, y;
    SDL_Rect position_reponse1 = {50, 160, 250, 50};
    SDL_Rect position_reponse2 = {50, 320, 250, 50};
    SDL_Rect position_reponse3 = {50, 480, 250, 50};

    if (event->type == SDL_MOUSEBUTTONDOWN) {
        x = event->button.x;
        y = event->button.y;
        fprintf(stderr, "DEBUG: Mouse click at (%d, %d)\n", x, y);

        if (x >= position_reponse1.x && x <= position_reponse1.x + position_reponse1.w &&
            y >= position_reponse1.y && y <= position_reponse1.y + position_reponse1.h) {
            fprintf(stderr, "DEBUG: Selected answer 0 (correct: %d)\n", e->correctAnswer == 0);
            return (e->correctAnswer == 0);
        }
        else if (x >= position_reponse2.x && x <= position_reponse2.x + position_reponse2.w &&
                 y >= position_reponse2.y && y <= position_reponse2.y + position_reponse2.h) {
            fprintf(stderr, "DEBUG: Selected answer 1 (correct: %d)\n", e->correctAnswer == 1);
            return (e->correctAnswer == 1);
        }
        else if (x >= position_reponse3.x && x <= position_reponse3.x + position_reponse3.w &&
                 y >= position_reponse3.y && y <= position_reponse3.y + position_reponse3.h) {
            fprintf(stderr, "DEBUG: Selected answer 2 (correct: %d)\n", e->correctAnswer == 2);
            return (e->correctAnswer == 2);
        }
    }
    return -1;
}

void updateScore(int *score, int isCorrect, Uint32 elapsedTime, int *combo) {
    fprintf(stderr, "DEBUG: Updating score (isCorrect: %d, elapsedTime: %u, combo: %d)\n", isCorrect, elapsedTime, *combo);
    if (isCorrect) {
        int timeBonus = (elapsedTime < 3000) ? BONUS_FAST : (elapsedTime < 5000) ? BONUS_MEDIUM : BONUS_SLOW;
        *combo += 1;
        *score += (*combo > 1) ? timeBonus * COMBO_MULTIPLIER : timeBonus;
        fprintf(stderr, "DEBUG: Score updated to %d (timeBonus: %d, combo: %d)\n", *score, timeBonus, *combo);
    } else {
        *combo = 0;
        *score -= SCORE_PENALTY;
        if (*score < 0) *score = 0;
        fprintf(stderr, "DEBUG: Score updated to %d (penalty: %d, combo reset)\n", *score, SCORE_PENALTY);
    }
}

void displayScoreMessage(SDL_Surface *screen, int score, int isCorrect) {
    if (!screen) {
        fprintf(stderr, "ERROR: Invalid screen surface\n");
        return;
    }
    fprintf(stderr, "DEBUG: Displaying score message (score: %d, isCorrect: %d)\n", score, isCorrect);
    SDL_Surface *message = SDL_CreateRGBSurface(SDL_SWSURFACE, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);
    if (!message) {
        fprintf(stderr, "ERROR: Failed to create message surface\n");
        return;
    }
    SDL_Color bgColor = isCorrect ? color_correct : color_incorrect;
    SDL_FillRect(message, NULL, SDL_MapRGB(message->format, bgColor.r, bgColor.g, bgColor.b));

    SDL_Surface *mask = createMask(SCREEN_WIDTH, SCREEN_HEIGHT, MASK_OPACITY);
    if (!mask) {
        fprintf(stderr, "ERROR: Failed to create mask\n");
        SDL_FreeSurface(message);
        return;
    }
    SDL_BlitSurface(mask, NULL, message, NULL);
    SDL_FreeSurface(mask);

    TTF_Font *font = TTF_OpenFont("assets/enigma/arial.ttf", 36);
    if (!font) {
        fprintf(stderr, "ERROR: Failed to load font: %s\n", TTF_GetError());
        SDL_FreeSurface(message);
        return;
    }
    SDL_Color textColor = {255, 255, 255, 255};
    char scoreText[100];
    sprintf(scoreText, "%s! Score: %d", isCorrect ? "Bonne réponse" : "Mauvaise réponse", score);
    SDL_Surface *text = TTF_RenderUTF8_Blended(font, scoreText, textColor);
    if (text) {
        SDL_Rect textPos = {(SCREEN_WIDTH - text->w)/2, (SCREEN_HEIGHT - text->h)/2, 0, 0};
        SDL_BlitSurface(text, NULL, message, &textPos);
        SDL_FreeSurface(text);
        fprintf(stderr, "DEBUG: Score text blitted\n");
    }
    SDL_BlitSurface(message, NULL, screen, NULL);
    SDL_Flip(screen);
    SDL_Delay(1500);
    SDL_FreeSurface(message);
    TTF_CloseFont(font);
}

void displayGameOver(SDL_Surface *screen, int score, int triesLeft) {
    if (!screen) {
        fprintf(stderr, "ERROR: Invalid screen surface\n");
        return;
    }
    fprintf(stderr, "DEBUG: Displaying game over (score: %d, triesLeft: %d)\n", score, triesLeft);
    SDL_Surface *message = SDL_CreateRGBSurface(SDL_SWSURFACE, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);
    if (!message) {
        fprintf(stderr, "ERROR: Failed to create game over surface\n");
        return;
    }
    SDL_FillRect(message, NULL, SDL_MapRGB(message->format, 0, 0, 0));

    SDL_Surface *mask = createMask(SCREEN_WIDTH, SCREEN_HEIGHT, MASK_OPACITY);
    if (mask) {
        SDL_BlitSurface(mask, NULL, message, NULL);
        SDL_FreeSurface(mask);
    }

    TTF_Font *font = TTF_OpenFont("assets/enigma/arial.ttf", 36);
    if (!font) {
        fprintf(stderr, "ERROR: Failed to load font: %s\n", TTF_GetError());
        SDL_FreeSurface(message);
        return;
    }
    SDL_Color textColor = {255, 255, 255, 255};
    char gameOverText[100];
    if (triesLeft == 0 && score >= 50) {
        sprintf(gameOverText, "Win! Final Score: %d", score);
    } else {
        sprintf(gameOverText, "Game Over! Final Score: %d", score);
    }
    SDL_Surface *text = TTF_RenderUTF8_Blended(font, gameOverText, textColor);
    if (text) {
        SDL_Rect textPos = {(SCREEN_WIDTH - text->w)/2, (SCREEN_HEIGHT - text->h)/2, 0, 0};
        SDL_BlitSurface(text, NULL, message, &textPos);
        SDL_FreeSurface(text);
        fprintf(stderr, "DEBUG: Game over text blitted\n");
    }
    SDL_BlitSurface(message, NULL, screen, NULL);
    SDL_Flip(screen);
    SDL_Delay(3000);
    SDL_FreeSurface(message);
    TTF_CloseFont(font);
}

SDL_Surface* createMask(int width, int height, Uint8 opacity) {
    fprintf(stderr, "DEBUG: Creating mask (width: %d, height: %d, opacity: %d)\n", width, height, opacity);
    SDL_Surface* mask = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32,
                                             0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (!mask) {
        fprintf(stderr, "ERROR: Failed to create mask surface\n");
        return NULL;
    }
    SDL_FillRect(mask, NULL, SDL_MapRGBA(mask->format, 0, 0, 0, opacity));
    return mask;
}

void showRotozoomAnimation(SDL_Surface *screen, const char *imagePath, int duration, int rotations) {
    if (!screen || !imagePath) {
        fprintf(stderr, "ERROR: Invalid screen or image path\n");
        return;
    }
    fprintf(stderr, "DEBUG: Starting rotozoom animation (%s, duration: %d, rotations: %d)\n", imagePath, duration, rotations);
    char fullPath[256];
    snprintf(fullPath, sizeof(fullPath), "assets/enigma/%s", imagePath);
    SDL_Surface *image = IMG_Load(fullPath);
    if (!image) {
        fprintf(stderr, "ERROR: Failed to load image %s: %s\n", fullPath, IMG_GetError());
        return;
    }

    Uint32 startTime = SDL_GetTicks();
    while (SDL_GetTicks() - startTime < duration) {
        float progress = (float)(SDL_GetTicks() - startTime) / duration;
        double angle = progress * 360 * rotations;
        double zoom = 1.0 + sin(progress * M_PI * 4) * 0.2;

        SDL_Surface *rotated = rotozoomSurface(image, angle, zoom, 1);
        if (rotated) {
            SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
            SDL_Rect pos = {(SCREEN_WIDTH - rotated->w)/2, (SCREEN_HEIGHT - rotated->h)/2};
            SDL_BlitSurface(rotated, NULL, screen, &pos);
            SDL_Flip(screen);
            SDL_FreeSurface(rotated);
        }
        SDL_Delay(16);
    }
    SDL_FreeSurface(image);
}

void showChestAnimation(SDL_Surface *screen) {
    if (!screen) {
        fprintf(stderr, "ERROR: Invalid screen surface\n");
        return;
    }
    fprintf(stderr, "DEBUG: Starting chest animation\n");
    SDL_Surface *spriteSheet = IMG_Load("assets/enigma/tresor f.png");
    if (!spriteSheet) {
        fprintf(stderr, "ERROR: Failed to load sprite sheet: %s\n", IMG_GetError());
        return;
    }

    SDL_Rect srcRect = {0, 0, 64, 64};
    SDL_Rect destRect = {(SCREEN_WIDTH - 64) / 2, (SCREEN_HEIGHT - 64) / 2, 64, 64};
    int frame = 0, numFrames = 3;

    Uint32 startTime = SDL_GetTicks();
    while (SDL_GetTicks() - startTime < 3000) {
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
        srcRect.x = frame * 64;
        SDL_BlitSurface(spriteSheet, &srcRect, screen, &destRect);
        SDL_Flip(screen);
        frame = (frame + 1) % numFrames;
        SDL_Delay(200);
    }
    SDL_FreeSurface(spriteSheet);
}

void initEnigme(GAME *game) {
    if (!game || !game->screen) {
        fprintf(stderr, "ERROR: Invalid game or screen\n");
        return;
    }

    EnigmaData *enigmaData = malloc(sizeof(EnigmaData));
    if (!enigmaData) {
        fprintf(stderr, "ERROR: Failed to allocate enigma data\n");
        return;
    }
    memset(enigmaData, 0, sizeof(EnigmaData));

    if (!initialiserMusique()) {
        fprintf(stderr, "ERROR: Failed to initialize SDL_mixer\n");
        free(enigmaData);
        return;
    }

    if (!initialiserBackg(&enigmaData->background) ||
        !initialiserA(&enigmaData->boutonA) ||
        !initialiserB(&enigmaData->boutonB) ||
        !initialiserC(&enigmaData->boutonC)) {
        fprintf(stderr, "ERROR: Failed to initialize game elements\n");
        libererBackg(&enigmaData->background);
        libererA(&enigmaData->boutonA);
        libererB(&enigmaData->boutonB);
        libererC(&enigmaData->boutonC);
        free(enigmaData);
        return;
    }

    enigmaData->music = Mix_LoadMUS("assets/enigma/music.mp3");
    if (enigmaData->music) {
        Mix_PlayMusic(enigmaData->music, -1);
        fprintf(stderr, "DEBUG: Music loaded and playing\n");
    } else {
        fprintf(stderr, "ERROR: Failed to load music: %s\n", Mix_GetError());
    }

    enigmaData->buttonSound = Mix_LoadWAV("assets/enigma/button.wav");
    if (!enigmaData->buttonSound) {
        fprintf(stderr, "ERROR: Failed to load button sound: %s\n", Mix_GetError());
    }

    for (int i = 0; i < 10; i++) {
        char filename[64];
        sprintf(filename, "assets/enigma/chrono%d.png", i + 1);
        enigmaData->chronoImages[i] = IMG_Load(filename);
        if (!enigmaData->chronoImages[i]) {
            fprintf(stderr, "ERROR: Failed to load chrono image %s: %s\n", filename, IMG_GetError());
        }
    }

    enigmaData->enigme = initialiserEnigme("assets/enigma/enigmes.txt");
    enigmaData->startTime = SDL_GetTicks();
    enigmaData->chronoPos.x = SCREEN_WIDTH - 150;
    enigmaData->chronoPos.y = 50;
    enigmaData->triesLeft = MAX_TRIES;
    enigmaData->score = INITIAL_SCORE;
    enigmaData->combo = 0;
    enigmaData->hoverA = 0;
    enigmaData->hoverB = 0;
    enigmaData->hoverC = 0;

    enigmaData->font = TTF_OpenFont("assets/enigma/arial.ttf", 24);
    if (!enigmaData->font) {
        fprintf(stderr, "ERROR: Failed to load font: %s\n", TTF_GetError());
    }

    game->enigma = enigmaData;
    fprintf(stderr, "DEBUG: Enigma initialized\n");
}

int updateEnigme(GAME *game, SDL_Event *event) {
    if (!game || !game->enigma || !event) {
        fprintf(stderr, "ERROR: Invalid game, enigma data, or event\n");
        return -1;
    }

    EnigmaData *enigmaData = (EnigmaData *)game->enigma;
    int result = -1;
    Uint32 elapsedTime;

    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) {
        if (enigmaData->buttonSound) Mix_PlayChannel(-1, enigmaData->buttonSound, 0);
        result = checkEnigme(event, &enigmaData->enigme);
        elapsedTime = SDL_GetTicks() - enigmaData->startTime;

        if (result != -1) {
            enigmaData->triesLeft--;
            updateScore(&enigmaData->score, result, elapsedTime, &enigmaData->combo);
            afficherBackg(enigmaData->background, game->screen);
            displayScoreMessage(game->screen, enigmaData->score, result);

            if (result) {
                showRotozoomAnimation(game->screen, "correct.png", 2000, 2);
                showChestAnimation(game->screen);
            } else {
                showRotozoomAnimation(game->screen, "echec.png", 2000, 1);
            }

            if (enigmaData->triesLeft > 0) {
                enigmaData->enigme = initialiserEnigme("assets/enigma/enigmes.txt");
                enigmaData->startTime = SDL_GetTicks();
                fprintf(stderr, "DEBUG: Loaded new enigme\n");
                return -1;
            } else {
                displayGameOver(game->screen, enigmaData->score, enigmaData->triesLeft);
                return (enigmaData->score >= 50) ? 1 : 0;
            }
        }
    }

    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    enigmaData->hoverA = (mouseX >= enigmaData->boutonA.posA.x && mouseX <= enigmaData->boutonA.posA.x + 250 &&
                          mouseY >= enigmaData->boutonA.posA.y && mouseY <= enigmaData->boutonA.posA.y + 50);
    enigmaData->hoverB = (mouseX >= enigmaData->boutonB.posB.x && mouseX <= enigmaData->boutonB.posB.x + 250 &&
                          mouseY >= enigmaData->boutonB.posB.y && mouseY <= enigmaData->boutonB.posB.y + 50);
    enigmaData->hoverC = (mouseX >= enigmaData->boutonC.posC.x && mouseX <= enigmaData->boutonC.posC.x + 250 &&
                          mouseY >= enigmaData->boutonC.posC.y && mouseY <= enigmaData->boutonC.posC.y + 50);

    elapsedTime = SDL_GetTicks() - enigmaData->startTime;
    enigmaData->currentImage = (elapsedTime / 1000) % 10;
    if (elapsedTime >= TIME_LIMIT) {
        enigmaData->triesLeft--;
        enigmaData->score = (enigmaData->score - SCORE_PENALTY > 0) ? enigmaData->score - SCORE_PENALTY : 0;
        if (enigmaData->triesLeft > 0) {
            enigmaData->enigme = initialiserEnigme("assets/enigma/enigmes.txt");
            enigmaData->startTime = SDL_GetTicks();
            fprintf(stderr, "DEBUG: Loaded new enigme after timeout\n");
            return -1;
        } else {
            displayGameOver(game->screen, enigmaData->score, enigmaData->triesLeft);
            return (enigmaData->score >= 50) ? 1 : 0;
        }
    }

    return -1;
}

void renderEnigme(GAME *game) {
    if (!game || !game->enigma || !game->screen) {
        fprintf(stderr, "ERROR: Invalid game, enigma data, or screen\n");
        return;
    }

    EnigmaData *enigmaData = (EnigmaData *)game->enigma;
    SDL_FillRect(game->screen, NULL, SDL_MapRGB(game->screen->format, 0, 0, 0));
    afficherBackg(enigmaData->background, game->screen);
    afficherEnigme(game->screen, &enigmaData->enigme);
    afficherA(enigmaData->boutonA, game->screen, enigmaData->hoverA);
    afficherB(enigmaData->boutonB, game->screen, enigmaData->hoverB);
    afficherC(enigmaData->boutonC, game->screen, enigmaData->hoverC);

    if (enigmaData->chronoImages[enigmaData->currentImage]) {
        SDL_BlitSurface(enigmaData->chronoImages[enigmaData->currentImage], NULL, game->screen, &enigmaData->chronoPos);
        fprintf(stderr, "DEBUG: Chrono image %d blitted at (%d, %d)\n", enigmaData->currentImage, enigmaData->chronoPos.x, enigmaData->chronoPos.y);
    }

    if (enigmaData->font) {
        char infoText[100];
        sprintf(infoText, "Essais: %d/%d | Score: %d | Combo: x%d",
                enigmaData->triesLeft, MAX_TRIES, enigmaData->score, enigmaData->combo);
        SDL_Color white = {255, 255, 255, 255};
        SDL_Surface *text = TTF_RenderText_Blended(enigmaData->font, infoText, white);
        if (text) {
            SDL_Rect textPos = {20, 20, 0, 0};
            SDL_BlitSurface(text, NULL, game->screen, &textPos);
            SDL_FreeSurface(text);
            fprintf(stderr, "DEBUG: Info text blitted\n");
        }
    }

    SDL_Flip(game->screen);
    fprintf(stderr, "DEBUG: Screen flipped\n");
}

void freeEnigme(GAME *game) {
    if (!game || !game->enigma) {
        fprintf(stderr, "ERROR: Invalid game or enigma data\n");
        return;
    }

    EnigmaData *enigmaData = (EnigmaData *)game->enigma;
    for (int i = 0; i < 10; i++) {
        if (enigmaData->chronoImages[i]) {
            SDL_FreeSurface(enigmaData->chronoImages[i]);
            enigmaData->chronoImages[i] = NULL;
        }
    }
    libererC(&enigmaData->boutonC);
    libererB(&enigmaData->boutonB);
    libererA(&enigmaData->boutonA);
    libererBackg(&enigmaData->background);
    if (enigmaData->music) {
        Mix_FreeMusic(enigmaData->music);
        enigmaData->music = NULL;
    }
    if (enigmaData->buttonSound) {
        Mix_FreeChunk(enigmaData->buttonSound);
        enigmaData->buttonSound = NULL;
    }
    if (enigmaData->font) {
        TTF_CloseFont(enigmaData->font);
        enigmaData->font = NULL;
    }
    libererMusique(NULL);
    free(enigmaData);
    game->enigma = NULL;
    fprintf(stderr, "DEBUG: Enigma freed\n");
}
