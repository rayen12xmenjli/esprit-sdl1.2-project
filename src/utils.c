#include "utils.h"
#include <SDL/SDL.h>
#include <stdlib.h>

// Flips a surface horizontally
SDL_Surface* flipHorizontally(SDL_Surface *src) {
    if (!src) return NULL;

    SDL_Surface *dest = SDL_CreateRGBSurface(src->flags, src->w, src->h,
                                            src->format->BitsPerPixel,
                                            src->format->Rmask,
                                            src->format->Gmask,
                                            src->format->Bmask,
                                            src->format->Amask);
    if (!dest) {
        fprintf(stderr, "flipHorizontally: SDL_CreateRGBSurface failed: %s\n", SDL_GetError());
        return NULL;
    }

    // Lock surfaces if needed
    if (SDL_MUSTLOCK(src)) SDL_LockSurface(src);
    if (SDL_MUSTLOCK(dest)) SDL_LockSurface(dest);

    // Copy pixels in reverse order
    for (int y = 0; y < src->h; y++) {
        for (int x = 0; x < src->w; x++) {
            Uint32 pixel = 0;
            // Read pixel from source
            if (src->format->BytesPerPixel == 4) {
                pixel = ((Uint32*)src->pixels)[y * src->w + x];
            } else if (src->format->BytesPerPixel == 2) {
                pixel = ((Uint16*)src->pixels)[y * src->w + x];
            }
            // Write pixel to destination
            if (dest->format->BytesPerPixel == 4) {
                ((Uint32*)dest->pixels)[y * dest->w + (src->w - 1 - x)] = pixel;
            } else if (dest->format->BytesPerPixel == 2) {
                ((Uint16*)dest->pixels)[y * dest->w + (src->w - 1 - x)] = pixel;
            }
        }
    }

    // Unlock surfaces
    if (SDL_MUSTLOCK(src)) SDL_UnlockSurface(src);
    if (SDL_MUSTLOCK(dest)) SDL_UnlockSurface(dest);

    // Copy colorkey
    if (src->flags & SDL_SRCCOLORKEY) {
        SDL_SetColorKey(dest, SDL_SRCCOLORKEY, src->format->colorkey);
    }

    return dest;
}

// Gets the RGB color of a pixel at (x, y) on the surface
SDL_Color get_pixel(SDL_Surface *surface, int x, int y) {
    SDL_Color color = {0, 0, 0};
    if (!surface || x < 0 || x >= surface->w || y < 0 || y >= surface->h) {
        return color;
    }

    // Lock surface if needed
    if (SDL_MUSTLOCK(surface)) SDL_LockSurface(surface);

    Uint32 pixel;
    if (surface->format->BytesPerPixel == 4) {
        pixel = ((Uint32*)surface->pixels)[y * surface->w + x];
    } else if (surface->format->BytesPerPixel == 2) {
        pixel = ((Uint16*)surface->pixels)[y * surface->w + x];
    } else {
        if (SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);
        return color;
    }

    // Convert pixel to RGB
    SDL_GetRGB(pixel, surface->format, &color.r, &color.g, &color.b);

    if (SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);
    return color;
}

// Checks if two rectangles intersect
int rectIntersect(const SDL_Rect *a, const SDL_Rect *b) {
    if (!a || !b) return 0;
    return (a->x < b->x + b->w &&
            a->x + a->w > b->x &&
            a->y < b->y + b->h &&
            a->y + a->h > b->y);
}

// Checks for pixel-perfect collision between two surfaces
int pixelPerfectCollision(SDL_Surface *surface1, SDL_Rect *rect1, SDL_Rect *srcRect1,
                         SDL_Surface *surface2, SDL_Rect *rect2, SDL_Rect *srcRect2) {
    if (!surface1 || !rect1 || !srcRect1 || !surface2 || !rect2 || !srcRect2) {
        return 0;
    }

    // Check if rectangles intersect
    if (!rectIntersect(rect1, rect2)) {
        return 0;
    }

    // Compute intersection rectangle
    int x1 = rect1->x > rect2->x ? rect1->x : rect2->x;
    int y1 = rect1->y > rect2->y ? rect1->y : rect2->y;
    int x2 = (rect1->x + rect1->w) < (rect2->x + rect2->w) ? (rect1->x + rect1->w) : (rect2->x + rect2->w);
    int y2 = (rect1->y + rect1->h) < (rect2->y + rect2->h) ? (rect1->y + rect1->h) : (rect2->y + rect2->h);

    // Get colorkeys
    Uint32 colorkey1 = 0, colorkey2 = 0;
    int hasColorkey1 = (surface1->flags & SDL_SRCCOLORKEY) != 0;
    int hasColorkey2 = (surface2->flags & SDL_SRCCOLORKEY) != 0;
    if (hasColorkey1) colorkey1 = surface1->format->colorkey;
    if (hasColorkey2) colorkey2 = surface2->format->colorkey;

    // Lock surfaces if needed
    if (SDL_MUSTLOCK(surface1)) SDL_LockSurface(surface1);
    if (SDL_MUSTLOCK(surface2)) SDL_LockSurface(surface2);

    // Check pixels in the intersection area
    for (int y = y1; y < y2; y++) {
        for (int x = x1; x < x2; x++) {
            // Convert screen coordinates to local surface coordinates
            int localX1 = x - rect1->x + srcRect1->x;
            int localY1 = y - rect1->y + srcRect1->y;
            int localX2 = x - rect2->x + srcRect2->x;
            int localY2 = y - rect2->y + srcRect2->y;

            // Ensure coordinates are within source rectangles
            if (localX1 < srcRect1->x || localX1 >= srcRect1->x + srcRect1->w ||
                localY1 < srcRect1->y || localY1 >= srcRect1->y + srcRect1->h ||
                localX2 < srcRect2->x || localX2 >= srcRect2->x + srcRect2->w ||
                localY2 < srcRect2->y || localY2 >= srcRect2->y + srcRect2->h) {
                continue;
            }

            // Get pixel colors
            SDL_Color color1 = get_pixel(surface1, localX1, localY1);
            SDL_Color color2 = get_pixel(surface2, localX2, localY2);

            // Convert colors to pixel values
            Uint32 pixel1 = SDL_MapRGB(surface1->format, color1.r, color1.g, color1.b);
            Uint32 pixel2 = SDL_MapRGB(surface2->format, color2.r, color2.g, color2.b);

            // Check if pixels are non-transparent (not equal to colorkey)
            int pixel1Opaque = !hasColorkey1 || pixel1 != colorkey1;
            int pixel2Opaque = !hasColorkey2 || pixel2 != colorkey2;

            // If both pixels are opaque, collision detected
            if (pixel1Opaque && pixel2Opaque) {
                if (SDL_MUSTLOCK(surface1)) SDL_UnlockSurface(surface1);
                if (SDL_MUSTLOCK(surface2)) SDL_UnlockSurface(surface2);
                return 1;
            }
        }
    }

    // Unlock surfaces
    if (SDL_MUSTLOCK(surface1)) SDL_UnlockSurface(surface1);
    if (SDL_MUSTLOCK(surface2)) SDL_UnlockSurface(surface2);

    return 0;
}


