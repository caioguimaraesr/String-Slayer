#ifndef AVOID_H
#define AVOID_H

#include "raylib.h"
#include "globals.h"

#define MAX_WALLS 5
#define WALL_WIDTH 60
#define WALL_GAP 200

typedef struct {
    float x;
    float gapY;
    bool passed;
} Wall;

typedef struct {
    Rectangle player;
    float velocity;
    Wall walls[MAX_WALLS];
    bool gameOver;
    float time;
    float bestTime;
    float countdown;
    bool started;
    float wallSpeed;
    float wallSpacing;
    float jumpForce;
    float gravity;
} AvoidGame;

void InitAvoidGame(AvoidGame *game);
void UpdateAvoidGame(AvoidGame *game);
void DrawAvoidGame(AvoidGame *game, Texture2D birdTexture, Texture2D pipeTexture, Texture2D background);
void RunAvoidWalls(GameState *state, Texture2D birdTexture, Texture2D pipeTexture, Texture2D background);

#endif