#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../header.h"
#include <time.h>  // For srand()

#define K 2  // Number of clusters (positive and negative)
#define MAX_TURNS 9


double centroids[K][9];


// Function to load centroids from a CSV file
void load_centroids(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error loading centroids file.\n");
        exit(1);
    }
    
    for (int i = 0; i < K; i++) {
        for (int j = 0; j < 3; j++) {
            fscanf(file, "%lf,", &centroids[i][j]);
        }
        fscanf(file, "\n");  // Skip the newline after each centroid row
    }
    fclose(file);
}

// Function to calculate Euclidean distance between a board state and a centroid
double euclidean_distance(int* board_state, double* centroid) {
    double dist = 0.0;
    for (int i = 0; i < 9; i++) {
        dist += pow(board_state[i] - centroid[i], 2);
    }
    return sqrt(dist);
}

// Function to classify the board state using the centroids
int classify_board_state(char board_state[3][3]) {
    double min_distance = INFINITY;
    int assigned_cluster = -1;

    // Convert the 2D board_state into a 1D array for classification
    int board_state_flat[9];
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            board_state_flat[i * 3 + j] = board_state[i][j] == ' ' ? 0 : (board_state[i][j] == 'X' ? 2 : 1);  // Empty: 0, X: 2, O: 1
        }
    }

    // Loop over each centroid and compute the Euclidean distance to the board_state
    for (int i = 0; i < K; i++) {
        double dist = euclidean_distance(board_state_flat, centroids[i]);
        if (dist < min_distance) {
            min_distance = dist;
            assigned_cluster = i;
        }
    }

    return assigned_cluster;  // Return the index of the assigned cluster (0 for positive, 1 for negative)
}
// Function to check if the AI can win
int can_ai_win(char board[3][3]) {
    // Check for a winning move for AI (symbol 2)
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            if (board[row][col] == 0) {  // If the cell is empty
                board[row][col] = 2;  // Try AI's move
                if (eval_board(board) == 10) {
                    return 1;  // Found a winning move
                }
                board[row][col] = 0;  // Revert move if not a winning move
            }
        }
    }
    return 0;  // No winning move found
}

// Function to find the best move for AI using KMeans classification
struct Move kmeans_find_best_move(char board[3][3]) {
    struct Move best_move = {-1, -1};  // Default invalid move

    // We will first classify the current board state
    int cluster = classify_board_state(board);

    // If it's a positive state (AI can win), try to make a winning move
    if (cluster == 0) {  // Positive (winning) state
        printf("AI recognizes a winning state!\n");

        // Try to make a winning move for the AI
        if (can_ai_win(board)) {
            // Loop through the board to find the winning move
            for (int row = 0; row < 3; row++) {
                for (int col = 0; col < 3; col++) {
                    if (board[row][col] == 0) {  // If the cell is empty
                        best_move.row = row;
                        best_move.col = col;
                        board[row][col] = 2;  // AI makes its move ('x')
                        return best_move;
                    }
                }
            }
        }
    }

    // If the state is negative or no winning move found, AI should block or play defensively
    printf("AI recognizes a neutral or losing state.\n");

    // If no winning move is available, just pick the first empty spot (this can be improved)
    // First, create a list of available empty spots
    struct Move available_moves[9];  // A maximum of 9 empty spots on the board
    int available_count = 0;

    // Collect empty spots
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            if (board[row][col] == 0) {  // If the cell is empty
                available_moves[available_count].row = row;
                available_moves[available_count].col = col;
                available_count++;
            }
        }
    }

    if (available_count > 0) {
        // Random number generator
        srand(time(NULL));

        // Choose a random move from the available spots
        int random_index = rand() % available_count;
        best_move = available_moves[random_index];

        // Make the move
        board[best_move.row][best_move.col] = 2;
    }

    return best_move;
}
