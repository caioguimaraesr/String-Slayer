#include "avoid_walls.h"
#include <stdlib.h>
#include <stdio.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

static Texture2D background;
static Texture2D birdTexture;
static Texture2D pipeTexture;

static Rectangle avoidPlayer;
static float avoidVelocity = 0;
static float avoidGravity = 500;
static float avoidJumpForce = -250;
static float avoidTime = 0;
static float avoidBestTime = 0;
static bool avoidGameOver = false;
static bool avoidStarted = false;
static float avoidCountdown = 3.0f;

static Wall walls[MAX_WALLS];
static float wallSpacing = 300;
static float wallSpeed = 200;

void AvoidInit(void) {
    background = LoadTexture("assets/images/Fundo Avoid the Walls.png");
    birdTexture = LoadTexture("assets/images/Flávio Caça-Rato.png");
    pipeTexture = LoadTexture("assets/images/pipe.png");

    avoidPlayer = (Rectangle){ 100, SCREEN_HEIGHT/2, 34, 24 };
    avoidVelocity = 0;
    avoidTime = 0;
    avoidGameOver = false;
    avoidStarted = false;
    avoidCountdown = 3.0f;

    for (int i = 0; i < MAX_WALLS; i++) {
        walls[i].x = SCREEN_WIDTH + i * wallSpacing;
        walls[i].gapY = 100 + rand() % (SCREEN_HEIGHT - WALL_GAP - 100);
        walls[i].passed = false;
    }
}

void AvoidUpdate(void) {
    if (avoidGameOver) return;

    if (!avoidStarted) {
        avoidCountdown -= GetFrameTime();
        if (avoidCountdown <= 0) {
            avoidStarted = true;
        }
        return;
    }

    avoidTime += GetFrameTime();

    if (IsKeyPressed(KEY_SPACE)) avoidVelocity = avoidJumpForce;

    avoidVelocity += avoidGravity * GetFrameTime();
    avoidPlayer.y += avoidVelocity * GetFrameTime();

    for (int i = 0; i < MAX_WALLS; i++) {
        walls[i].x -= wallSpeed * GetFrameTime();

        #define MIN_SPACING 250

// no AvoidUpdate, dentro do for das paredes:
        if (walls[i].x + WALL_WIDTH < 0) {
            float maxX = 0;
            for (int j = 0; j < MAX_WALLS; j++) {
                if (walls[j].x > maxX) maxX = walls[j].x;
    }
    
            float spacing = MIN_SPACING + (rand() % 201); // espaçamento aleatório entre 250 e 450 px
            walls[i].x = maxX + spacing;
            walls[i].gapY = 100 + rand() % (SCREEN_HEIGHT - WALL_GAP - 100);
            walls[i].passed = false;
}

        Rectangle topWall = { walls[i].x, 0, WALL_WIDTH, walls[i].gapY };
        Rectangle bottomWall = { walls[i].x, walls[i].gapY + WALL_GAP, WALL_WIDTH, SCREEN_HEIGHT - (walls[i].gapY + WALL_GAP) };

        if (CheckCollisionRecs(avoidPlayer, topWall) || CheckCollisionRecs(avoidPlayer, bottomWall) ||
            avoidPlayer.y < 0 || avoidPlayer.y + avoidPlayer.height > SCREEN_HEIGHT) {
            avoidGameOver = true;
            if (avoidTime > avoidBestTime) avoidBestTime = avoidTime;
        }
    }
}

void AvoidDraw(void) {
    DrawTexture(background, 0, 0, WHITE);

    if (!avoidGameOver) {
        if (!avoidStarted) {
            int countDisplay = (int)avoidCountdown + 1;
            if (countDisplay > 0) {
                DrawText(TextFormat("%d", countDisplay), SCREEN_WIDTH/2 - 20, SCREEN_HEIGHT/2 - 40, 80, WHITE);
            } else {
                DrawText("Vai!", SCREEN_WIDTH/2 - 40, SCREEN_HEIGHT/2 - 40, 80, WHITE);
            }
        } else {
            for (int i = 0; i < MAX_WALLS; i++) {
                Rectangle topWall = { walls[i].x, 0, WALL_WIDTH, walls[i].gapY };
                Rectangle bottomWall = { walls[i].x, walls[i].gapY + WALL_GAP, WALL_WIDTH, SCREEN_HEIGHT - (walls[i].gapY + WALL_GAP) };

                DrawTexturePro(pipeTexture,
                    (Rectangle){0, 0, pipeTexture.width, pipeTexture.height},
                    topWall,
                    (Vector2){0, 0},
                    0.0f,
                    WHITE);

                DrawTexturePro(pipeTexture,
                    (Rectangle){0, 0, pipeTexture.width, pipeTexture.height},
                    bottomWall,
                    (Vector2){0, 0},
                    0.0f,
                    WHITE);
            }

            DrawTextureEx(birdTexture, (Vector2){avoidPlayer.x, avoidPlayer.y}, 0.0f, 0.5f, WHITE);
            DrawText(TextFormat("Tempo: %.2f", avoidTime), 20, 20, 20, WHITE);
            DrawText(TextFormat("Melhor: %.2f", avoidBestTime), 20, 50, 20, YELLOW);
        }
    } else {
        DrawText("Game Over!", SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 - 30, 40, RED);
        DrawText("Pressione ENTER para voltar ao menu", SCREEN_WIDTH/2 - 180, SCREEN_HEIGHT/2 + 10, 20, LIGHTGRAY);
    }
}

bool AvoidIsGameOver(void) {
    return avoidGameOver;
}

float AvoidGetBestTime(void) {
    return avoidBestTime;
}

void AvoidUnload(void) {
    UnloadTexture(background);
    UnloadTexture(birdTexture);
    UnloadTexture(pipeTexture);
}
