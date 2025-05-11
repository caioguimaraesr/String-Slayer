#include "raylib.h"
#include <stdlib.h>
#include <time.h>

// === DEFINIÇÕES ===
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450
#define PADDLE_WIDTH 10
#define PADDLE_HEIGHT 100
#define PADDLE_SPEED 14
#define BALL_RADIUS 8
#define BALL_SPEED 26
#define MAX_HISTORY 100
#define ROWS 15
#define COLS 20
#define MAX_ASTEROIDS 20
#define WALL_WIDTH 80
#define WALL_GAP 200
#define MAX_WALLS 5

Rectangle avoidPlayer;
float avoidVelocity = 0;
float avoidGravity = 600;
float avoidJumpForce = -300;

typedef struct {
    float x;
    float gapY;
    bool passed;
} Wall;

Wall walls[MAX_WALLS];
float wallSpeed = 200;
int wallSpacing = 300;
bool avoidGameOver = false;
float avoidTime = 0.0f;
float avoidBestTime = 0.0f;
bool avoidStarted = false;
float avoidCountdown = 5.0f;

// === ENUM ESTADO DO JOGO ===
typedef enum GameState {
    MENU,
    GAME,
    ASTRO_DODGE,
    AVOID_WALLS,
} GameState;

// === ESTRUTURAS ===
typedef struct BallPositionNode {
    Vector2 position;
    struct BallPositionNode *next;
} BallPositionNode;

typedef struct Asteroid {
    Vector2 position;
    float speed;
    bool active;
} Asteroid;

// === VARIÁVEIS GLOBAIS ===
int grid[ROWS][COLS] = {0};
float gameTime = 120.0f;
bool gameEnded = false;
GameState currentState = MENU;
BallPositionNode *history = NULL;
int historyCount = 0;

// === FUNÇÕES DE RASTRO ===
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

void ResetAvoidGame() {
    avoidPlayer = (Rectangle){ 100, SCREEN_HEIGHT/2 - 25, 40, 40 };
    avoidVelocity = 0;
    avoidGameOver = false;
    avoidStarted = false;
    avoidCountdown = 5.0f;
    avoidTime = 0.0f;

    for (int i = 0; i < MAX_WALLS; i++) {
        walls[i].x = SCREEN_WIDTH + i * wallSpacing;
        walls[i].gapY = 100 + rand() % (SCREEN_HEIGHT - WALL_GAP - 100);
        walls[i].passed = false;
    }
}

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

    Music music = LoadMusicStream("assets/music/musica-errada.ogg");
    PlayMusicStream(music);
    SetMusicVolume(music, 1.0f);
    
    SetTargetFPS(60);
    
    const char *menuItems[] = { "Jogar Pong", "Astro Dodge", "Avoid the Walls", "Sair" };
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
        if (currentState == MENU) {
            if (!IsMusicStreamPlaying(music)) PlayMusicStream(music);   
        } else {
            if (IsMusicStreamPlaying(music)) PauseMusicStream(music);
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

                    currentState = GAME;

                } else if (selectedOption == 1) {
                    currentState = ASTRO_DODGE;
                } else if (selectedOption == 2) {
                    ResetAvoidGame();
                    currentState = AVOID_WALLS;
                }else if (selectedOption == 3){
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

        else if (currentState == GAME) {
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

            if (!gameEnded) {
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

                if (ballPosition.y <= BALL_RADIUS || ballPosition.y >= SCREEN_HEIGHT - BALL_RADIUS)
                    ballSpeed.y *= -1;

                if (CheckCollisionCircleRec(ballPosition, BALL_RADIUS, player1) && ballSpeed.x < 0)
                    ballSpeed.x *= -1;
                if (CheckCollisionCircleRec(ballPosition, BALL_RADIUS, player2) && ballSpeed.x > 0)
                    ballSpeed.x *= -1;

                if (ballPosition.x < 0) {
                    score2++;
                    ballPosition = (Vector2){SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f};
                    ballSpeed = (Vector2){BALL_SPEED, BALL_SPEED};
                } else if (ballPosition.x > SCREEN_WIDTH) {
                    score1++;
                    ballPosition = (Vector2){SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f};
                    ballSpeed = (Vector2){-BALL_SPEED, BALL_SPEED};
                }
            }

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
                playerSpeed = 8.0f;
                for (int i = 0; i < MAX_ASTEROIDS; i++) {
                    asteroids[i].position = (Vector2){ rand() % SCREEN_WIDTH, (float)(-(rand() % 600)) };
                    asteroids[i].speed = 10 + rand() % 6; // Velocidade dos asteroides
                    asteroids[i].active = true;
                }
                gameOver = false;
                survivalTime = 0.0f;
                hitSound = LoadSound("assets/hit.wav");
                astroInitialized = true;
                startCountdown = 5.0f;
                gameStarted = false;
            }

            // TIMER DE INÍCIO ANTES DO GAME COMEÇAR
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
            }

            DrawTexturePro(
                shipTexture,
                (Rectangle){ 0, 0, (float)shipTexture.width, (float)shipTexture.height },
                (Rectangle){ player.x, player.y, player.width, player.height },
                (Vector2){ 0, 0 },
                0.0f,
                WHITE
            );

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
        } 
        else if (currentState == AVOID_WALLS) {
            BeginDrawing();
            DrawTexture(background, 0, 0, WHITE);
            ClearBackground(DARKBLUE);

            if (!avoidGameOver) {
                if (!avoidStarted) {
                    avoidCountdown -= GetFrameTime();
                    DrawText(TextFormat("Começando em %.0f...", avoidCountdown), SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 - 20, 30, WHITE);
                    if (avoidCountdown <= 0) {
                        avoidStarted = true;
                    }
                } else {
                    avoidTime += GetFrameTime();

                    if (IsKeyPressed(KEY_SPACE)) avoidVelocity = avoidJumpForce;

                    avoidVelocity += avoidGravity * GetFrameTime();
                    avoidPlayer.y += avoidVelocity * GetFrameTime();

                    for (int i = 0; i < MAX_WALLS; i++) {
                        walls[i].x -= wallSpeed * GetFrameTime();

                        if (walls[i].x + WALL_WIDTH < 0) {
                            walls[i].x = SCREEN_WIDTH + wallSpacing;
                            walls[i].gapY = 100 + rand() % (SCREEN_HEIGHT - WALL_GAP - 100);
                            walls[i].passed = false;
                        }

                        Rectangle topWall = { walls[i].x, 0, WALL_WIDTH, walls[i].gapY };
                        Rectangle bottomWall = { walls[i].x, walls[i].gapY + WALL_GAP, WALL_WIDTH, SCREEN_HEIGHT - (walls[i].gapY + WALL_GAP) };

                        if (CheckCollisionRecs(avoidPlayer, topWall) || CheckCollisionRecs(avoidPlayer, bottomWall) || avoidPlayer.y < 0 || avoidPlayer.y + avoidPlayer.height > SCREEN_HEIGHT) {
                            avoidGameOver = true;
                            if (avoidTime > avoidBestTime) avoidBestTime = avoidTime;
                        }

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

                    float scale = 0.5f; // Aumenta o tamanho para o dobro
                    DrawTextureEx(birdTexture, (Vector2){avoidPlayer.x, avoidPlayer.y}, 0.0f, scale, WHITE);
                    DrawText(TextFormat("Tempo: %.2f", avoidTime), 20, 20, 20, WHITE);
                    DrawText(TextFormat("Melhor: %.2f", avoidBestTime), 20, 50, 20, YELLOW);
                }
            } else {
                DrawText("Game Over!", SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 - 30, 40, RED);
                DrawText("Pressione ENTER para voltar ao menu", SCREEN_WIDTH/2 - 180, SCREEN_HEIGHT/2 + 10, 20, LIGHTGRAY);
                if (IsKeyPressed(KEY_ENTER)) {
                    currentState = MENU;
                }
            }
        }

        EndDrawing();
    }

    UnloadTexture(pipeTexture);
    UnloadTexture(background);
    UnloadTexture(birdTexture);
    UnloadTexture(asteroidTexture);
    UnloadTexture(shipTexture);
    UnloadMusicStream(music);
    CloseAudioDevice();
    FreeBallHistory(history);
    CloseWindow();
    return 0;
}
