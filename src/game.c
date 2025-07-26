#include "game.h"
#include <stdio.h>

static int hasIntersection(const SDL_Rect *a, const SDL_Rect *b) {
    return (a->x < b->x + b->w &&
            a->x + a->w > b->x &&
            a->y < b->y + b->h &&
            a->y + a->h > b->y);
}

void initGame(Game *game, SDL_Surface *screen, GameState *state) {
    game->screen = screen;
    game->state = *state;
    game->paddle1.x = 50; game->paddle1.y = 310; game->paddle1.w = 20; game->paddle1.h = 100;
    game->paddle2.x = 1210; game->paddle2.y = 310; game->paddle2.w = 20; game->paddle2.h = 100;
    game->ballPos.x = 640; game->ballPos.y = 360; game->ballPos.w = 20; game->ballPos.h = 20;
    game->ballVelX = 5; game->ballVelY = 5;
    game->score1 = 0; game->score2 = 0;
}

void updateGame(Game *game) {
    game->ballPos.x += game->ballVelX;
    game->ballPos.y += game->ballVelY;

    if (game->ballPos.y <= 0 || game->ballPos.y + game->ballPos.h >= 720) {
        game->ballVelY = -game->ballVelY;
    }

    if (hasIntersection(&game->ballPos, &game->paddle1) || 
        hasIntersection(&game->ballPos, &game->paddle2)) {
        game->ballVelX = -game->ballVelX;
    }

    if (game->ballPos.x <= 0) {
        game->score2++;
        game->ballPos.x = 640; game->ballPos.y = 360; game->ballVelX = 5;
    } else if (game->ballPos.x + game->ballPos.w >= 1280) {
        game->score1++;
        game->ballPos.x = 640; game->ballPos.y = 360; game->ballVelX = -5;
    }

    if (game->paddle1.y < 0) game->paddle1.y = 0;
    if (game->paddle1.y + game->paddle1.h > 720) game->paddle1.y = 720 - game->paddle1.h;
    if (game->paddle2.y < 0) game->paddle2.y = 0;
    if (game->paddle2.y + game->paddle2.h > 720) game->paddle2.y = 720 - game->paddle2.h;
}

void renderGame(Game *game) {
    SDL_FillRect(game->screen, NULL, SDL_MapRGB(game->screen->format, 0, 0, 0));
    SDL_FillRect(game->screen, &game->paddle1, SDL_MapRGB(game->screen->format, 255, 255, 255));
    SDL_FillRect(game->screen, &game->paddle2, SDL_MapRGB(game->screen->format, 255, 255, 255));
    SDL_FillRect(game->screen, &game->ballPos, SDL_MapRGB(game->screen->format, 255, 255, 255));
    SDL_Flip(game->screen);
}

void handleGameEvents(Game *game, SDL_Event *event) {
    if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.sym) {
            case SDLK_w: game->paddle1.y -= 20; break;
            case SDLK_s: game->paddle1.y += 20; break;
            case SDLK_UP: game->paddle2.y -= 20; break;
            case SDLK_DOWN: game->paddle2.y += 20; break;
            case SDLK_ESCAPE: game->state = STATE_MAIN_MENU; break;
            default: break;
        }
    }
}

void cleanupGame(Game *game) {
    // No additional cleanup needed
}
