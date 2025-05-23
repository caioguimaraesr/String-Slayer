#include "pong.h"
#include <stdlib.h>
#include "score.h"
#include <stdio.h>
#include <game.h>
#include <string.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

int historyCount = 0;
BallPositionNode *history = NULL;
int grid[ROWS][COLS] = {0};
Sound pongBar;
Sound pongPoint;
Sound pongRebound;
Vector2 ballPosition = {SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f};
Vector2 ballSpeed = {BALL_SPEED, BALL_SPEED};
Rectangle player1 = {10, SCREEN_HEIGHT/2 - PADDLE_HEIGHT/2, PADDLE_WIDTH, PADDLE_HEIGHT};
Rectangle player2 = {SCREEN_WIDTH - 10 - PADDLE_WIDTH, SCREEN_HEIGHT/2 - PADDLE_HEIGHT/2, PADDLE_WIDTH, PADDLE_HEIGHT};
int score1 = 0;
int score2 = 0;
float gameTime = 60.0f;
bool gameEnded = false;
bool pongStarted = false;
float pongCountdown = 3.0f;
bool pointScored = false;
float pointDelay = 0.0f;
bool rebound = false;
bool barHit = false;
static char playerName[MAX_NAME_LENGTH] = {0};
static bool nameInputActive = false;
static int nameLetterCount = 0;


void HandlePointScored() {
    if (pointScored) {
        pointDelay -= GetFrameTime();
        if (pointDelay <= 0.0f) {
            pointScored = false;
        }
    }
}

void DrawCountdown() {
    if (!pongStarted) {
        pongCountdown -= GetFrameTime();

        int countDisplay = (int)pongCountdown + 1;
        if (countDisplay > 0) {
            DrawText(TextFormat("%d", countDisplay), SCREEN_WIDTH/2 - 20, SCREEN_HEIGHT/2 - 40, 80, WHITE);
        } else {
            DrawText("Vai!", SCREEN_WIDTH/2 - 40, SCREEN_HEIGHT/2 - 40, 80, WHITE);
        }

        if (pongCountdown <= 0) {
            pongStarted = true;
        }
    }
}

void ResetAfterPoint() {
    ballPosition = (Vector2){SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f};
    FreeBallHistory(history);
    history = NULL;
    historyCount = 0;
    ResetGrid();
    pointScored = true;
    pointDelay = 0.5f;
}

void CheckCollisions(Sound pongRebound,Sound pongBar) {
    if (ballPosition.y <= BALL_RADIUS || ballPosition.y >= SCREEN_HEIGHT - BALL_RADIUS) {
        if (!rebound) {
            ballSpeed.y *= -1;
            PlaySound(pongRebound);
            rebound = true;
        }
    } else {
        rebound = false;
    }

    if (CheckCollisionCircleRec(ballPosition, BALL_RADIUS, player1) && ballSpeed.x < 0) {
        if (!barHit) {
            ballSpeed.x *= -1;
            PlaySound(pongBar);
            barHit = true;
        }
    } else {
        barHit = false;
    }

    if (CheckCollisionCircleRec(ballPosition, BALL_RADIUS, player2) && ballSpeed.x > 0) {
        if (!barHit) {
            ballSpeed.x *= -1;
            PlaySound(pongBar);
            barHit = true;
        }
    } else {
        barHit = false;
    }
}

void UpdateGame(Sound pongPoint, Sound pongRebound, Sound pongBar) {
    if (!pongStarted) return;

    if (!gameEnded && !pointScored) {
        gameTime -= GetFrameTime();
        if (gameTime <= 0.0f) {
            gameTime = 0.0f;
            gameEnded = true;
        }

        if (IsKeyDown(KEY_W) && player1.y > 0) player1.y -= PADDLE_SPEED;
        if (IsKeyDown(KEY_S) && player1.y + PADDLE_HEIGHT < SCREEN_HEIGHT) player1.y += PADDLE_SPEED;
        if (IsKeyDown(KEY_UP) && player2.y > 0) player2.y -= PADDLE_SPEED;
        if (IsKeyDown(KEY_DOWN) && player2.y + PADDLE_HEIGHT < SCREEN_HEIGHT) player2.y += PADDLE_SPEED;

        ballPosition.x += ballSpeed.x;
        ballPosition.y += ballSpeed.y;

        int col = (int)(ballPosition.x / (SCREEN_WIDTH / COLS));
        int row = (int)(ballPosition.y / (SCREEN_HEIGHT / ROWS));
        if (row >= 0 && row < ROWS && col >= 0 && col < COLS) grid[row][col]++;

        AppendBallPosition(&history, ballPosition, &historyCount);

        CheckCollisions(pongRebound, pongBar);

        if (ballPosition.x < 0 || ballPosition.x > SCREEN_WIDTH) {
            if (ballPosition.x < 0) {
                score2++;
                PlaySound(pongPoint);
                ballSpeed = (Vector2){-BALL_SPEED, BALL_SPEED};
            } else {
                score1++;
                PlaySound(pongPoint);
                ballSpeed = (Vector2){BALL_SPEED, BALL_SPEED};
            }
            ResetAfterPoint();
        }
    }
}


void DrawGame() {
    // Desenha grid
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (grid[r][c] > 0) {
                float intensity = (float)grid[r][c] / 10.0f;
                if (intensity > 1.0f) intensity = 1.0f;
                Color cellColor = Fade(BLUE, intensity);
                DrawRectangle(c * (SCREEN_WIDTH / COLS), r * (SCREEN_HEIGHT / ROWS), SCREEN_WIDTH / COLS, SCREEN_HEIGHT / ROWS, cellColor);
            }
        }
    }

    for (int i = 0; i < SCREEN_HEIGHT; i += 20)
        DrawRectangle(SCREEN_WIDTH/2 - 1, i, 2, 10, GRAY);

    DrawRectangleRec(player1, WHITE);
    DrawRectangleRec(player2, WHITE);
    DrawCircleV(ballPosition, BALL_RADIUS, WHITE);

    BallPositionNode *curr = history;
    while (curr != NULL) {
        DrawCircleV(curr->position, 2, RED);
        curr = curr->next;
    }

    int seconds = (int)gameTime;
    DrawText(TextFormat("Tempo: %02d:%02d", seconds / 60, seconds % 60), SCREEN_WIDTH/2 - 70, 10, 30, LIGHTGRAY);
    DrawText(TextFormat("%d", score1), SCREEN_WIDTH/4, 50, 40, WHITE);
    DrawText(TextFormat("%d", score2), 3*SCREEN_WIDTH/4, 50, 40, WHITE);
}

void HandleGameEnd() {
    if (gameEnded) {
        const char *msg = (score1 > score2) ? "Jogador 1 Venceu!" : (score2 > score1) ? "Jogador 2 Venceu!" : "Empate!";
        DrawText(msg, SCREEN_WIDTH/2 - MeasureText(msg, 40)/2, SCREEN_HEIGHT/2 - 20, 40, YELLOW);
        
        int winningScore = (score1 > score2) ? score1 : score2;
        
        // Verifica se é um high score
        bool highScore = false;
        if (highScores[0].count < MAX_SCORES || winningScore > highScores[0].entries[highScores[0].count-1].score) {
            highScore = true;
        }

        if (highScore && !nameInputActive) {
            nameInputActive = true;
            memset(playerName, 0, MAX_NAME_LENGTH);
            nameLetterCount = 0;
        }

        if (nameInputActive) {
            // Captura entrada do teclado para o nome
            int key = GetCharPressed();
            while (key > 0) {
                if ((key >= 32) && (key <= 125) && (nameLetterCount < MAX_NAME_LENGTH - 1)) {
                    playerName[nameLetterCount] = (char)key;
                    playerName[nameLetterCount + 1] = '\0';
                    nameLetterCount++;
                }
                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE)) {
                nameLetterCount--;
                if (nameLetterCount < 0) nameLetterCount = 0;
                playerName[nameLetterCount] = '\0';
            }

            DrawText("Novo High Score! Digite seu nome:", SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/2 + 20, 20, GREEN);
            DrawText(playerName, SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 50, 30, WHITE);
            DrawText("Pressione ENTER para confirmar", SCREEN_WIDTH/2 - 180, SCREEN_HEIGHT/2 + 90, 20, GRAY);

            if (IsKeyPressed(KEY_ENTER) && nameLetterCount > 0) {
                nameInputActive = false;
                atualizarScore(0, winningScore, playerName); // 0 para PONG
                salvarScores("scores.dat");
                currentState = GAMES_MENU;
            }
        } else {
            DrawText("Pressione ENTER para voltar ao menu", SCREEN_WIDTH/2 - 180, SCREEN_HEIGHT/2 + 30, 20, GRAY);
            
            if (IsKeyPressed(KEY_ENTER)) {
                atualizarScore(0, winningScore, "Anonimo");
                salvarScores("scores.dat");
                currentState = GAMES_MENU;
            }
        }
    }
}

void ResetGrid() {
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            grid[r][c] = 0;
        }
    }
}

void ResetBallHistory() {
    FreeBallHistory(history);
    history = NULL;
    historyCount = 0;
}

void FreeBallHistory(BallPositionNode *head) {
    while (head != NULL) {
        BallPositionNode *temp = head;
        head = head->next;
        free(temp);
    }
}

void AppendBallPosition(BallPositionNode **head, Vector2 pos, int *count) {
    BallPositionNode *newNode = malloc(sizeof(BallPositionNode));
    newNode->position = pos;
    newNode->next = NULL;

    if (*head == NULL) {
        *head = newNode;
    } else {
        BallPositionNode *curr = *head;
        while (curr->next != NULL)
            curr = curr->next;
        curr->next = newNode;
    }

    (*count)++;
    if (*count > MAX_HISTORY) {
        BallPositionNode *temp = *head;
        *head = (*head)->next;
        free(temp);
        (*count)--;
    }
}

void ResetPongGame() {
    nameInputActive = false;
    memset(playerName, 0, MAX_NAME_LENGTH);
    nameLetterCount = 0;
}