#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../header.h"

struct Move get_naive_bayes_move() {
    FILE *fp;
    char path[1035];
    struct Move best_move = {-1, -1}; // Default to invalid move
    
    // Construct the command with the board state
    char command[256] = "python3 naive-bayes/classification.py";
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            char cell[3];
            sprintf(cell, " %c", board[i][j] == '\0' ? 'b' : board[i][j]);
            strcat(command, cell);
        }
    }

    // Open the command for reading
    fp = popen(command, "r");
    if (fp == NULL) {
        printf("Failed to run command\n");
        return best_move;
    }

    // Read the output a line at a time - output expected as "row col"
    if (fgets(path, sizeof(path), fp) != NULL) {
        sscanf(path, "%d %d", &best_move.row, &best_move.col);
    }

    // Close the process
    pclose(fp);

    return best_move;
}