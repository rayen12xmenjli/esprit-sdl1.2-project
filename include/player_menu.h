#ifndef PLAYER_MENU_H
#define PLAYER_MENU_H

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h> 

#define NUM_PLAYER_BUTTONS_MAIN 3 
#define NUM_PLAYER_BUTTONS_AVATAR 6 


#define PLAYER_MENU_MAIN    0   
#define PLAYER_MENU_AVATAR  1   
#define PLAYER_MENU_SCORES  2   


#define BTN_NORMAL 0 
#define BTN_HOVER  1 
#define BTN_HIDDEN 2 


typedef struct {
    SDL_Surface *normal;
    SDL_Surface *hover;
    SDL_Rect position;
    int state; 
} PlayerButton;


extern PlayerButton mainButtons[NUM_PLAYER_BUTTONS_MAIN]; 
extern PlayerButton avatarButtons[NUM_PLAYER_BUTTONS_AVATAR]; 
extern SDL_Surface *playerBackground;
extern Mix_Chunk *hoverSound;
extern TTF_Font *font;
extern int playerMenuState; 
extern int isSelectionMade; 

void PlayerMenu_Init();
int PlayerMenu_HandleEvents(SDL_Event *event);
void PlayerMenu_Render(SDL_Surface *screen);
void PlayerMenu_Cleanup();

#define PLAYER_MENU_CONTINUE 1
#define PLAYER_MENU_BACK 0
#define PLAYER_MENU_PROCEED 2

#endif // PLAYER_MENU_H
