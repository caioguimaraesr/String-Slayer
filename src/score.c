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
            char *entryToken = strtok(token, ",");
            int scoreIndex = 0;
            while (entryToken != NULL && scoreIndex < MAX_SCORES) {
                char *equalSign = strchr(entryToken, '=');
                if (equalSign != NULL) {
                    *equalSign = '\0';
                    strncpy(highScores[minijogoIndex].entries[scoreIndex].name, entryToken, MAX_NAME_LENGTH-1);
                    highScores[minijogoIndex].entries[scoreIndex].name[MAX_NAME_LENGTH-1] = '\0';
                    highScores[minijogoIndex].entries[scoreIndex].score = atoi(equalSign + 1);
                    scoreIndex++;
                }
                entryToken = strtok(NULL, ",");
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
            fprintf(file, "%s=%d", highScores[i].entries[j].name, highScores[i].entries[j].score);
            if (j < highScores[i].count - 1) {
                fprintf(file, ",");
            }
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

void atualizarScore(int minijogoIndex, int novoScore, const char *name) {
    if (minijogoIndex < 0 || minijogoIndex >= MAX_MINIJOGOS) return;

    HighScores *hs = &highScores[minijogoIndex];

    if (hs->count >= MAX_SCORES && novoScore <= hs->entries[MAX_SCORES-1].score) {
        return;
    }

    ScoreEntry newEntry;
    newEntry.score = novoScore;
    strncpy(newEntry.name, name, MAX_NAME_LENGTH-1);
    newEntry.name[MAX_NAME_LENGTH-1] = '\0';

    if (hs->count < MAX_SCORES) {
        hs->entries[hs->count++] = newEntry;
    } else {
        hs->entries[MAX_SCORES-1] = newEntry;
    }

    for (int i = 0; i < hs->count - 1; i++) {
        for (int j = i + 1; j < hs->count; j++) {
            if (hs->entries[j].score > hs->entries[i].score) {
                ScoreEntry tmp = hs->entries[i];
                hs->entries[i] = hs->entries[j];
                hs->entries[j] = tmp;
            }
        }
    }

    if (hs->count > MAX_SCORES) {
        hs->count = MAX_SCORES;
    }
}
