#ifndef GAME_H
#define GAME_H

#include "raylib.h"

// === DEFINIÇÕES GLOBAIS ===
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// === ENUM ESTADO DO JOGO ===
typedef enum GameState {
    MENU,
    PONG,
    ASTRO_DODGE,
    AVOID_WALLS,
    COMMANDS,
} GameState;

extern GameState currentState;
#endif