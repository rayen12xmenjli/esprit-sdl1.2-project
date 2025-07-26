#ifndef PLAYER_H
#define PLAYER_H

#include "game.h"

void initPlayer(Player *player, SDL_Surface *screen);
void updatePlayer(Player *player, GAME *game);
void renderPlayer(Player *player, GAME *game);
void freePlayer(Player *player);

#endif
