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

/*Minmax algorithm to determine best next move*/
int minmax(int depth, bool ismax, char board[3][3]) {

    int score = eval_board(board); /*Evaluate current board*/

    if (score == 10) { /*If player wins*/
        return score - depth; /*Return score adjusted with depth*/
    }
    else if (score == -10) { /*If opponent wins*/
        return score + depth; /*Return score adjusted with depth*/
    }

    if (!MovesLeft(board)) { /*If no moves left*/
        return 0; /*Return a tie game*/
    }

    if (ismax) { /*If it's maximize player's (Player) turn*/
        int best = -10000; /*Set low best score*/

        for (int i = 0; i < 3; i++) { /*Loop through rows*/
            for (int j = 0; j < 3; j++) { /*Loop through columns*/
                if (board[i][j] == '\0') { /*If cell is empty*/

                    board[i][j] = opponent; /*Make potential AI move*/

                    int result = minmax(depth + 1, false, board); /*Recursive minmax algorithm and compare the scores*/

                    if (result > best) { /*If result is better than current best score*/
                        best = result; /*Update best score*/
                    }

                    board[i][j] = '\0'; /*Reset the move*/
                }
            }
        }
        return best;  /*Return best score for maximising player*/
    }
    else { /*If it's minimizing player's (Opponent) turn*/

        int best = 10000; /*Set high best score*/   

        for (int i = 0; i < 3; i++) { /*Loop rows*/
            for (int j = 0; j < 3; j++) { /*Loop columns*/
                if (board[i][j] == '\0') { /*If cell is empty*/

                    board[i][j] = opponent; /*Make move*/

                    int result = minmax(depth + 1, true, board); /*Recursive minmax recursively and compare scores*/

                    if (result < best) { /*If result better than best score*/
                        best = result; /*Update best score with result*/
                    }

                    board[i][j] = '\0'; /*Reset board and undo move*/
                }
            }
        }
        return best; /*Return best score for minimizing player*/
    }
}

/*Priority ranking of positions on the board for tie-breaking (from most to least valuable)*/
int position_priority(int row, int col) {
    // Center is the highest priority
    if (row == 1 && col == 1) return 3;

    //Edges and corners priority do not matter
    return 1;
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

/*Find best move for player*/
struct Move find_best_move(char board[3][3]) {

    int bestmove_value = -1000; /*Set low best move value*/
    
    struct Move best_move; /*Structure to store best value*/
    best_move.row = -1; /*Start row*/
    best_move.col = -1; /*Start column*/

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

    for (int i = 0; i < 3; i++) { /*Loop row*/
        for (int j = 0; j < 3; j++) { /*Loop column*/
            if (board[i][j] == '\0') { /*If cell is empty*/

                /*make move*/
                board[i][j] = player;

                /*evaluate move using minmax algorithm*/
                int move = minmax(0, false, board);

                /* Debugging: print each move evaluated */
                printf("Evaluating move at (%d, %d): %d\n", i, j, move  );

                /*undo move*/
                board[i][j] = '\0';

                /*compare value of current move vs current best move*/
                if (bestmove_value < move || (move == bestmove_value && position_priority(i, j) > position_priority(best_move.row, best_move.col))) {
                    best_move.row = i; /*Update best move row*/
                    best_move.col = j; /*Update best move column*/
                    bestmove_value = move; /*Update best move value*/
                }
            }
        }
    }

    printf("The value of the best move is : %d\n\n", bestmove_value); /*Print best move value*/
    return best_move; /*Return best move found*/
}