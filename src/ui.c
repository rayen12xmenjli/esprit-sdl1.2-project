#include "ui.h"
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void initUI(UI *ui) {
    printf("Starting initUI...\n");

    if (!TTF_WasInit() && TTF_Init() == -1) {
        fprintf(stderr, "TTF_Init failed: %s\n", TTF_GetError());
        exit(1);
    }

    ui->avatar = IMG_Load("assets/ui/avatar.png");
    if (!ui->avatar) {
        fprintf(stderr, "Failed to load avatar.png: %s\n", SDL_GetError());
    }

    ui->font = TTF_OpenFont("assets/fonts/arial.ttf", 24);
    if (!ui->font) {
        fprintf(stderr, "Failed to load arial.ttf: %s\n", TTF_GetError());
        ui->font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 24);
        if (!ui->font) {
            fprintf(stderr, "Failed to load fallback font: %s\n", TTF_GetError());
            exit(1);
        }
    }
    ui->textColor = (SDL_Color){255, 255, 255, 255};

    for (int i = 0; i < WASTED_FRAMES; i++) {
        ui->wastedImages[i] = NULL;
        char filename[64];
        snprintf(filename, sizeof(filename), "assets/WASTED/%d.png", i + 1);
        ui->wastedImages[i] = IMG_Load(filename);
        if (!ui->wastedImages[i]) {
            fprintf(stderr, "Failed to load %s: %s\n", filename, SDL_GetError());
        }
    }

    // Load ammo icon
    ui->ammoIcon = IMG_Load("assets/ui/ammo_icon.png");
    if (!ui->ammoIcon) {
        fprintf(stderr, "Failed to load ammo_icon.png: %s\n", SDL_GetError());
    } else {
        SDL_SetColorKey(ui->ammoIcon, SDL_SRCCOLORKEY, SDL_MapRGB(ui->ammoIcon->format, 255, 0, 255));
    }

    ui->cachedScoreSurface = NULL;
    ui->cachedAmmoSurface = NULL;
    ui->wastedFrame = 0;
    ui->wastedDelay = 0;
    ui->showWasted = 0;
    ui->pos_avatar.x = 20;
    ui->pos_avatar.y = 15;
    ui->pos_lives.x = 120;
    ui->pos_lives.y = 110;
    ui->pos_healthBar.x = 153;
    ui->pos_healthBar.y = 50;
    ui->pos_score.x = 10;
    ui->pos_score.y = 160;
    printf("UI initialized.\n");
}

void updateUI(UI *ui, int playerLives) {
    if (ui->showWasted) {
        ui->wastedDelay++;
        if (ui->wastedDelay >= 5) {
            ui->wastedFrame++;
            if (ui->wastedFrame >= WASTED_FRAMES) {
                ui->wastedFrame = WASTED_FRAMES - 1;
            }
            ui->wastedDelay = 0;
        }
    } else {
        ui->wastedFrame = 0;
        ui->wastedDelay = 0;
    }
}

void renderUI(UI *ui, SDL_Surface *screen, Player *player, Global *global) {
    if (ui->avatar) {
        SDL_BlitSurface(ui->avatar, NULL, screen, &ui->pos_avatar);
    }

    char livesText[20];
    snprintf(livesText, sizeof(livesText), "%d", player->lives);
    SDL_Surface *livesSurface = TTF_RenderText_Solid(ui->font, livesText, ui->textColor);
    if (livesSurface) {
        SDL_Rect livesRect = {ui->pos_lives.x, ui->pos_lives.y, 0, 0};
        SDL_BlitSurface(livesSurface, NULL, screen, &livesRect);
        SDL_FreeSurface(livesSurface);
    } else {
        fprintf(stderr, "Warning: Failed to render lives text: %s\n", TTF_GetError());
    }

    // Health Bar Rendering
    if (player->healthBarBg && player->healthBarGreen) {
        if (player->health > 0) {
            int health = player->health;
            if (health < 0) health = 0;
            if (health > player->maxHealth) health = player->maxHealth;
            int greenWidth = (health * player->healthBarBg->w) / player->maxHealth;
            if (greenWidth > player->healthBarBg->w) greenWidth = player->healthBarBg->w;
            SDL_Rect greenSrc = {0, 0, greenWidth, player->healthBarGreen->h};
            SDL_Rect greenDst = {ui->pos_healthBar.x, ui->pos_healthBar.y, greenWidth, player->healthBarGreen->h};
            if (SDL_BlitSurface(player->healthBarGreen, &greenSrc, screen, &greenDst) < 0) {
                fprintf(stderr, "Warning: Failed to blit healthBarGreen: %s\n", SDL_GetError());
            }
        }
        SDL_Rect bgRect = {ui->pos_healthBar.x, ui->pos_healthBar.y, player->healthBarBg->w, player->healthBarBg->h};
        if (SDL_BlitSurface(player->healthBarBg, NULL, screen, &bgRect) < 0) {
            fprintf(stderr, "Warning: Failed to blit healthBarBg: %s\n", SDL_GetError());
        }
    } else {
        if (!player->healthBarBg) fprintf(stderr, "Warning: healthBarBg is NULL\n");
        if (!player->healthBarGreen && player->health > 0) fprintf(stderr, "Warning: healthBarGreen is NULL\n");
    }

    // Score Rendering
    static int lastScore = -1;
    if (player->score != lastScore) {
        if (ui->cachedScoreSurface) {
            SDL_FreeSurface(ui->cachedScoreSurface);
            ui->cachedScoreSurface = NULL;
        }
        char scoreText[32];
        snprintf(scoreText, sizeof(scoreText), "Score: %d", player->score);
        ui->cachedScoreSurface = TTF_RenderText_Solid(ui->font, scoreText, ui->textColor);
        lastScore = player->score;
        if (!ui->cachedScoreSurface) {
            fprintf(stderr, "Warning: Failed to render score text: %s\n", TTF_GetError());
        }
    }
    if (ui->cachedScoreSurface) {
        SDL_Rect scoreRect = {ui->pos_score.x, ui->pos_score.y, 0, 0};
        SDL_BlitSurface(ui->cachedScoreSurface, NULL, screen, &scoreRect);
    }

    // Ammo Icon and Count Rendering
    if (ui->ammoIcon) {
        SDL_Rect ammoIconRect = {ui->pos_score.x, ui->pos_score.y + 30, 0, 0};
        SDL_BlitSurface(ui->ammoIcon, NULL, screen, &ammoIconRect);
    }
    static int lastAmmo = -1;
    if (player->ammo != lastAmmo) {
        if (ui->cachedAmmoSurface) {
            SDL_FreeSurface(ui->cachedAmmoSurface);
            ui->cachedAmmoSurface = NULL;
        }
        char ammoText[20];
        snprintf(ammoText, sizeof(ammoText), "%d", player->ammo);
        ui->cachedAmmoSurface = TTF_RenderText_Solid(ui->font, ammoText, ui->textColor);
        lastAmmo = player->ammo;
        if (!ui->cachedAmmoSurface) {
            fprintf(stderr, "Warning: Failed to render ammo text: %s\n", TTF_GetError());
        }
    }
    if (ui->cachedAmmoSurface) {
        int iconWidth = ui->ammoIcon ? ui->ammoIcon->w : 0;
        SDL_Rect ammoRect = {ui->pos_score.x + iconWidth + 10, ui->pos_score.y + 30, 0, 0};
        SDL_BlitSurface(ui->cachedAmmoSurface, NULL, screen, &ammoRect);
    }

    if (ui->showWasted) {
        if (ui->wastedFrame < WASTED_FRAMES && ui->wastedImages[ui->wastedFrame]) {
            SDL_Rect wastedRect = {(SCREEN_WIDTH - ui->wastedImages[ui->wastedFrame]->w) / 2,
                                   (SCREEN_HEIGHT - ui->wastedImages[ui->wastedFrame]->h) / 2,
                                   0, 0};
            SDL_BlitSurface(ui->wastedImages[ui->wastedFrame], NULL, screen, &wastedRect);
        }
    }

    if (global->gameOver) {
        const char *gameOverText = "Game Over - Press R to Restart";
        SDL_Surface *gameOverSurface = TTF_RenderText_Solid(ui->font, gameOverText, ui->textColor);
        if (gameOverSurface) {
            SDL_Rect rect = {(SCREEN_WIDTH - gameOverSurface->w) / 2, SCREEN_HEIGHT / 2 + 50, 0, 0};
            SDL_BlitSurface(gameOverSurface, NULL, screen, &rect);
            SDL_FreeSurface(gameOverSurface);
        } else {
            fprintf(stderr, "Warning: Failed to render game over text: %s\n", TTF_GetError());
        }
    }

    if (global->showMessage && strlen(global->message) > 0) {
        SDL_Surface *messageSurface = TTF_RenderText_Solid(ui->font, global->message, ui->textColor);
        if (messageSurface) {
            SDL_Rect messageRect = {global->messagePosition.x, global->messagePosition.y, 0, 0};
            SDL_BlitSurface(messageSurface, NULL, screen, &messageRect);
            SDL_FreeSurface(messageSurface);
        } else {
            fprintf(stderr, "Warning: Failed to render message text: %s\n", TTF_GetError());
        }
    }
}

void freeUI(UI *ui) {
    if (ui->avatar) {
        SDL_FreeSurface(ui->avatar);
        ui->avatar = NULL;
    }
    for (int i = 0; i < WASTED_FRAMES; i++) {
        if (ui->wastedImages[i]) {
            SDL_FreeSurface(ui->wastedImages[i]);
            ui->wastedImages[i] = NULL;
        }
    }
    if (ui->font) {
        TTF_CloseFont(ui->font);
        ui->font = NULL;
    }
    if (ui->cachedScoreSurface) {
        SDL_FreeSurface(ui->cachedScoreSurface);
        ui->cachedScoreSurface = NULL;
    }
    if (ui->ammoIcon) {
        SDL_FreeSurface(ui->ammoIcon);
        ui->ammoIcon = NULL;
    }
    if (ui->cachedAmmoSurface) {
        SDL_FreeSurface(ui->cachedAmmoSurface);
        ui->cachedAmmoSurface = NULL;
    }
    printf("UI resources freed\n");
}
