#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include "game.h"
#include <stdio.h>

void initInventory(Inventory *inventory) {
    printf("Starting initInventory...\n");

    inventory->uiImage = IMG_Load("assets/inventory/inventory.png");
    if (!inventory->uiImage) {
        fprintf(stderr, "Failed to load assets/inventory/inventory.png: %s\n", SDL_GetError());
        exit(1);
    }
    SDL_SetColorKey(inventory->uiImage, SDL_SRCCOLORKEY, SDL_MapRGB(inventory->uiImage->format, 255, 0, 255));
    printf("Loaded inventory UI image: assets/inventory/inventory.png (%dx%d)\n", inventory->uiImage->w, inventory->uiImage->h);

    inventory->frameImage = IMG_Load("assets/inventory/frame.png");
    if (!inventory->frameImage) {
        fprintf(stderr, "Failed to load assets/inventory/frame.png: %s\n", SDL_GetError());
        exit(1);
    }
    SDL_SetColorKey(inventory->frameImage, SDL_SRCCOLORKEY, SDL_MapRGB(inventory->frameImage->format, 255, 0, 255));
    printf("Loaded frame image: assets/inventory/frame.png (%dx%d)\n", inventory->frameImage->w, inventory->frameImage->h);

    inventory->items[0].icon = IMG_Load("assets/inventory/item0.png");
    if (!inventory->items[0].icon) {
        fprintf(stderr, "Failed to load assets/inventory/item0.png: %s\n", SDL_GetError());
        exit(1);
    }
    inventory->items[0].have = 1;
    printf("Loaded paper icon (item0.png) in slot 0: 40x40\n");

    inventory->items[1].icon = IMG_Load("assets/inventory/item1.png");
    if (!inventory->items[1].icon) {
        fprintf(stderr, "Failed to load assets/inventory/item1.png: %s\n", SDL_GetError());
        exit(1);
    }
    inventory->items[1].have = 1;
    printf("Loaded item1.png in slot 1\n");

    for (int i = 2; i < MAX_ITEMS; i++) {
        inventory->items[i].icon = NULL;
        inventory->items[i].have = 0;
    }
    inventory->count = 2;
    inventory->selectedSlot = 0; // Initialize selected slot to 0
    printf("Inventory initialized with %d items (paper in slot 0, item1 in slot 1), selectedSlot=%d.\n", inventory->count, inventory->selectedSlot);
}

void updateInventory(GAME *game) {
    if (game->input.pickup) {
        if (game->inventory.count < MAX_ITEMS) {
            game->inventory.items[game->inventory.count].have = 1;
            game->inventory.count++;
            printf("Picked up item %d/%d\n", game->inventory.count, MAX_ITEMS);
        } else {
            printf("Inventory full (%d/%d items)\n", game->inventory.count, MAX_ITEMS);
        }
        game->input.pickup = 0;
    }
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (game->input.useItem[i] && game->inventory.items[i].have == 1 && game->inventory.selectedSlot == i) {
            if (i == 0) { // Item 0 (paper) shows instructions.png
                game->global.showInstructions = 1;
                printf("Using item 0 (paper): showing instructions.png\n");
            } else {
                printf("Using item %d: no action defined.\n", i);
            }
            game->input.useItem[i] = 0;
        }
    }
}

void renderInventory(Inventory *inventory, SDL_Surface *screen, int showInventory, SDL_Rect playerPos) {
    if (!showInventory) {
        printf("Inventory not shown (inventoryVisible is false)\n");
        return;
    }

    if (inventory->uiImage) {
        SDL_Rect invPos = {playerPos.x + 30, playerPos.y - 90, 0, 0};
        SDL_BlitSurface(inventory->uiImage, NULL, screen, &invPos);
        printf("Rendering inventory UI at (%d, %d)\n", invPos.x, invPos.y);

        for (int i = 0; i < MAX_ITEMS; i++) {
            if (inventory->items[i].have && inventory->items[i].icon) {
                SDL_Rect itemPos = {invPos.x + 10 + i * 50, invPos.y + 10, 0, 0};
                SDL_BlitSurface(inventory->items[i].icon, NULL, screen, &itemPos);
                printf("Rendering item %d icon at (%d, %d)\n", i, itemPos.x, itemPos.y);
            }
        }

        if (inventory->frameImage) {
            SDL_Rect framePos = {invPos.x + 10 + inventory->selectedSlot * 50, invPos.y + 10, 0, 0};
            SDL_BlitSurface(inventory->frameImage, NULL, screen, &framePos);
            printf("Rendering frame.png on slot %d at (%d, %d)\n", inventory->selectedSlot, framePos.x, framePos.y);
        } else {
            fprintf(stderr, "Warning: frameImage is NULL\n");
        }
    } else {
        fprintf(stderr, "Warning: Cannot render inventory (uiImage is NULL)\n");
        return;
    }

    char countText[20];
    sprintf(countText, "%d/%d", inventory->count, MAX_ITEMS);
    TTF_Font *font = TTF_OpenFont("assets/fonts/arial.ttf", 16);
    if (!font) {
        fprintf(stderr, "Failed to load font for inventory count: %s\n", TTF_GetError());
        return;
    }
    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, countText, textColor);
    if (textSurface) {
        SDL_Rect textRect = {playerPos.x + 30, playerPos.y - 110, 0, 0};
        SDL_BlitSurface(textSurface, NULL, screen, &textRect);
        SDL_FreeSurface(textSurface);
    } else {
        fprintf(stderr, "Failed to render inventory count text: %s\n", TTF_GetError());
    }
    TTF_CloseFont(font);
}

void freeInventory(Inventory *inventory) {
    if (inventory->uiImage) {
        SDL_FreeSurface(inventory->uiImage);
        inventory->uiImage = NULL;
    }
    if (inventory->frameImage) {
        SDL_FreeSurface(inventory->frameImage);
        inventory->frameImage = NULL;
    }
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (inventory->items[i].icon) {
            SDL_FreeSurface(inventory->items[i].icon);
            inventory->items[i].icon = NULL;
        }
    }
    inventory->count = 0;
    inventory->selectedSlot = 0;
    printf("Inventory freed.\n");
}
