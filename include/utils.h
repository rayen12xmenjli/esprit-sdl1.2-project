#ifndef UTILS_H
#define UTILS_H

#include <SDL/SDL.h>

typedef struct {
    int active;
    int x, y;
    int frame;
    int direction;
} Bullet;

SDL_Surface* flipHorizontally(SDL_Surface *src);
SDL_Color get_pixel(SDL_Surface *surface, int x, int y);
int rectIntersect(const SDL_Rect *a, const SDL_Rect *b);
int pixelPerfectCollision(SDL_Surface *surface1, SDL_Rect *rect1, SDL_Rect *srcRect1,
                         SDL_Surface *surface2, SDL_Rect *rect2, SDL_Rect *srcRect2);

#endif
