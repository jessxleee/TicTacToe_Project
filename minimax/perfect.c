#include "../header.h"
#include <stdio.h>

/*Check if there are any moves left on the board*/
int MovesLeft(char board[3][3]) {

    for (int i = 0; i < 3; i++) { /*Loop rows*/
        for (int j = 0; j < 3; j++) { /*Loop columns*/
            if (board[i][j] == '\0') { /*if find empty cell*/
                return 1; /*Tell program game can continue*/
            }
        }
    }
    return 0; /*Tell program game over*/
}

/*Evaluate board and find if win*/
int eval_board(char board[3][3]) {

    /*Find Row for X or O win*/
    for (int row = 0; row < 3; row++) {
        if (board[row][0] == board[row][1] && board[row][1] == board[row][2]) { /*If all cells in row match*/
            if (board[row][0] == player) {
                return 10; /*If player win*/
            }
            else if (board[row][0] == opponent) {
                return -10; /*If opponent win*/
            }
        }
    }

    /*Find Column for X or O win*/  
    for (int col = 0; col < 3; col++) {
        if (board[0][col] == board[1][col] && board[1][col] == board[2][col]) { /*If all cells in column match*/
            if (board[0][col] == player) {
                return 10; /*If player win*/
            }
            else if (board[0][col] == opponent) {
                return -10; /*If opponent win*/
            }
        }
    }

    /*Check Diagonals for X or O victory*/
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2]) {
        if (board[0][0] == player) {
            return 10; /*If player win*/
        }
        else if (board[0][0] == opponent) {
            return -10; /*If opponent win*/
        }
    }
    if (board[0][2] == board[1][1] && board[1][1] == board[2][0]) {
        if (board[0][2] == player) {
            return 10; /*If player win*/
        }
        else if (board[0][2] == opponent) {
            return -10; /*If opponent win*/
        }
    }

    return 0; /*No win, game continues*/
}

/* Minimax algorithm to determine best next move */
int minmax(int depth, bool ismax, char board[3][3]) {
    int score = eval_board(board);  // Evaluate current board

    if (score == 10) return score - depth;
    if (score == -10) return score + depth;

    if (!MovesLeft(board)) return 0;  // Tie

    if (ismax) {  // Maximizing for AI (player)
        int best = -10000;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (board[i][j] == '\0') {
                    board[i][j] = player;
                    int result = minmax(depth + 1, false, board);  // Recursive Minimax
                    best = (result > best) ? result : best;
                    board[i][j] = '\0';  // Undo move
                }
            }
        }
        return best;
    } else {  // Minimizing for opponent
        int best = 10000;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (board[i][j] == '\0') {
                    board[i][j] = opponent;
                    int result = minmax(depth + 1, true, board);  // Recursive Minimax
                    best = (result < best) ? result : best;
                    board[i][j] = '\0';  // Undo move
                }
            }
        }
        return best;
    }
}

/*Priority ranking of positions on the board for tie-breaking (from most to least valuable)*/
int position_priority(int row, int col) {
    // Center is the highest priority
    if (row == 1 && col == 1) {
        return 3;  // Highest priority
    }

    // Corners and edges dont matter
    return 0;  // Lowest priority (corners and edges)
}


/* Check if placing a move in a given spot blocks the opponent from winning */
int check_block_move(int row, int col, char board[3][3], char player) {
    // Temporarily make the move
    board[row][col] = player;

    // Check if this move blocks the opponent's win
    int score = eval_board(board);

    // Undo the move
    board[row][col] = '\0';

    return score;
}

/* Find best move for player */
struct Move find_best_move(char board[3][3]) {
    int bestmove_value = -1000;
    struct Move best_move = {-1, -1};  // Default move if no better move is found


     // First, try to block the opponent from winning
    for (int i = 0; i < 3; i++) { /* Loop rows */
        for (int j = 0; j < 3; j++) { /* Loop columns */
            if (board[i][j] == '\0') { /* If cell is empty */

                // Check if placing a move here blocks the opponent's win
                if (check_block_move(i, j, board, opponent) == -10) {
                    best_move.row = i; /* Update best move row */
                    best_move.col = j; /* Update best move column */
                    return best_move; /* Return the blocking move immediately */
                }
            }
        }
    }
    

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == '\0') {
                board[i][j] = player;
                int move = minmax(0, false, board);
                board[i][j] = '\0';

                if (move > bestmove_value || (move == bestmove_value && position_priority(i, j) > position_priority(best_move.row, best_move.col))) {
                    best_move.row = i;
                    best_move.col = j;
                    bestmove_value = move;
                }
            }
        }
    }
    return best_move;
}