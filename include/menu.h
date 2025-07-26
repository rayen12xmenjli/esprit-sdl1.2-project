#ifndef MENU_H
#define MENU_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>


// Constants
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define OPENING_ANIMATION_FRAMES 241
#define GAME_INTRO_FRAMES 9
#define LOADING_ANIMATION_FRAMES 93

// Game states
typedef enum {
    STATE_OPENING_ANIMATION,
    STATE_MAIN_MENU,
    STATE_GAME_INTRO,
    STATE_LOADING,
    STATE_COUNTDOWN,
    STATE_OPTIONS,
    STATE_SAVE_LOAD,
    STATE_PLAYER,
    STATE_SCORES,
    STATE_ENIGMA,
    STATE_ENIGMA_QUIZ,
    STATE_ENIGMA_PUZZLE,
    STATE_GAME,
    STATE_QUIT
} GameState;

// Background states
typedef enum {
    BACKGROUND_STATIC,
    OPENING_ANIMATION,
    GAME_INTRO_ANIMATION,
    LOADING_ANIMATION,
    COUNTDOWN_ANIMATION
} BackgroundState;


// Declare global variables for background assets
extern SDL_Surface *opening_layers[OPENING_ANIMATION_FRAMES];
extern SDL_Surface *game_intro_layers[GAME_INTRO_FRAMES];
extern SDL_Surface *loading_layers[LOADING_ANIMATION_FRAMES];
extern SDL_Surface *background_static;
extern SDL_Surface *countdown_background;

// Declare global variables for sounds
extern Mix_Chunk *countdown_beep;

// Declare global variables for animation
extern int current_frame;
extern Uint32 last_frame_time;
extern int animation_complete;




typedef struct {
    SDL_Rect position;
    SDL_Surface *image;
    SDL_Surface *hover_image;
    int hovered;
} Button;



// Function prototypes
void loadBackgroundAssets();
void renderBackground(SDL_Surface *screen, BackgroundState background_state);

void initialize_sdl();
void handle_events(int *running, int *selected, Button buttons[], int button_count, Mix_Chunk *hover_sound, Mix_Chunk *click_sound, int *selected_button);
void render(SDL_Surface *screen, SDL_Surface *background, Button buttons[], int button_count, TTF_Font *font, SDL_Color textColor, int selected_button);
void cleanup(Button buttons[], int button_count, SDL_Surface *background, Mix_Music *music, Mix_Chunk *hover_sound, Mix_Chunk *click_sound, TTF_Font *font);









#endif // MENU_H
