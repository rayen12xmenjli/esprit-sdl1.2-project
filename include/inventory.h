#ifndef INVENTORY_H
#define INVENTORY_H

#include "game.h"

void initInventory(Inventory *inventory);
void updateInventory(GAME *game);
void renderInventory(Inventory *inventory, SDL_Surface *screen, int showInventory, SDL_Rect playerPos);
void freeInventory(Inventory *inventory);

#endif
