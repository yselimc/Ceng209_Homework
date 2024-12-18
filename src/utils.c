#include "utils.h"

void removeWhitespaces(char *sentence)
{
    int i = 0, j = 0;
    while (sentence[i] != '\0')
    {
        if (sentence[i] != ' ' && sentence[i] != '\t' && sentence[i] != '\n')
        {
            sentence[j++] = sentence[i];
        }
        i++;
    }
    sentence[j] = '\0'; // Null-terminate the modified string
}

char **parseString(const char *input, char delimiter, int *count)
{
    // Count the number of delimiter occurrences to determine the number of tokens
    int tokens = 1; // At least one token
    for (const char *ptr = input; *ptr; ptr++)
    {
        if (*ptr == delimiter)
        {
            tokens++;
        }
    }

    // Allocate memory for the array of strings
    char **result = malloc(tokens * sizeof(char *));
    if (!result)
    {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    // Split the string
    const char *start = input;
    int index = 0;
    for (const char *ptr = input;; ptr++)
    {
        if (*ptr == delimiter || *ptr == '\0')
        {
            size_t len = ptr - start;
            result[index] = malloc((len + 1) * sizeof(char));
            if (!result[index])
            {
                perror("malloc failed");
                exit(EXIT_FAILURE);
            }
            strncpy(result[index], start, len);
            result[index][len] = '\0'; // Null-terminate the string
            // remove the whitespaces from the token
            removeWhitespaces(result[index]);
            index++;
            start = ptr + 1; // Move to the next token
        }
        if (*ptr == '\0')
        {
            break;
        }
    }

    *count = tokens; // Return the number of tokens
    return result;
}

char ***readBatches(FILE *file, int N, int *roomCount, int *batchCount)
{
    char line[1024];
    char ***batches = NULL;

    *roomCount = 0;
    *batchCount = 0;

    // Read the first line to get the RoomCount
    if (fgets(line, sizeof(line), file))
    {
        if (strncmp(line, "RoomCount:", 10) == 0)
        {
            *roomCount = atoi(line + 10); // Extract the number after "RoomCount:"
        }
        else
        {
            fprintf(stderr, "Invalid file format: RoomCount not found\n");
            exit(EXIT_FAILURE);
        }
    }

    // Skip the next empty line
    if (fgets(line, sizeof(line), file) && strcmp(line, "\n") != 0)
    {
        fprintf(stderr, "Invalid file format: Expected an empty line after RoomCount\n");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for the array of batches
    batches = malloc(*roomCount * sizeof(char **));
    if (!batches)
    {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < *roomCount; i++)
    {
        // Allocate memory for the array of strings
        batches[i] = malloc(N * sizeof(char *));
        if (!batches[i])
        {
            perror("malloc failed");
            exit(EXIT_FAILURE);
        }
        for (int j = 0; j < N; j++)
        {
            if (!fgets(line, sizeof(line), file))
            {
                fprintf(stderr, "Invalid file format: Unexpected end of file\n");
                exit(EXIT_FAILURE);
            }
            if (strcmp(line, "\n") == 0)
            {
                fprintf(stderr, "Invalid file format: Empty line found\n");
                exit(EXIT_FAILURE);
            }
            //removeWhitespaces(line);
            batches[i][j] = strdup(line);
        }

        // Skip the next empty line
        if (fgets(line, sizeof(line), file) && strcmp(line, "\n") != 0)
        {
            fprintf(stderr, "Invalid file format: Expected an empty line after a batch\n");
            exit(EXIT_FAILURE);
        }

        (*batchCount)++;
    }
    return batches;
}

void readWordsAndIntegers(FILE *file, char ***words, int **numbers, int *count)
{
    char line[1024]; // Buffer to hold each line from the file
    *words = NULL;   // Array to hold the words
    *numbers = NULL; // Array to hold the integers
    *count = 0;      // Initialize the count of elements

    // Read lines from the file
    while (fgets(line, sizeof(line), file)) {
        // Remove the newline character at the end of the line
        line[strcspn(line, "\n")] = '\0';

        // If the line is not empty, process it
        if (strlen(line) > 0) {
            // Split the line into word and number by the delimiter ','
            char *word = strtok(line, ",");
            char *numStr = strtok(NULL, ",");

            if (word && numStr) {
                // Allocate memory for the new word and number
                *words = realloc(*words, (*count + 1) * sizeof(char *));
                *numbers = realloc(*numbers, (*count + 1) * sizeof(int));

                if (!(*words) || !(*numbers)) {
                    perror("Failed to allocate memory");
                    exit(EXIT_FAILURE);
                }

                // Allocate memory for the word and copy it
                (*words)[*count] = strdup(word);
                if (!(*words)[*count]) {
                    perror("Failed to copy word");
                    exit(EXIT_FAILURE);
                }

                // Convert the string to an integer
                (*numbers)[*count] = atoi(numStr);

                (*count)++; // Increment count
            }
        }
    }
}

FILE *openFile(const char *filePath)
{
    // Check if the file exists by trying to open it in read mode
    FILE *file = fopen(filePath, "r");
    if (!file)
    {
        perror("Error opening file"); // Print error if the file cannot be opened
        return NULL;
    }

    // If successful, return the FILE pointer
    return file;
}