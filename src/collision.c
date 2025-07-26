#include "collision.h"
#include <SDL/SDL.h>
#include <stdio.h>
#include "game.h"
#include "utils.h"

void PutPixel(SDL_Surface *surface, int x, int y, Uint32 pixel) {
    if (!surface || x < 0 || x >= surface->w || y < 0 || y >= surface->h) {
        fprintf(stderr, "PutPixel: Invalid coordinates (%d,%d) for surface (%d,%d)\n", x, y, surface->w, surface->h);
        return;
    }

    SDL_LockSurface(surface);
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp) {
        case 1:
            *p = pixel;
            break;
        case 2:
            *(Uint16 *)p = pixel;
            break;
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            } else {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;
        case 4:
            *(Uint32 *)p = pixel;
            break;
    }

    SDL_UnlockSurface(surface);
}

void placePlayerOnGround(GAME *game) {
    if (!game || !game->background.levelCollision) {
        fprintf(stderr, "placePlayerOnGround: Game or levelCollision is NULL\n");
        return;
    }

    int x = game->player.world_x + game->player.position.w / 2;
    int y_offset = (SCREEN_HEIGHT - game->background.levelCollision->h) / 2;
    int y_start = game->player.position.y - y_offset;

    if (x < 0 || x >= game->background.levelCollision->w || y_start < -50 || y_start >= game->background.levelCollision->h + 50) {
        printf("placePlayerOnGround: Invalid coordinates x=%d, y_start=%d\n", x, y_start);
        return;
    }

    for (int y = y_start - 20; y <= y_start + 20 && y < game->background.levelCollision->h; y++) {
        if (y < 0) continue;
        SDL_Color color = get_pixel(game->background.levelCollision, x, y);
        if (color.r == 255 && color.g == 255 && color.b == 255) {
            game->player.position.y = y - game->player.position.h + y_offset;
            game->player.onGround = 1;
            game->player.yVelocity = 0;
            printf("placePlayerOnGround: Player placed at y=%d, onGround=%d\n", game->player.position.y, game->player.onGround);
            return;
        }
    }
}

void placeSoldierOnGround(GAME *game, Soldier *soldier) {
    if (!game || !game->background.levelCollision || !soldier) {
        fprintf(stderr, "placeSoldierOnGround: Game, soldier, or levelCollision is NULL\n");
        return;
    }

    int x = soldier->world_x + soldier->position.w / 2;
    int y_offset = (SCREEN_HEIGHT - game->background.levelCollision->h) / 2;
    int y_start = soldier->position.y - y_offset;

    if (x < 0 || x >= game->background.levelCollision->w || y_start < -50 || y_start >= game->background.levelCollision->h) {
        printf("placeSoldierOnGround: Invalid coordinates x=%d, y_start=%d\n", x, y_start);
        return;
    }

    for (int y = y_start - 20; y <= y_start + 20 && y < game->background.levelCollision->h; y++) {
        if (y < 0) continue;
        SDL_Color color = get_pixel(game->background.levelCollision, x, y);
        if (color.r == 255 && color.g == 255 && color.b == 255) {
            soldier->position.y = y - soldier->position.h + y_offset;
            soldier->yVelocity = 0;
            soldier->onGround = 1;
            printf("placeSoldierOnGround: Soldier placed at y=%d\n", soldier->position.y);
            return;
        }
    }

    soldier->yVelocity = 0;
    soldier->onGround = 1;
}

void placeSoldier2OnGround(GAME *game, Soldier2 *soldier) {
    if (!game || !game->background.levelCollision || !soldier) {
        fprintf(stderr, "placeSoldier2OnGround: Game, soldier, or levelCollision is NULL\n");
        return;
    }

    int x = soldier->world_x + soldier->position.w / 2;
    int y_offset = (SCREEN_HEIGHT - game->background.levelCollision->h) / 2;
    int y_start = soldier->position.y - y_offset;

    if (x < 0 || x >= game->background.levelCollision->w || y_start < -50 || y_start >= game->background.levelCollision->h) {
        printf("placeSoldier2OnGround: Invalid coordinates x=%d, y_start=%d\n", x, y_start);
        return;
    }

    for (int y = y_start - 20; y <= y_start + 20 && y < game->background.levelCollision->h; y++) {
        if (y < 0) continue;
        SDL_Color color = get_pixel(game->background.levelCollision, x, y);
        if (color.r == 255 && color.g == 255 && color.b == 255) {
            soldier->position.y = y - soldier->position.h + y_offset;
            soldier->yVelocity = 0;
            soldier->onGround = 1;
            printf("placeSoldier2OnGround: Soldier2 placed at y=%d\n", soldier->position.y);
            return;
        }
    }

    soldier->yVelocity = 0;
    soldier->onGround = 1;
}

void placeRobotOnGround(GAME *game, Robot *robot) {
    if (!game || !game->background.levelCollision || !robot) {
        fprintf(stderr, "placeRobotOnGround: Game, robot, or levelCollision is NULL\n");
        return;
    }

    int x = robot->world_x + robot->position.w / 2;
    int y_offset = (SCREEN_HEIGHT - game->background.levelCollision->h) / 2;
    int y_start = robot->position.y - y_offset;

    if (x < 0 || x >= game->background.levelCollision->w || y_start < -50 || y_start >= game->background.levelCollision->h) {
        printf("placeRobotOnGround: Invalid coordinates x=%d, y_start=%d\n", x, y_start);
        return;
    }

    for (int y = y_start - 20; y <= y_start + 20 && y < game->background.levelCollision->h; y++) {
        if (y < 0) continue;
        SDL_Color color = get_pixel(game->background.levelCollision, x, y);
        if (color.r == 255 && color.g == 255 && color.b == 255) {
            robot->position.y = y - robot->position.h + y_offset;
            robot->yVelocity = 0;
            robot->onGround = 1;
            printf("placeRobotOnGround: Robot placed at y=%d\n", robot->position.y);
            return;
        }
    }

    robot->yVelocity = 0;
    robot->onGround = 1;
}

void placeEnemyOnGround(GAME *game, Enemy *enemy) {
    if (!game || !enemy || !game->background.levelCollision) {
        fprintf(stderr, "placeEnemyOnGround: Game, enemy, or levelCollision is NULL\n");
        return;
    }

    int x_left = enemy->world_x;
    int y_offset = (SCREEN_HEIGHT - game->background.levelCollision->h) / 2;
    int y = enemy->position.y - y_offset;

    if (x_left < 0 || x_left >= game->background.levelCollision->w || y < -50 || y >= game->background.levelCollision->h) {
        printf("placeEnemyOnGround: Invalid coordinates x=%d, y=%d\n", x_left, y);
        return;
    }

    while (y < game->background.levelCollision->h) {
        int grounded = 0;
        for (int x = x_left; x <= x_left + enemy->position.w - 1; x += 5) {
            if (x >= 0 && x < game->background.levelCollision->w && y >= 0) {
                SDL_Color color = get_pixel(game->background.levelCollision, x, y);
                if (color.r == 255 && color.g == 255 && color.b == 255) {
                    enemy->position.y = y - enemy->position.h + y_offset;
                    enemy->yVelocity = 0;
                    enemy->onGround = 1;
                    printf("placeEnemyOnGround: Enemy placed at y=%d\n", enemy->position.y);
                    grounded = 1;
                    break;
                }
            }
        }
        if (grounded) break;
        y++;
    }
}

void placeEnemyLvl2OnGround(GAME *game, SDL_Rect *position, int world_x) {
    if (!game || !position || !game->background.levelCollision) {
        fprintf(stderr, "placeEnemyLvl2OnGround: Game, position, or levelCollision is NULL\n");
        return;
    }

    int x = world_x + position->w / 2;
    int y_offset = (SCREEN_HEIGHT - game->background.levelCollision->h) / 2;
    int y_start = position->y - y_offset;

    for (int y = y_start; y < game->background.levelCollision->h; y++) {
        if (y >= 0) {
            SDL_Color color = get_pixel(game->background.levelCollision, x, y);
            if (color.r == 255 && color.g == 255 && color.b == 255) {
                position->y = y - position->h + y_offset;
                printf("placeEnemyLvl2OnGround: Enemy placed at y=%d\n", position->y);
                return;
            }
        }
    }

    position->y = game->background.levelCollision->h - position->h + y_offset;
    printf("placeEnemyLvl2OnGround: No ground found, placed at y=%d\n", position->y);
}

void placeNPCOnGround(GAME *game, NPC *npc) {
    if (!game || !game->background.levelCollision || !npc) {
        fprintf(stderr, "placeNPCOnGround: Game, npc, or levelCollision is NULL\n");
        return;
    }

    int x_left = npc->world_x;
    int y_offset = (SCREEN_HEIGHT - game->background.levelCollision->h) / 2;
    int y_start = npc->position.y - y_offset;

    if (x_left < 0 || x_left >= game->background.levelCollision->w || y_start < -50 || y_start >= game->background.levelCollision->h) {
        printf("placeNPCOnGround: Invalid coordinates x=%d, y_start=%d\n", x_left, y_start);
        return;
    }

    while (y_start < game->background.levelCollision->h) {
        int grounded = 0;
        for (int x = x_left; x <= x_left + npc->position.w - 1; x += 5) {
            if (x >= 0 && x < game->background.levelCollision->w && y_start >= 0) {
                SDL_Color color = get_pixel(game->background.levelCollision, x, y_start);
                if (color.r == 255 && color.g == 255 && color.b == 255) {
                    npc->position.y = y_start - npc->position.h + y_offset;
                    printf("placeNPCOnGround: NPC placed at x=%d, y=%d\n", npc->world_x, npc->position.y);
                    grounded = 1;
                    break;
                }
            }
        }
        if (grounded) break;
        y_start++;
    }

    if (y_start >= game->background.levelCollision->h) {
        npc->position.y = game->background.levelCollision->h - npc->position.h + y_offset;
        printf("placeNPCOnGround: No ground found, placed at y=%d\n", npc->position.y);
    }
}

void placeNPC2OnGround(GAME *game, NPC2 *npc) {
    if (!game || !game->background.levelCollision || !npc) {
        fprintf(stderr, "placeNPC2OnGround: Game, npc, or levelCollision is NULL\n");
        return;
    }

    int x_left = npc->world_x;
    int y_offset = (SCREEN_HEIGHT - game->background.levelCollision->h) / 2;
    int y_start = npc->position.y - y_offset;

    if (x_left < 0 || x_left >= game->background.levelCollision->w || y_start < -50 || y_start >= game->background.levelCollision->h) {
        fprintf(stderr, "placeNPC2OnGround: Invalid coordinates x=%d, y_start=%d\n", x_left, y_start);
        return;
    }

    while (y_start < game->background.levelCollision->h) {
        int grounded = 0;
        for (int x = x_left; x <= x_left + npc->position.w - 1; x += 5) {
            if (x >= 0 && x < game->background.levelCollision->w && y_start >= 0) {
                SDL_Color color = get_pixel(game->background.levelCollision, x, y_start);
                if (color.r == 255 && color.g == 255 && color.b == 255) {
                    npc->position.y = y_start - npc->position.h + y_offset;
                    printf("placeNPC2OnGround: NPC2 placed at x=%d, y=%d, size=%dx%d\n",
                           npc->world_x, npc->position.y, npc->position.w, npc->position.h);
                    grounded = 1;
                    break;
                }
            }
        }
        if (grounded) break;
        y_start++;
    }

    if (y_start >= game->background.levelCollision->h) {
        npc->position.y = game->background.levelCollision->h - npc->position.h + y_offset;
        fprintf(stderr, "placeNPC2OnGround: No ground found, placed at y=%d\n", npc->position.y);
    }

    // Verify placement
    if (npc->position.y < 0 || npc->position.y > game->background.levelCollision->h - npc->position.h + y_offset) {
        fprintf(stderr, "placeNPC2OnGround: Invalid y position %d, clamping\n", npc->position.y);
        npc->position.y = game->background.levelCollision->h - npc->position.h + y_offset;
    }
}

Color collision_color(GAME *game, int x, int y) {
    if (!game || !game->background.levelCollision) {
        fprintf(stderr, "collision_color: Game or levelCollision is NULL\n");
        return 0;
    }

    if (x < 0 || x >= game->background.levelCollision->w || y < 0 || y >= game->background.levelCollision->h) {
        printf("collision_color: Invalid coordinates (%d,%d) for levelCollision (%d,%d)\n",
               x, y, game->background.levelCollision->w, game->background.levelCollision->h);
        return 0;
    }

    SDL_Color color = get_pixel(game->background.levelCollision, x, y);
    if (color.r == 255 && color.g == 0 && color.b == 0) {
        return RED;
    }
    if (color.r == 255 && color.g == 0 && color.b == 42) {
        return DOOR_RED;
    }
    if (color.r == 0 && color.g == 255 && color.b == 0) {
        return GREEN_ZONE;
    }
    return 0;
}

void placePlayer2OnGround(GAME *game) {
    if (!game || !game->background.levelCollision) {
        fprintf(stderr, "placePlayer2OnGround: Game or levelCollision is NULL\n");
        return;
    }

    int x = game->player2.world_x + game->player2.position.w / 2;
    int y_offset = (SCREEN_HEIGHT - game->background.levelCollision->h) / 2;
    int y_start = game->player2.position.y - y_offset;

    if (x < 0 || x >= game->background.levelCollision->w || y_start < -50 || y_start >= game->background.levelCollision->h + 50) {
        printf("placePlayer2OnGround: Invalid coordinates x=%d, y_start=%d\n", x, y_start);
        return;
    }

    for (int y = y_start - 20; y <= y_start + 20 && y < game->background.levelCollision->h; y++) {
        if (y < 0) continue;
        SDL_Color color = get_pixel(game->background.levelCollision, x, y);
        if (color.r == 255 && color.g == 255 && color.b == 255) {
            game->player2.position.y = y - game->player2.position.h + y_offset;
            game->player2.onGround = 1;
            game->player2.yVelocity = 0;
            printf("placePlayer2OnGround: Player2 placed at y=%d, onGround=%d\n", game->player2.position.y, game->player2.onGround);
            return;
        }
    }
}
