#ifndef ASTRO_DODGE_H
#define ASTRO_DODGE_H

#include "raylib.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define MAX_ASTEROIDS 15
#define MAX_BULLETS 50
#define MAX_AMMO_DROPS 10


typedef struct Bullet {
    Vector2 position;
    bool active;
} Bullet;

typedef struct Asteroid {
    Vector2 position;
    float speed;
    bool active;
} Asteroid;

typedef struct {
    Vector2 position;
    float speed;
    bool active;
}AmmoDrop;

void InitAstroDodge(Texture2D shipTex, Texture2D asteroidTex);
void UpdateAstroDodge(void);
void DrawAstroDodge(void);
void UnloadAstroDodge(void);
bool IsAstroGameOver(void);
void RestartAstroDodge(void);
bool AstroWantsToReturnToMenu(void);
#endif
