#ifndef GAME_H
#define GAME_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>

#include "enemy.h"      
#include "soldier.h"    
#include "soldier2.h"   
#include "robot.h"     
#include "boss.h"       
#include "portal.h"     
#include "enigme.h"     
#include "npc.h" 
#include "npc2.h"       
#include "enemylvl2.h"  
#include "player2.h"    

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define FPS 60
#define WASTED_FRAMES 25
#define GRAVITY 0.5
#define JUMP_POWER 8
#define WALK_SPEED 8
#define RUN_SPEED 8
#define MAX_ITEMS 4
#define MAX_SOLDIERS 10
#define MAX_SOLDIERS2 10
#define MAX_ROBOTS 10
#define MAX_ENEMIES 10
#define MAX_NPCS 10
#define MAX_NPC2S 10
#define ACTIVATION_ZONE 400
#define FIRE_ZONE 300
#define MAX_MUMMIES 5
#define MAX_DECEASEDS 5
#define MAX_GORGONS 5
#define MAX_SPEARMEN 5
#define MAX_WARRIORS 5
#define MAX_WIZARDS 5
#define MAX_NECROMANCERS 5
#define MAX_COINS 14    

typedef enum { IDLE, WALK, RUN, JUMP, ATTACK_1, SHOT, RECHARGE, HURT, DEAD } PlayerState;
typedef enum { LEFT, RIGHT } Facing;
typedef enum { RED = 0xFF0000, DOOR_RED = 0xFF002A, GREEN_ZONE = 0x00FF00 } Color;




typedef struct {
    SDL_Surface *icon;  // Changed from 'image' to 'icon'
    int have;
} InventoryItem;

typedef struct {
    SDL_Surface *uiImage;  // Added for inventory background
    SDL_Surface *frameImage; // Added for frame.png
    InventoryItem items[MAX_ITEMS];
    int count;
    int selectedSlot; // Added for frame system
} Inventory;

typedef struct {
    int left, right, up, jump, jumpHeld, enter, scroll, pickup, skip;
    int attack, shoot, recharge, damage, showInventory, run;
    int restart;
    int mouseLeft, mouseX, mouseY;
    int p2_left, p2_right, p2_jump, p2_jumpHeld, p2_attack, p2_damage, p2_run;
    int useItem[MAX_ITEMS];
    int closeInstructions; // Added for closing instructions pop-up
} Input;

typedef struct {
    SDL_Surface *image;
    SDL_Rect position;
    int active;
    Uint32 spawnTime;
} Item;

typedef struct {
    SDL_Surface *image;
    SDL_Surface *levelCollision;
    int scroll_x;
    int width;
    int height;
} Background;

typedef struct {
    SDL_Surface *idleSheet, *idleSheetFlipped;
    SDL_Surface *walkSheet, *walkSheetFlipped;
    SDL_Surface *runSheet, *runSheetFlipped;
    SDL_Surface *jumpSheet, *jumpSheetFlipped;
    SDL_Surface *attack1Sheet, *attack1SheetFlipped;
    SDL_Surface *shotSheet, *shotSheetFlipped;
    SDL_Surface *rechargeSheet, *rechargeSheetFlipped;
    SDL_Surface *hurtSheet, *hurtSheetFlipped;
    SDL_Surface *deadSheet, *deadSheetFlipped;
    SDL_Surface *healthBarBg, *healthBarGreen;
    SDL_Surface *killIcon1, *killIcon2, *killIcon3, *killIcon4;
    SDL_Surface *bulletSheet, *bulletSheetFlipped;
    Item healthItem;
    SDL_Rect position;
    SDL_Rect bullet;
    int bullet_w, bullet_h;
    PlayerState state;
    int frame, totalFrames;
    Facing facing;
    float yVelocity;
    int onGround;
    int freezeYMovement;
    int health, maxHealth;
    int lives, score, ammo;
    int isShooting, bulletActive, bulletFrame;
    int bulletDirection;
    int isAttacking;
    int kills, showIcon;
    Uint32 iconDisplayTime;
    int died, isJumping, nearDoor;
    int world_x;
    Mix_Chunk *jumpSound;
    Mix_Chunk *attackSound;
    Mix_Chunk *shootSound;
    Mix_Chunk *hurtSound;
    Mix_Chunk *deathSound;
    Mix_Chunk *itemPickupSound;
    Mix_Chunk *doorSound;
    int bulletStartX;
    int shootDebounce;
} Player;

typedef struct {
    SDL_Surface *avatar;
    TTF_Font *font;
    SDL_Color textColor;
    SDL_Surface *wastedImages[WASTED_FRAMES];
    int wastedFrame, wastedDelay;
    int showWasted;
    SDL_Rect pos_avatar, pos_lives;
    SDL_Rect pos_healthBar;
    SDL_Rect pos_score;
    SDL_Surface *cachedScoreSurface;
    SDL_Surface *ammoIcon;         // Added
    SDL_Surface *cachedAmmoSurface; // Added
} UI;

typedef enum {
    JET_FLYING,
    JET_OFFSCREEN
} JetState;

typedef struct {
    SDL_Rect position;
    int active;
    SDL_Surface* frames[27];
    int currentFrame;
} Fire;

typedef struct {
    SDL_Rect position;
    int active;
    SDL_Surface* spriteSheet;
    int frame;
    float yVelocity;
    Fire fire;
} Bomb;

typedef struct {
    SDL_Surface* spriteSheet;
    SDL_Rect position;
    JetState state;
    int speed;
    int frame;
    Bomb bomb;
    Uint32 lastSpawnTime;
    int active;
} Jet;

typedef struct {
    int showMessage, currentLevel;
    char message[50];
    SDL_Rect messagePosition;
    SDL_Surface *messageSurface;
    int gameOver;
    int currentGreenZone;
    int quizActive;
    int level;
    TTF_Font *font;
    Mix_Music *backgroundMusic;
    SDL_Surface *healthIcon28;
    int showHealthIcon;
    Uint32 healthIconTimer;
    int useDoubleBackground;
    int showInstructions; // Added to show instructions screen
    SDL_Surface *instructionsImage; // Added for instructions image
} Global;

typedef struct GAME{
    SDL_Surface* screen;
    int running, level;
    Background background;
    Player player;
    Player2 player2;
    UI ui;
    Inventory inventory;
    Input input;
    Jet jet;
    Global global;
    Soldier soldiers[MAX_SOLDIERS];
    int numSoldiers;
    Soldier2 soldiers2[MAX_SOLDIERS2];
    int numSoldiers2;
    Robot robots[MAX_ROBOTS];
    int numRobots;
    Enemy enemies[MAX_ENEMIES];
    int numEnemies;
    Boss boss;
    int bossActive;
    Portal portal;
    NPC npcs[MAX_NPCS];
    int numNPCs;
    NPC2 npc2s[MAX_NPC2S];
    int numNPC2s;
    Mummy mummies[MAX_MUMMIES];
    int numMummies;
    Deceased deceaseds[MAX_DECEASEDS];
    int numDeceaseds;
    Gorgon gorgons[MAX_GORGONS];
    int numGorgons;
    SkeletonSpearman spearmen[MAX_SPEARMEN];
    int numSpearmen;
    Coin coins[MAX_COINS];
    int numCoins;
    void *enigma;
    int inventoryVisible; // Added for inventory toggle
} GAME;

void load_level(struct GAME *game, int level);
void playLevel(struct GAME *game);
void freeResources(struct GAME *game);
void initGame(struct GAME *game);
void freeGame(struct GAME *game);

#endif
