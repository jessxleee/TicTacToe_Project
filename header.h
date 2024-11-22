#include <stdbool.h>

#ifndef header_h

#define header_h

#define MAX_STATES 19683  // Size of Q-Table
#define NUM_MOVES 9  // Total possible moves

/*Datatype structure to represent board*/
struct Move
{
    int row,col; /*row and column of move*/
};

extern int player_turn;
extern char player, opponent; // Identify player and opponent
extern char board[3][3];
extern double q_table[MAX_STATES][NUM_MOVES];  // Represent actual Q-Table
extern int num_states;  // Track number of states loaded from Q-Table

/*Datatype structure to represent board*/
extern struct Move best_move;

// Functions for Minimax
int MovesLeft(char board[3][3]);
int eval_board(char board[3][3]);
int minmax(int depth, bool ismax, char board[3][3]);
int position_priority(int row, int col);
int check_block_move(int row, int col, char board[3][3], char player);
struct Move find_best_move(char board[3][3]);

// Functions for Epsilon Greedy
void loadQ_Table(const char *q_table_100k);  // Function to load q_table file 
int get_state_index(char board[3][3]);  // Function to convert board to a state string to match Q-Table states
struct Move get_epsilonGreedy_move(char board[3][3], int player_turn);  // Function to find best move based on q_table
#endif