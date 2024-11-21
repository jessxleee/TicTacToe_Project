#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 3                  // TicTacToe board (3x3)
#define ACTIONS 9               // Number of possible actions (cells on board)
#define STATE_SPACE 19683       // Possible action states (3^9)
#define NUM_EPS 100000          // Number of training episodes
#define ALPHA 0.05              // Learning rate (Controls how much the new information influences the Q-Values)
#define GAMMA 0.95              // Discount index (balance immediate & future rewards)
#define EPSILON_MIN 0.1         // Limit for minimum exploration rate (epsilon)
#define DECAY 0.9999            // Epsilon decay rate over episodes from 1.0 to 0.1
#define WIN 1                   // Reward to define winning
#define LOSS -1                 // Penalty to define losing
#define DRAW 0                  // Reward to define draw

// Q-Table to store Q-Values for each state-action
double Q[STATE_SPACE][ACTIONS];
// TicTacToe Board (3x3)
char board[SIZE][SIZE];
// Starting epsilon value (exploration rate)
float epsilon = 1.0;


// Function to intitialise Q-Table
void initialise_Q_table(){
    for (int i = 0; i < STATE_SPACE; i++){  // i = index for states
        for (int j = 0; j < ACTIONS; j++){  // j = index for actions
            Q[i][j] = 0.0;  // Initialise Q-table state to 0
        }
    }
}

// Function to initialise TicTacToe board
void initialise_board(){
    for (int i = 0; i < SIZE; i++){
        for (int j = 0; j < SIZE; j++){
            board[i][j] = ' ';  // Initialise TicTacToe board to a blank state
        }
    }
}

// Function to print TicTacToe board in terminal for checking
void print_board(){
    printf("\n");
    for (int i = 0; i < SIZE; i++){
        for (int j = 0; j < SIZE; j++){
            printf("%c", board[i][j]);
            if (j < SIZE -1){
                printf(" | ");  // Print vertical separators between columns
            }
        }
        printf("\n");
        if (i < SIZE - 1){
            printf("--+---+--\n");  // Print horizontal separators between rows
        }
    }
    printf("\n");
}

// Function checks if move is valid (cell must be empty)
int validMove(int action){
    int row = action / SIZE;  // Calculate row index of board
    int col = action % SIZE;  // Calculate column index of board
    return board[row][col] == ' ';  // Checks if cell position is empty, return 1 (True) if valid, return 0 (False) if cells already contains 'X' / 'O'
}

// Function checks for winning state based on board state
int check_reward(){
    for (int i = 0; i < SIZE; i++){
        if (board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][1] == board[i][2]){  // Check rows for win
           return (board[i][0] == 'X') ? 1 : -1;                           
        }
    
        if (board[0][i] != ' ' && board[0][i] == board[1][i] && board[1][i] == board[2][i]){  // Check columns for win
           return (board[0][i] == 'X') ? 1 : -1;                           
        }
    }
    if (board[0][0] != ' ' && board[0][0] == board[1][1] && board[1][1] == board[2][2]){  // Check top-left to bottom-right diagonal win
        return (board[0][0] == 'X') ? 1 : -1;                           
    }
    if (board[0][2] != ' ' && board[0][2] == board[1][1] && board[1][1] == board[2][0]){  // Check top-right to bottom-left diagonal win
        return (board[0][2] == 'X') ? 1 : -1;                           
    }
    return 0;
}

// Function checks for draw
int draw(){
    for (int i = 0; i < SIZE; i++){
        for (int j = 0; j < SIZE; j++){
            if (board[i][j] == ' '){  // Checks for empty cells. If there are empty cells, it is not a draw
                return 0;
            }
        }
    }
    return 1;  // If there are no empty cells, it is a draw
}

// Function to encode each board state to a unique index to identify each board configuration as a distinct combination (Q-Table indexing)
int encodeState(){
    int state = 0;  // There are 3 possible states: 0 = empty, 1 = occupied by 'X', 2 = occupied by 'O'
    int index = 1;

    for (int i = 0; i < SIZE; i++){
        for (int j =0; j < SIZE; j++){
            int value = (board[i][j] == 'X') ? 1 : (board[i][j] == 'O') ? 2: 0;
            state += value * index;
            index *= 3;  // Multiply by 3 as we are treating the board as a ternary number as there are 3 possible states                        
        }
    }
    return state;
}

/*Encoding Example: (0,0) = 'X' → value = 1
                        (0,1) = 'O' → value = 2
                        (0,2) = ' ' → value = 0
                        (1,0) = ' ' → value = 0
                        (1,1) = 'X' → value = 1
                        (1,2) = 'O' → value = 2
                        (2,0) = ' ' → value = 0
                        (2,1) = ' ' → value = 0
                        (2,2) = 'X' → value = 1

                        state = (1 * 3^0) + (2 * 3^1) + (0 * 3^2) + (0 * 3^3) + (1 * 3^4) + (2 * 3^5) + (0 * 3^6) + (0 * 3^7) + (1 * 3^8)
                        state = 1 + 6 + 0 + 0 + 81 + 486 + 0 + 0 + 6561
                        state = 7135
*/

// Function for Epsilon-Greedy algorithm  (exploration vs exploitation)
int epsilon_greedy(int state, float epsilon){
    double rand_val = (double)rand() / RAND_MAX;  // Random number generator to generate a number between 0 and 1
    int action;

    // If random value < epsilon value = exploration (random action)
    if (rand_val < epsilon){
        do{
            action = rand() % ACTIONS;
        } while (!validMove(action));  // Check if action is valid, will keep looping as long as action is not valid
        return action;
    } 
    // Exploit: choosing action with highest Q-value from Q-table
    else {
        int best_action = -1;  // Initialise to an invalid action state to indicate a valid action has not been selected yet
        double maxQ = -9999;  // Intialise to a very low value to ensure any updated value will be higher than initial "starting" value

        // Interate through all possible actions (0-8)
        for (int i = 0; i < ACTIONS; i++){
            if (validMove(i) && Q[state][i] > maxQ){  // For each action, check for validity
                maxQ = Q[state][i];  // If Q-value of current action > current max Q-Value, maxQ variable is updated
                best_action = i;                      
            }
        }

        // If no valid best action is found, choose a random valid action (should not happen but this is a failsafe)
        if (best_action == -1){
            do{
                best_action = rand() % ACTIONS;   
            } while (!validMove(best_action));
        }
        return best_action;
    }
}

// Function to update Q-Table using the Q-Learning update rule
void update_Q_Table(int state, int action, int reward, int nextState){
    double nxt_maxQ = -9999;  // Represents best expected future reward (intialise to a very low value to ensure any updated value will be higher than initial "starting" value)
    
    // Find the max Q-Value for the next state
    for (int nextAction = 0; nextAction < ACTIONS; nextAction++){
        if (Q[nextState][nextAction] > nxt_maxQ){ 
            nxt_maxQ = Q[nextState][nextAction];  // Max Q-Value is updated if a higher Q-Value is found
        }
    }

    // Q-Learning Update Rule for current state-action
    Q[state][action] += ALPHA * (reward + GAMMA *nxt_maxQ - Q[state][action]);
}

// Function to save training data (Q-Table)
void saveQ_Table(const char *q_table){
    FILE *file_ptr;
    file_ptr = fopen("q_table.txt", "w");  // File pointer to open and update Q-Table.txt (file is created if not found, content in file is overwritten if file already exists)

    // Validation to ensure file can be accessed
    if (file_ptr == NULL){ 
        printf("Error saving Q-Table.\n");
        return;
    }
    
    // Iterate through Q-Table data and save to Q-Table.txt
    for (int state = 0; state < STATE_SPACE; state++){
        for (int action = 0; action < ACTIONS; action ++){
            fprintf(file_ptr, "%7.4f\t", Q[state][action]);
        }
        fprintf(file_ptr, "\n");
    }

    fclose(file_ptr);  // Close Q-table.txt after updating is completed
    printf("\nQ-Table saved successfully to %s.\n", "q_table.txt");
}

// Function to load saved Q-Table
void loadQ_Table(const char *q_table){
    FILE *file_ptr; 
    file_ptr = fopen("q_table.txt", "r");  // Access data in Q-Table.txt for reading only (Access trained model data)

    if (file_ptr == NULL){
        printf("Error loading Q-Table,\n");  // Error message if Q-Table.txt does not exist
        return;
    }

    // Iterate through Q-Table.txt to retrieve Q-Table data
    for (int state = 0; state < STATE_SPACE; state++){
        for (int action = 0; action < ACTIONS; action ++){
            fscanf(file_ptr, "%lf", &Q[state][action]);
        }
    }

    fclose(file_ptr);  // Close Q-Table.txt after Q-Table is retrieved
    printf("Q-Table loaded successfully from %s.\n", "q_table.txt");
}

// Function to save win rate data after each episode for model performance tracking
void winData(int episode, int totGames, int wins, int losses, int draws, const char *winData){
    double winRate = (double)wins / NUM_EPS * 100.0;
    double drawRate = (double)draws / NUM_EPS * 100.0;
    double lossRate = (double)losses / NUM_EPS * 100.0; 

    FILE *win_ptr;
    win_ptr = fopen("winData.txt", "a+");  // Open winData.txt to update at the end of the file (creates a new file if it does not exist)

    if (win_ptr == NULL){
        printf("Error opening win rate data file.\n");
        return;
    }

    fprintf(win_ptr, "%d %.2f %.2f %.2f\n", episode, winRate, lossRate, drawRate);

    fclose(win_ptr);
}

// Function for Reinforcement Learning Training Loop
void train(){
    // Initialise counter of each game state for statistics tracking
    int totWins = 0;                       
    int wins = 0;                                
    int losses = 0;        
    int draws = 0;

    for (int episode = 0; episode < NUM_EPS; episode++){
        initialise_board();  // Resets board at start of each episode
        int state = encodeState();  // Encode initial board state
        int player = 1;  // Player 1 starts
        int winner = 0;  // Variable to track winner

        // Creates an infinite loop till game-ending condition
        while (1){
            int action = epsilon_greedy(state, epsilon);  // Choose action using Epsilon-greedy
            int row = action / SIZE;  // Calculate row index based on action
            int col = action % SIZE;  // Calculate col index based on action
            board[row][col] = (player == 1) ? 'X' : 'O';  // Player 1 = 'X', Player 2 = 'O'

            int nextState = encodeState();  // Encodes board state after move
            winner = check_reward();  // Check for winner

            if (winner != 0){  // Game over: win / loss
                update_Q_Table(state, action, (winner == 1) ? WIN : LOSS, nextState);  // Update Q-Table for final state
                print_board();
                printf("Player %d wins!\n", (winner == 1) ? 1 : 2);

                if (winner == 1){
                    wins++;  // Increase win counter
                }
                else{
                    losses++;  // Increase loss counter
                }
                break;  // Exit loop, game over
            }
            else if (draw()) {  // Check for draw
                update_Q_Table(state, action, DRAW, nextState);
                print_board();
                printf("It's a draw!\n");
                draws++;  // Increase draw counter
                break;  // Exit loop, draw has occured
            } 
            else{  // Continue to update the Q-Table and continue the game
                update_Q_Table(state, action, 0, nextState);
            }

            state = nextState;  // Move to next state
            player = -player;  // Switch turn to player 2
        }

        // Epsilon decay: gradually reduce exploration over the episodes
        if (epsilon > EPSILON_MIN){
            epsilon *= DECAY;  // Decay epsilon value at end of each episode to explore more intially and gradually shift to exploiting (Learning curve)
            printf("Epsilon after episode %d: %f\n", episode + 1, epsilon);  // Display epsilon value per episode as it decays
        } else {
            printf("Epsilon after episode %d: %f\n", episode + 1, epsilon);  // Display the minimum epsilon value once decay stops
        }

        winData(episode + 1, NUM_EPS, wins, losses, draws, "winData.txt");  // Update winData.txt with win statistics
   }

    // Statistics display when training is completed
    double winRate = (double)wins / NUM_EPS * 100.0;  // Calculate winrate
    double drawRate = (double)draws / NUM_EPS * 100.0;  // Calculate draw rate
    double lossRate = (double)losses / NUM_EPS * 100.0;  // Calculate loss rate

    // Print statistics
    printf("\n~Training complete!~\nGame Statistics: \n");
    printf("Total episodes: %d\n", NUM_EPS);
    printf("Win Rate: %.2f%%\n", winRate);
    printf("Draw Rate: %.2f%%\n", drawRate);
    printf("Loss Rate: %.2f%%\n", lossRate);
}

// Function for user vs AI (using retrieved trained data from Q-Table.txt)
void play(){
    initialise_board();
    int state = encodeState();
    int player = 1;  // User -> player 1 (X), AI -> player 2 (O)
    int winner = 0;

    printf("\nTic-Tac-Toe! You are 'X', I am 'O'.\n");
    print_board();

    while (1){
        if (player == 1){  // User's turn
            int row, col;
            printf("Enter your move in terms of row and col): ");
            scanf("%d %d", &row, &col);

            // User input validation
            if (row < 0 || row >= SIZE || col < 0 || col >= SIZE || board[row][col] != ' '){
                printf("Invalid move! Try again.\n");
                continue;
            }
            board[row][col] = 'X';
        } else{
            int action;
            do{  // AI's turn (using Q-Table)
                action = epsilon_greedy(state, epsilon);
            } while (!validMove(action));  // Ensure AI's move is valid (does not overwrite user's move)

            int row = action / SIZE;
            int col = action % SIZE;
            printf("AI chooses (%d, %d)\n", row, col);  // Display clearly which cell AI has chosen
            board[row][col] = 'O';
        }

        print_board();  // Print board state after AI makes its valid move
        winner = check_reward();  // Check if there are any winners

        if (winner != 0){
            printf("Player %d wins!\n", (winner == 1) ? 1 : 2);  // If there is a winner, 
            break;
        }else if (draw()) {
            printf("It's a draw!\n");
            break;
        } 

        player = -player;  // Switch player
        state = encodeState();  // Update state to new board configuration
    }
}

int main()
{
    srand(time(NULL));                                 // Initialise random seed with current time

    initialise_Q_table();                              // Initialise Q-Table
    train();                                           // Train RL model
    saveQ_Table("q_table.txt");                        // Save training data

    loadQ_Table("q_table.txt");                        // Load training data for user vs AI
    play();                                            // User vs AI

    return 0;
}