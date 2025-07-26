#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_rotozoom.h>
#include "game.h"
#include "player2.h"
#include "collision.h"
#include "mouvement.h"

void initPlayer2(Player2 *player, int x, int y, struct GAME *game) {
    SDL_Surface* loaded = IMG_Load("assets/player2.png");
    if (!loaded) {
        fprintf(stderr, "initPlayer2: Failed to load sprite: %s\n", IMG_GetError());
        exit(1);
    }
    player->spriteSheet = loaded;
    player->spriteSheetFlipped = rotozoomSurfaceXY(loaded, 0, -1, 1, 0);
    if (!player->spriteSheetFlipped) {
        fprintf(stderr, "initPlayer2: Failed to create flipped sprite\n");
        exit(1);
    }
    SDL_SetColorKey(player->spriteSheet, SDL_SRCCOLORKEY, SDL_MapRGB(player->spriteSheet->format, 255, 0, 255));
    SDL_SetColorKey(player->spriteSheetFlipped, SDL_SRCCOLORKEY, SDL_MapRGB(player->spriteSheetFlipped->format, 255, 0, 255));

    player->position.x = x;
    player->position.y = y;
    player->world_x = x;
    player->frame.w = 192;
    player->frame.h = 192;
    player->position.w = player->frame.w;
    player->position.h = player->frame.h;
    player->frame.x = 0;
    player->frame.y = 0;
    player->frame_actuelle = 0;
    player->frame_max = 4;
    player->yVelocity = 0;
    player->vitesse = WALK_SPEED;
    player->onGround = 0;
    player->etat = P2_IDLE;
    player->lookingRight = 1;
    player->lives = 3;
    player->score = 0;
    player->health = 100;
    player->maxHealth = 100;
    player->ligneEtat = 0;
    player->kills = 0;

    // Place player on ground at initialization
    placePlayer2OnGround(game);
}

void updatePlayer2(Player2 *player, struct GAME *game) {
    static int frameDelay = 0;
    int frameDelayMax;

    // Handle movement and physics via movementPlayer2
    movementPlayer2(game);

    // Ensure jump input is reset when jump key is released
    if (!game->input.p2_jump && game->input.p2_jumpHeld) {
        game->input.p2_jumpHeld = 0;
        printf("updatePlayer2: Jump input released, p2_jumpHeld reset\n");
    }

    // Set animation parameters based on state
    switch (player->etat) {
        case P2_IDLE: 
            player->ligneEtat = 0; 
            player->frame_max = 4; 
            frameDelayMax = 20; 
            break;
        case P2_WALK: 
            player->ligneEtat = 1; 
            player->frame_max = 4; 
            frameDelayMax = 10; 
            break;
        case P2_RUN: 
            player->ligneEtat = 2; 
            player->frame_max = 4; 
            frameDelayMax = 6; 
            break;
        case P2_JUMP: 
            player->ligneEtat = 3; 
            player->frame_max = 2; 
            frameDelayMax = 10; 
            break;
        case P2_ATTACK_1: 
            player->ligneEtat = 4; 
            player->frame_max = 3; 
            frameDelayMax = 10; 
            break;
        case P2_HURT: 
            player->ligneEtat = 5; 
            player->frame_max = 4; 
            frameDelayMax = 10; 
            break;
        case P2_DEAD: 
            player->ligneEtat = 6; 
            player->frame_max = 4; 
            frameDelayMax = 15; 
            break;
    }

    // Update animation
    int previousEtat = player->etat;
    if (++frameDelay >= frameDelayMax) {
        player->frame_actuelle++;
        if (player->etat == P2_HURT || player->etat == P2_ATTACK_1) {
            if (player->frame_actuelle >= player->frame_max) {
                player->etat = P2_IDLE;
                player->frame_actuelle = 0;
            }
        } else if (player->etat == P2_DEAD) {
            if (player->frame_actuelle >= player->frame_max) {
                player->frame_actuelle = player->frame_max - 1;
                player->died = 1;
            }
        } else {
            player->frame_actuelle %= player->frame_max;
        }
        player->frame.x = player->frame_actuelle * player->frame.w;
        player->frame.y = player->ligneEtat * player->frame.h;
        frameDelay = 0;
    }

    // Re-ground player when transitioning to IDLE from HURT or ATTACK_1
    if (previousEtat != P2_IDLE && player->etat == P2_IDLE) {
        placePlayer2OnGround(game);
        printf("updatePlayer2: Re-grounded Player2 after state change to P2_IDLE\n");
    }

    // Handle melee attack
    if (game->input.p2_attack && player->etat != P2_DEAD && player->etat != P2_HURT) {
        player->etat = P2_ATTACK_1;
        player->frame_actuelle = 0;
    }

    // Handle damage
    if (game->input.p2_damage) {
        player->health -= 25;
        if (player->health <= 0) {
            player->health = 0;
            player->etat = P2_DEAD;
            player->lives--;
            player->frame_actuelle = 0;
        } else {
            player->etat = P2_HURT;
            player->frame_actuelle = 0;
        }
        game->input.p2_damage = 0;
    }
}

void renderPlayer2(SDL_Surface *screen, Player2 *player) {
    SDL_Surface* spriteToDisplay = player->lookingRight ? player->spriteSheet : player->spriteSheetFlipped;
    SDL_Rect frameToDisplay = player->frame;
    SDL_Rect destRect = {player->position.x, player->position.y, player->position.w, player->position.h};

    if (!player->lookingRight) {
        frameToDisplay.x = (spriteToDisplay->w - frameToDisplay.x) - frameToDisplay.w;
    }
    SDL_BlitSurface(spriteToDisplay, &frameToDisplay, screen, &destRect);
}

void freePlayer2(Player2 *player) {
    if (player->spriteSheet) SDL_FreeSurface(player->spriteSheet);
    if (player->spriteSheetFlipped) SDL_FreeSurface(player->spriteSheetFlipped);
}

void initCoins(Coin coins[], int count, int useDoubleBackground) {
    for (int i = 0; i < count; i++) {
        coins[i].sprite = IMG_Load("assets/coin.png");
        if (!coins[i].sprite) {
            fprintf(stderr, "initCoins: Failed to load coin %d: %s\n", i, IMG_GetError());
            exit(1);
        }
        SDL_SetColorKey(coins[i].sprite, SDL_SRCCOLORKEY, SDL_MapRGB(coins[i].sprite->format, 255, 0, 255));
        coins[i].x = 220 + (i % 7) * 70;
        coins[i].y = (i < 7) ? 500 : (useDoubleBackground ? 110 : 500);
        coins[i].position.x = coins[i].x;
        coins[i].position.y = coins[i].y;
        coins[i].position.w = COIN_WIDTH;
        coins[i].position.h = COIN_HEIGHT;
        coins[i].active = (i < 7 || useDoubleBackground);
    }
}

void renderCoins(SDL_Surface *screen, Coin coins[], int count) {
    for (int i = 0; i < count; i++) {
        if (coins[i].active) {
            SDL_Rect destRect = {coins[i].position.x - screen->clip_rect.x, coins[i].y, COIN_WIDTH, COIN_HEIGHT};
            SDL_BlitSurface(coins[i].sprite, NULL, screen, &destRect);
        }
    }
}

void displayScoreLivesPlayer2(SDL_Surface *screen, Player2 *player, int playerNum, TTF_Font *font) {
    char texte[50];
    SDL_Color couleur = {255, 255, 255, 0};
    sprintf(texte, "Joueur %d - Vies: %d Score: %d", playerNum, player->lives, player->score);
    SDL_Surface *texteSurface = TTF_RenderText_Solid(font, texte, couleur);
    if (!texteSurface) {
        fprintf(stderr, "displayScoreLivesPlayer2: TTF_RenderText_Solid failed: %s\n", TTF_GetError());
        return;
    }
    SDL_Rect posTexte = {10, playerNum * 30 + 20, 0, 0};
    SDL_BlitSurface(texteSurface, NULL, screen, &posTexte);
    SDL_FreeSurface(texteSurface);
}
