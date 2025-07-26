#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include "game.h"
#include "collision.h"
#include "mouvement.h"
#include "player.h"
#include "inventory.h"
#include "jet.h"
#include "ui.h"
#include "soldier.h"
#include "soldier2.h"
#include "robot.h"
#include "enemy.h"
#include "boss.h"
#include "portal.h"
#include "enigme.h"
#include "events.h"
#include "events2.h"
#include "npc.h"
#include "npc2.h"
#include "enemylvl2.h"
#include "player2.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

static int level1_width = 6244;
static int level1_height = 720;
static int level2_width = 9651;
static int level2_height = 672;
static int level3_width = 5000;
static int level3_height = 720;

void freeResources(GAME *game) {
    if (!game) {
        fprintf(stderr, "freeResources: Game pointer is NULL\n");
        return;
    }
    if (game->background.image) {
        SDL_FreeSurface(game->background.image);
        game->background.image = NULL;
    }
    if (game->background.levelCollision) {
        SDL_FreeSurface(game->background.levelCollision);
        game->background.levelCollision = NULL;
    }
    for (int i = 0; i < MAX_SOLDIERS; i++) {
        if (game->soldiers[i].active) {
            freeSoldier(&game->soldiers[i]);
            game->soldiers[i].active = 0;
        }
    }
    for (int i = 0; i < MAX_SOLDIERS2; i++) {
        if (game->soldiers2[i].active) {
            freeSoldier2(&game->soldiers2[i]);
            game->soldiers2[i].active = 0;
        }
    }
    for (int i = 0; i < MAX_ROBOTS; i++) {
        if (game->robots[i].active) {
            freeRobot(&game->robots[i]);
            game->robots[i].active = 0;
        }
    }
    for (int i = 0; i < MAX_NPCS; i++) {
        if (game->npcs[i].active) {
            freeNPC(&game->npcs[i]);
            game->npcs[i].active = 0;
        }
    }
    for (int i = 0; i < MAX_NPC2S; i++) {
        if (game->npc2s[i].active) {
            freeNPC2(&game->npc2s[i]);
            game->npc2s[i].active = 0;
        }
    }
    if (game->bossActive) {
        freeBoss(&game->boss);
        game->bossActive = 0;
    }
    if (game->portal.active) {
        freePortal(&game->portal);
        game->portal.active = 0;
    }
    if (game->global.messageSurface) {
        SDL_FreeSurface(game->global.messageSurface);
        game->global.messageSurface = NULL;
    }
    for (int i = 0; i < game->numMummies; i++) {
        if (game->mummies[i].active) {
            freeMummy(&game->mummies[i]);
        }
    }
    for (int i = 0; i < game->numDeceaseds; i++) {
        if (game->deceaseds[i].active) {
            freeDeceased(&game->deceaseds[i]);
        }
    }
    for (int i = 0; i < game->numGorgons; i++) {
        if (game->gorgons[i].active) {
            freeGorgon(&game->gorgons[i]);
        }
    }
    for (int i = 0; i < game->numSpearmen; i++) {
        if (game->spearmen[i].active) {
            freeSkeletonSpearman(&game->spearmen[i]);
        }
    }
    if (game->global.backgroundMusic) {
        Mix_FreeMusic(game->global.backgroundMusic);
        game->global.backgroundMusic = NULL;
    }
    if (game->global.healthIcon28) {
        SDL_FreeSurface(game->global.healthIcon28);
        game->global.healthIcon28 = NULL;
    }
    if (game->global.instructionsImage) {
        SDL_FreeSurface(game->global.instructionsImage);
        game->global.instructionsImage = NULL;
    }
    freeEnemyLvl2Sprites();
    freeEnigme(game);
    printf("Resources freed\n");
}

void load_level(GAME *game, int level) {
    if (!game) {
        fprintf(stderr, "load_level: Game pointer is NULL\n");
        exit(1);
    }

    char bg_path[50], mask_path[50];
    sprintf(bg_path, "assets/levels/level%d.png", level);
    sprintf(mask_path, "assets/levels/level%d_mask.png", level);

    freeResources(game);

    game->background.image = IMG_Load(bg_path);
    if (!game->background.image) {
        fprintf(stderr, "Failed to load %s: %s\n", bg_path, IMG_GetError());
        exit(1);
    }
    SDL_Surface *optimized = SDL_DisplayFormat(game->background.image);
    if (!optimized) {
        fprintf(stderr, "SDL_DisplayFormat failed for %s: %s\n", bg_path, SDL_GetError());
        SDL_FreeSurface(game->background.image);
        game->background.image = NULL;
        exit(1);
    }
    SDL_FreeSurface(game->background.image);
    game->background.image = optimized;

    game->background.levelCollision = IMG_Load(mask_path);
    if (!game->background.levelCollision) {
        fprintf(stderr, "Failed to load %s: %s\n", mask_path, IMG_GetError());
        SDL_FreeSurface(game->background.image);
        game->background.image = NULL;
        exit(1);
    }
    optimized = SDL_DisplayFormat(game->background.levelCollision);
    if (!optimized) {
        fprintf(stderr, "SDL_DisplayFormat failed for %s: %s\n", mask_path, SDL_GetError());
        SDL_FreeSurface(game->background.image);
        SDL_FreeSurface(game->background.levelCollision);
        game->background.image = NULL;
        game->background.levelCollision = NULL;
        exit(1);
    }
    SDL_FreeSurface(game->background.levelCollision);
    game->background.levelCollision = optimized;

    // Verify collision map dimensions for level 2
    if (level == 2 && (game->background.levelCollision->w != level2_width || game->background.levelCollision->h != level2_height)) {
        fprintf(stderr, "Level 2 collision map size mismatch: expected %dx%d, got %dx%d\n",
                level2_width, level2_height, game->background.levelCollision->w, game->background.levelCollision->h);
        SDL_FreeSurface(game->background.image);
        SDL_FreeSurface(game->background.levelCollision);
        game->background.image = NULL;
        game->background.levelCollision = NULL;
        exit(1);
    }

    // Load background music
    game->global.backgroundMusic = Mix_LoadMUS("assets/sounds/background_music.mp3");
    if (!game->global.backgroundMusic) {
        fprintf(stderr, "Failed to load background music: %s\n", Mix_GetError());
    } else {
        printf("Background music loaded\n");
    }

    // Load health icon28
    game->global.healthIcon28 = IMG_Load("assets/ui/icon28.png");
    if (!game->global.healthIcon28) {
        fprintf(stderr, "Failed to load icon28: %s\n", IMG_GetError());
    } else {
        optimized = SDL_DisplayFormat(game->global.healthIcon28);
        if (!optimized) {
            fprintf(stderr, "SDL_DisplayFormat failed for icon28: %s\n", SDL_GetError());
            SDL_FreeSurface(game->global.healthIcon28);
            game->global.healthIcon28 = NULL;
        } else {
            SDL_FreeSurface(game->global.healthIcon28);
            game->global.healthIcon28 = optimized;
            printf("Health icon28 loaded\n");
        }
    }

    // Load instructions image
    game->global.instructionsImage = IMG_Load("assets/inventory/instructions.png");
    if (!game->global.instructionsImage) {
        fprintf(stderr, "Failed to load instructions.png: %s\n", IMG_GetError());
    } else {
        optimized = SDL_DisplayFormat(game->global.instructionsImage);
        if (!optimized) {
            fprintf(stderr, "SDL_DisplayFormat failed for instructions.png: %s\n", SDL_GetError());
            SDL_FreeSurface(game->global.instructionsImage);
            game->global.instructionsImage = NULL;
        } else {
            SDL_FreeSurface(game->global.instructionsImage);
            game->global.instructionsImage = optimized;
            printf("Instructions image loaded\n");
        }
    }

    for (int i = 0; i < MAX_SOLDIERS; i++) game->soldiers[i].active = 0;
    for (int i = 0; i < MAX_SOLDIERS2; i++) game->soldiers2[i].active = 0;
    for (int i = 0; i < MAX_ROBOTS; i++) game->robots[i].active = 0;
    for (int i = 0; i < MAX_NPCS; i++) game->npcs[i].active = 0;
    for (int i = 0; i < MAX_NPC2S; i++) game->npc2s[i].active = 0;
    game->numMummies = 0;
    game->numDeceaseds = 0;
    game->numGorgons = 0;
    game->numSpearmen = 0;

    if (level == 1) {
        game->background.width = level1_width;
        game->background.height = level1_height;
        initJet(&game->jet);

        game->numSoldiers = 3;
        int soldierPositions[3] = {500, 2000, 5000};
        printf("Initializing %d Soldiers for Level 1\n", game->numSoldiers);
        for (int i = 0; i < game->numSoldiers; i++) {
            int x = soldierPositions[i];
            int y = 0;
            if (x >= 0 && x <= game->background.width - 256) {
                printf("Attempting to initialize Soldier %d at x=%d, y=%d\n", i, x, y);
                initSoldier(&game->soldiers[i], x, y, game);
                placeSoldierOnGround(game, &game->soldiers[i]);
                game->soldiers[i].yVelocity = 0.0f;
                game->soldiers[i].onGround = onGroundSoldier(game, &game->soldiers[i]);
                game->soldiers[i].world_x = x;
                game->soldiers[i].position.x = x;
                game->soldiers[i].active = 1;
                printf("Soldier %d initialized at x=%d, y=%d\n", i, x, game->soldiers[i].position.y);
            } else {
                fprintf(stderr, "Invalid soldier position x=%d for index %d\n", x, i);
                game->soldiers[i].active = 0;
            }
        }

        game->numSoldiers2 = 3;
        int soldier2Positions[3] = {750, 3000, 6000};
        printf("Initializing %d Soldier2s for Level 1\n", game->numSoldiers2);
        for (int i = 0; i < game->numSoldiers2; i++) {
            int x = soldier2Positions[i];
            int y = 0;
            if (x >= 0 && x <= game->background.width - 256) {
                printf("Attempting to initialize Soldier2 %d at x=%d, y=%d\n", i, x, y);
                initSoldier2(&game->soldiers2[i], x, y, game);
                placeSoldier2OnGround(game, &game->soldiers2[i]);
                game->soldiers2[i].yVelocity = 0.0f;
                game->soldiers2[i].onGround = onGroundSoldier2(game, &game->soldiers2[i]);
                game->soldiers2[i].world_x = x;
                game->soldiers2[i].position.x = x;
                game->soldiers2[i].active = 1;
                printf("Soldier2 %d initialized at x=%d, y=%d\n", i, x, game->soldiers2[i].position.y);
            } else {
                fprintf(stderr, "Invalid soldier2 position x=%d for index %d\n", x, i);
                game->soldiers2[i].active = 0;
            }
        }

        game->numNPCs = 2;
        int npcPositions[2] = {1000, 4000};
        const char *npcDialogues[2] = {"I sell ammo!", "I restore health!"};
        printf("Initializing %d NPCs for Level 1\n", game->numNPCs);
        for (int i = 0; i < game->numNPCs; i++) {
            int x = npcPositions[i];
            int y = SCREEN_HEIGHT - 256 - 100;
            if (x >= 0 && x <= game->background.width - 128) {
                printf("Attempting to initialize NPC %d at x=%d, y=%d\n", i, x, y);
                initNPC(&game->npcs[i], x, y, npcDialogues[i], game);
                placeNPCOnGround(game, &game->npcs[i]);
                game->npcs[i].world_x = x;
                game->npcs[i].position.x = x;
                game->npcs[i].active = 1;
                printf("NPC %d initialized at x=%d, y=%d\n", i, x, game->npcs[i].position.y);
            } else {
                fprintf(stderr, "Invalid NPC position x=%d for index %d\n", x, i);
                game->npcs[i].active = 0;
            }
        }
        game->numNPC2s = 0;
    } else if (level == 2) {
        game->background.width = level2_width;
        game->background.height = level2_height;

        game->numSoldiers = 1;
        printf("Initializing %d Soldiers for Level 2\n", game->numSoldiers);
        for (int i = 0; i < game->numSoldiers; i++) {
            int x = 500 + (i * 200);
            int y = 0;
            if (x >= 0 && x <= game->background.width - 256) {
                printf("Attempting to initialize Soldier %d at x=%d, y=%d\n", i, x, y);
                initSoldier(&game->soldiers[i], x, y, game);
                placeSoldierOnGround(game, &game->soldiers[i]);
                game->soldiers[i].yVelocity = 0.0f;
                game->soldiers[i].onGround = onGroundSoldier(game, &game->soldiers[i]);
                game->soldiers[i].world_x = x;
                game->soldiers[i].position.x = x;
                game->soldiers[i].active = 1;
                printf("Soldier %d initialized at x=%d, y=%d\n", i, x, game->soldiers[i].position.y);
            } else {
                fprintf(stderr, "Invalid soldier position x=%d for index %d\n", x, i);
                game->soldiers[i].active = 0;
            }
        }

        game->numSoldiers2 = 1;
        printf("Initializing %d Soldier2s for Level 2\n", game->numSoldiers2);
        for (int i = 0; i < game->numSoldiers2; i++) {
            int x = 600 + (i * 200);
            int y = 0;
            if (x >= 0 && x <= game->background.width - 256) {
                printf("Attempting to initialize Soldier2 %d at x=%d, y=%d\n", i, x, y);
                initSoldier2(&game->soldiers2[i], x, y, game);
                placeSoldier2OnGround(game, &game->soldiers2[i]);
                game->soldiers2[i].yVelocity = 0.0f;
                game->soldiers2[i].onGround = onGroundSoldier2(game, &game->soldiers2[i]);
                game->soldiers2[i].world_x = x;
                game->soldiers2[i].position.x = x;
                game->soldiers2[i].active = 1;
                printf("Soldier2 %d initialized at x=%d, y=%d\n", i, x, game->soldiers2[i].position.y);
            } else {
                fprintf(stderr, "Invalid soldier2 position x=%d for index %d\n", x, i);
                game->soldiers2[i].active = 0;
            }
        }

        game->numRobots = 1;
        int robotPositions[1][2] = {{2500, 300}};
        printf("Initializing %d Robots for Level 2\n", game->numRobots);
        for (int i = 0; i < game->numRobots; i++) {
            int x = robotPositions[i][0];
            if (x >= 0 && x <= game->background.width - 256) {
                printf("Attempting to initialize Robot %d at x=%d, y=%d\n", i, x, robotPositions[i][1]);
                initRobot(&game->robots[i], x, robotPositions[i][1]);
                placeRobotOnGround(game, &game->robots[i]);
                game->robots[i].yVelocity = 0.0f;
                game->robots[i].onGround = onGroundEntity(game, &game->robots[i].position, x);
                game->robots[i].world_x = x;
                game->robots[i].position.x = x;
                game->robots[i].active = 1;
                printf("Robot %d initialized at x=%d, y=%d\n", i, x, game->robots[i].position.y);
            } else {
                fprintf(stderr, "Invalid robot position x=%d for index %d\n", x, i);
                game->robots[i].active = 0;
            }
        }

        game->numNPC2s = 1;
        int npc2Positions[1] = {4000};
        printf("Initializing %d NPC2s for Level 2\n", game->numNPC2s);
        for (int i = 0; i < game->numNPC2s; i++) {
            int x = npc2Positions[i];
            int y = SCREEN_HEIGHT - 256 - 100;
            if (x >= 0 && x <= game->background.width - 48) {
                printf("Attempting to initialize NPC2 %d at x=%d, y=%d\n", i, x, y);
                initNPC2(&game->npc2s[i], x, y, game);
                placeNPCOnGround(game, (NPC*)&game->npc2s[i]);
                game->npc2s[i].world_x = x;
                game->npc2s[i].position.x = x;
                game->npc2s[i].active = 1;
                printf("NPC2 %d initialized at x=%d, y=%d\n", i, x, game->npc2s[i].position.y);
            } else {
                fprintf(stderr, "Invalid NPC2 position x=%d for index %d\n", x, i);
                game->npc2s[i].active = 0;
            }
        }

        printf("Attempting to initialize Boss at x=9000, y=300\n");
        initBoss(&game->boss, 9000, 300);
        placeBossOnGround(game, &game->boss);
        game->boss.world_x = 9000;
        game->boss.position.x = 9000;
        game->bossActive = 1;
        printf("Boss initialized at x=9000, y=%d\n", game->boss.position.y);
    } else if (level == 3) {
        game->background.width = level3_width;
        game->background.height = level3_height;
        game->numSoldiers = 0;
        game->numSoldiers2 = 0;
        game->numRobots = 0;
        game->numNPCs = 0;
        game->numNPC2s = 0;
        game->bossActive = 0;
        game->portal.active = 0;

        game->numMummies = 2;
        int mummyPositions[2] = {1000, 1500};
        for (int i = 0; i < game->numMummies; i++) {
            int x = mummyPositions[i];
            int y = 0;
            if (x < 0 || x > game->background.width - 256) {
                fprintf(stderr, "Invalid mummy position x=%d for index %d\n", x, i);
                game->mummies[i].active = 0;
                continue;
            }
            printf("Attempting to initialize Mummy %d at x=%d, y=%d\n", i, x, y);
            initMummy(&game->mummies[i], x, y, game);
            if (!game->mummies[i].active) {
                fprintf(stderr, "Failed to initialize Mummy %d at x=%d\n", i, x);
                game->mummies[i].active = 0;
                continue;
            }
            placeEnemyLvl2OnGround(game, &game->mummies[i].position, x);
            game->mummies[i].world_x = x;
            game->mummies[i].position.x = x;
            game->mummies[i].active = 1;
            printf("Mummy %d initialized at x=%d, y=%d\n", i, x, game->mummies[i].position.y);
        }

        game->numDeceaseds = 1;
        int deceasedPositions[1] = {1200};
        for (int i = 0; i < game->numDeceaseds; i++) {
            int x = deceasedPositions[i];
            int y = 0;
            if (x < 0 || x > game->background.width - 256) {
                fprintf(stderr, "Invalid deceased position x=%d for index %d\n", x, i);
                game->deceaseds[i].active = 0;
                continue;
            }
            printf("Attempting to initialize Deceased %d at x=%d, y=%d\n", i, x, y);
            initDeceased(&game->deceaseds[i], x, y, game);
            if (!game->deceaseds[i].active) {
                fprintf(stderr, "Failed to initialize Deceased %d at x=%d\n", i, x);
                game->deceaseds[i].active = 0;
                continue;
            }
            placeEnemyLvl2OnGround(game, &game->deceaseds[i].position, x);
            game->deceaseds[i].world_x = x;
            game->deceaseds[i].position.x = x;
            game->deceaseds[i].active = 1;
            printf("Deceased %d initialized at x=%d, y=%d\n", i, x, game->deceaseds[i].position.y);
        }

        game->numGorgons = 1;
        int gorgonPositions[1] = {1800};
        for (int i = 0; i < game->numGorgons; i++) {
            int x = gorgonPositions[i];
            int y = 0;
            if (x < 0 || x > game->background.width - 256) {
                fprintf(stderr, "Invalid gorgon position x=%d for index %d\n", x, i);
                game->gorgons[i].active = 0;
                continue;
            }
            printf("Attempting to initialize Gorgon %d at x=%d, y=%d\n", i, x, y);
            initGorgon(&game->gorgons[i], x, y, game);
            if (!game->gorgons[i].active) {
                fprintf(stderr, "Failed to initialize Gorgon %d at x=%d\n", i, x);
                game->gorgons[i].active = 0;
                continue;
            }
            placeEnemyLvl2OnGround(game, &game->gorgons[i].position, x);
            game->gorgons[i].world_x = x;
            game->gorgons[i].position.x = x;
            game->gorgons[i].active = 1;
            printf("Gorgon %d initialized at x=%d, y=%d\n", i, x, game->gorgons[i].position.y);
        }

        game->numSpearmen = 1;
        int spearmanPositions[1] = {2000};
        for (int i = 0; i < game->numSpearmen; i++) {
            int x = spearmanPositions[i];
            int y = 0;
            if (x < 0 || x > game->background.width - 256) {
                fprintf(stderr, "Invalid spearman position x=%d for index %d\n", x, i);
                game->spearmen[i].active = 0;
                continue;
            }
            printf("Attempting to initialize SkeletonSpearman %d at x=%d, y=%d\n", i, x, y);
            initSkeletonSpearman(&game->spearmen[i], x, y, game);
            if (!game->spearmen[i].active) {
                fprintf(stderr, "Failed to initialize SkeletonSpearman %d at x=%d\n", i, x);
                game->spearmen[i].active = 0;
                continue;
            }
            placeEnemyLvl2OnGround(game, &game->spearmen[i].position, x);
            game->spearmen[i].world_x = x;
            game->spearmen[i].position.x = x;
            game->spearmen[i].active = 1;
            printf("SkeletonSpearman %d initialized at x=%d, y=%d\n", i, x, game->spearmen[i].position.y);
        }
    }

    game->background.scroll_x = 0;
    game->level = level;
    game->global.currentLevel = level;
    game->global.quizActive = 0;
    game->player.position.x = 200;
    game->player.world_x = 200;
    game->player.position.y = SCREEN_HEIGHT - 256 - 100;
    game->player.yVelocity = 0;
    game->player.onGround = 0;
    game->player.nearDoor = -1;
    game->player.health = game->player.maxHealth;
    game->player.lives = 3;
    game->player.freezeYMovement = 0;
    game->player.isJumping = 0;
    game->player.kills = 0;
    game->player.score = 0;
    game->player.ammo = 10;
    game->player.state = IDLE;
    game->player.died = 0;
    game->global.showMessage = 0;
    game->global.currentGreenZone = 0;
    game->portal.active = 0;
    game->input.jump = 0;
    game->input.jumpHeld = 0;
    game->input.up = 0;
    game->input.scroll = 0;
    game->input.enter = 0;
    game->input.restart = 0;
    game->input.skip = 0;
    game->global.showHealthIcon = 0;
    game->global.healthIconTimer = 0;
    placePlayerOnGround(game);
    printf("Level %d loaded, player at x=%d, y=%d\n", level, game->player.world_x, game->player.position.y);
}

void playLevel(GAME *game) {
    if (!game || !game->screen || !game->background.image || !game->background.levelCollision) {
        fprintf(stderr, "playLevel: Game, screen, or background is NULL\n");
        exit(1);
    }

    // Start background music if loaded and not already playing
    if (game->global.backgroundMusic && !Mix_PlayingMusic()) {
        if (Mix_PlayMusic(game->global.backgroundMusic, -1) == -1) {
            fprintf(stderr, "Failed to play background music: %s\n", Mix_GetError());
        } else {
            printf("Background music started\n");
        }
    }

    Uint32 lastTime = SDL_GetTicks();
    while (game->running) {
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        // Handle events for Player2 in level 3, otherwise for Player
        if (game->level == 3) {
            handleEventsPlayer2(game);
        } else {
            handleEvents(game);
        }

        // Check if instructions pop-up is active
        if (game->global.showInstructions) {
            // Skip game updates to freeze the game
            if (game->input.closeInstructions) {
                game->global.showInstructions = 0;
                game->input.closeInstructions = 0;
                printf("Instructions pop-up closed, resuming game\n");
            }

            // Clear screen
            SDL_FillRect(game->screen, NULL, SDL_MapRGB(game->screen->format, 0, 0, 0));

            // Render instructions image centered
            if (game->global.instructionsImage) {
                SDL_Rect dest = {
                    (SCREEN_WIDTH - game->global.instructionsImage->w) / 2,
                    (SCREEN_HEIGHT - game->global.instructionsImage->h) / 2,
                    game->global.instructionsImage->w,
                    game->global.instructionsImage->h
                };
                SDL_BlitSurface(game->global.instructionsImage, NULL, game->screen, &dest);
                printf("Rendering instructions pop-up at x=%d, y=%d\n", dest.x, dest.y);
            } else {
                fprintf(stderr, "Warning: instructionsImage is NULL\n");
            }

            SDL_Flip(game->screen);
            Uint32 frameTime = SDL_GetTicks() - currentTime;
            const Uint32 targetFrameTime = 16;
            if (frameTime < targetFrameTime) {
                SDL_Delay(targetFrameTime - frameTime);
            }
            continue; // Skip normal game loop
        }

        int updateDistance = 1500;
        int renderDistance = updateDistance + 256;
        int playerWorldX = (game->level == 3) ? game->player2.world_x : game->player.world_x;
        float target_scroll_x = playerWorldX - SCREEN_WIDTH / 2;
        if (target_scroll_x < 0) target_scroll_x = 0;
        if (target_scroll_x > game->background.width - SCREEN_WIDTH) target_scroll_x = game->background.width - SCREEN_WIDTH;

        game->background.scroll_x += (target_scroll_x - game->background.scroll_x) * 0.1f;
        int scroll_x = (int)game->background.scroll_x;

        if (!game->global.quizActive) {
            if (game->level == 2 && game->input.skip) {
                game->global.quizActive = 1;
                initEnigme(game);
                game->input.skip = 0;
                if (game->portal.active) {
                    game->portal.active = 0;
                    freePortal(&game->portal);
                }
                game->global.showMessage = 0;
                printf("Skip button pressed, enigma activated\n");
            }

            if (!game->boss.frostExplosionVFX->active && game->player.freezeYMovement) {
                game->player.freezeYMovement = 0;
            }

            // Update Player2 for level 3, Player otherwise
            if (game->level == 3) {
                game->player2.onGround = onGroundPlayer2(game, &game->player2);
                movementPlayer2(game);
                updatePlayer2(&game->player2, game);
            } else {
                game->player.onGround = onGround(game, &game->player);
                movement(game);
                updatePlayer(&game->player, game);
            }

            updateInventory(game);
            updateUI(&game->ui, game->player.lives);

            if (game->level == 1) {
                updateJet(&game->jet, &game->player, scroll_x);
            }

            for (int i = 0; i < game->numSoldiers; i++) {
                if (game->soldiers[i].active) {
                    int soldierWorldX = game->soldiers[i].world_x;
                    int dx = playerWorldX - soldierWorldX;
                    int distance = abs(dx);
                    if (distance <= updateDistance) {
                        updateSoldier(&game->soldiers[i], game->player.position, &game->player.health, game->player.maxHealth, game);
                        if (game->soldiers[i].health <= 0 && game->soldiers[i].state == SOLDIER_DEAD && game->soldiers[i].frame >= game->soldiers[i].totalFrames - 1) {
                            game->player.kills++;
                            game->soldiers[i].active = 0;
                            freeSoldier(&game->soldiers[i]);
                            printf("Soldier %d killed, player kills=%d\n", i, game->player.kills);
                        }
                    }
                }
            }

            for (int i = 0; i < game->numSoldiers2; i++) {
                if (game->soldiers2[i].active) {
                    int soldierWorldX = game->soldiers2[i].world_x;
                    int dx = playerWorldX - soldierWorldX;
                    int distance = abs(dx);
                    if (distance <= updateDistance) {
                        updateSoldier2(&game->soldiers2[i], game->player.position, &game->player.health, game->player.maxHealth, game);
                        if (game->soldiers2[i].health <= 0 && game->soldiers2[i].state == SOLDIER2_DEAD && game->soldiers2[i].frame >= game->soldiers2[i].totalFrames - 1) {
                            game->player.kills++;
                            game->soldiers2[i].active = 0;
                            freeSoldier2(&game->soldiers2[i]);
                            printf("Soldier2 %d killed, player kills=%d\n", i, game->player.kills);
                        }
                    }
                }
            }

            for (int i = 0; i < game->numRobots; i++) {
                if (game->robots[i].active) {
                    int robotWorldX = game->robots[i].world_x;
                    int dx = playerWorldX - robotWorldX;
                    int distance = abs(dx);
                    if (distance <= updateDistance) {
                        updateRobot(&game->robots[i], game->player.position, &game->player.health, game->player.maxHealth, game);
                        if (game->robots[i].health <= 0 && game->robots[i].state == ROBOT_DEAD && game->robots[i].frame >= game->robots[i].totalFrames - 1) {
                            game->player.kills++;
                            game->robots[i].active = 0;
                            freeRobot(&game->robots[i]);
                            printf("Robot %d killed, player kills=%d\n", i, game->player.kills);
                        }
                    }
                }
            }

            for (int i = 0; i < game->numNPCs; i++) {
                if (game->npcs[i].active) {
                    int npcWorldX = game->npcs[i].world_x;
                    int dx = playerWorldX - npcWorldX;
                    int distance = abs(dx);
                    if (distance <= updateDistance) {
                        updateNPC(&game->npcs[i], deltaTime);
                        SDL_Rect playerRect = {game->player.world_x, game->player.position.y, game->player.position.w, game->player.position.h};
                        SDL_Rect npcRect = {game->npcs[i].world_x, game->npcs[i].position.y, game->npcs[i].position.w, game->npcs[i].position.h};
                        if (rectIntersect(&playerRect, &npcRect)) {
                            game->global.showMessage = 1;
                            if (i == 0) {
                                strcpy(game->global.message, "Press E to buy ammo");
                            } else {
                                strcpy(game->global.message, "Press E to restore health");
                            }
                            if (game->global.font) {
                                SDL_Surface *tempSurface = TTF_RenderText_Solid(game->global.font, game->global.message, (SDL_Color){255, 255, 255});
                                if (tempSurface) {
                                    game->global.messagePosition.x = (npcWorldX - scroll_x) + (npcRect.w / 2) - (tempSurface->w / 2);
                                    game->global.messagePosition.y = game->npcs[i].position.y - 40;
                                    SDL_FreeSurface(tempSurface);
                                } else {
                                    game->global.messagePosition.x = (npcWorldX - scroll_x) + (npcRect.w / 2) - 50;
                                    game->global.messagePosition.y = game->npcs[i].position.y - 40;
                                }
                            } else {
                                game->global.messagePosition.x = (npcWorldX - scroll_x) + (npcRect.w / 2) - 50;
                                game->global.messagePosition.y = game->npcs[i].position.y - 40;
                            }
                            game->global.messagePosition.w = npcWorldX;
                            game->global.messagePosition.h = i;
                            if (game->input.enter && !game->npcs[i].dealing) {
                                game->npcs[i].state = NPC_DIALOGUE;
                                game->npcs[i].frame = 0;
                                game->npcs[i].dealing = 1;
                                game->input.enter = 0;
                                if (i == 0) {
                                    game->player.ammo += 5;
                                    printf("NPC %d: Traded 5 ammo, player ammo=%d\n", i, game->player.ammo);
                                } else {
                                    game->player.health = game->player.maxHealth;
                                    game->global.showHealthIcon = 1;
                                    game->global.healthIconTimer = currentTime;
                                    printf("NPC %d: Restored health, player health=%d, showing icon28\n", i, game->player.health);
                                }
                                game->npcs[i].state = NPC_APPROVAL;
                                game->npcs[i].frame = 0;
                            }
                        } else {
                            if (game->global.showMessage && 
                                game->global.messagePosition.w == game->npcs[i].world_x &&
                                game->global.messagePosition.h == i) {
                                game->global.showMessage = 0;
                                game->global.messagePosition.w = -1;
                                game->global.messagePosition.h = -1;
                            }
                        }
                    }
                }
            }

            SDL_Rect playerRect = {game->player.world_x, game->player.position.y, game->player.position.w, game->player.position.h};
            for (int i = 0; i < game->numNPC2s; i++) {
                if (game->npc2s[i].active) {
                    int npc2WorldX = game->npc2s[i].world_x;
                    int dx = playerWorldX - npc2WorldX;
                    int distance = abs(dx);
                    if (distance <= updateDistance) {
                        updateNPC2(&game->npc2s[i], deltaTime, &playerRect);
                        SDL_Rect npc2Rect = {game->npc2s[i].world_x, game->npc2s[i].position.y, game->npc2s[i].position.w, game->npc2s[i].position.h};
                        if (rectIntersect(&playerRect, &npc2Rect) && !game->npc2s[i].dialogueActive) {
                            game->global.showMessage = 1;
                            strcpy(game->global.message, "Press E to talk");
                            if (game->global.font) {
                                SDL_Surface *tempSurface = TTF_RenderText_Solid(game->global.font, game->global.message, (SDL_Color){255, 255, 255});
                                if (tempSurface) {
                                    game->global.messagePosition.x = (npc2WorldX - scroll_x) + (npc2Rect.w / 2) - (tempSurface->w / 2);
                                    game->global.messagePosition.y = game->npc2s[i].position.y - 40;
                                    SDL_FreeSurface(tempSurface);
                                } else {
                                    game->global.messagePosition.x = (npc2WorldX - scroll_x) + (npc2Rect.w / 2) - 50;
                                    game->global.messagePosition.y = game->npc2s[i].position.y - 40;
                                }
                            } else {
                                game->global.messagePosition.x = (npc2WorldX - scroll_x) + (npc2Rect.w / 2) - 50;
                                game->global.messagePosition.y = game->npc2s[i].position.y - 40;
                            }
                            game->global.messagePosition.w = npc2WorldX;
                            game->global.messagePosition.h = i + MAX_NPCS;
                        } else if (!rectIntersect(&playerRect, &npc2Rect)) {
                            if (game->global.showMessage && 
                                game->global.messagePosition.w == game->npc2s[i].world_x &&
                                game->global.messagePosition.h == i + MAX_NPCS) {
                                game->global.showMessage = 0;
                                game->global.messagePosition.w = -1;
                                game->global.messagePosition.h = -1;
                            }
                        }
                    }
                }
            }

            if (game->bossActive) {
                int bossWorldX = game->boss.world_x;
                int dx = playerWorldX - bossWorldX;
                int distance = abs(dx);
                if (distance <= updateDistance) {
                    updateBoss(&game->boss, game);
                    if (game->boss.health <= 0 && game->boss.active && game->boss.state == BOSS_DEATH && game->boss.frame >= game->boss.deathFrameCount - 1) {
                        game->boss.active = 0; // Deactivate boss but don't free yet
                        printf("Boss deactivated, waiting for level-up icon\n");
                    }
                }
            }
            // Free boss resources only when level-up icon is no longer active
            if (!game->boss.active && game->bossActive && !game->boss.levelUpIconActive) {
                initPortal(&game->portal, 9000, 400);
                game->portal.active = 1;
                freeBoss(&game->boss);
                game->bossActive = 0;
                game->player.score += 1000;
                game->player.kills++;
                printf("Boss killed, portal activated at x=%d, y=%d\n", game->portal.position.x, game->portal.position.y);
            }

            if (game->level == 3) {
                for (int i = 0; i < game->numMummies; i++) {
                    if (game->mummies[i].active) {
                        int mummyWorldX = game->mummies[i].world_x;
                        int dx = playerWorldX - mummyWorldX;
                        int distance = abs(dx);
                        if (distance <= updateDistance && game->mummies[i].state != ENEMY2_DEAD) {
                            updateMummy(&game->mummies[i], &game->player2.health, game->player2.maxHealth, game, currentTime);
                        }
                        if (game->mummies[i].health <= 0 && game->mummies[i].state == ENEMY2_DEAD && game->mummies[i].frame >= game->mummies[i].totalFrames - 1) {
                            game->player2.kills++;
                            game->mummies[i].active = 0;
                            freeMummy(&game->mummies[i]);
                            printf("Mummy %d killed, player2 kills=%d\n", i, game->player2.kills);
                        }
                    }
                }
                for (int i = 0; i < game->numDeceaseds; i++) {
                    if (game->deceaseds[i].active) {
                        int deceasedWorldX = game->deceaseds[i].world_x;
                        int dx = playerWorldX - deceasedWorldX;
                        int distance = abs(dx);
                        if (distance <= updateDistance && game->deceaseds[i].state != ENEMY2_DEAD) {
                            updateDeceased(&game->deceaseds[i], &game->player2.health, game->player2.maxHealth, game, currentTime);
                        }
                        if (game->deceaseds[i].health <= 0 && game->deceaseds[i].state == ENEMY2_DEAD && game->deceaseds[i].frame >= game->deceaseds[i].totalFrames - 1) {
                            game->player2.kills++;
                            game->deceaseds[i].active = 0;
                            freeDeceased(&game->deceaseds[i]);
                            printf("Deceased %d killed, player2 kills=%d\n", i, game->player2.kills);
                        }
                    }
                }
                for (int i = 0; i < game->numGorgons; i++) {
                    if (game->gorgons[i].active) {
                        int gorgonWorldX = game->gorgons[i].world_x;
                        int dx = playerWorldX - gorgonWorldX;
                        int distance = abs(dx);
                        if (distance <= updateDistance && game->gorgons[i].state != ENEMY2_DEAD) {
                            updateGorgon(&game->gorgons[i], &game->player2.health, game->player2.maxHealth, game, currentTime);
                        }
                        if (game->gorgons[i].health <= 0 && game->gorgons[i].state == ENEMY2_DEAD && game->gorgons[i].frame >= game->gorgons[i].totalFrames - 1) {
                            game->player2.kills++;
                            game->gorgons[i].active = 0;
                            freeGorgon(&game->gorgons[i]);
                            printf("Gorgon %d killed, player2 kills=%d\n", i, game->player2.kills);
                        }
                    }
                }
                for (int i = 0; i < game->numSpearmen; i++) {
                    if (game->spearmen[i].active) {
                        int spearmanWorldX = game->spearmen[i].world_x;
                        int dx = playerWorldX - spearmanWorldX;
                        int distance = abs(dx);
                        if (distance <= updateDistance && game->spearmen[i].state != ENEMY2_DEAD) {
                            updateSkeletonSpearman(&game->spearmen[i], &game->player2.health, game->player2.maxHealth, game, currentTime);
                        }
                        if (game->spearmen[i].health <= 0 && game->spearmen[i].state == ENEMY2_DEAD && game->spearmen[i].frame >= game->spearmen[i].totalFrames - 1) {
                            game->player2.kills++;
                            game->spearmen[i].active = 0;
                            freeSkeletonSpearman(&game->spearmen[i]);
                            printf("SkeletonSpearman %d killed, player2 kills=%d\n", i, game->player2.kills);
                        }
                    }
                }

                int allEnemiesDead = 1;
                for (int i = 0; i < game->numMummies; i++) {
                    if (game->mummies[i].active) allEnemiesDead = 0;
                }
                for (int i = 0; i < game->numDeceaseds; i++) {
                    if (game->deceaseds[i].active) allEnemiesDead = 0;
                }
                for (int i = 0; i < game->numGorgons; i++) {
                    if (game->gorgons[i].active) allEnemiesDead = 0;
                }
                for (int i = 0; i < game->numSpearmen; i++) {
                    if (game->spearmen[i].active) allEnemiesDead = 0;
                }
                if (allEnemiesDead && !game->global.quizActive) {
                    game->global.showMessage = 1;
                    strcpy(game->global.message, "Congratulations! You cleared Level 3!");
                    game->global.messagePosition.x = (game->player2.world_x - scroll_x) - 100;
                    game->global.messagePosition.y = game->player2.position.y - 50;
                    game->global.messagePosition.w = game->player2.world_x;
                    game->global.messagePosition.h = -1;
                    if (game->input.enter) {
                        game->running = 0;
                        printf("Level 3 cleared, game ended\n");
                    }
                }
            }

            if (game->portal.active) {
                updatePortal(&game->portal);
                SDL_Rect playerRect = {game->player.world_x, game->player.position.y, game->player.position.w, game->player.position.h};
                SDL_Rect portalRect = {game->portal.position.x, game->portal.position.y, game->portal.position.w, game->portal.position.h};
                if (rectIntersect(&playerRect, &portalRect)) {
                    game->global.showMessage = 1;
                    strcpy(game->global.message, "Press E to solve the enigma");
                    game->global.messagePosition.x = game->portal.position.x - scroll_x - 50;
                    game->global.messagePosition.y = game->portal.position.y - 50;
                    game->global.messagePosition.w = game->portal.position.x;
                    game->global.messagePosition.h = -1;
                    if (game->input.enter) {
                        game->global.quizActive = 1;
                        initEnigme(game);
                        game->input.enter = 0;
                        game->portal.active = 0;
                        freePortal(&game->portal);
                        printf("Enigma activated\n");
                    }
                } else {
                    if (game->global.showMessage && 
                        game->global.messagePosition.w == game->portal.position.x &&
                        game->global.messagePosition.h == -1) {
                        game->global.showMessage = 0;
                        game->global.messagePosition.w = -1;
                        game->global.messagePosition.h = -1;
                    }
                }
            }

            int x = game->player.world_x + game->player.position.w / 2;
            int y = game->player.position.y + game->player.position.h;
            int y_offset = (SCREEN_HEIGHT - game->background.height) / 2;
            y -= y_offset;

            if (x >= 0 && x < game->background.levelCollision->w && y >= 0 && y < game->background.levelCollision->h) {
                if (game->level == 1) {
                    Color col = collision_color(game, x, y);
                    if (col == DOOR_RED) {
                        game->player.nearDoor = 1;
                        game->global.showMessage = 1;
                        strcpy(game->global.message, "Press E to enter Level 2");
                        game->global.messagePosition.x = x - scroll_x - 50;
                        game->global.messagePosition.y = game->player.position.y - 50;
                        game->global.messagePosition.w = x;
                        game->global.messagePosition.h = -1;
                    } else if (!game->portal.active) {
                        if (game->global.showMessage && 
                            game->global.messagePosition.w == x &&
                            game->global.messagePosition.h == -1) {
                            game->global.showMessage = 0;
                            game->global.messagePosition.w = -1;
                            game->global.messagePosition.h = -1;
                        }
                    }
                } else if (game->level == 2) {
                    int radius = 30;
                    int foundDoor = 0;
                    int doorWorldX = 0;
                    for (int dx = -radius; dx <= radius && !foundDoor; dx += 2) {
                        for (int dy = -radius; dy <= radius; dy += 2) {
                            if (dx * dx + dy * dy <= radius * radius) {
                                int check_x = x + dx;
                                int check_y = y + dy;
                                if (check_x >= 0 && check_x < game->background.levelCollision->w &&
                                    check_y >= 0 && check_y < game->background.levelCollision->h) {
                                    Color col = collision_color(game, check_x, check_y);
                                    if (col == DOOR_RED) {
                                        foundDoor = 1;
                                        doorWorldX = check_x;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    if (foundDoor) {
                        game->player.nearDoor = 1;
                        game->global.showMessage = 1;
                        sprintf(game->global.message, "Press E to teleport to Zone %d", game->global.currentGreenZone + 1);
                        game->global.messagePosition.x = doorWorldX - scroll_x - 50;
                        game->global.messagePosition.y = game->player.position.y - 50;
                        game->global.messagePosition.w = doorWorldX;
                        game->global.messagePosition.h = -1;
                    } else if (!game->portal.active) {
                        if (game->global.showMessage && 
                            game->global.messagePosition.w == doorWorldX &&
                            game->global.messagePosition.h == -1) {
                            game->player.nearDoor = -1;
                            game->global.showMessage = 0;
                            game->global.messagePosition.w = -1;
                            game->global.messagePosition.h = -1;
                        }
                    }
                }
            } else if (!game->portal.active) {
                game->player.nearDoor = -1;
                if (game->global.showMessage && 
                    game->global.messagePosition.w == game->player.world_x &&
                    game->global.messagePosition.h == -1) {
                    game->global.showMessage = 0;
                    game->global.messagePosition.w = -1;
                    game->global.messagePosition.h = -1;
                }
            }

            if (game->player.nearDoor >= 0 && game->input.enter) {
                game->global.showMessage = 0;
                game->input.enter = 0;
                if (game->level == 1 && game->player.nearDoor == 1) {
                    load_level(game, 2);
                    game->player.position.x = 200;
                    game->player.world_x = 200;
                    game->player.position.y = SCREEN_HEIGHT - 256 - 100;
                    game->player.yVelocity = 0;
                    game->player.onGround = 0;
                    game->global.currentGreenZone = 0;
                    placePlayerOnGround(game);
                    printf("Teleported to Level 2, Zone 1, player at screen_x=%d, world_x=%d\n",
                           game->player.position.x, game->player.world_x);
                } else if (game->level == 2 && game->player.nearDoor == 1 && game->global.currentGreenZone < 3) {
                    int greenZones[3][2] = {{1900, 400}, {4080, 400}, {7800, 400}};
                    int zoneIndex = game->global.currentGreenZone;
                    int greenZoneX = greenZones[zoneIndex][0];
                    int greenZoneY = greenZones[zoneIndex][1];
                    int true_x = greenZoneX;
                    game->player.position.y = greenZoneY - game->player.position.h - 10;
                    game->player.world_x = true_x;
                    game->background.scroll_x = true_x - SCREEN_WIDTH / 2;
                    if (game->background.scroll_x < 0) game->background.scroll_x = 0;
                    if (game->background.scroll_x > game->background.width - SCREEN_WIDTH) {
                        game->background.scroll_x = game->background.width - SCREEN_WIDTH;
                    }
                    game->player.position.x = true_x - game->background.scroll_x;
                    game->player.yVelocity = 0;
                    game->player.onGround = 0;
                    placePlayerOnGround(game);
                    game->global.currentGreenZone++;
                    printf("Teleported to Level 2, Zone %d, player at screen_x=%d, world_x=%d, y=%d\n",
                           zoneIndex + 1, game->player.position.x, game->player.world_x, game->player.position.y);
                }
                game->player.nearDoor = -1;
                if (game->global.messageSurface) {
                    SDL_FreeSurface(game->global.messageSurface);
                    game->global.messageSurface = NULL;
                }
            }

            // Respawn logic for Player2 in level 3
            if (game->level == 3 && game->player2.died) {
                game->player2.died = 0;
                game->player2.position.x = 200;
                game->player2.world_x = 200;
                game->player2.position.y = SCREEN_HEIGHT - 256 - 100;
                placePlayer2OnGround(game);
                game->background.scroll_x = 0;
                game->global.showHealthIcon = 0;
                game->global.healthIconTimer = 0;
                printf("Player2 respawned at screen_x=%d, world_x=%d, y=%d\n",
                       game->player2.position.x, game->player2.world_x, game->player2.position.y);
            } else if (game->player.died) {
                game->player.died = 0;
                game->player.position.x = 200;
                game->player.world_x = 200;
                game->player.position.y = SCREEN_HEIGHT - 256 - 100;
                placePlayerOnGround(game);
                game->background.scroll_x = 0;
                game->global.showHealthIcon = 0;
                game->global.healthIconTimer = 0;
                printf("Player respawned at screen_x=%d, world_x=%d, y=%d\n",
                       game->player.position.x, game->player.world_x, game->player.position.y);
            }
        }

        if (game->global.quizActive) {
            renderEnigme(game);
        } else {
            if (game->global.gameOver && game->input.restart) {
                game->global.gameOver = 0;
                game->ui.showWasted = 0;
                game->player.lives = 3;
                game->player.health = game->player.maxHealth;
                game->player.score = 0;
                game->player.ammo = 10;
                game->player.state = IDLE;
                game->player.frame = 0;
                game->player.position.x = 200;
                game->player.world_x = 200;
                game->player.position.y = SCREEN_HEIGHT - 256 - 100;
                game->player.yVelocity = 0.0f;
                game->player.onGround = 0;
                game->player.died = 0;
                game->player.isJumping = 0;
                game->player.nearDoor = -1;
                game->player.freezeYMovement = 0;
                game->player.kills = 0;
                game->input.jump = 0;
                game->input.jumpHeld = 0;
                game->input.up = 0;
                game->input.scroll = 0;
                game->input.enter = 0;
                game->input.restart = 0;
                game->input.skip = 0;
                game->background.scroll_x = 0;
                game->global.currentGreenZone = 0;
                game->global.showHealthIcon = 0;
                game->global.healthIconTimer = 0;
                if (game->global.messageSurface) {
                    SDL_FreeSurface(game->global.messageSurface);
                    game->global.messageSurface = NULL;
                }
                load_level(game, 1);
                placePlayerOnGround(game);
                printf("Game restarted, player at x=%d, y=%d\n", game->player.world_x, game->player.position.y);
            }

            if (!game->screen) {
                fprintf(stderr, "playLevel: Screen is NULL before rendering\n");
                exit(1);
            }
            SDL_FillRect(game->screen, NULL, SDL_MapRGB(game->screen->format, 0, 0, 0));
            SDL_Rect src = {scroll_x, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_Rect dest = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            if (src.w > game->background.width - scroll_x) src.w = game->background.width - scroll_x;
            if (src.h > game->background.height) src.h = game->background.height;
            if (game->background.image) {
                SDL_BlitSurface(game->background.image, &src, game->screen, &dest);
            } else {
                fprintf(stderr, "playLevel: Background image is NULL\n");
                exit(1);
            }

            if (game->level == 1) {
                renderJet(game->screen, &game->jet, scroll_x);
            }

            for (int i = 0; i < game->numSoldiers; i++) {
                if (game->soldiers[i].active && (game->soldiers[i].state != SOLDIER_DEAD || game->soldiers[i].frame < game->soldiers[i].totalFrames - 1)) {
                    int soldierWorldX = game->soldiers[i].world_x;
                    int dx = playerWorldX - soldierWorldX;
                    int shouldRender = (abs(dx) <= renderDistance || (game->soldiers[i].position.x - scroll_x < SCREEN_WIDTH + 256 &&
                                                                     game->soldiers[i].position.x - scroll_x > -256));
                    if (shouldRender) {
                        renderSoldier(game->screen, &game->soldiers[i], scroll_x);
                    }
                }
            }

            for (int i = 0; i < game->numSoldiers2; i++) {
                if (game->soldiers2[i].active && (game->soldiers2[i].state != SOLDIER2_DEAD || game->soldiers2[i].frame < game->soldiers2[i].totalFrames - 1)) {
                    int soldierWorldX = game->soldiers2[i].world_x;
                    int dx = playerWorldX - soldierWorldX;
                    int shouldRender = (abs(dx) <= renderDistance || (game->soldiers2[i].position.x - scroll_x < SCREEN_WIDTH + 256 &&
                                                                     game->soldiers2[i].position.x - scroll_x > -256));
                    if (shouldRender) {
                        renderSoldier2(game->screen, &game->soldiers2[i], scroll_x);
                    }
                }
            }

            for (int i = 0; i < game->numRobots; i++) {
                if (game->robots[i].active && (game->robots[i].state != ROBOT_DEAD || game->robots[i].frame < game->robots[i].totalFrames - 1)) {
                    int robotWorldX = game->robots[i].world_x;
                    int dx = playerWorldX - robotWorldX;
                    int shouldRender = (abs(dx) <= renderDistance || (game->robots[i].position.x - scroll_x < SCREEN_WIDTH + 256 &&
                                                                     game->robots[i].position.x - scroll_x > -256));
                    if (shouldRender) {
                        renderRobot(game->screen, &game->robots[i], scroll_x);
                    }
                }
            }

            if (game->level == 3) {
                for (int i = 0; i < game->numMummies; i++) {
                    if (game->mummies[i].active) {
                        int mummyWorldX = game->mummies[i].world_x;
                        int dx = playerWorldX - mummyWorldX;
                        int shouldRender = (abs(dx) <= renderDistance || (game->mummies[i].position.x - scroll_x < SCREEN_WIDTH + 256 &&
                                                                         game->mummies[i].position.x - scroll_x > -256));
                        if (shouldRender) {
                            renderMummy(game->screen, &game->mummies[i], scroll_x);
                        }
                    }
                }
                for (int i = 0; i < game->numDeceaseds; i++) {
                    if (game->deceaseds[i].active) {
                        int deceasedWorldX = game->deceaseds[i].world_x;
                        int dx = playerWorldX - deceasedWorldX;
                        int shouldRender = (abs(dx) <= renderDistance || (game->deceaseds[i].position.x - scroll_x < SCREEN_WIDTH + 256 &&
                                                                         game->deceaseds[i].position.x - scroll_x > -256));
                        if (shouldRender) {
                            renderDeceased(game->screen, &game->deceaseds[i], scroll_x);
                        }
                    }
                }
                for (int i = 0; i < game->numGorgons; i++) {
                    if (game->gorgons[i].active) {
                        int gorgonWorldX = game->gorgons[i].world_x;
                        int dx = playerWorldX - gorgonWorldX;
                        int shouldRender = (abs(dx) <= renderDistance || (game->gorgons[i].position.x - scroll_x < SCREEN_WIDTH + 256 &&
                                                                         game->gorgons[i].position.x - scroll_x > -256));
                        if (shouldRender) {
                            renderGorgon(game->screen, &game->gorgons[i], scroll_x);
                        }
                    }
                }
                for (int i = 0; i < game->numSpearmen; i++) {
                    if (game->spearmen[i].active) {
                        int spearmanWorldX = game->spearmen[i].world_x;
                        int dx = playerWorldX - spearmanWorldX;
                        int shouldRender = (abs(dx) <= renderDistance || (game->spearmen[i].position.x - scroll_x < SCREEN_WIDTH + 256 &&
                                                                         game->spearmen[i].position.x - scroll_x > -256));
                        if (shouldRender) {
                            renderSkeletonSpearman(game->screen, &game->spearmen[i], scroll_x);
                        }
                    }
                }
            }

            if (game->level == 1) {
                for (int i = 0; i < game->numNPCs; i++) {
                    if (game->npcs[i].active) {
                        int npcWorldX = game->npcs[i].world_x;
                        int dx = playerWorldX - npcWorldX;
                        int shouldRender = (abs(dx) <= renderDistance || (game->npcs[i].position.x - scroll_x < SCREEN_WIDTH + 256 &&
                                                                         game->npcs[i].position.x - scroll_x > -256));
                        if (shouldRender) {
                            renderNPC(game->screen, &game->npcs[i], scroll_x, game);
                        }
                    }
                }
            }

            if (game->level == 2) {
                for (int i = 0; i < game->numNPC2s; i++) {
                    if (game->npc2s[i].active) {
                        int npc2WorldX = game->npc2s[i].world_x;
                        int dx = playerWorldX - npc2WorldX;
                        int shouldRender = (abs(dx) <= renderDistance || (game->npc2s[i].position.x - scroll_x < SCREEN_WIDTH + 256 &&
                                                                         game->npc2s[i].position.x - scroll_x > -256));
                        if (shouldRender) {
                            renderNPC2(game->screen, &game->npc2s[i], scroll_x, game);
                        }
                    }
                }
            }

            // Render boss if active or level-up icon is active
            if ((game->bossActive && (game->boss.state != BOSS_DEATH || game->boss.frame < game->boss.deathFrameCount - 1)) || game->boss.levelUpIconActive) {
                int bossWorldX = game->boss.world_x;
                int dx = playerWorldX - bossWorldX;
                int shouldRender = (abs(dx) <= renderDistance || (bossWorldX - scroll_x < SCREEN_WIDTH + 256 && bossWorldX - scroll_x > -256));
                if (shouldRender) {
                    renderBoss(game->screen, &game->boss, scroll_x, game);
                }
            }

            if (game->portal.active) {
                renderPortal(game->screen, &game->portal, scroll_x);
            }

            // Render Player2 for level 3, Player otherwise
            if (game->level == 3) {
                renderPlayer2(game->screen, &game->player2);
            } else {
                renderPlayer(&game->player, game);
            }

            // Render health icon28 if active
            if (game->global.showHealthIcon && game->global.healthIcon28) {
                if (currentTime - game->global.healthIconTimer < 2000) {
                    SDL_Rect iconDest;
                    if (game->level == 3) {
                        iconDest.x = game->player2.position.x + (game->player2.position.w / 2) - (game->global.healthIcon28->w / 2);
                        iconDest.y = game->player2.position.y - game->global.healthIcon28->h - 10;
                    } else {
                        iconDest.x = game->player.position.x + (game->player.position.w / 2) - (game->global.healthIcon28->w / 2);
                        iconDest.y = game->player.position.y - game->global.healthIcon28->h - 10;
                    }
                    iconDest.w = game->global.healthIcon28->w;
                    iconDest.h = game->global.healthIcon28->h;
                    SDL_BlitSurface(game->global.healthIcon28, NULL, game->screen, &iconDest);
                    printf("Rendering icon28 at x=%d, y=%d\n", iconDest.x, iconDest.y);
                } else {
                    game->global.showHealthIcon = 0;
                    game->global.healthIconTimer = 0;
                    printf("Stopped rendering icon28\n");
                }
            }

            if (game->level != 3) {
                renderInventory(&game->inventory, game->screen, game->inventoryVisible, game->player.position);
                renderUI(&game->ui, game->screen, &game->player, &game->global);
            }

            if (game->global.showMessage && game->global.font) {
                if (game->global.messageSurface) {
                    SDL_FreeSurface(game->global.messageSurface);
                    game->global.messageSurface = NULL;
                }
                game->global.messageSurface = TTF_RenderText_Solid(game->global.font, game->global.message, (SDL_Color){255, 255, 255});
                if (game->global.messageSurface) {
                    SDL_Rect renderPos = game->global.messagePosition;
                    if (game->global.messagePosition.h >= 0 && game->global.messagePosition.h < game->numNPCs) {
                        int npcIndex = game->global.messagePosition.h;
                        if (game->npcs[npcIndex].active) {
                            int npcWorldX = game->npcs[npcIndex].world_x;
                            renderPos.x = (npcWorldX - scroll_x) + (game->npcs[npcIndex].position.w / 2) - (game->global.messageSurface->w / 2);
                        }
                    } else if (game->global.messagePosition.h >= MAX_NPCS && game->global.messagePosition.h < MAX_NPCS + game->numNPC2s) {
                        int npc2Index = game->global.messagePosition.h - MAX_NPCS;
                        if (game->npc2s[npc2Index].active) {
                            int npc2WorldX = game->npc2s[npc2Index].world_x;
                            renderPos.x = (npc2WorldX - scroll_x) + (game->npc2s[npc2Index].position.w / 2) - (game->global.messageSurface->w / 2);
                        }
                    } else {
                        int messageWorldX = game->global.messagePosition.w;
                        renderPos.x = (messageWorldX - scroll_x) - (game->global.messageSurface->w / 2);
                    }
                    SDL_BlitSurface(game->global.messageSurface, NULL, game->screen, &renderPos);
                } else {
                    fprintf(stderr, "Failed to render message surface: %s\n", TTF_GetError());
                }
            }
        }

        SDL_Flip(game->screen);
        Uint32 frameTime = SDL_GetTicks() - currentTime;
        const Uint32 targetFrameTime = 16; // ~60 FPS
        if (frameTime < targetFrameTime) {
            SDL_Delay(targetFrameTime - frameTime);
        }
    }
}
