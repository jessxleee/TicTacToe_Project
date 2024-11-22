#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../header.h"

/* NAIVE BAYES */
/* Function to execute classification.py using Popen and retrieve the Naive Bayes move */
struct Move get_naive_bayes_move() {
    FILE *fp;
    char path[1035];
    struct Move best_move = {-1, -1}; // Default to invalid move

    // Construct the command with the board state
    char command[256] = "python naive-bayes/classification.py";
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            char value = (board[i][j] == 'X') ? 'x' : (board[i][j] == 'O') ? 'o' : '0';

            // Append the value to the command string
            char cell[4];
            snprintf(cell, sizeof(cell), " %c", value);
            strncat(command, cell, sizeof(command) - strlen(command) - 1);
        }
    }

    // Open the command for reading
    fp = popen(command, "r");
    if (fp == NULL) {
        printf("Failed to run command\n");
        return best_move;
    }

    if (fgets(path, sizeof(path), fp) != NULL) {
        sscanf(path, "%d %d", &best_move.row, &best_move.col);
    }

    // Close the process
    pclose(fp);

    return best_move;
}