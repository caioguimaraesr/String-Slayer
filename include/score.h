#ifndef SCORE_H
#define SCORE_H

#define MAX_SCORES 3
#define MAX_MINIJOGOS 3
#define MAX_NAME_LENGTH 20

typedef struct {
    int score;
    char name[MAX_NAME_LENGTH];
} ScoreEntry;

typedef struct {
    ScoreEntry entries[MAX_SCORES];
    int count;
} HighScores;

extern HighScores highScores[MAX_MINIJOGOS];

void carregarScores(const char *filename);
void salvarScores(const char *filename);
void atualizarScore(int minijogoIndex, int novoScore, const char *name);

#endif