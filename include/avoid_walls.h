#ifndef AVOID_WALLS_H
#define AVOID_WALLS_H

#include "raylib.h"

// === DEFINIÇÕES AVOID WALLS ===
#define WALL_WIDTH 80
#define WALL_GAP 200
#define MAX_WALLS 5

// === ESTRUTURAS AVOID WALLS ===
typedef struct {
    float x;
    float gapY;
    bool passed;
} Wall;

#endif