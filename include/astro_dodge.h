#ifndef ASTRO_DODGE_H
#define ASTRO_DODGE_H

#include "raylib.h"
#include "globals.h"

#define MAX_ASTEROIDS 20
#define MAX_BULLETS 50
#define BULLET_SPEED 10.0f

typedef struct {
    Vector2 position;
    float speed;
    bool active;
} Asteroid;

typedef struct {
    Vector2 position;
    bool active;
} Bullet;

typedef struct {
    Rectangle player;
    float playerSpeed;
    Asteroid asteroids[MAX_ASTEROIDS];
    Bullet bullets[MAX_BULLETS];
    bool gameOver;
    float survivalTime;
    float bestTime;
    float startCountdown;
    bool gameStarted;
    Sound hitSound;
} AstroDodgeGame;

void InitAstroDodge(AstroDodgeGame *game);
void UpdateAstroDodge(AstroDodgeGame *game);
void DrawAstroDodge(AstroDodgeGame *game, Texture2D shipTexture, Texture2D asteroidTexture);
void RunAstroDodgeGame(GameState *state, Texture2D shipTexture, Texture2D asteroidTexture);

#endif