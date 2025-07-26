#ifndef UI_H
#define UI_H

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include "game.h"

void initUI(UI *ui);
void updateUI(UI *ui, int playerLives);
void renderUI(UI *ui, SDL_Surface *screen, Player *player, Global *global);
void freeUI(UI *ui);

#endif
