#include "raylib.h"
#include "pong.h"
#include "astro_dodge.h"
#include "avoid.h"
#include "globals.h"
#include <stdlib.h>
#include <time.h>

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "String Slayer");
    InitAudioDevice();
    srand(time(NULL));

    // Texturas e música
    Texture2D shipTexture = LoadTexture("assets/images/nave_espacial.png");
    Texture2D asteroidTexture = LoadTexture("assets/images/asteroid.png");
    Texture2D menuBackground = LoadTexture("assets/images/String Slayer - Background.png");
    Texture2D birdTexture = LoadTexture("assets/images/Flávio Caça-Rato.png");
    Texture2D background = LoadTexture("assets/images/Fundo Avoid the Walls.png");
    Texture2D pipeTexture = LoadTexture("assets/images/pipe.png");

    Music music = LoadMusicStream("assets/music/musica-errada.ogg");
    Music astroMusic = LoadMusicStream("assets/music/astro.ogg");

    PlayMusicStream(music);
    SetMusicVolume(music, 1.0f);
    PlayMusicStream(astroMusic);
    PauseMusicStream(astroMusic); // apenas toca depois

    SetTargetFPS(60);

    GameState currentState = MENU;
    bool astroMusicPlaying = false;

    const char *menuItems[] = { "Jogar Pong", "Astro Dodge", "Avoid the Walls", "Sair" };
    int selectedOption = 0;
    int menuItemsCount = sizeof(menuItems) / sizeof(menuItems[0]);

    while (!WindowShouldClose()) {
        // Música
        if (currentState == MENU) {
            UpdateMusicStream(music);
            if (!IsMusicStreamPlaying(music)) PlayMusicStream(music);
            astroMusicPlaying = false;
        } else if (currentState == ASTRO_DODGE) {
            UpdateMusicStream(astroMusic);
            if (!astroMusicPlaying) {
                StopMusicStream(music);
                PlayMusicStream(astroMusic);
                astroMusicPlaying = true;
            }
        }

        BeginDrawing();

        if (currentState == MENU) {
            Rectangle source = { 0, 0, (float)menuBackground.width, (float)menuBackground.height };
            Rectangle dest = { 0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
            Vector2 origin = { 0, 0 };

            DrawTexturePro(menuBackground, source, dest, origin, 0.0f, WHITE);
            ClearBackground(BLACK);

            // Menu
            if (IsKeyPressed(KEY_DOWN)) selectedOption = (selectedOption + 1) % menuItemsCount;
            if (IsKeyPressed(KEY_UP)) selectedOption = (selectedOption - 1 + menuItemsCount) % menuItemsCount;

            if (IsKeyPressed(KEY_ENTER)) {
                switch (selectedOption) {
                    case 0:
                        currentState = PONG;  // Mude GAME para PONG
                        break;
                    case 1:
                        currentState = ASTRO_DODGE;
                        break;
                    case 2:
                        currentState = AVOID_WALLS;
                        break;
                    case 3:
                        CloseWindow();
                        return 0;
                }
            }

            for (int i = 0; i < menuItemsCount; i++) {
                Color color = (i == selectedOption) ? YELLOW : WHITE;
                int fontSize = 30;
                int textWidth = MeasureText(menuItems[i], fontSize);
                DrawText(menuItems[i], SCREEN_WIDTH / 2 - textWidth / 2, 270 + i * 40, fontSize, color);
            }
        } else if (currentState == PONG) {  // Mude GAME para PONG
            RunPongGame(&currentState);
        } else if (currentState == ASTRO_DODGE) {
            RunAstroDodgeGame(&currentState, shipTexture, asteroidTexture);
        } else if (currentState == AVOID_WALLS) {
            RunAvoidWalls(&currentState, birdTexture, pipeTexture, background);
        }

        EndDrawing();
    }

    // Libera recursos
    UnloadTexture(shipTexture);
    UnloadTexture(asteroidTexture);
    UnloadTexture(pipeTexture);
    UnloadTexture(background);
    UnloadTexture(birdTexture);
    UnloadTexture(menuBackground);
    UnloadMusicStream(music);
    UnloadMusicStream(astroMusic);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}