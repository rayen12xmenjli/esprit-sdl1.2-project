#include <SDL/SDL_mixer.h>
#include "sound.h"
#include <stdio.h>

// Jouer de la musique
void play_music(const char *filepath) {
    Mix_Music *music = Mix_LoadMUS(filepath); // Charger la musique
    if (!music) {
        fprintf(stderr, "Erreur : Impossible de charger la musique (%s).\n", Mix_GetError());
        return;
    }

    // Jouer la musique en boucle (-1 pour une lecture infinie)
    if (Mix_PlayMusic(music, -1) == -1) {
        fprintf(stderr, "Erreur : Impossible de jouer la musique (%s).\n", Mix_GetError());
        Mix_FreeMusic(music);
        return;
    }
}

// Fonction pour jouer un effet sonore
void play_sound_effect(Mix_Chunk *sound) {
    if (sound) {
        Mix_PlayChannel(-1, sound, 0);

        // Ajuster le volume des effets sonores (0 à 128)
        Mix_VolumeChunk(sound, 64); // Volume moyen
    }
}
