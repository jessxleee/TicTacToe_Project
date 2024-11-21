#include <stdbool.h>

#ifndef header_h

#define header_h

/*Datatype structure to represent board*/
struct Move
{
    int row,col; /*row and column of move*/
};


extern char player, opponent; //Identify player and opponent
extern char board[3][3];

/*Datatype structure to represent board*/
extern struct Move best_move;

int MovesLeft(char board[3][3]);
int eval_board(char board[3][3]);
int minmax(int depth, bool ismax, char board[3][3]);
int position_priority(int row, int col);
int check_block_move(int row, int col, char board[3][3], char player);
struct Move find_best_move(char board[3][3]);
#endif