#ifndef JET_H
#define JET_H

#include <SDL/SDL.h>
#include "game.h"



void initJet(Jet* jet);
void updateJet(Jet* jet, Player* player, int scroll_x);
void renderJet(SDL_Surface* screen, Jet* jet, int scroll_x);
void freeJet(Jet* jet);

#endif
