#ifndef ASTRO_DODGE_H
#define ASTRO_DODGE_H

#include "raylib.h"

// === DEFINIÇÕES ASTRO DODGE ===
#define MAX_ASTEROIDS 20
#define MAX_BULLETS 50

// === ESTRUTURAS ASTRO DODGE ===
typedef struct Bullet {
    Vector2 position;
    bool active;
} Bullet;

typedef struct Asteroid {
    Vector2 position;
    float speed;
    bool active;
} Asteroid;

#endif