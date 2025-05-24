#ifndef PONG_H
#define PONG_H

#include "raylib.h"  // Presumo que você use raylib pelo GetFrameTime(), DrawText(), etc.

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define ROWS 15
#define COLS 20

#define MAX_HISTORY 60
#define BALL_RADIUS 8
#define BALL_SPEED 8
#define PADDLE_SPEED 12
#define PADDLE_HEIGHT 100
#define PADDLE_WIDTH 10 

typedef struct Vector2 Vector2;

typedef struct BallPositionNode {
    Vector2 position;
    struct BallPositionNode *next;
} BallPositionNode;


extern int historyCount;
extern BallPositionNode *history;
extern int grid[ROWS][COLS];
extern Vector2 ballPosition;
extern Vector2 ballSpeed;
extern Rectangle player1;
extern Rectangle player2;
extern int score1;
extern int score2;
extern float gameTime;
extern bool gameEnded;
extern bool pongStarted;
extern float pongCountdown;
extern bool pointScored;
extern float pointDelay;
extern bool rebound;
extern bool barHit;



void DrawCountdown(void);
void UpdateGame(Sound pongPoint,Sound pongRebound,Sound pongBar);
void DrawGame(void);
void HandleGameEnd(void);
void ResetGrid(void);
void ResetAfterPoint(void);
void CheckCollisions(Sound pongRebound,Sound pongBar);
void AppendBallPosition(BallPositionNode **head, Vector2 pos, int *count);
void FreeBallHistory(BallPositionNode *head);
void ResetBallHistory();
void ResetGrid();
#endif 
