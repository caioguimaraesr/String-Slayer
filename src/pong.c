#include "pong.h"
#include "globals.h"
#include <stdlib.h>

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

void InitPong(PongGame *game) {
    game->player1 = (Rectangle){30, SCREEN_HEIGHT/2 - PADDLE_HEIGHT/2, PADDLE_WIDTH, PADDLE_HEIGHT};
    game->player2 = (Rectangle){SCREEN_WIDTH-30-PADDLE_WIDTH, SCREEN_HEIGHT/2 - PADDLE_HEIGHT/2, PADDLE_WIDTH, PADDLE_HEIGHT};
    game->ballPosition = (Vector2){SCREEN_WIDTH/2, SCREEN_HEIGHT/2};
    game->ballSpeed = (Vector2){BALL_SPEED, BALL_SPEED};
    game->score1 = 0;
    game->score2 = 0;
    game->history = NULL;
    game->historyCount = 0;
    game->gameTime = 120.0f;
    game->gameEnded = false;
    game->started = false;
    game->countdown = 5.0f;
    game->pongStarted = false;  // Inicialização nova
    game->pongCountdown = 3.0f; // Inicialização nova

    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            game->grid[r][c] = 0;
}

void UpdatePong(PongGame *game) {
    if (!game->pongStarted) {
        game->pongCountdown -= GetFrameTime();
        if (game->pongCountdown <= 0) {
            game->pongStarted = true;
        }
        return;
    }

    if (!game->gameEnded) {
        game->gameTime -= GetFrameTime();
        if (game->gameTime <= 0) {
            game->gameTime = 0;
            game->gameEnded = true;
        }

        if (IsKeyDown(KEY_W) && game->player1.y > 0)
            game->player1.y -= PADDLE_SPEED;
        if (IsKeyDown(KEY_S) && game->player1.y + PADDLE_HEIGHT < SCREEN_HEIGHT)
            game->player1.y += PADDLE_SPEED;
        if (IsKeyDown(KEY_UP) && game->player2.y > 0)
            game->player2.y -= PADDLE_SPEED;
        if (IsKeyDown(KEY_DOWN) && game->player2.y + PADDLE_HEIGHT < SCREEN_HEIGHT)
            game->player2.y += PADDLE_SPEED;

        game->ballPosition.x += game->ballSpeed.x;
        game->ballPosition.y += game->ballSpeed.y;

        int col = (int)(game->ballPosition.x / (SCREEN_WIDTH / COLS));
        int row = (int)(game->ballPosition.y / (SCREEN_HEIGHT / ROWS));
        if (row >= 0 && row < ROWS && col >= 0 && col < COLS)
            game->grid[row][col]++;

        AppendBallPosition(&game->history, game->ballPosition, &game->historyCount);

        if (game->ballPosition.y <= BALL_RADIUS || game->ballPosition.y >= SCREEN_HEIGHT - BALL_RADIUS)
            game->ballSpeed.y *= -1;

        if (CheckCollisionCircleRec(game->ballPosition, BALL_RADIUS, game->player1) && game->ballSpeed.x < 0)
            game->ballSpeed.x *= -1;
        if (CheckCollisionCircleRec(game->ballPosition, BALL_RADIUS, game->player2) && game->ballSpeed.x > 0)
            game->ballSpeed.x *= -1;

        if (game->ballPosition.x < 0) {
            game->score2++;
            game->ballPosition = (Vector2){SCREEN_WIDTH/2, SCREEN_HEIGHT/2};
            game->ballSpeed = (Vector2){BALL_SPEED, BALL_SPEED};
        } else if (game->ballPosition.x > SCREEN_WIDTH) {
            game->score1++;
            game->ballPosition = (Vector2){SCREEN_WIDTH/2, SCREEN_HEIGHT/2};
            game->ballSpeed = (Vector2){-BALL_SPEED, BALL_SPEED};
        }
    }
}

void DrawPong(PongGame *game) {
    // Desenha o grid
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (game->grid[r][c] > 0) {
                float intensity = (float)game->grid[r][c] / 10.0f;
                if (intensity > 1.0f) intensity = 1.0f;
                Color cellColor = Fade(BLUE, intensity);
                DrawRectangle(c * (SCREEN_WIDTH / COLS), r * (SCREEN_HEIGHT / ROWS), 
                             SCREEN_WIDTH / COLS, SCREEN_HEIGHT / ROWS, cellColor);
            }
        }
    }

    // Linha central
    for (int i = 0; i < SCREEN_HEIGHT; i += 20)
        DrawRectangle(SCREEN_WIDTH/2 - 1, i, 2, 10, GRAY);

    // Desenha jogadores e bola
    DrawRectangleRec(game->player1, WHITE);
    DrawRectangleRec(game->player2, WHITE);
    DrawCircleV(game->ballPosition, BALL_RADIUS, WHITE);

    // Desenha histórico da bola
    BallPositionNode *curr = game->history;
    while (curr != NULL) {
        DrawCircleV(curr->position, 2, RED);
        curr = curr->next;
    }

    // Contagem regressiva inicial
    if (!game->pongStarted) {
        int countDisplay = (int)game->pongCountdown + 1;
        if (countDisplay > 0) {
            DrawText(TextFormat("%d", countDisplay), SCREEN_WIDTH/2 - 20, SCREEN_HEIGHT/2 - 40, 80, WHITE);
        } else {
            DrawText("Vai!", SCREEN_WIDTH/2 - 40, SCREEN_HEIGHT/2 - 40, 80, WHITE);
        }
    }

    // Placar e tempo
    int seconds = (int)game->gameTime;
    DrawText(TextFormat("Tempo: %02d:%02d", seconds / 60, seconds % 60), SCREEN_WIDTH/2 - 70, 10, 30, LIGHTGRAY);
    DrawText(TextFormat("%d", game->score1), SCREEN_WIDTH/4, 50, 40, WHITE);
    DrawText(TextFormat("%d", game->score2), 3*SCREEN_WIDTH/4, 50, 40, WHITE);

    // Tela de fim de jogo
    if (game->gameEnded) {
        const char *msg = (game->score1 > game->score2) ? "Jogador 1 Venceu!" : 
                         (game->score2 > game->score1) ? "Jogador 2 Venceu!" : "Empate!";
        DrawText(msg, SCREEN_WIDTH/2 - MeasureText(msg, 40)/2, SCREEN_HEIGHT/2 - 20, 40, YELLOW);
        DrawText("Pressione ENTER para voltar ao menu", SCREEN_WIDTH/2 - 180, SCREEN_HEIGHT/2 + 30, 20, GRAY);
    }
}

void RunPongGame(GameState *state) {
    static PongGame game;
    static bool initialized = false;

    if (!initialized) {
        InitPong(&game);
        initialized = true;
    }

    BeginDrawing();
    ClearBackground(BLACK);
    
    UpdatePong(&game);
    DrawPong(&game);
    
    if (game.gameEnded && IsKeyPressed(KEY_ENTER)) {
        initialized = false;
        *state = MENU;
        FreeBallHistory(game.history);
    }
    
    EndDrawing();
}