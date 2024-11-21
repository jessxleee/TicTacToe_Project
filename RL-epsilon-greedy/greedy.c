#include <stdio.h>
#include <stdlib.h>
#include "../header.h"

double q_table[MAX_STATES][NUM_MOVES];  // Actual Q-Table
int num_states = 0;  // Counter to track number of states in Q-Table
int q_table_loaded = 0;  // Flag to ensure Q-Table only loaded once

// Function to load Q-Table from file
void loadQ_Table(const char *q_table_100k){
    // Prevent Q-Table from loading everytime its the AI's turn
    if (q_table_loaded) {
        return;
    }

    FILE *file_ptr; 
    file_ptr = fopen("RL-epsilon-greedy/q_table_100k.txt", "r");  // Access data in Q_Table_100k.txt for reading only (Access trained model data)

    if (file_ptr == NULL){
        printf("Error loading Q-Table,\n");  // Error message if Q_Table_100k.txt does not exist
        return;
    }

    printf("Success in loading Q-Table\n");

    for (int state = 0; state < MAX_STATES; state++){
        int not_all_zeros = 0;  // Flag to check if all values in the row are zero
        for (int move = 0; move < NUM_MOVES; move++){
            double q_val;
            fscanf(file_ptr, "%lf", &q_table[state][move]);  // Read Q-Value for state-action pair

            //If at least one value non-zero, it is loaded in
            if (q_table[state][move] != 0.0) {
                not_all_zeros = 1;
            }
        }
        
        if (!not_all_zeros) {
            continue;  // Skip loading the row entirely
        }
    }

    fclose(file_ptr);  // Close Q_Table_100k.txt after Q-Table is retrieved
}

// Function to convert board to a state string to match Q-Table states
int get_state_index(char board[3][3]) {
    int index = 0;
    int factor = 1;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int cell = (board[i][j] == 'X') ? 1: (board[i][j] == 'O') ? 2 : 0;
            index += cell * factor;
            factor *= 3;  // Multiply by 3 to account for all 9 positions
        }
    }
    return index;
}

// Function to find best move based on Q-Table
struct Move get_epsilonGreedy_move(char board[3][3], int player_turn) {
    struct Move best_move = {-1, -1};  // Initialise with invalid move
    double maxQ = -1e9;  // Initialise with very low value to find max

    if (player_turn == 2) {  // Ensure its AI turn to make move
        // Get state index from board
        int state_index = get_state_index(board);
        printf("Current state index: %d\n", state_index);

        // Iterate through possible moves (0-8)
        for (int i = 0; i < NUM_MOVES; ++i){
            int row = i / 3;
            int col = i % 3;

            if (board[row][col] == '\0'){  // Check if cell is empty
                double q_val = q_table[state_index][i];
                printf("Q-Value for move [%d][%d]: %.2f\n", row, col, q_val);

                if (q_val > maxQ) {
                    maxQ = q_val;
                    best_move.row = row;
                    best_move.col = col;
                }
            }
        }
    }

    return best_move;  // Return best move in row & col
}