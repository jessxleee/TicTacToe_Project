#include <gtk/gtk.h>
#include <stdbool.h>
#include <cairo.h>
#include <minmax.h>

#include <limits.h>
#include <stdio.h>

// Game State Variables
char board[3][3];
GtkWidget *buttons[9];
int game_mode = 0;
int player_turn = 1; // 1 for X, 2 for O
GtkWidget *status_label;
GtkWidget *result_label;

void gameMenu();
void main_page();
void gameBoard_mode2();
char checkWinner();
bool checkTie();
void printWinner(char winner);
void endGame();
void resetBoard();
void exitGame();

void load_css() {
    GtkCssProvider *provider = gtk_css_provider_new();
    GdkDisplay *display = gdk_display_get_default();
    GdkScreen *screen = gdk_display_get_default_screen(display);

    // Load the external CSS file
    gtk_css_provider_load_from_path(provider, "style.css", NULL);

    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(provider);  // Free the provider after use
}

/*Identify player and opponent*/
char player = 'X', opponent ='O';

/*Datatype structure to represent board*/
struct Move
{
    int row,col; /*row and column of move*/
};

/*Check if there are any moves left on the board*/
int MovesLeft(char board[3][3]){

    for (int i=0; i < 3; i++){ /*Loop rows*/

        for(int j=0; j<3; j++){/*Loop columns*/
            
            if(board[i][j] == '\0'){/*if find empty cell*/
                return 1; /*Tell program game can continue*/
            }
        }
    }
    return 0; /*Tell program game over*/
}

/*Evaluate board and find if win*/
int eval_board(char board[3][3]){
    
    /*Find Row for X or O win*/
    for (int row = 0; row < 3; row++){
        if (board[row][0] == board[row][1] && board[row][1] == board[row][2]){ /*If all cells in row match*/
            if (board[row][0] == player){
                return 10; /*If player win*/
            }
            else if (board[row][0] == opponent){
                return -10; /*If opponent win*/
            }
        }

    }

    /*Find Column for X or O win*/  
    for (int col = 0; col < 3; col++){
        if (board[0][col] == board[1][col] && board[1][col] == board[2][col]){ /*If all cells in column match*/
            if (board[0][col] == player){
                return 10; /*If player win*/
            }
            else if (board[0][col] == opponent){
                return -10; /*If opponent win*/
            }
        }

    }

    /*Check Diagonals for X or O victory*/
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2]){
            if (board[0][0] == player){
                return 10; /*If player win*/
            }
            else if (board[0][0] == opponent){
                return -10; /*If opponent win*/
            }
        }
    if (board[0][2] == board[1][1] && board[1][1] == board[2][0]){
            if (board[0][2] == player){
                return 10; /*If player win*/
            }
            else if (board[0][2] == opponent){
                return -10; /*If opponent win*/
            }
        }

    return 0; /*No win, game continues*/
}

/*Minmax algorithm to determine best next move*/
int minmax(char board[3][3], int depth, bool ismax){

    int score = eval_board(board); /*Evaluate current board*/

    if(score == 10){ /*If player wins*/
        return score - depth; /*Return score adjusted with depth*/
    }
    else if (score == -10) /*If opponent wins*/
    {
        return score + depth; /*Return score adjusted with depth*/
    }
    
    if (!MovesLeft(board)){ /*If no moves left*/
        return 0; /*Return a tie game*/
    }

    if(ismax){ /*If it's maximise player's (Player) turn*/
        int best = -1000; /*Set low best score*/

        for (int i = 0; i < 3; i++){ /*Loop through rows*/
            for (int j = 0; j < 3; j++) /*Loop through columns*/
            {
                if (board[i][j] == '\0'){ /*If cell is empty*/
                    
                    board[i][j] = player; /*Make potential player move*/

                    int result = minmax(board, depth + 1, false); /*Recursive minmax algorithm and compare the scores*/

                    if (result > best){ /*If result is better than current best score*/
                        best = result; /*Update best score*/
                    }

                    board[i][j] = '\0'; /*Reset the move*/
                }

            }
        }
        return best;  /*Return best score for maximising player*/
    }
    else { /*If it's minimizing player's (Opponent) turn*/

        int best = 1000; /*Set high best score*/

        for (int i = 0; i < 3; i++){ /*Loop rows*/
            for (int j = 0; j < 3; j++) /*Loop columns*/
            {
                if (board[i][j] == '\0'){ /*If cell is empty*/
                    
                    board[i][j] = opponent; /*Make move*/

                    int result = minmax(board, depth + 1, true); /*Recursive minmax recursively and compare scores*/

                    if (result < best){ /*If result better than best score*/
                        best = result; /*Update best score with result*/
                    }

                    board[i][j] = '\0'; /*Reset board and undo move*/
                }

            }
        }
        return best; /*Return best score for minimizing player*/

    }

    
}

/*Find best move for player*/
struct Move find_best_move(char board[3][3]){

    int bestmove_value = -1000; /*Set low best move value*/
    
    struct Move best_move; /*Structure to store best value*/
    best_move.row = -1; /*Start row*/
    best_move.col = -1; /*Start column*/

    for (int i=0; i < 3; i++){ /*Loop row*/

        for(int j=0; j<3; j++){ /*Loop column*/
            if(board[i][j]  == '\0') { /*If cell is empty*/
                
                /*make move*/
                board[i][j] = player;

                /*evaluate move using minmax algorithm*/
                int move = minmax(board, 0, false);

                /*undo move*/
                board[i][j] = '\0';

                /*compare value of current move vs current best move*/
                if (bestmove_value < move){

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


void getGtkBoardState(char output[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            GtkWidget *button = buttons[i * 3 + j];  // Get the corresponding button
            const gchar *label = gtk_button_get_label(GTK_BUTTON(button));

            if (g_strcmp0(label, "X") == 0) {
                output[i][j] = 'X';  // Player X
            } else if (g_strcmp0(label, "O") == 0) {
                output[i][j] = 'O';  // Player O
            } else {
                output[i][j] = '\0';  // Empty cell
            }
        }
    }
}


void getCurrentBoardState(char output[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            output[i][j] = board[i][j];  // Copy current board state to output
        }
    }
}

void computer_Move() {
    struct Move best_move = find_best_move(board);  // Get the best move using Minimax

    if (best_move.row != -1 && best_move.col != -1) {  // Check if a valid move was found
        board[best_move.row][best_move.col] = 'O';  // Update the board
        gtk_button_set_label(GTK_BUTTON(buttons[best_move.row * 3 + best_move.col]), "O");  // Update the button label

        player_turn = 1;  // Switch back to player 1
        gtk_label_set_text(GTK_LABEL(status_label), "Player 1's Turn");  // Update status

        char winner = checkWinner();  // Check for a winner
        printWinner(winner);
    }
}


void gameMenu(GtkWidget *window){
    GtkWidget *box;
    GtkWidget *button_1p, *button_2p;

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
    gtk_container_add(GTK_CONTAINER(window), box);

    // Header
    GtkWidget *heading = gtk_label_new("TIC TAC TOE");
    gtk_widget_set_name(heading, "heading");
    gtk_widget_set_halign(heading, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(box), heading, FALSE, FALSE, 5);

    // icon
    GtkWidget *image = gtk_image_new_from_file("images/icon.png");
    gtk_box_pack_start(GTK_BOX(box), image, FALSE, FALSE, 0);
    gtk_widget_set_name(image, "image");

    // "1 Player" button
    button_1p = gtk_button_new_with_label("1 Player");
    g_signal_connect(button_1p, "clicked", G_CALLBACK(main_page), GINT_TO_POINTER(1));
    gtk_box_pack_start(GTK_BOX(box), button_1p, TRUE, TRUE, 0);
    gtk_widget_set_name(button_1p, "players-button");

    //"2 Players" button
    button_2p = gtk_button_new_with_label("2 Player");
    g_signal_connect(button_2p, "clicked", G_CALLBACK(main_page), GINT_TO_POINTER(2));
    gtk_box_pack_start(GTK_BOX(box), button_2p, TRUE, TRUE, 0);
    gtk_widget_set_name(button_2p, "players-button");

}

void clear_window(GtkWidget *window) {
    GList *children, *iter;
    children = gtk_container_get_children(GTK_CONTAINER(window));

    for (iter = children; iter != NULL; iter = g_list_next(iter)) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);
}

void main_page(GtkWidget *widget, gpointer data) {
    GtkWidget *window = gtk_widget_get_toplevel(widget);  
    game_mode = GPOINTER_TO_INT(data);

    clear_window(window);

    if (game_mode == 1) {
        g_print("1 Player game mode selected.\n");
        clear_window(window);
        gameBoard_mode2(window);

    } else if (game_mode == 2) {
        g_print("2 Players game mode selected.\n");
        clear_window(window);
        gameBoard_mode2(window);

    }

     gtk_widget_show_all(window);
}

    
void inputHandler(GtkWidget *widget, gpointer data) {
    int row = GPOINTER_TO_INT(data) / 3;
    int col = GPOINTER_TO_INT(data) % 3;

    if (board[row][col] == '\0') {  // If cell is empty
        if (player_turn == 1) {
            gtk_button_set_label(GTK_BUTTON(widget), "X");
            board[row][col] = 'X';
            player_turn = 2;
            gtk_label_set_text(GTK_LABEL(status_label), "Player 2's Turn");

        } else {
            gtk_button_set_label(GTK_BUTTON(widget), "O");
            board[row][col] = 'O';
            player_turn = 1;
            gtk_label_set_text(GTK_LABEL(status_label), "Player 1's Turn");
        }
        
        char currentBoard[3][3];
        getGtkBoardState(currentBoard);

        char winner = checkWinner();
        printWinner(winner);

        if (game_mode == 1 && player_turn == 2) {
            computer_Move();
        }
    }
}

void gameBoard_mode2(GtkWidget *window){
    //Scoreboard label
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), vbox); 
    status_label = gtk_label_new("Player 1's Turn");
    gtk_widget_set_name(status_label, "label");
    gtk_widget_set_halign(status_label, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(vbox), status_label, FALSE, FALSE, 10);

    result_label = gtk_label_new("");  // Empty label initially
    gtk_widget_set_name(result_label, "result-label");
    gtk_box_pack_start(GTK_BOX(vbox), result_label, FALSE, FALSE, 10);

    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);

    //Creating 9 buttons on the grid
      for (int i = 0; i < 9; i++) {
        buttons[i] = gtk_button_new_with_label("");  // Initially empty
        g_signal_connect(buttons[i], "clicked", G_CALLBACK(inputHandler), GINT_TO_POINTER(i));
        gtk_widget_set_size_request(buttons[i], 200, 200);
        gtk_grid_attach(GTK_GRID(grid), buttons[i], i % 3, i / 3, 1, 1);  // Add to grid
    }

    gtk_box_pack_start(GTK_BOX(vbox), grid, TRUE, TRUE, 10); 

    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
       
    GtkWidget *reset_button= gtk_button_new_with_label("Reset");
    g_signal_connect(reset_button, "clicked", G_CALLBACK(resetBoard), window);
    gtk_box_pack_start(GTK_BOX(hbox), reset_button, TRUE, TRUE, 0);
    gtk_widget_set_name(reset_button, "hbox-button");

    GtkWidget *exit_button= gtk_button_new_with_label("Exit");
    g_signal_connect(exit_button, "clicked", G_CALLBACK(exitGame), window);
    gtk_box_pack_start(GTK_BOX(hbox), exit_button, TRUE, TRUE, 0);
    gtk_widget_set_name(exit_button, "hbox-button");

     gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 10);

}

char checkWinner(){

    //check rows
    for(int row = 0; row < 3; row++){
        if (board[row][0] == board[row][1] &&  board[row][0] == board[row][2]){
            return board[row][0];
        }
    }
    // check columns
    for(int col = 0; col < 3; col++){  
        if (board[0][col] == board[1][col] && board[0][col] == board[2][col]){
            return board[0][col];
        }
    }
    // check diagonal
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2]){
        return board[0][0];
    }

    if (board[0][2] == board[1][1] && board[1][1] == board[2][0]){
        return board[0][2];
    }

    return '\0';
}

bool checkTie(){
     for(int row = 0; row < 3; row++){
        for(int col = 0; col < 3; col++){
            if (board[row][col] == '\0')
                return false;
            }
     }

    return true;
}

void printWinner(char winner){
    if (winner == '\0' && checkTie()){
        g_print("\nIt is a tie!\n");
        gtk_label_set_text(GTK_LABEL(result_label), "It is a tie!");
        endGame();
    }   
    else if (winner == 'X'){
        g_print("\nPlayer 1 Wins!\n");
        gtk_label_set_text(GTK_LABEL(result_label), "Player 1 Wins!");
        endGame();
    }
    else if (winner == 'O'){
        g_print("\nPlayer 2 wins!\n");
        gtk_label_set_text(GTK_LABEL(result_label), "Player 2 Wins!");
        endGame();
    }
    else{}
}

void endGame(){
    for(int i = 0; i < 9; i++){
        gtk_widget_set_sensitive(buttons[i], FALSE); //disable all buttons 
    }
}


void resetBoard(GtkWidget *widget, gpointer window){
        for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            board[row][col] = '\0';  // Reset each cell to empty
        }
    }

    for (int i = 0; i < 9; i++) {
        gtk_button_set_label(GTK_BUTTON(buttons[i]), "");  // Clear button text
        gtk_widget_set_sensitive(buttons[i], TRUE);  // Enable all buttons
    }

    player_turn = 1;
    gtk_label_set_text(GTK_LABEL(status_label), "Player 1's Turn");  // Reset status label
    gtk_label_set_text(GTK_LABEL(result_label), "");  // Clear the result label
}

void exitGame(GtkWidget *widget, gpointer window) {
    clear_window(window);
    gameMenu(window);
    gtk_widget_show_all(window);
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;

    // Create a new window associated with the application
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Tic Tac Toe");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 900);

    load_css();
    gameMenu(window);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
}

int main(int argc, char *argv[]) {
    GtkApplication *app;
    int status;

    // Create a new application
    app = gtk_application_new("com.example.tictactoe", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    // Run the application
    status = g_application_run(G_APPLICATION(app), argc, argv);

    // Clean up the application object
    g_object_unref(app);

    return status;
}






