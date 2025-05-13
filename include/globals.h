#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdbool.h>

// Tamanho da janela
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450

// Astro Dodge
#define MAX_ASTEROIDS 20
#define MAX_BULLETS 50

// Avoid the Walls
#define MAX_WALLS 5
#define WALL_WIDTH 60
#define WALL_GAP 200

// Estado geral do jogo
typedef enum GameState {
    MENU,
    PONG,
    ASTRO_DODGE,
    AVOID_WALLS,
} GameState;

#endif
