#include "avoid_walls.h"
#include <stdlib.h>
#include <stdio.h>
#include "score.h"
#include <string.h>
#include "game.h"
#include "raylib.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

extern GameState currentState;
static Texture2D background;
static Texture2D birdTexture;
static Texture2D pipeTexture;
static Rectangle avoidPlayer;
static float avoidVelocity = 0;
static float avoidGravity = 500;
static float avoidJumpForce = -250;
static int pipePassedRecord = 0;
static int lastPipeCheckpoint = 0;
static bool avoidGameOver = false;
static bool avoidStarted = false;
static float avoidCountdown = 3.0f;
static Wall walls[MAX_WALLS];
static float wallSpacing = 300;
static float wallSpeed = 200;
static int pipePassed;
static Sound pointSound;
static Sound gameOverSound;
static Sound flapSound;

static char playerName[MAX_NAME_LENGTH] = {0};
static bool nameInputActive = false;
static int nameLetterCount = 0;

void AvoidInit(void) {
    background = LoadTexture("assets/images/Fundo-avoid.jpg");
    birdTexture = LoadTexture("assets/images/Pombo.png");
    pipeTexture = LoadTexture("assets/images/pipe.png");
    pointSound = LoadSound("assets/music/Correct-Avoid.wav");
    gameOverSound = LoadSound("assets/music/GameOver-Avoid.wav");
    flapSound = LoadSound("assets/music/Fly-Avoid.wav");
    wallSpeed = 200;
    avoidPlayer = (Rectangle){ 100, SCREEN_HEIGHT/2, 34, 24 };
    avoidVelocity = 0;
    avoidGameOver = false;
    avoidStarted = false;
    avoidCountdown = 3.0f;
    nameInputActive = false;
    memset(playerName, 0, MAX_NAME_LENGTH);
    nameLetterCount = 0;

    for (int i = 0; i < MAX_WALLS; i++) {
        walls[i].x = SCREEN_WIDTH + i * wallSpacing;
        walls[i].gapY = 100 + rand() % (SCREEN_HEIGHT - WALL_GAP - 100);
        walls[i].passed = false;
    }
}

void AvoidUpdate(void) {
    if (avoidGameOver) {
        bool highScore = false;
        if (highScores[2].count < MAX_SCORES || pipePassed > highScores[2].entries[highScores[2].count-1].score) {
            highScore = true;
        }

        if (highScore && !nameInputActive) {
            nameInputActive = true;
            memset(playerName, 0, MAX_NAME_LENGTH);
            nameLetterCount = 0;
        }

        if (nameInputActive) {
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

            if (IsKeyPressed(KEY_ENTER) && nameLetterCount > 0) {
                nameInputActive = false;
                atualizarScore(2, pipePassed, playerName); // 2 para AVOID WALLS
                salvarScores("scores.dat");
                currentState = GAMES_MENU;
            }
        } else {
            if (IsKeyPressed(KEY_ENTER)) {
                atualizarScore(2, pipePassed, "Anonimo"); 
                salvarScores("scores.dat");
                currentState = GAMES_MENU;
            }
        }
        return;
    }

    if (!avoidStarted) {
        avoidCountdown -= GetFrameTime();
        if (avoidCountdown <= 0) {
            avoidStarted = true;
            pipePassed = 0;
        }
        return;
    }

    if (IsKeyPressed(KEY_SPACE)) {
        avoidVelocity = avoidJumpForce;
        PlaySound(flapSound);
        SetSoundVolume(flapSound,0.5f);
    }

    avoidVelocity += avoidGravity * GetFrameTime();
    avoidPlayer.y += avoidVelocity * GetFrameTime();

    for (int i = 0; i < MAX_WALLS; i++) {
        walls[i].x -= wallSpeed * GetFrameTime();

        if (!walls[i].passed && (walls[i].x + WALL_WIDTH) < avoidPlayer.x) {
            pipePassed++;
            walls[i].passed = true;
            PlaySound(pointSound);
            SetSoundVolume(pointSound,0.05f);
        }
        
        if (pipePassed - lastPipeCheckpoint >= 5) { //aumentando em cada 5 canos
            wallSpeed += 15;
            if (wallSpeed > 500) wallSpeed = 500; //velocidade maxima = 500
            lastPipeCheckpoint = pipePassed;
        }

        if (walls[i].x + WALL_WIDTH < 0) {
            float maxX = 0;
            for (int j = 0; j < MAX_WALLS; j++) {
                if (walls[j].x > maxX) maxX = walls[j].x;
            }

            float spacing = MIN_SPACING + (rand() % 201);
            walls[i].x = maxX + spacing;
            walls[i].gapY = 100 + rand() % (SCREEN_HEIGHT - WALL_GAP - 100);
            walls[i].passed = false;
        }

        Rectangle topWall = { walls[i].x, 0, WALL_WIDTH, walls[i].gapY };
        Rectangle bottomWall = { walls[i].x, walls[i].gapY + WALL_GAP, WALL_WIDTH, SCREEN_HEIGHT - (walls[i].gapY + WALL_GAP) };

        if (CheckCollisionRecs(avoidPlayer, topWall) || CheckCollisionRecs(avoidPlayer, bottomWall) ||
            avoidPlayer.y < 0 || avoidPlayer.y + avoidPlayer.height > SCREEN_HEIGHT) {
            avoidGameOver = true;
            if (pipePassed > pipePassedRecord) pipePassedRecord = pipePassed;
            PlaySound(gameOverSound);
            SetSoundVolume(gameOverSound,0.15f);
        }
    }
}

void AvoidDraw(void) {
    Rectangle source = { 0, 0, (float)background.width, (float)background.height };
    Rectangle dest = { SCREEN_WIDTH/2 - (background.width / 4.0f), SCREEN_HEIGHT/2 - (background.height / 4.0f), background.width / 2.0f, background.height / 2.0f };
    Vector2 origin = { 0, 0 };

    DrawTexturePro(background, source, dest, origin, 0.0f, WHITE);

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
                    (Rectangle){0, 0, pipeTexture.width, -pipeTexture.height},
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

            DrawTextureEx(birdTexture, (Vector2){avoidPlayer.x, avoidPlayer.y}, 0.0f, 0.3f, WHITE);
            DrawText(TextFormat("Pontuação: %d", pipePassed), 20, 20, 20, YELLOW);
        }
    } else {
        DrawRectangle(0, SCREEN_HEIGHT/2 - 100, SCREEN_WIDTH, 250, Fade(BLACK, 0.5f));
        DrawRectangleLines(0, SCREEN_HEIGHT/2 - 100, SCREEN_WIDTH, 250, GOLD);

        DrawText("GAME OVER", SCREEN_WIDTH/2 - MeasureText("GAME OVER", 60)/2, SCREEN_HEIGHT/2 - 70, 60, RED);
        
        DrawText(TextFormat("Pontuação: %d", pipePassed), SCREEN_WIDTH/2 - 90, SCREEN_HEIGHT/2, 30, WHITE);

        bool highScore = false;
        if (highScores[2].count < MAX_SCORES || pipePassed > highScores[2].entries[highScores[2].count-1].score) {
            highScore = true;
        }

        if (highScore) {
            if (nameInputActive) {
                DrawText("NOVO HIGH SCORE!", SCREEN_WIDTH/2 - MeasureText("NOVO HIGH SCORE!", 30)/2, SCREEN_HEIGHT/2 + 40, 30, GOLD);

                DrawText("Digite seu nome:", SCREEN_WIDTH/2 - MeasureText("Digite seu nome:", 20)/2, SCREEN_HEIGHT/2 + 80, 20, WHITE);
                DrawRectangle(SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 + 110, 300, 40, Fade(WHITE, 0.3f));
                DrawRectangleLines(SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 + 110, 300, 40, GOLD);
                DrawText(playerName, SCREEN_WIDTH/2 - MeasureText(playerName, 25)/2, SCREEN_HEIGHT/2 + 120, 25, WHITE);
                
                DrawText("Pressione ENTER para confirmar", SCREEN_WIDTH/2 - MeasureText("Pressione ENTER para confirmar", 20)/2, SCREEN_HEIGHT/2 + 170, 20, WHITE);
            }
        } else {
            DrawText("Pressione ENTER para voltar ao menu", 
                    SCREEN_WIDTH/2 - MeasureText("Pressione ENTER para voltar ao menu", 20)/2, 
                    SCREEN_HEIGHT/2 + 50, 20, WHITE);
        }
    }
}

bool AvoidIsGameOver(void) {
    return avoidGameOver;
}

int AvoidRecord(void) {
    return pipePassedRecord;
}

int GetAvoidScore(void) {
    return pipePassed; 
}

void AvoidUnload(void) {
    UnloadTexture(background);
    UnloadTexture(birdTexture);
    UnloadSound(pointSound);
    UnloadSound(flapSound);
    UnloadSound(gameOverSound);
    UnloadTexture(pipeTexture);
}