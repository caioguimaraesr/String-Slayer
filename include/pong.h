#ifndef PONG_H
#define PONG_H

#include "raylib.h"
#include "globals.h"

#define ROWS 15
#define COLS 20
#define PADDLE_WIDTH 10
#define PADDLE_HEIGHT 100
#define PADDLE_SPEED 14
#define BALL_RADIUS 8
#define BALL_SPEED 10
#define MAX_HISTORY 100

typedef struct BallPositionNode {
    Vector2 position;
    struct BallPositionNode *next;
} BallPositionNode;

typedef struct {
    Rectangle player1;
    Rectangle player2;
    Vector2 ballPosition;
    Vector2 ballSpeed;
    int score1;
    int score2;
    int grid[ROWS][COLS];
    BallPositionNode *history;
    int historyCount;
    float gameTime;
    bool gameEnded;
    bool started;
    float countdown;
    bool pongStarted;  // Novo campo
    float pongCountdown; // Novo campo
} PongGame;

void InitPong(PongGame *game);
void UpdatePong(PongGame *game);
void DrawPong(PongGame *game);
void FreeBallHistory(BallPositionNode *head);
void RunPongGame(GameState *state);

#endif