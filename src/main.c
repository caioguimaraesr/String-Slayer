#include "raylib.h"
#include "pong.h"
#include "astro_dodge.h"
#include "avoid_walls.h"
#include "game.h"
#include <stdlib.h>
#include <time.h>

// == AVOID THE WALLS ==
Rectangle avoidPlayer; 
float avoidVelocity = 0;
float avoidGravity = 600;
float avoidJumpForce = -300;
Wall walls[MAX_WALLS]; 
float wallSpeed = 200;
int wallSpacing = 300;
bool avoidGameOver = false;
float avoidCountdown = 5.0f;
bool avoidInitialized = false;

// == ASTRO DODGE ==
int asteroidsDestroyed = 0;
int asteroidsToActivate = 5;
Bullet bullets[MAX_BULLETS];
float bulletSpeed = 500.0f;

GameState currentState = MENU;


int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "String Slayer");
    Texture2D shipTexture = LoadTexture("assets/images/nave_espacial.png");
    Texture2D asteroidTexture = LoadTexture("assets/images/asteroid.png");
    Texture2D menuBackground = LoadTexture("assets/images/String Slayer - Background.png");
    Texture2D birdTexture = LoadTexture("assets/images/Flávio Caça-Rato.png");
    Texture2D background = LoadTexture("assets/images/Fundo-avoid.jpg");
    Texture2D pipeTexture = LoadTexture("assets/images/pipe.png");
    Texture2D menuGameBackground = LoadTexture("assets/images/Photo.png");

    InitAudioDevice();
    srand(time(NULL));

    Music music = LoadMusicStream("assets/music/musica-errada.wav");
    Music astroMusic = LoadMusicStream("assets/music/astro.ogg");
    Sound pongBar = LoadSound("assets/music/Barra.wav");
    Sound pongPoint = LoadSound("assets/music/pontoPong.wav");
    Sound pongRebound = LoadSound("assets/music/Rebound.wav");


    PlayMusicStream(astroMusic);
    SetMusicVolume(astroMusic,0.3f);
    SetSoundVolume(pongPoint,0.05f);
    SetSoundVolume(pongBar,0.05f);
    SetSoundVolume(pongRebound,0.05f);

    SetTargetFPS(60);
    
    bool astroMusicPlaying = false;
    const char *menuItems[] = { "PLAY", "COMANDS", "EXIT" };
    int mainMenuItemsCount = sizeof(menuItems) / sizeof(menuItems[0]);

    const char *gameMenuItems[] = { "PONG", "ASTRO DODGE", "AVOID THE WALLS", "BACK" };
    int gameMenuItemsCount = sizeof(gameMenuItems) / sizeof(gameMenuItems[0]);

    int selectedOption = 0;
    
    while (!WindowShouldClose()) {
        // Atualizar a música apropriada
        if (currentState == MENU) {
            UpdateMusicStream(music);

            if (!IsMusicStreamPlaying(music)) PlayMusicStream(music);
            astroMusicPlaying = false;
        } 
        else if (currentState == ASTRO_DODGE) {
            UpdateMusicStream(astroMusic);

            if (!astroMusicPlaying) {
                StopMusicStream(music);
                PlayMusicStream(astroMusic);
                astroMusicPlaying = true;
            }
        }
        BeginDrawing();
        UpdateMusicStream(music);

        if (currentState == MENU) {
            Rectangle source = { 0, 0, (float)menuBackground.width, (float)menuBackground.height };
            Rectangle dest = { 0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
            Vector2 origin = { 0, 0 };

            DrawTexturePro(menuBackground, source, dest, origin, 0.0f, WHITE);
        }

        ClearBackground(BLACK);

       if (currentState == MENU) {
            Rectangle source = { 0, 0, (float)menuBackground.width, (float)menuBackground.height };
            Rectangle dest = { 0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
            Vector2 origin = { 0, 0 };

            DrawTexturePro(menuBackground, source, dest, origin, 0.0f, WHITE);

            ClearBackground(BLACK);

            // Navegação
            if (IsKeyPressed(KEY_DOWN)) selectedOption = (selectedOption + 1) % mainMenuItemsCount;
            if (IsKeyPressed(KEY_UP)) selectedOption = (selectedOption - 1 + mainMenuItemsCount) % mainMenuItemsCount;

            if (IsKeyPressed(KEY_ENTER)) {
                if (selectedOption == 0) {
                    currentState = GAMES_MENU;
                    selectedOption = 0;
                } else if (selectedOption == 1) {
                    currentState = COMMANDS;
                } else if (selectedOption == 2) {
                    break;
                }
            }

            // Botão destacado: "Play"
            int playFontSize = 60;
            const char* playText = menuItems[0];
            int playTextWidth = MeasureText(playText, playFontSize);
            int playX = SCREEN_WIDTH / 2 - playTextWidth / 2;
            int playY = 420;  // Mais abaixo que antes

            // Retângulo de fundo arredondado
            int padding = 30;
            DrawRectangleRounded((Rectangle){
                playX - padding/2, playY - 10, playTextWidth + padding, playFontSize + 20
            }, 0.4f, 10, Fade(DARKGRAY, 0.5f));

            // Texto
            Color playColor = (selectedOption == 0) ? ORANGE : WHITE;
            DrawText(playText, playX, playY, playFontSize, playColor);

            // Se quiser mostrar os outros itens menores (opcional):
            int fontSize = 25;
            for (int i = 1; i < mainMenuItemsCount; i++) {
                Color color = (i == selectedOption) ? YELLOW : LIGHTGRAY;
                int textWidth = MeasureText(menuItems[i], fontSize);
                int x = SCREEN_WIDTH / 2 - textWidth / 2;
                int y = 500 + (i - 1) * 35;
                DrawText(menuItems[i], x, y, fontSize, color);
            }
        }

        else if (currentState == GAMES_MENU) {
            DrawTexture(menuGameBackground, 0, 0, WHITE); 

            const char *title = "START GAME";
            int titleFontSize = 50;
            int titleWidth = MeasureText(title, titleFontSize);
            DrawText(title, SCREEN_WIDTH/2 - titleWidth/2, 100, titleFontSize, YELLOW);

            for (int i = 0; i < gameMenuItemsCount; i++) {
                bool isSelected = (i == selectedOption);
                Color color = isSelected ? YELLOW : WHITE;
                int fontSize = isSelected ? 35 : 30;

                int textWidth = MeasureText(gameMenuItems[i], fontSize);
                int x = SCREEN_WIDTH / 2 - textWidth / 2;
                int y = 250 + i * 60;

                if (isSelected) {
                    DrawRectangle(x - 20, y - 5, textWidth + 40, fontSize + 10, Fade(DARKGRAY, 0.4f));
                }

                DrawText(gameMenuItems[i], x, y, fontSize, color);
            }

            if (IsKeyPressed(KEY_DOWN)) selectedOption = (selectedOption + 1) % gameMenuItemsCount;
            if (IsKeyPressed(KEY_UP)) selectedOption = (selectedOption - 1 + gameMenuItemsCount) % gameMenuItemsCount;

            if (IsKeyPressed(KEY_ENTER)) {
                if (selectedOption == 0) {
                    pongStarted = false;
                    pongCountdown = 5.0f;
                    for (int r = 0; r < ROWS; r++)
                        for (int c = 0; c < COLS; c++)
                            grid[r][c] = 0;

                    player1.y = SCREEN_HEIGHT/2 - PADDLE_HEIGHT/2;
                    player2.y = SCREEN_HEIGHT/2 - PADDLE_HEIGHT/2;
                    ballPosition = (Vector2){SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f};
                    ballSpeed = (Vector2){BALL_SPEED, BALL_SPEED};
                    score1 = score2 = 0;
                    gameTime = 120.0f;
                    gameEnded = false;
                    FreeBallHistory(history);
                    history = NULL;
                    historyCount = 0;

                    currentState = PONG;
                } else if (selectedOption == 1) {
                    currentState = ASTRO_DODGE;
                } else if (selectedOption == 2) {
                    currentState = AVOID_WALLS;
                } else if (selectedOption == 3) {
                    currentState = MENU;
                    selectedOption = 0;
                }
            }
        }

        // Inicio do Jogo Pong
        else if (currentState == PONG) {
            HandlePointScored();
            DrawCountdown();
            UpdateGame(pongPoint,pongRebound,pongBar);
            DrawGame();
            HandleGameEnd();
        }

        // Inicio do Jogo Astro Dodge
        else if (currentState == ASTRO_DODGE) {
            static bool astroInitialized = false;
            static Rectangle player;
            static float playerSpeed;
            static Asteroid asteroids[MAX_ASTEROIDS];
            static bool gameOver;
            static float survivalTime;
            static Sound hitSound;
            static float bestTime = 0.0f;
            static float startCountdown;
            static bool gameStarted;

            if (!astroInitialized) {
                player = (Rectangle){ SCREEN_WIDTH/2.0f - 20, SCREEN_HEIGHT - 60, 40, 40 };
                playerSpeed = 10.0f;
                for (int i = 0; i < MAX_ASTEROIDS; i++) {
                    asteroids[i].position = (Vector2){ rand() % SCREEN_WIDTH, (float)(-(rand() % 600)) };
                    asteroids[i].speed = 5 + rand() % 2; // Velocidade dos asteroides
                    asteroids[i].active = true;
                }

                for (int i = 0; i < MAX_BULLETS; i++) {
                    bullets[i].active = false;
                }

                gameOver = false;
                survivalTime = 0.0f;
                hitSound = LoadSound("assets/hit.wav");
                astroInitialized = true;
                startCountdown = 5.0f;
                gameStarted = false;
            }

            // TIMER DE INÍCIO ANTES DO ASTRO COMEÇAR
            if (!gameStarted) {
                startCountdown -= GetFrameTime();

                int countDisplay = (int)startCountdown + 1;
                if (countDisplay > 0) {
                    DrawText(TextFormat("%d", countDisplay), SCREEN_WIDTH/2 - 20, SCREEN_HEIGHT/2 - 40, 80, WHITE);
                } else {
                    DrawText("Vai!", SCREEN_WIDTH/2 - 40, SCREEN_HEIGHT/2 - 40, 80, WHITE);
                }

                if (startCountdown <= 0) {
                    gameStarted = true;
                }

                EndDrawing();
                continue;
            }
            
            if (!gameOver) {
                survivalTime += GetFrameTime();
                if (IsKeyDown(KEY_LEFT) && player.x > 0) player.x -= playerSpeed;
                if (IsKeyDown(KEY_RIGHT) && player.x + player.width < SCREEN_WIDTH) player.x += playerSpeed;
                if (IsKeyDown(KEY_UP) && player.y > 0) player.y -= playerSpeed;
                if (IsKeyDown(KEY_DOWN) && player.y + player.height < SCREEN_HEIGHT) player.y += playerSpeed;

                if (IsKeyPressed(KEY_SPACE)) {
                    for (int i = 0; i < MAX_BULLETS; i++) {
                        if (!bullets[i].active) {
                            bullets[i].active = true;
                            bullets[i].position = (Vector2){
                                player.x + player.width / 2,
                                player.y
                            };
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
                            asteroids[i].speed = 10 + rand() % 6;
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

                        if (bullets[i].position.y < 0) {
                            bullets[i].active = false;
                        }

                        for (int j = 0; j < MAX_ASTEROIDS; j++) {
                            if (asteroids[j].active) {
                                Rectangle asteroidRect = { asteroids[j].position.x, asteroids[j].position.y, 30, 30 };
                                if (CheckCollisionCircleRec(bullets[i].position, 5, asteroidRect)) {
                                    asteroids[j].active = false;
                                    bullets[i].active = false;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
                
            DrawTexturePro(
                shipTexture,
                (Rectangle){ 0, 0, (float)shipTexture.width, (float)shipTexture.height },
                (Rectangle){ player.x, player.y, player.width, player.height },
                (Vector2){ 0, 0 },
                0.0f,
                WHITE
            );

            for (int i = 0; i < MAX_BULLETS; i++) {
                if (bullets[i].active) {
                    DrawCircleV(bullets[i].position, 5, YELLOW);
                }
            }

            for (int i = 0; i < MAX_ASTEROIDS; i++) {
                if (asteroids[i].active)
                    DrawTextureEx(asteroidTexture, asteroids[i].position, 0.0f, 30.0f / asteroidTexture.width, WHITE);
            }
            DrawText(TextFormat("Tempo: %.2f", survivalTime), 10, 10, 20, WHITE);
            DrawText(TextFormat("Melhor: %.2f", bestTime), 10, 40, 20, YELLOW);

            if (gameOver) {
                DrawText("Game Over!", SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 - 20, 40, YELLOW);
                DrawText("Pressione ENTER para voltar ao menu", SCREEN_WIDTH/2 - 180, SCREEN_HEIGHT/2 + 30, 20, GRAY);
                if (IsKeyPressed(KEY_ENTER)) {
                    astroInitialized = false;
                    UnloadSound(hitSound);
                    currentState = MENU;
                }
            }

            if (IsKeyPressed(KEY_R)) {
                astroInitialized = false;
                UnloadSound(hitSound);
            }

            if (IsKeyPressed(KEY_M)) {
                astroInitialized = false;
                UnloadSound(hitSound);
                currentState = MENU;
            }
            
        } 

        // Inicio do Jogo Avoid Walls
        else if (currentState == AVOID_WALLS) {
            if (!avoidInitialized) {
                AvoidInit();
                avoidInitialized = true;
            }
            AvoidUpdate();
            AvoidDraw();
        
            if (AvoidIsGameOver() && IsKeyPressed(KEY_ENTER)) {
                AvoidUnload();
                avoidInitialized = false;
                currentState = MENU;
            }
        }
        else if (currentState == COMMANDS) {
            ClearBackground(BLACK);
        
            int y = 50;
            int spacing = 30;
            int sectionSpacing = 50;
            DrawText("Comandos do Jogo:", SCREEN_WIDTH/2 - MeasureText("Comandos do Jogo:", 30)/2, y, 30, YELLOW);
            y += sectionSpacing;
            // PONG
            DrawText("PONG", SCREEN_WIDTH/2 - MeasureText("PONG", 25)/2, y, 25, WHITE);
            y += spacing;
            DrawText("Jogador 1: W (Cima), S (Baixo)", SCREEN_WIDTH/2 - MeasureText("Jogador 1: W (Cima), S (Baixo)", 20)/2, y, 20, LIGHTGRAY);
            y += spacing;
            DrawText("Jogador 2: Setas Cima, Baixo", SCREEN_WIDTH/2 - MeasureText("Jogador 2: Setas Cima, Baixo", 20)/2, y, 20, LIGHTGRAY);
            y += sectionSpacing;
            // ASTRO DODGE
            DrawText("ASTRO DODGE", SCREEN_WIDTH/2 - MeasureText("ASTRO DODGE", 25)/2, y, 25, WHITE);
            y += spacing;
            DrawText("WASD para mover, Espaço para atirar", SCREEN_WIDTH/2 - MeasureText("WASD para mover, Espaço para atirar", 20)/2, y, 20, LIGHTGRAY);
            y += sectionSpacing;
            // AVOID THE WALLS
            DrawText("AVOID THE WALLS", SCREEN_WIDTH/2 - MeasureText("AVOID THE WALLS", 25)/2, y, 25, WHITE);
            y += spacing;
            DrawText("Espaço para pular", SCREEN_WIDTH/2 - MeasureText("Espaço para pular", 20)/2, y, 20, LIGHTGRAY);
            y += sectionSpacing;
            DrawText("Pressione ENTER para voltar", SCREEN_WIDTH/2 - MeasureText("Pressione ENTER para voltar", 20)/2, SCREEN_HEIGHT - 50, 20, GRAY);
        
            if (IsKeyPressed(KEY_ENTER)) {
                currentState = MENU;
            }
        }
        
        
        EndDrawing();
    }

    // Images
    UnloadTexture(pipeTexture);
    UnloadTexture(background);
    UnloadTexture(birdTexture);
    UnloadTexture(asteroidTexture);
    UnloadTexture(shipTexture);
    UnloadTexture(menuGameBackground);

    // Music
    UnloadMusicStream(astroMusic);
    UnloadMusicStream(music);
    UnloadSound(pongPoint);
    UnloadSound(pongBar);
    UnloadSound(pongRebound);
    CloseAudioDevice();
    FreeBallHistory(history);
    CloseWindow();
    return 0;
    }

