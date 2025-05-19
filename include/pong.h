#ifndef PONG_H
#define PONG_H

#include "raylib.h"

// === DEFINIÇÕES PONG ===
#define PADDLE_WIDTH 10
#define PADDLE_HEIGHT 100
#define PADDLE_SPEED 14
#define BALL_RADIUS 8
#define BALL_SPEED 10
#define MAX_HISTORY 100
#define ROWS 15
#define COLS 20

// === ESTRUTURAS PONG ===
typedef struct BallPositionNode {
    Vector2 position;
    struct BallPositionNode *next;
} BallPositionNode;

#endif