#include "astro_dodge.h"
#include <stdlib.h>

void InitAstroDodge(AstroDodgeGame *game) {
    game->player = (Rectangle){ SCREEN_WIDTH/2.0f - 20, SCREEN_HEIGHT - 60, 40, 40 };
    game->playerSpeed = 10.0f;
    
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        game->asteroids[i].position = (Vector2){ rand() % SCREEN_WIDTH, (float)(-(rand() % 600)) };
        game->asteroids[i].speed = 5 + rand() % 2;
        game->asteroids[i].active = true;
    }

    for (int i = 0; i < MAX_BULLETS; i++) {
        game->bullets[i].active = false;
    }

    game->gameOver = false;
    game->survivalTime = 0.0f;
    game->hitSound = LoadSound("assets/hit.wav");
    game->startCountdown = 5.0f;
    game->gameStarted = false;
}

void UpdateAstroDodge(AstroDodgeGame *game) {
    if (!game->gameStarted) {
        game->startCountdown -= GetFrameTime();
        if (game->startCountdown <= 0) {
            game->gameStarted = true;
        }
        return;
    }
    
    if (!game->gameOver) {
        game->survivalTime += GetFrameTime();
        
        // Controles do jogador
        if (IsKeyDown(KEY_LEFT) && game->player.x > 0) game->player.x -= game->playerSpeed;
        if (IsKeyDown(KEY_RIGHT) && game->player.x + game->player.width < SCREEN_WIDTH) game->player.x += game->playerSpeed;
        if (IsKeyDown(KEY_UP) && game->player.y > 0) game->player.y -= game->playerSpeed;
        if (IsKeyDown(KEY_DOWN) && game->player.y + game->player.height < SCREEN_HEIGHT) game->player.y += game->playerSpeed;

        // Disparar projéteis
        if (IsKeyPressed(KEY_SPACE)) {
            for (int i = 0; i < MAX_BULLETS; i++) {
                if (!game->bullets[i].active) {
                    game->bullets[i].active = true;
                    game->bullets[i].position = (Vector2){
                        game->player.x + game->player.width / 2,
                        game->player.y
                    };
                    break;
                }
            }
        }

        // Atualizar asteroides
        for (int i = 0; i < MAX_ASTEROIDS; i++) {
            if (game->asteroids[i].active) {
                game->asteroids[i].position.y += game->asteroids[i].speed;
                if (game->asteroids[i].position.y > SCREEN_HEIGHT) {
                    game->asteroids[i].position.y = -10;
                    game->asteroids[i].position.x = rand() % SCREEN_WIDTH;
                    game->asteroids[i].speed = 10 + rand() % 6;
                }
                
                Rectangle asteroidRect = { 
                    game->asteroids[i].position.x, 
                    game->asteroids[i].position.y, 
                    30, 30 
                };
                
                if (CheckCollisionRecs(game->player, asteroidRect)) {
                    PlaySound(game->hitSound);
                    game->gameOver = true;
                    if (game->survivalTime > game->bestTime) {
                        game->bestTime = game->survivalTime;
                    }
                }
            }
        }

        // Atualizar projéteis
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (game->bullets[i].active) {
                game->bullets[i].position.y -= BULLET_SPEED * GetFrameTime();

                if (game->bullets[i].position.y < 0) {
                    game->bullets[i].active = false;
                }

                for (int j = 0; j < MAX_ASTEROIDS; j++) {
                    if (game->asteroids[j].active) {
                        Rectangle asteroidRect = { 
                            game->asteroids[j].position.x, 
                            game->asteroids[j].position.y, 
                            30, 30 
                        };
                        
                        if (CheckCollisionCircleRec(game->bullets[i].position, 5, asteroidRect)) {
                            game->asteroids[j].active = false;
                            game->bullets[i].active = false;
                            break;
                        }
                    }
                }
            }
        }
    }
}

void DrawAstroDodge(AstroDodgeGame *game, Texture2D shipTexture, Texture2D asteroidTexture) {
    // Contagem regressiva inicial
    if (!game->gameStarted) {
        int countDisplay = (int)game->startCountdown + 1;
        if (countDisplay > 0) {
            DrawText(TextFormat("%d", countDisplay), SCREEN_WIDTH/2 - 20, SCREEN_HEIGHT/2 - 40, 80, WHITE);
        } else {
            DrawText("Vai!", SCREEN_WIDTH/2 - 40, SCREEN_HEIGHT/2 - 40, 80, WHITE);
        }
        return;
    }

    // Desenhar nave
    DrawTexturePro(
        shipTexture,
        (Rectangle){ 0, 0, (float)shipTexture.width, (float)shipTexture.height },
        (Rectangle){ game->player.x, game->player.y, game->player.width, game->player.height },
        (Vector2){ 0, 0 },
        0.0f,
        WHITE
    );

    // Desenhar projéteis
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (game->bullets[i].active) {
            DrawCircleV(game->bullets[i].position, 5, YELLOW);
        }
    }

    // Desenhar asteroides
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (game->asteroids[i].active) {
            DrawTextureEx(
                asteroidTexture, 
                game->asteroids[i].position, 
                0.0f, 
                30.0f / asteroidTexture.width, 
                WHITE
            );
        }
    }

    // Desenhar HUD
    DrawText(TextFormat("Tempo: %.2f", game->survivalTime), 10, 10, 20, WHITE);
    DrawText(TextFormat("Melhor: %.2f", game->bestTime), 10, 40, 20, YELLOW);

    // Tela de Game Over
    if (game->gameOver) {
        DrawText("Game Over!", SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 - 20, 40, YELLOW);
        DrawText("Pressione ENTER para voltar ao menu", SCREEN_WIDTH/2 - 180, SCREEN_HEIGHT/2 + 30, 20, GRAY);
    }
}

void RunAstroDodgeGame(GameState *state, Texture2D shipTexture, Texture2D asteroidTexture) {
    static AstroDodgeGame game;
    static bool initialized = false;

    if (!initialized) {
        InitAstroDodge(&game);
        initialized = true;
    }

    BeginDrawing();
    ClearBackground(BLACK);
    
    UpdateAstroDodge(&game);
    DrawAstroDodge(&game, shipTexture, asteroidTexture);
    
    if (game.gameOver && IsKeyPressed(KEY_ENTER)) {
        initialized = false;
        UnloadSound(game.hitSound);
        *state = MENU;
    }

    if (IsKeyPressed(KEY_R)) {
        initialized = false;
        UnloadSound(game.hitSound);
    }

    if (IsKeyPressed(KEY_M)) {
        initialized = false;
        UnloadSound(game.hitSound);
        *state = MENU;
    }
    
    EndDrawing();
}