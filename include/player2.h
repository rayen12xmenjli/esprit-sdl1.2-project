#ifndef PLAYER2_H
#define PLAYER2_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

// Forward declaration for GAME
struct GAME;

typedef enum {
    P2_IDLE,
    P2_WALK,
    P2_RUN,
    P2_JUMP,
    P2_ATTACK_1,
    P2_HURT,
    P2_DEAD
} Player2State;


#define COIN_WIDTH 64
#define COIN_HEIGHT 64

typedef struct {
    int x, y;
    int active;
    SDL_Surface *sprite;
    SDL_Rect position;
} Coin;

typedef struct {
    SDL_Rect position;
    int world_x;
    float yVelocity; // Changed from vitesse_y
    int onGround; // Changed from en_saut (inverted logic)
    Player2State etat;
    int lookingRight;
    int frame_actuelle;
    SDL_Surface *spriteSheet;
    SDL_Surface *spriteSheetFlipped;
    SDL_Rect frame;
    int frame_max;
    int health;
    int maxHealth;
    int lives;
    int score;
    int kills;
    int vitesse;
    int ligneEtat;
    int died;
    int freezeYMovement;  // Add this line
} Player2;

void initPlayer2(Player2 *player, int x, int y, struct GAME *game);
void updatePlayer2(Player2 *player, struct GAME *game);
void renderPlayer2(SDL_Surface *screen, Player2 *player);
void freePlayer2(Player2 *player);
void initCoins(Coin coins[], int count, int useDoubleBackground);
void renderCoins(SDL_Surface *screen, Coin coins[], int count);
void displayScoreLivesPlayer2(SDL_Surface *screen, Player2 *player, int playerNum, TTF_Font *font);
void placePlayer2OnGround(struct GAME *game);
int onGroundPlayer2(struct GAME *game, Player2 *player);
void movementPlayer2(struct GAME *game);

#endif
