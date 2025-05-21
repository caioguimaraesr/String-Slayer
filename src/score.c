#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "score.h"

HighScores highScores[MAX_MINIJOGOS];

void carregarScores(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        for (int i = 0; i < MAX_MINIJOGOS; i++) {
            highScores[i].count = 0;
        }
        return;
    }

    char linha[256];
    int minijogoIndex = 0;
    while (fgets(linha, sizeof(linha), file) && minijogoIndex < MAX_MINIJOGOS) {
        char *token = strtok(linha, ":");
        token = strtok(NULL, ":");
        if (token != NULL) {
            char *scoreToken = strtok(token, ",");
            int scoreIndex = 0;
            while (scoreToken != NULL && scoreIndex < MAX_SCORES) {
                highScores[minijogoIndex].scores[scoreIndex] = atoi(scoreToken);
                scoreToken = strtok(NULL, ",");
                scoreIndex++;
            }
            highScores[minijogoIndex].count = scoreIndex;
        }
        minijogoIndex++;
    }

    fclose(file);
}

void salvarScores(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo para escrita.\n");
        return;
    }

    for (int i = 0; i < MAX_MINIJOGOS; i++) {
        fprintf(file, "minijogo%d:", i + 1);
        for (int j = 0; j < highScores[i].count; j++) {
            fprintf(file, "%d", highScores[i].scores[j]);
            if (j < highScores[i].count - 1) {
                fprintf(file, ",");
            }
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

void atualizarScore(int minijogoIndex, int novoScore) {
    if (minijogoIndex < 0 || minijogoIndex >= MAX_MINIJOGOS) return;

    HighScores *hs = &highScores[minijogoIndex];

    if (hs->count >= MAX_SCORES && novoScore <= hs->scores[MAX_SCORES-1]) {
        return;
    }

    if (hs->count < MAX_SCORES) {
        hs->scores[hs->count++] = novoScore;
    } 
    else {
        hs->scores[MAX_SCORES-1] = novoScore;
    }

    for (int i = 0; i < hs->count - 1; i++) {
        for (int j = i + 1; j < hs->count; j++) {
            if (hs->scores[j] > hs->scores[i]) {
                int tmp = hs->scores[i];
                hs->scores[i] = hs->scores[j];
                hs->scores[j] = tmp;
            }
        }
    }

    if (hs->count > MAX_SCORES) {
        hs->count = MAX_SCORES;
    }
}
