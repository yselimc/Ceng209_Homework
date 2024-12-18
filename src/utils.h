#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void removeWhitespaces(char *sentence);
char **parseString(const char *input, char delimiter, int *count);
char ***readBatches(FILE *file, int N, int *roomCount, int *batchCount);
void readWordsAndIntegers(FILE *file, char ***words, int **numbers, int *count);
FILE *openFile(const char *filePath);

#endif
