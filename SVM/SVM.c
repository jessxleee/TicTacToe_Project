#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../header.h"

struct Move get_SVM_move() {
    FILE *fp;
    char path[1035];
    struct Move best_move = {-1, -1}; // Default to invalid move

    // Base command with the Python interpreter and script path
    char command[1024] = "python3 SVM/SVM_main.py";

    // Append the processed board state to the command
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int value = (board[i][j] == 'X') ? 1 : (board[i][j] == 'O') ? -1 : 0;

            // Append the value to the command string
            char cell[4];
            snprintf(cell, sizeof(cell), " %d", value);
            strncat(command, cell, sizeof(command) - strlen(command) - 1);
            
        }
    }

    // Open the command for reading
    fp = popen(command, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error: Failed to run command\n");
        return best_move;
    }

    /*// Parse the last line of the output to get the move
    if (sscanf(path, "%d %d", &best_move.row, &best_move.col) != 2) {
        fprintf(stderr, "Error: Failed to parse Python output\n");
        best_move.row = -1;
        best_move.col = -1;
    }*/

    // Read the output a line at a time - output expected as "row col"
    if (fgets(path, sizeof(path), fp) != NULL) {
        sscanf(path, "%d %d", &best_move.row, &best_move.col);
    }
    // Close the process
    if (pclose(fp) != 0) {
        fprintf(stderr, "Error: Command execution failed\n");
    }

    return best_move;
}