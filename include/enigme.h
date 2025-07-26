#ifndef ENIGME_H
#define ENIGME_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include "game.h"

#define QUESTION_LENGTH 256
#define ANSWER_LENGTH 128
#define MAX_TRIES 3
#define INITIAL_SCORE 100
#define SCORE_PENALTY 20
#define BONUS_FAST 30
#define BONUS_MEDIUM 20
#define BONUS_SLOW 10
#define COMBO_MULTIPLIER 2
#define TIME_LIMIT 10000
#define MASK_OPACITY 128
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define SCREEN_BPP 32

typedef struct {
    SDL_Surface *Bg;
    SDL_Rect posAff;
} Bg;

typedef struct {
    SDL_Surface *choixA, *choixAGlow;
    SDL_Rect posA;
} A;

typedef struct {
    SDL_Surface *choixB, *choixBGlow;
    SDL_Rect posB;
} B;

typedef struct {
    SDL_Surface *choixC, *choixCGlow;
    SDL_Rect posC;
} C;

typedef struct {
    char question[QUESTION_LENGTH];
    char answers[3][ANSWER_LENGTH];
    int correctAnswer;
} Enigme;

typedef struct {
    Bg background;
    A boutonA;
    B boutonB;
    C boutonC;
    Enigme enigme;
    Mix_Music *music;
    Mix_Chunk *buttonSound;
    SDL_Surface *chronoImages[10];
    SDL_Rect chronoPos;
    TTF_Font *font;
    Uint32 startTime;
    int currentImage;
    int triesLeft;
    int score;
    int combo;
    int hoverA, hoverB, hoverC;
} EnigmaData;

int initialiserBackg(Bg *b);
void afficherBackg(Bg b, SDL_Surface *ecran);
void libererBackg(Bg *b);
int initialiserA(A *a);
void afficherA(A a, SDL_Surface *ecran, int isHovered);
void libererA(A *a);
int initialiserB(B *b);
void afficherB(B b, SDL_Surface *ecran, int isHovered);
void libererB(B *b);
int initialiserC(C *c);
void afficherC(C c, SDL_Surface *ecran, int isHovered);
void libererC(C *c);
int initialiserMusique();
void libererMusique(Mix_Music *music);
Enigme initialiserEnigme(char *filename);
void afficherEnigme(SDL_Surface *screen, Enigme *e);
int checkEnigme(SDL_Event *event, Enigme *e);
void updateScore(int *score, int isCorrect, Uint32 elapsedTime, int *combo);
void displayScoreMessage(SDL_Surface *screen, int score, int isCorrect);
void displayGameOver(SDL_Surface *screen, int score, int triesLeft);
SDL_Surface* createMask(int width, int height, Uint8 opacity);
void showRotozoomAnimation(SDL_Surface *screen, const char *imagePath, int duration, int rotations);
void showChestAnimation(SDL_Surface *screen);
void initEnigme(GAME *game);
int updateEnigme(GAME *game, SDL_Event *event);
void renderEnigme(GAME *game);
void freeEnigme(GAME *game);

#endif
