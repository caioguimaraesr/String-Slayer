#include "avoid.h"
#include <stdlib.h>

void InitAvoidGame(AvoidGame *game) {
    game->player = (Rectangle){ 100, SCREEN_HEIGHT / 2 - 25, 50, 50 };
    game->velocity = 0.0f;
    game->wallSpacing = SCREEN_WIDTH / MAX_WALLS;
    game->wallSpeed = 200.0f;
    game->gameOver = false;
    game->time = 0.0f;
    game->bestTime = 0.0f;
    game->countdown = 3.0f;
    game->started = false;
    game->jumpForce = -350.0f;
    game->gravity = 800.0f;

    for (int i = 0; i < MAX_WALLS; i++) {
        game->walls[i].x = SCREEN_WIDTH + i * game->wallSpacing;
        game->walls[i].gapY = 100 + rand() % (SCREEN_HEIGHT - WALL_GAP - 100);
        game->walls[i].passed = false;
    }
}

void UpdateAvoidGame(AvoidGame *game) {
    if (!game->started) {
        game->countdown -= GetFrameTime();
        if (game->countdown <= 0) {
            game->started = true;
        }
        return;
    }

    if (!game->gameOver) {
        game->time += GetFrameTime();

        if (IsKeyPressed(KEY_SPACE)) {
            game->velocity = game->jumpForce;
        }

        game->velocity += game->gravity * GetFrameTime();
        game->player.y += game->velocity * GetFrameTime();

        for (int i = 0; i < MAX_WALLS; i++) {
            game->walls[i].x -= game->wallSpeed * GetFrameTime();

            if (game->walls[i].x + WALL_WIDTH < 0) {
                game->walls[i].x = SCREEN_WIDTH + game->wallSpacing;
                game->walls[i].gapY = 100 + rand() % (SCREEN_HEIGHT - WALL_GAP - 100);
                game->walls[i].passed = false;
            }

            Rectangle topWall = { game->walls[i].x, 0, WALL_WIDTH, game->walls[i].gapY };
            Rectangle bottomWall = { game->walls[i].x, game->walls[i].gapY + WALL_GAP, WALL_WIDTH, 
                                  SCREEN_HEIGHT - (game->walls[i].gapY + WALL_GAP) };

            if (CheckCollisionRecs(game->player, topWall) || 
                CheckCollisionRecs(game->player, bottomWall) ||
                game->player.y < 0 || 
                game->player.y + game->player.height > SCREEN_HEIGHT) {
                game->gameOver = true;
                if (game->time > game->bestTime) {
                    game->bestTime = game->time;
                }
            }
        }
    }
}

void DrawAvoidGame(AvoidGame *game, Texture2D birdTexture, Texture2D pipeTexture, Texture2D background) {
    DrawTexture(background, 0, 0, WHITE);
    ClearBackground(DARKBLUE);

    if (!game->started) {
        int countDisplay = (int)game->countdown + 1;
        if (countDisplay > 0) {
            DrawText(TextFormat("%d", countDisplay), SCREEN_WIDTH/2 - 20, SCREEN_HEIGHT/2 - 40, 80, WHITE);
        } else {
            DrawText("Vai!", SCREEN_WIDTH/2 - 40, SCREEN_HEIGHT/2 - 40, 80, WHITE);
        }
        return;
    }

    for (int i = 0; i < MAX_WALLS; i++) {
        Rectangle topWall = { game->walls[i].x, 0, WALL_WIDTH, game->walls[i].gapY };
        Rectangle bottomWall = { game->walls[i].x, game->walls[i].gapY + WALL_GAP, 
                               WALL_WIDTH, SCREEN_HEIGHT - (game->walls[i].gapY + WALL_GAP) };

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

    float scale = 0.5f;
    DrawTextureEx(birdTexture, (Vector2){game->player.x, game->player.y}, 0.0f, scale, WHITE);
    
    DrawText(TextFormat("Tempo: %.2f", game->time), 20, 20, 20, WHITE);
    DrawText(TextFormat("Melhor: %.2f", game->bestTime), 20, 50, 20, YELLOW);

    if (game->gameOver) {
        DrawText("Game Over!", SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 - 30, 40, RED);
        DrawText("Pressione ENTER para voltar ao menu", SCREEN_WIDTH/2 - 180, SCREEN_HEIGHT/2 + 10, 20, LIGHTGRAY);
    }
}

void RunAvoidWalls(GameState *state, Texture2D birdTexture, Texture2D pipeTexture, Texture2D background) {
    static AvoidGame game;
    static bool initialized = false;

    if (!initialized) {
        InitAvoidGame(&game);
        initialized = true;
    }

    BeginDrawing();
    
    UpdateAvoidGame(&game);
    DrawAvoidGame(&game, birdTexture, pipeTexture, background);
    
    if (game.gameOver && IsKeyPressed(KEY_ENTER)) {
        initialized = false;
        *state = MENU;
    }
    
    EndDrawing();
}