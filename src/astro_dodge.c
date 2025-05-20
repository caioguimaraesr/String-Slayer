#include "astro_dodge.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static Rectangle player;
static float playerSpeed;
static Bullet bullets[MAX_BULLETS];
static Asteroid asteroids[MAX_ASTEROIDS];
static bool gameOver = false;
static float survivalTime = 0.0f;
static float bestTime = 0.0f;
static float startCountdown = 3.0f;
static bool gameStarted = false;
static int score = 0;
static Sound hitSound;
static Sound shootSound;
static Texture2D shipTexture;
static Texture2D asteroidTexture;
static int ammo = 15; 
static Rectangle ammoPickup;
static bool ammoPickupActive = false;
static float ammoSpawnTimer = 0.0f;
static Texture2D ammoTexture; 
static bool returnToMenu = false;
static AmmoDrop ammoDrops[MAX_AMMO_DROPS];
static float ammoDropTimer = 0.0f; 
static const float bulletSpeed = 500.0f;

void InitAstroDodge(Texture2D shipTex, Texture2D asteroidTex) {
    shipTexture = shipTex;
    asteroidTexture = asteroidTex;
    ammoTexture = LoadTexture("assets/images/municao.png");
    player = (Rectangle){ SCREEN_WIDTH/2.0f - 20, SCREEN_HEIGHT - 60, 40, 40 };
    playerSpeed = 5.0f;
    survivalTime = 0.0f;
    startCountdown = 5.0f;
    gameStarted = false;
    gameOver = false;
    returnToMenu = false;
    ammo = 15;
    ammoSpawnTimer = 0.0f;
    ammoPickupActive = false;
    ammoPickup = (Rectangle){ 0, 0, 30, 30 };
    score = 0;
    hitSound = LoadSound("assets/music/Hit-Astro.wav");
    shootSound = LoadSound("assets/music/Tiro-Astro.wav");
    SetSoundVolume(hitSound,0.3f);
    SetSoundVolume(shootSound,0.3f);

    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        asteroids[i].position = (Vector2){ rand() % SCREEN_WIDTH, (float)(-(rand() % 600)) };
        asteroids[i].speed = 6 + rand() % 2;
        asteroids[i].active = true;
    }

    for (int i = 0; i < MAX_BULLETS; i++) {
        bullets[i].active = false;
    }
    for (int i = 0; i < MAX_AMMO_DROPS; i++) {
        ammoDrops[i].active = false;
        ammoDrops[i].position = (Vector2){0, 0};
        ammoDrops[i].speed = 6 + rand() % 2; 
    }
    ammoDropTimer = 0.0f;
}

void UpdateAstroDodge(void) {
    if (!gameStarted) {
        startCountdown -= GetFrameTime();
        if (startCountdown <= 0) {
            gameStarted = true;
        }
        return;
    }

    if (gameOver) {
        if (IsKeyPressed(KEY_ENTER)) {
            returnToMenu = true;
        }
        return;
    }

    survivalTime += GetFrameTime();

    if (IsKeyDown(KEY_LEFT) && player.x > 0) player.x -= playerSpeed;
    if (IsKeyDown(KEY_RIGHT) && player.x + player.width < SCREEN_WIDTH) player.x += playerSpeed;
    if (IsKeyDown(KEY_UP) && player.y > 0) player.y -= playerSpeed;
    if (IsKeyDown(KEY_DOWN) && player.y + player.height < SCREEN_HEIGHT) player.y += playerSpeed;

    if (IsKeyPressed(KEY_SPACE) && ammo > 0) {
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (!bullets[i].active) {
                bullets[i].active = true;
                bullets[i].position = (Vector2){ player.x + player.width / 2, player.y };
                PlaySound(shootSound);
                ammo--;
                break;
            }
        }
    }

    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (asteroids[i].active) {
            asteroids[i].position.y += asteroids[i].speed;
            if (asteroids[i].position.y > SCREEN_HEIGHT) {
                asteroids[i].position.y = -10;
                asteroids[i].position.x = rand() % SCREEN_WIDTH;
                asteroids[i].speed = 8 + rand() % 6;
            }

            Rectangle asteroidRect = { asteroids[i].position.x, asteroids[i].position.y, 30, 30 };
            if (CheckCollisionRecs(player, asteroidRect)) {
                PlaySound(hitSound);
                gameOver = true;
                if (survivalTime > bestTime) {
                    bestTime = survivalTime;
                }
            }
        }
    }


    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            bullets[i].position.y -= bulletSpeed * GetFrameTime();
            if (bullets[i].position.y < 0) bullets[i].active = false;

            for (int j = 0; j < MAX_ASTEROIDS; j++) {
                if (asteroids[j].active) {
                    Rectangle asteroidRect = { asteroids[j].position.x, asteroids[j].position.y, 30, 30 };
                    if (CheckCollisionCircleRec(bullets[i].position, 5, asteroidRect)) {
                        asteroids[j].position = (Vector2){ rand() % SCREEN_WIDTH, (float)(-(rand() % 600)) };
                        asteroids[j].speed = 5 + rand() % 2;
                        asteroids[j].active = true;
                        bullets[i].active = false;
                        score += 20;
                        break;
                    }
                }
            }
        }
    }

    ammoDropTimer += GetFrameTime();
    if (ammoDropTimer >= 10.0f) {
        ammoDropTimer = 0.0f;
        for (int i = 0; i < MAX_AMMO_DROPS; i++) {
            if (!ammoDrops[i].active) {
                ammoDrops[i].active = true;
                ammoDrops[i].position.x = rand() % (SCREEN_WIDTH - 30);
                ammoDrops[i].position.y = -30; 
                ammoDrops[i].speed = 80 + rand() % 40; 
                break;
            }
        }
    }


    for (int i = 0; i < MAX_AMMO_DROPS; i++) {
        if (ammoDrops[i].active) {
            ammoDrops[i].position.y += ammoDrops[i].speed * GetFrameTime();
            if (ammoDrops[i].position.y > SCREEN_HEIGHT) {
                ammoDrops[i].active = false;  
            }

            Rectangle ammoRect = { ammoDrops[i].position.x, ammoDrops[i].position.y, 30, 30 };
            if (CheckCollisionRecs(player, ammoRect)) {
                ammoDrops[i].active = false;
                ammo += 10;  
                PlaySound(shootSound); 
            }
        }
    }
}

void DrawAstroDodge(void) {
    ClearBackground(BLACK);
    if (!gameStarted) {
        int countDisplay = (int)startCountdown + 1;
        if (countDisplay > 0)
            DrawText(TextFormat("%d", countDisplay), SCREEN_WIDTH/2 - 20, SCREEN_HEIGHT/2 - 40, 80, WHITE);
        else
            DrawText("Vai!", SCREEN_WIDTH/2 - 40, SCREEN_HEIGHT/2 - 40, 80, WHITE);
        return;
    }

    DrawTexturePro(shipTexture, (Rectangle){ 0, 0, shipTexture.width, shipTexture.height },
    (Rectangle){ player.x, player.y, player.width, player.height },
    (Vector2){ 0, 0 }, 0.0f, WHITE);

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active)
            DrawCircleV(bullets[i].position, 5, YELLOW);
    }

    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (asteroids[i].active)
            DrawTextureEx(asteroidTexture, asteroids[i].position, 0.0f, 30.0f / asteroidTexture.width, WHITE);
    }

    DrawText(TextFormat("Tempo: %.2f", survivalTime), 10, 10, 20, WHITE);
    DrawText(TextFormat("Melhor: %.2f", bestTime), 10, 40, 20, YELLOW);
    const int fontSize = 20;
    const int spacing = 30;
    DrawText(TextFormat("Score: %d", score), SCREEN_WIDTH - 150, 10, fontSize, YELLOW);
    DrawText(TextFormat("Munição: %d", ammo), SCREEN_WIDTH - 150, 10 + spacing, fontSize, WHITE);

    if (gameOver) {
        DrawText("Game Over!", SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 - 20, 40, YELLOW);
        DrawText("Pressione ENTER para voltar ao menu", SCREEN_WIDTH/2 - 180, SCREEN_HEIGHT/2 + 30, 20, GRAY);
    }
    for (int i = 0; i < MAX_AMMO_DROPS; i++) {
        if (ammoDrops[i].active) {
            DrawTexture(ammoTexture, ammoDrops[i].position.x, ammoDrops[i].position.y, WHITE);
        }
    }
}

void RestartAstroDodge(void) {
    UnloadSound(hitSound);
    UnloadSound(shootSound);
    InitAstroDodge(shipTexture, asteroidTexture);
}

void UnloadAstroDodge(void) {
    UnloadSound(hitSound);
    UnloadSound(shootSound);
    UnloadTexture(ammoTexture);
}

bool IsAstroGameOver(void) {
    return gameOver;
}

bool AstroWantsToReturnToMenu(void) {
    return returnToMenu || IsKeyPressed(KEY_M);
}
