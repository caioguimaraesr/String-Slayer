#include "raylib.h"
#include "commands.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

void DrawCommandsScreen(void) {
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
    DrawText("Setas para mover, Espaço para atirar", SCREEN_WIDTH/2 - MeasureText("Setas para mover, Espaço para atirar", 20)/2, y, 20, LIGHTGRAY);
    y += sectionSpacing;

    // AVOID THE WALLS
    DrawText("AVOID THE WALLS", SCREEN_WIDTH/2 - MeasureText("AVOID THE WALLS", 25)/2, y, 25, WHITE);
    y += spacing;
    DrawText("Espaço para pular", SCREEN_WIDTH/2 - MeasureText("Espaço para pular", 20)/2, y, 20, LIGHTGRAY);
    y += sectionSpacing;

    // Voltar
    DrawText("Pressione ENTER para voltar", SCREEN_WIDTH/2 - MeasureText("Pressione ENTER para voltar", 20)/2, SCREEN_HEIGHT - 50, 20, GRAY);
}
