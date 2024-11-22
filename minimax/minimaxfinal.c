#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>
#include <stdlib.h>
#include "../header.h"
#include <string.h>

#define PLAYER 'X'
#define OPPONENT 'O'
#define EMPTY '\0'

int MAX_DEPTH;

int player_win_score, opponent_win_score;




int diffminmax(int depth, bool ismax, char board[3][3], int alpha, int beta) {
    // Evaluate the current board state
    int score = eval_board(board);

    // If the game is over, return the score adjusted by depth
    if (score == 10) {
        return score - depth; // Player wins (good for player, penalize deeper)
    }
    if (score == -10) {
        return score + depth; // Opponent wins (good for opponent, penalize deeper)
    }
    if (!MovesLeft(board)) {
        return 0; // Draw (no moves left, tie)
    }

    if (ismax) { // Maximizing player (player's turn)
        int best = -10000; // Start with a very low value

        // Try all possible moves and choose the one with the best score
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (board[i][j] == EMPTY) {
                    board[i][j] = OPPONENT; // Make the move
                    int move_value = diffminmax(depth + 1, false, board, alpha, beta); // Recursively evaluate
                    best = (move_value > best) ? move_value : best; /*If condition met update value of best to value, else best*/
                    alpha = (alpha > best) ? alpha : best; /*If condition met update value of alpha to alpha, else best*/
                    board[i][j] = EMPTY; // Undo the move
                    if (beta <= alpha) {
                        break;
                    }
                }
            }
        }
        return best; // Return the best score for the maximizing player
    } else { // Minimizing opponent (opponent's turn)
        int best = 10000; // Start with a very high value

        // Try all possible moves and choose the one with the worst score (minimizing)
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (board[i][j] == EMPTY) {
                    board[i][j] = OPPONENT; // Make the move
                    int move_value = diffminmax(depth + 1, true, board, alpha, beta); // Recursively evaluate
                    best = (move_value < best) ? move_value : best;
                    beta = (beta < best) ? beta : best;
                    board[i][j] = EMPTY; // Undo the move
                    if (beta <= alpha) {
                        break;
                    }
                }
            }
        }
        return best; // Return the best score for the minimizing opponent
    }
}



/* Find the best move for the player */
struct Move diff_find_best_move(char board[3][3]) {
    int bestmove_value = -10000; /* Set low best move value */
    struct Move best_move;       /* Structure to store the best value */
    best_move.row = -1;          /* Start row */
    best_move.col = -1;          /* Start column */

    printf("[DEBUG] Difficulty selected: %c\n", selected_difficulty);

    // First, try to block the opponent from winning (only for Hard difficulty)
    if (strcmp(selected_difficulty, "Hard") == 0) {
        for (int i = 0; i < 3; i++) { /* Loop rows */
            for (int j = 0; j < 3; j++) { /* Loop columns */
                if (board[i][j] == EMPTY) { /* If cell is empty */
                    // Check if placing a move here blocks the opponent's win
                    if (check_block_move(i, j, board, OPPONENT) == -10) {
                        printf("[DEBUG] Blocking move at Row: %d, Col: %d\n", i, j);
                        best_move.row = i;
                        best_move.col = j;
                        return best_move;
                    }
                }
            }
        }
    }

    // Iterate through all possible moves
    for (int i = 0; i < 3; i++) { /* Loop rows */
        for (int j = 0; j < 3; j++) { /* Loop columns */
            if (board[i][j] == EMPTY) { /* If cell is empty */
                printf("[DEBUG] Evaluating move at Row: %d, Col: %d\n", i, j);

                /* Make move */
                board[i][j] = PLAYER;

                /* Evaluate move using Minimax algorithm */
                int move_value = diffminmax(0, false, board, -10000, 10000);
                printf("[DEBUG] Move value: %d at Row: %d, Col: %d\n", move_value, i, j);

                if (strcmp(selected_difficulty, "Hard") == 0) {
                    // Hard difficulty: Use position priority in addition to move value
                    if (move_value > bestmove_value || 
                        (move_value == bestmove_value && position_priority(i, j) > position_priority(best_move.row, best_move.col))) {
                        best_move.row = i;
                        best_move.col = j;
                        bestmove_value = move_value;
                    }
                } else if (strcmp(selected_difficulty, "Easy") == 0) {
                    // Easy difficulty: 20% chance for a random move
                    if (rand() % 10 < 2) { // 20% chance for random move
                        printf("[DEBUG] Easy difficulty: Attempting random move.\n");
                        int attempts = 0;  // Limit attempts to prevent infinite loops
                        do {
                            best_move.row = rand() % 3;
                            best_move.col = rand() % 3;
                            attempts++;
                        } while (board[best_move.row][best_move.col] != EMPTY && attempts < 9);

                        printf("[DEBUG] Random move chosen at Row: %d, Col: %d (Attempts: %d)\n", 
                               best_move.row, best_move.col, attempts);

                        if (board[best_move.row][best_move.col] == EMPTY) {
                            board[i][j] = EMPTY; // Undo move before returning random choice
                            return best_move;    // Return random move
                        }
                    }
                }

                // Fallback: Normal move logic for all difficulties
                if (move_value > bestmove_value) {
                    best_move.row = i;
                    best_move.col = j;
                    bestmove_value = move_value;
                }

                /* Undo move */
                board[i][j] = EMPTY;
            }
        }
    }

    printf("[DEBUG] Best move chosen at Row: %d, Col: %d with value: %d\n", 
           best_move.row, best_move.col, bestmove_value);
    return best_move; /* Return best move found */
}
