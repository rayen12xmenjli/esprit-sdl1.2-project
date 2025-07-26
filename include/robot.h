#ifndef ROBOT_H
#define ROBOT_H

#include <SDL/SDL.h>
#include "utils.h"

// Forward declaration
struct GAME;

typedef enum {
    ROBOT_IDLE,
    ROBOT_WALKING,
    ROBOT_ATTACK1,
    ROBOT_ATTACK3,
    ROBOT_SPECIAL,
    ROBOT_HURT,
    ROBOT_DEAD
} RobotState;

typedef struct {
    SDL_Rect position;
    RobotState state;
    int frame;
    int totalFrames;
    int health;
    int maxHealth;
    int speed;
    int facingLeft;
    Uint32 lastMeleeTime;
    Uint32 lastRangedTime;
    Uint32 lastSpecialTime;
    int projectileActive;
    SDL_Rect projectilePosition;
    int projectileDirection;
    int invulnerabilityTimer;
    int active;
    float yVelocity;
    int onGround;
    int frameDelay;
    int world_x;
    int shieldActive;
    int shieldTimer;
    int bullets;
    int damageCooldown;
    int activationZone;
    int fireZone;
    int patrolDirection;
    int patrolTimer;
    int patrolLeft;    // Added for patrol boundaries
    int patrolRight;   // Added for patrol boundaries
    int specialCooldown; // Added for special attack cooldown
    int stateCooldown;
} Robot;

// Global sprite sheets
extern SDL_Surface* robot_idleSheet;
extern SDL_Surface* robot_idleSheetFlipped;
extern SDL_Surface* robot_walkSheet;
extern SDL_Surface* robot_walkSheetFlipped;
extern SDL_Surface* robot_attack1Sheet;
extern SDL_Surface* robot_attack1SheetFlipped;
extern SDL_Surface* robot_attack3Sheet;
extern SDL_Surface* robot_attack3SheetFlipped;
extern SDL_Surface* robot_specialSheet;
extern SDL_Surface* robot_specialSheetFlipped;
extern SDL_Surface* robot_hurtSheet;
extern SDL_Surface* robot_hurtSheetFlipped;
extern SDL_Surface* robot_deathSheet;
extern SDL_Surface* robot_deathSheetFlipped;
extern SDL_Surface* robot_projectileSheet;
extern SDL_Surface* robot_projectileSheetFlipped;

void initRobot(Robot* robot, int x, int y);
void updateRobot(Robot* robot, SDL_Rect playerPosition, int* playerHealth, int playerMaxHealth, struct GAME *game);
void renderRobot(SDL_Surface* screen, Robot* robot, int scroll_x);
void freeRobot(Robot* robot);
void freeRobotSprites(void);

#endif
