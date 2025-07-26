#include "mouvement.h"
#include "game.h"
#include "collision.h"
#include <SDL/SDL.h>
#include <stdio.h>
#include "utils.h"

int onGround(GAME *game, Player *player) {
    if (!game || !player || !game->background.levelCollision) {
        fprintf(stderr, "onGround: Game, player, or levelCollision is NULL\n");
        return 0;
    }

    int x_left = player->world_x;
    int x_right = x_left + player->position.w - 1;
    int y_offset = (SCREEN_HEIGHT - game->background.levelCollision->h) / 2;
    int y_bottom = player->position.y + player->position.h - y_offset;

    if (y_bottom < 0 || y_bottom >= game->background.levelCollision->h) {
        printf("onGround: Invalid y_bottom %d for levelCollision height %d\n",
               y_bottom, game->background.levelCollision->h);
        return 0;
    }

    for (int x = x_left; x <= x_right; x += 5) {
        if (x >= 0 && x < game->background.levelCollision->w) {
            SDL_Color color = get_pixel(game->background.levelCollision, x, y_bottom);
            printf("onGround: Checking pixel at (%d, %d): R=%d, G=%d, B=%d\n", x, y_bottom, color.r, color.g, color.b);
            if (color.r == 255 && color.g == 255 && color.b == 255) {
                return 1;
            }
        }
    }
    return 0;
}

int onGroundSoldier(GAME *game, Soldier *soldier) {
    if (!game || !soldier || !game->background.levelCollision) {
        fprintf(stderr, "onGroundSoldier: Game, soldier, or levelCollision is NULL\n");
        return 0;
    }

    int x_left = soldier->world_x;
    int x_right = x_left + soldier->position.w - 1;
    int y_offset = (SCREEN_HEIGHT - game->background.levelCollision->h) / 2;
    int y_bottom = soldier->position.y + soldier->position.h - y_offset;

    if (y_bottom < 0 || y_bottom >= game->background.levelCollision->h) {
        return 0;
    }

    for (int x = x_left; x <= x_right; x += 5) {
        if (x >= 0 && x < game->background.levelCollision->w) {
            SDL_Color color = get_pixel(game->background.levelCollision, x, y_bottom);
            if (color.r == 255 && color.g == 255 && color.b == 255) {
                return 1;
            }
        }
    }
    return 0;
}

int onGroundSoldier2(GAME *game, Soldier2 *soldier) {
    if (!game || !soldier || !game->background.levelCollision) {
        fprintf(stderr, "onGroundSoldier2: Game, soldier, or levelCollision is NULL\n");
        return 0;
    }

    int x_left = soldier->world_x;
    int x_right = x_left + soldier->position.w - 1;
    int y_offset = (SCREEN_HEIGHT - game->background.levelCollision->h) / 2;
    int y_bottom = soldier->position.y + soldier->position.h - y_offset;

    if (y_bottom < 0 || y_bottom >= game->background.levelCollision->h) {
        return 0;
    }

    for (int x = x_left; x <= x_right; x += 5) {
        if (x >= 0 && x < game->background.levelCollision->w) {
            SDL_Color color = get_pixel(game->background.levelCollision, x, y_bottom);
            if (color.r == 255 && color.g == 255 && color.b == 255) {
                return 1;
            }
        }
    }
    return 0;
}

int onGroundEntity(GAME *game, SDL_Rect *position, int world_x) {
    if (!game || !position || !game->background.levelCollision) {
        fprintf(stderr, "onGroundEntity: Game, position, or levelCollision is NULL\n");
        return 0;
    }

    int x_left = world_x;
    int x_right = x_left + position->w - 1;
    int y_offset = (SCREEN_HEIGHT - game->background.levelCollision->h) / 2;
    int y_bottom = position->y + position->h - y_offset;

    if (y_bottom < 0 || y_bottom >= game->background.levelCollision->h) {
        return 0;
    }

    for (int x = x_left; x <= x_right; x += 5) {
        if (x >= 0 && x < game->background.levelCollision->w) {
            SDL_Color color = get_pixel(game->background.levelCollision, x, y_bottom);
            if (color.r == 255 && color.g == 255 && color.b == 255) {
                return 1;
            }
        }
    }
    return 0;
}

int onGroundEnemyLvl2(GAME *game, SDL_Rect *position, int world_x) {
    return onGroundEntity(game, position, world_x);
}

int onGroundBoss(GAME *game, Boss *boss) {
    if (!game || !boss || !game->background.levelCollision) {
        fprintf(stderr, "onGroundBoss: Game, boss, or levelCollision is NULL\n");
        return 0;
    }

    int x_left = boss->world_x;
    int x_right = x_left + boss->position.w - 1;
    int y_offset = (SCREEN_HEIGHT - game->background.levelCollision->h) / 2;
    int y_bottom = boss->y + boss->position.h - y_offset;

    if (y_bottom < 0 || y_bottom >= game->background.levelCollision->h) {
        printf("onGroundBoss: Invalid y_bottom %d for levelCollision height %d\n",
               y_bottom, game->background.levelCollision->h);
        return 0;
    }

    for (int x = x_left; x <= x_right; x += 5) {
        if (x >= 0 && x < game->background.levelCollision->w) {
            SDL_Color color = get_pixel(game->background.levelCollision, x, y_bottom);
            if (color.r == 255 && color.g == 255 && color.b == 255) {
                return 1;
            }
        }
    }
    return 0;
}

int onGroundPlayer2(GAME *game, Player2 *player2) {
    if (!game || !player2 || !game->background.levelCollision) {
        fprintf(stderr, "onGroundPlayer2: Game, player2, or levelCollision is NULL\n");
        return 0;
    }

    int x_left = player2->world_x;
    int x_right = x_left + player2->position.w - 1;
    int y_offset = (SCREEN_HEIGHT - game->background.levelCollision->h) / 2;
    int y_bottom = player2->position.y + player2->position.h - y_offset;

    if (y_bottom < 0 || y_bottom >= game->background.levelCollision->h) {
        printf("onGroundPlayer2: Invalid y_bottom %d for levelCollision height %d\n",
               y_bottom, game->background.levelCollision->h);
        return 0;
    }

    for (int x = x_left; x <= x_right; x += 5) {
        if (x >= 0 && x < game->background.levelCollision->w) {
            SDL_Color color = get_pixel(game->background.levelCollision, x, y_bottom);
            printf("onGroundPlayer2: Checking pixel at (%d, %d): R=%d, G=%d, B=%d\n", 
                   x, y_bottom, color.r, color.g, color.b);
            if (color.r == 255 && color.g == 255 && color.b == 255) {
                return 1;
            }
        }
    }
    return 0;
}
void movement(GAME *game) {
    if (!game || !game->background.levelCollision) {
        fprintf(stderr, "movement: Game or levelCollision is NULL\n");
        return;
    }

    Player *player = &game->player;

   
    if (!player->onGround && !player->freezeYMovement) {
        player->yVelocity += GRAVITY;
    }

    
    if ((game->input.up || game->input.jump) && player->onGround && !game->input.jumpHeld && 
        player->state != HURT && player->state != DEAD && !player->freezeYMovement) {
        player->yVelocity = -JUMP_POWER;
        player->onGround = 0;
        player->isJumping = 1;
        player->state = JUMP;
        game->input.jumpHeld = 1;
        printf("Jump triggered, yVelocity=%.2f, onGround=%d, state=%d\n", 
               player->yVelocity, player->onGround, player->state);
    } else if (!(game->input.up || game->input.jump) && game->input.jumpHeld) {
        game->input.jumpHeld = 0;
    }

   
    if (!player->freezeYMovement) {
        player->position.y += (int)player->yVelocity;
    }

   
    int newOnGround = onGround(game, player);
    if (newOnGround && player->yVelocity > 0 && !player->freezeYMovement) {
        player->yVelocity = 0;
        placePlayerOnGround(game);
    }
    player->onGround = newOnGround;

   
    int moveSpeed = game->input.run ? RUN_SPEED : WALK_SPEED;
    int dx = 0;
    if (game->input.right && player->state != HURT && player->state != DEAD) {
        dx = moveSpeed;
        player->facing = RIGHT;
        if (player->onGround && (player->state == IDLE || player->state == WALK || player->state == RUN)) {
            player->state = game->input.run ? RUN : WALK;
        }
    }
    if (game->input.left && player->state != HURT && player->state != DEAD) {
        dx = -moveSpeed;
        player->facing = LEFT;
        if (player->onGround && (player->state == IDLE || player->state == WALK || player->state == RUN)) {
            player->state = game->input.run ? RUN : WALK;
        }
    }

    
    player->world_x += dx;

    
    int target_x = SCREEN_WIDTH / 2;
    int desired_scroll_x = player->world_x - target_x;
    if (desired_scroll_x < 0) {
        desired_scroll_x = 0;
    } else if (desired_scroll_x > game->background.image->w - SCREEN_WIDTH) {
        desired_scroll_x = game->background.image->w - SCREEN_WIDTH;
    }

    float lerp_factor = 0.1f;
    game->background.scroll_x += (int)((desired_scroll_x - game->background.scroll_x) * lerp_factor);
    if (game->background.scroll_x < 0) {
        game->background.scroll_x = 0;
    } else if (game->background.scroll_x > game->background.image->w - SCREEN_WIDTH) {
        game->background.scroll_x = game->background.image->w - SCREEN_WIDTH;
    }

   
    player->position.x = player->world_x - game->background.scroll_x;

    
    if (player->world_x < 0) {
        player->world_x = 0;
        player->position.x = 0;
    } else if (player->world_x > game->background.image->w - player->position.w) {
        player->world_x = game->background.image->w - player->position.w;
        player->position.x = player->world_x - game->background.scroll_x;
    }

    
    if (!game->input.left && !game->input.right && player->onGround &&
        (player->state == WALK || player->state == RUN)) {
        player->state = IDLE;
        player->frame = 0;
    }

    printf("Player: screen_x=%d, world_x=%d, y=%d, onGround=%d, yVelocity=%.2f, state=%d, scroll_x=%d\n",
           player->position.x, player->world_x, player->position.y, player->onGround, 
           player->yVelocity, player->state, game->background.scroll_x);
}

void movementPlayer2(GAME *game) {
    if (!game || !game->background.levelCollision) {
        fprintf(stderr, "movementPlayer2: Game or levelCollision is NULL\n");
        return;
    }

    Player2 *player2 = &game->player2;

    // Apply gravity if not grounded
    if (!player2->onGround) {
        player2->yVelocity += GRAVITY;
    }

    // Handle jump
    if (game->input.p2_jump && player2->onGround && !game->input.p2_jumpHeld && 
        player2->etat != P2_HURT && player2->etat != P2_DEAD) {
        player2->yVelocity = -JUMP_POWER;
        player2->onGround = 0;
        player2->etat = P2_JUMP;
        game->input.p2_jumpHeld = 1;
        printf("Player2 Jump triggered, yVelocity=%.2f, onGround=%d, etat=%d\n", 
               player2->yVelocity, player2->onGround, player2->etat);
    } else if (!game->input.p2_jump && game->input.p2_jumpHeld) {
        game->input.p2_jumpHeld = 0;
    }

    // Apply vertical movement
    player2->position.y += (int)player2->yVelocity;

    // Check ground status
    int newOnGround = onGroundPlayer2(game, player2);
    if (newOnGround && player2->yVelocity > 0) {
        player2->yVelocity = 0;
        placePlayer2OnGround(game);
    }
    player2->onGround = newOnGround;

    // Handle horizontal movement
    int moveSpeed = game->input.p2_run ? RUN_SPEED : WALK_SPEED;
    int dx = 0;
    if (game->input.p2_right && player2->etat != P2_HURT && player2->etat != P2_DEAD) {
        dx = moveSpeed;
        player2->lookingRight = 1;
        if (player2->onGround && (player2->etat == P2_IDLE || player2->etat == P2_WALK || player2->etat == P2_RUN)) {
            player2->etat = game->input.p2_run ? P2_RUN : P2_WALK;
        }
    }
    if (game->input.p2_left && player2->etat != P2_HURT && player2->etat != P2_DEAD) {
        dx = -moveSpeed;
        player2->lookingRight = 0;
        if (player2->onGround && (player2->etat == P2_IDLE || player2->etat == P2_WALK || player2->etat == P2_RUN)) {
            player2->etat = game->input.p2_run ? P2_RUN : P2_WALK;
        }
    }

    // Update world_x
    player2->world_x += dx;

    // Update screen position (relative to scroll_x, which is controlled by Player1)
    player2->position.x = player2->world_x - game->background.scroll_x;

    // Keep Player2 within map bounds
    if (player2->world_x < 0) {
        player2->world_x = 0;
        player2->position.x = 0;
    } else if (player2->world_x > game->background.image->w - player2->position.w) {
        player2->world_x = game->background.image->w - player2->position.w;
        player2->position.x = player2->world_x - game->background.scroll_x;
    }

    // Transition to idle state when not moving
    if (!game->input.p2_left && !game->input.p2_right && player2->onGround &&
        (player2->etat == P2_WALK || player2->etat == P2_RUN)) {
        player2->etat = P2_IDLE;
        player2->frame_actuelle = 0;
    }

    printf("Player2: screen_x=%d, world_x=%d, y=%d, onGround=%d, yVelocity=%.2f, etat=%d, scroll_x=%d\n",
           player2->position.x, player2->world_x, player2->position.y, player2->onGround, 
           player2->yVelocity, player2->etat, game->background.scroll_x);
}
