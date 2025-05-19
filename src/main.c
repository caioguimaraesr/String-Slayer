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
float avoidTime = 0.0f;
float avoidBestTime = 0.0f;
float avoidCountdown = 5.0f;
bool avoidInitialized = false;
// == PONG ==
bool pointScored = false; 
float pointDelay = 0.0f;
static bool rebound = false;
static bool barHit = false;
int historyCount = 0;
BallPositionNode *history = NULL;

// == ASTRO DODGE ==
int asteroidsDestroyed = 0;
int asteroidsToActivate = 5;
Bullet bullets[MAX_BULLETS];
float bulletSpeed = 500.0f;

// === VARIÁVEIS GLOBAIS ===
int grid[ROWS][COLS] = {0};
float gameTime = 120.0f;
bool gameEnded = false;
GameState currentState = MENU;

// === FUNÇÕES DO PONG ===
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

void FreeBallHistory(BallPositionNode *head) {
    while (head != NULL) {
        BallPositionNode *temp = head;
        head = head->next;
        free(temp);
    }
}

void ResetBallHistory() {
    FreeBallHistory(history);
    history = NULL;
    historyCount = 0;
}

void ResetGrid() {
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            grid[r][c] = 0;
        }
    }
}

// == FUNÇÕES DO AVOID ==
// void ResetAvoidGame() {
//     avoidPlayer = (Rectangle){ 100, SCREEN_HEIGHT/2 - 25, 40, 40};
//     avoidVelocity = 0;
//     avoidGameOver = false;
//     avoidStarted = false;
//     avoidCountdown = 5.0f;
//     avoidTime = 0.0f;

//     for (int i = 0; i < MAX_WALLS; i++) {
//         walls[i].x = SCREEN_WIDTH + i * wallSpacing;
//         walls[i].gapY = 100 + rand() % (SCREEN_HEIGHT - WALL_GAP - 100);
//         walls[i].passed = false;
//     }
// }

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "String Slayer");
    Texture2D shipTexture = LoadTexture("assets/images/nave_espacial.png");
    Texture2D asteroidTexture = LoadTexture("assets/images/asteroid.png");
    Texture2D menuBackground = LoadTexture("assets/images/String Slayer - Background.png");
    Texture2D birdTexture = LoadTexture("assets/images/Flávio Caça-Rato.png");
    Texture2D background = LoadTexture("assets/images/Fundo Avoid the Walls.png");
    Texture2D pipeTexture = LoadTexture("assets/images/pipe.png");
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
    const char *menuItems[] = { "Jogar Pong", "Astro Dodge", "Avoid the Walls", "Comandos", "Sair" };
    int selectedOption = 0;
    int menuItemsCount = sizeof(menuItems) / sizeof(menuItems[0]);
    bool pongStarted = false;
    float pongCountdown = 5.0f;

    Rectangle player1 = {30, SCREEN_HEIGHT/2 - PADDLE_HEIGHT/2, PADDLE_WIDTH, PADDLE_HEIGHT};
    Rectangle player2 = {SCREEN_WIDTH - 40, SCREEN_HEIGHT/2 - PADDLE_HEIGHT/2, PADDLE_WIDTH, PADDLE_HEIGHT};
    Vector2 ballPosition = {SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f};
    Vector2 ballSpeed = {BALL_SPEED, BALL_SPEED};
    int score1 = 0, score2 = 0;
    
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
                StopMusicStream(music); // parar música do menu se estiver tocando
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
            if (IsKeyPressed(KEY_DOWN)) selectedOption = (selectedOption + 1) % menuItemsCount;
            if (IsKeyPressed(KEY_UP)) selectedOption = (selectedOption - 1 + menuItemsCount) % menuItemsCount;

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
                }else if (selectedOption == 3){
                    currentState = COMMANDS;
                }else if (selectedOption == 4){
                    break;
                }
            }

            for (int i = 0; i < menuItemsCount; i++) {
                Color color = (i == selectedOption) ? YELLOW : WHITE;
                const char* text = menuItems[i];
                int fontSize = 30;
                int textWidth = MeasureText(text, fontSize);
                
                // Ajuste as coordenadas Y e X aqui:
                int x = SCREEN_WIDTH / 2 - textWidth / 2;
                int y = 270 + i * 40;  // 200 é o topo inicial, 50 é o espaçamento vertical

                DrawText(text, x, y, fontSize, color);
            }
        }
        
        else if (currentState == PONG) {
            if (pointScored) {
                pointDelay -= GetFrameTime();
                if (pointDelay <= 0.0f) {
                    pointScored = false;
                }
            }
        
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
        
                EndDrawing();
                continue;
            }
        
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
        
                if (ballPosition.y <= BALL_RADIUS || ballPosition.y >= SCREEN_HEIGHT - BALL_RADIUS) {
                    if(!rebound) {
                        ballSpeed.y *= -1;
                        PlaySound(pongRebound);
                        rebound = true;
                    }
                }else {
                    rebound = false;
                }
                if (CheckCollisionCircleRec(ballPosition, BALL_RADIUS, player1) && ballSpeed.x < 0) {
                    if(!barHit) {
                        ballSpeed.x *= -1;
                        PlaySound(pongBar);
                        barHit = true;
                        }
                }else{
                    barHit = false;
                    } 
                if (CheckCollisionCircleRec(ballPosition, BALL_RADIUS, player2) && ballSpeed.x > 0) {
                    if(!barHit) {
                        ballSpeed.x *= -1;
                        PlaySound(pongBar);
                        barHit = true;
                        }
                }else {
                    barHit = false;
                } 
                if (ballPosition.x < 0 || ballPosition.x > SCREEN_WIDTH) {
                    if (ballPosition.x < 0) {
                        score2++;
                        PlaySound(pongPoint);
                        ballSpeed = (Vector2){-BALL_SPEED, BALL_SPEED}; // comeca a bola com quem tomou ponto
                    }else {
                        score1++;
                        PlaySound(pongPoint);
                        ballSpeed = (Vector2){BALL_SPEED, BALL_SPEED}; // comeca a bola com quem tomou ponto
                        }
                    ballPosition = (Vector2){SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f};
                    FreeBallHistory(history);
                    history = NULL;
                    historyCount = 0; // reinicia o rastro da bola
                    ResetGrid(); // reinicia o fundo p n ficar feio
                    pointScored = true;
                    pointDelay = 0.5f;
                    }
                }                    
        
            // Desenho (independe se o jogo está pausado)
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
        
            if (gameEnded) {
                const char *msg = (score1 > score2) ? "Jogador 1 Venceu!" : (score2 > score1) ? "Jogador 2 Venceu!" : "Empate!";
                DrawText(msg, SCREEN_WIDTH/2 - MeasureText(msg, 40)/2, SCREEN_HEIGHT/2 - 20, 40, YELLOW);
                DrawText("Pressione ENTER para voltar ao menu", SCREEN_WIDTH/2 - 180, SCREEN_HEIGHT/2 + 30, 20, GRAY);
                if (IsKeyPressed(KEY_ENTER)) currentState = MENU;
            }
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