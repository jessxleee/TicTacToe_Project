#include <gtk/gtk.h>
#include <stdbool.h>
#include <cairo.h>
//#include <minmax.h>
#include <limits.h>
#include <stdio.h>

// Game State Variables
char board[3][3];
char output[3][3];
GtkWidget *buttons[9];
int game_mode = 0;
int player_turn = 1; // 1 for X, 2 for O
char player = 'X', opponent ='O'; //Identify player and opponent
GtkWidget *status_label;
GtkWidget *result_label;
const char *selected_difficulty = "Easy"; 

/*Datatype structure to represent board*/
struct Move
{
    int row,col; /*row and column of move*/
};

struct WinnerResult {
    char winner;
    int winning_cells[3];
};

struct WinnerResult result;

void gameMenu();
void main_page();
void gameBoard_mode1();
void gameBoard_mode2();
int MovesLeft();
int minmax(int depth, bool ismax);
struct Move find_best_move();
void getGtkBoardState();
struct WinnerResult checkWinner();
bool checkTie();
void printWinner();
void endGame();
void resetBoard();
void exitGame();

void load_css() {
    GtkCssProvider *provider = gtk_css_provider_new();
    GdkDisplay *display = gdk_display_get_default();

    // Load the external CSS file
    gtk_css_provider_load_from_path(provider, "style.css");

    gtk_style_context_add_provider_for_display(display, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(provider);  // Free the provider after use
}

/* NAIVE BAYES */
/* Function to execute classification.py using Popen and retrieve the Naive Bayes move */
struct Move get_naive_bayes_move() {
    FILE *fp;
    char path[1035];
    struct Move best_move = {-1, -1}; // Default to invalid move
    
    // Construct the command with the board state
    char command[256] = "python3 naive-bayes/classification.py";
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            char cell[3];
            sprintf(cell, " %c", board[i][j] == '\0' ? 'b' : board[i][j]);
            strcat(command, cell);
        }
    }

    // Open the command for reading
    fp = popen(command, "r");
    if (fp == NULL) {
        printf("Failed to run command\n");
        return best_move;
    }

    // Read the output a line at a time - output expected as "row col"
    if (fgets(path, sizeof(path), fp) != NULL) {
        sscanf(path, "%d %d", &best_move.row, &best_move.col);
    }

    // Close the process
    pclose(fp);

    return best_move;
}



void on_difficulty_changed(GtkDropDown *dropdown, GParamSpec *pspec, gpointer user_data) {
    GtkStringObject *selected_item = GTK_STRING_OBJECT(gtk_drop_down_get_selected_item(dropdown));
    if (selected_item != NULL) {
        const char *difficulty = gtk_string_object_get_string(selected_item);
        
        selected_difficulty = difficulty; 
        g_print("Difficulty changed to: %s\n", difficulty); 
    }
}


/*Check if there are any moves left on the board*/
int MovesLeft(){

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
int eval_board(){
    
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
int minmax(int depth, bool ismax){

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

                    int result = minmax(depth + 1, false); /*Recursive minmax algorithm and compare the scores*/

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

                    int result = minmax(depth + 1, true); /*Recursive minmax recursively and compare scores*/

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
struct Move find_best_move(){

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
                int move = minmax(0, false);

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


void getGtkBoardState() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            GtkButton *button = GTK_BUTTON(buttons[i * 3 + j]);  // Get the corresponding button
            const gchar *label = gtk_button_get_label(button);

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
    struct Move best_move;

        // Check the selected difficulty and use the appropriate method
    if (strcmp(selected_difficulty, "Naive") == 0) {
        // Use the Naive Bayes model to determine the move
        best_move = get_naive_bayes_move();
    } else {
        // Use the Minimax algorithm for other difficulty levels
        best_move = find_best_move();
    }

    if (best_move.row != -1 && best_move.col != -1) {  // Check if a valid move was found
        board[best_move.row][best_move.col] = 'O';  // Update the board
        gtk_button_set_label(GTK_BUTTON(buttons[best_move.row * 3 + best_move.col]), "O");  // Update the button label

        player_turn = 1;  // Switch back to player 1
        gtk_label_set_text(GTK_LABEL(status_label), "Player 1's Turn");  // Update status

        struct WinnerResult winner = checkWinner();
        printWinner(winner);

    }
}

/*
void computer_Move() {
    struct Move best_move = find_best_move(board);  // Get the best move using Minimax

    if (best_move.row != -1 && best_move.col != -1) {  // Check if a valid move was found
        board[best_move.row][best_move.col] = 'O';  // Update the board
        gtk_button_set_label(GTK_BUTTON(buttons[best_move.row * 3 + best_move.col]), "O");  // Update the button label

        player_turn = 1;  // Switch back to player 1
        gtk_label_set_text(GTK_LABEL(status_label), "Player 1's Turn");  // Update status

        struct WinnerResult winner = checkWinner();
        printWinner(winner);

    }
}
*/

void gameMenu(GtkWidget *window){
    GtkWidget *box;
    GtkWidget *button_1p, *button_2p;

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
    gtk_window_set_child(GTK_WINDOW(window), box);

    // Header
    GtkWidget *heading = gtk_label_new("TIC TAC TOE");
    gtk_widget_set_name(heading, "heading");
    gtk_widget_set_halign(heading, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(box), heading);

    // icon
    GtkWidget *picture = gtk_picture_new_for_filename("images/icon.png");
    gtk_box_append(GTK_BOX(box), picture);
    gtk_widget_set_name(picture, "picture");

    // "1 Player" button
    button_1p = gtk_button_new_with_label("1 Player");
    g_signal_connect(button_1p, "clicked", G_CALLBACK(main_page), GINT_TO_POINTER(1));
    gtk_box_append(GTK_BOX(box), button_1p);
    gtk_widget_set_name(button_1p, "players-button");

    //"2 Players" button
    button_2p = gtk_button_new_with_label("2 Player");
    g_signal_connect(button_2p, "clicked", G_CALLBACK(main_page), GINT_TO_POINTER(2));
    gtk_box_append(GTK_BOX(box), button_2p);
    gtk_widget_set_name(button_2p, "players-button");

}

void clear_window(GtkWidget *window) {
    GtkWidget *child = gtk_window_get_child(GTK_WINDOW(window));
    if (child != NULL) {
        gtk_window_set_child(GTK_WINDOW(window), NULL);  // Remove the child safely
    }
}

void main_page(GtkWidget *widget, gpointer data) {
    GtkWidget *window = GTK_WIDGET(gtk_widget_get_root(widget));  
    game_mode = GPOINTER_TO_INT(data);

    clear_window(window);

    if (game_mode == 1) {
        g_print("1 Player game mode selected.\n");
        clear_window(window);
        gameBoard_mode1(window);

    } else if (game_mode == 2) {
        g_print("2 Players game mode selected.\n");
        clear_window(window);
        gameBoard_mode2(window);

    }
    gtk_widget_set_visible(window, TRUE);
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

        struct WinnerResult winner = checkWinner();
        printWinner(winner);

        if (game_mode == 1 && player_turn == 2) {
            computer_Move();
        }
    }
}

void gameBoard_mode1(GtkWidget *window) {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_window_set_child(GTK_WINDOW(window), vbox); 

     // Difficulty dropdown
     GtkStringList *string_list = gtk_string_list_new((const char *[]) {"Easy", "Normal", "Hard", "Naive", NULL});
    GtkWidget *dropdown = gtk_drop_down_new(G_LIST_MODEL(string_list), NULL);

    // Set the ID for CSS styling and alignment
    gtk_widget_set_name(dropdown, "dropdown");
    gtk_widget_set_size_request(dropdown, 70, 30);  // Set dropdown size
    gtk_widget_set_halign(dropdown, GTK_ALIGN_START);  // Align to the left

    // Connect the signal to handle selection changes
    g_signal_connect(dropdown, "notify::selected", G_CALLBACK(on_difficulty_changed), NULL);

    // Add dropdown to the vbox
    gtk_box_append(GTK_BOX(vbox), dropdown);

    // Scoreboard label
    status_label = gtk_label_new("Player 1's Turn");
    gtk_widget_set_name(status_label, "label");
    gtk_widget_set_halign(status_label, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(vbox), status_label);

    result_label = gtk_label_new("");  // Empty label initially
    gtk_widget_set_name(result_label, "result-label");
    gtk_box_append(GTK_BOX(vbox), result_label);

    GtkWidget *grid = gtk_grid_new();
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(vbox), grid);

    // Creating 9 buttons on the grid
    for (int i = 0; i < 9; i++) {
        buttons[i] = gtk_button_new_with_label("");  // Initially empty
        g_signal_connect(buttons[i], "clicked", G_CALLBACK(inputHandler), GINT_TO_POINTER(i));
        gtk_widget_set_size_request(buttons[i], 200, 200);
        gtk_grid_attach(GTK_GRID(grid), buttons[i], i % 3, i / 3, 1, 1);  // Add to grid
    }

    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
     gtk_widget_set_halign(hbox, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(vbox), hbox);

    GtkWidget *reset_button = gtk_button_new_with_label("Reset");
    g_signal_connect(reset_button, "clicked", G_CALLBACK(resetBoard), window);
    gtk_widget_set_name(reset_button, "hbox-button");
    gtk_box_append(GTK_BOX(hbox), reset_button);

    GtkWidget *exit_button = gtk_button_new_with_label("Exit");
    g_signal_connect(exit_button, "clicked", G_CALLBACK(exitGame), window);
    gtk_widget_set_name(exit_button, "hbox-button");
    gtk_box_append(GTK_BOX(hbox), exit_button);
}

void gameBoard_mode2(GtkWidget *window) {
    // Scoreboard label
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_window_set_child(GTK_WINDOW(window), vbox);  
    status_label = gtk_label_new("Player 1's Turn");
    gtk_widget_set_name(status_label, "label");
    gtk_widget_set_halign(status_label, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(vbox), status_label);

    result_label = gtk_label_new("");  // Empty label initially
    gtk_widget_set_name(result_label, "result-label");
    gtk_box_append(GTK_BOX(vbox), result_label);

    GtkWidget *grid = gtk_grid_new();
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(grid, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(vbox), grid);

    // Creating 9 buttons on the grid
    for (int i = 0; i < 9; i++) {
        buttons[i] = gtk_button_new_with_label("");  // Initially empty
        g_signal_connect(buttons[i], "clicked", G_CALLBACK(inputHandler), GINT_TO_POINTER(i));
        gtk_widget_set_size_request(buttons[i], 200, 200);
        gtk_grid_attach(GTK_GRID(grid), buttons[i], i % 3, i / 3, 1, 1);  // Add to grid
    }

    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_halign(hbox, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(vbox), hbox);

    GtkWidget *reset_button = gtk_button_new_with_label("Reset");
    g_signal_connect(reset_button, "clicked", G_CALLBACK(resetBoard), window);
    gtk_widget_set_name(reset_button, "hbox-button");
    gtk_box_append(GTK_BOX(hbox), reset_button);

    GtkWidget *exit_button = gtk_button_new_with_label("Exit");
    g_signal_connect(exit_button, "clicked", G_CALLBACK(exitGame), window);
    gtk_widget_set_name(exit_button, "hbox-button");
    gtk_box_append(GTK_BOX(hbox), exit_button);
}

struct WinnerResult checkWinner(){

    struct WinnerResult result;
    result.winner = '\0';
    //check rows
    for(int row = 0; row < 3; row++){
        if (board[row][0] != '\0' && board[row][0] == board[row][1] &&  board[row][0] == board[row][2]){
            result.winning_cells[0] = row * 3 + 0;
            result.winning_cells[1] = row * 3 + 1;
            result.winning_cells[2] = row * 3 + 2;
            result.winner = board[row][0];
            return result;
        }
    }
    // check columns
    for(int col = 0; col < 3; col++){  
        if (board[0][col] != '\0' && board[0][col] == board[1][col] && board[0][col] == board[2][col]){
            result.winning_cells[0] = 0 * 3 + col;
            result.winning_cells[1] = 1 * 3 + col;
            result.winning_cells[2] = 2 * 3 + col;
            result.winner =  board[0][col];
            return result;
        }
    }
    // check diagonal
    if (board[0][0] != '\0' && board[0][0] == board[1][1] && board[1][1] == board[2][2]){
        result.winning_cells[0] = 0 * 3 + 0;
        result.winning_cells[1] = 1 * 3 + 1;
        result.winning_cells[2] = 2 * 3 + 2;
        result.winner = board[0][0];
        return result;
    }

    if (board[0][2] != '\0' && board[0][2] == board[1][1] && board[1][1] == board[2][0]){
        result.winning_cells[0] = 0 * 3 + 2;
        result.winning_cells[1] = 1 * 3 + 1;
        result.winning_cells[2] = 2 * 3 + 0;
        result.winner = board[0][2];
        return result;
    }

    return result;
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

void highlight_winning_cells(int cell1, int cell2, int cell3){
    gtk_widget_remove_css_class(buttons[cell1], "default-cell");
    gtk_widget_remove_css_class(buttons[cell2], "default-cell");
    gtk_widget_remove_css_class(buttons[cell3], "default-cell");
    
    gtk_widget_add_css_class(buttons[cell1], "winning-cells");
    gtk_widget_add_css_class(buttons[cell2], "winning-cells");
    gtk_widget_add_css_class(buttons[cell3], "winning-cells");
}

void printWinner(struct WinnerResult result){
    if (result.winner == '\0' && checkTie()){
        g_print("\nIt is a tie!\n");
        gtk_label_set_text(GTK_LABEL(result_label), "It is a tie!");
        endGame();
    }   
    else if (result.winner == 'X' || result.winner == 'O') {
        g_print("\nPlayer %c Wins!\n", (result.winner == 'X') ? '1' : '2');
        gtk_label_set_text(GTK_LABEL(result_label), (result.winner == 'X') ? "Player 1 Wins!" : "Player 2 Wins!");
        highlight_winning_cells(result.winning_cells[0], result.winning_cells[1], result.winning_cells[2]); 
        endGame();
    }
    else{}
}


void endGame(){
    for(int i = 0; i < 9; i++){
        gtk_widget_set_sensitive(buttons[i], FALSE); //disable all buttons 
    }
}

void resetBoard(GtkWidget *widget, gpointer window) {
    // Clear the game board array
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            board[row][col] = '\0';  // Reset each cell to empty
        }
    }

    // Reset button labels, CSS classes, and enable buttons
    for (int i = 0; i < 9; i++) {
        gtk_button_set_label(GTK_BUTTON(buttons[i]), "");  // Clear button text
        gtk_widget_remove_css_class(buttons[i], "winning-cells");   // Remove "winning-cells" class
        gtk_widget_set_sensitive(buttons[i], TRUE);  // Enable all buttons
    }

    // Reset the winning cells in the global `result` struct
    result.winner = '\0';  // Clear the winner character
    for (int i = 0; i < 3; i++) {
        result.winning_cells[i] = -1;  // Set to -1 to indicate no winning cells
    }
    load_css();

    // Reset other game state variables
    player_turn = 1;
    gtk_label_set_text(GTK_LABEL(status_label), "Player 1's Turn");  // Reset status label
    gtk_label_set_text(GTK_LABEL(result_label), "");  // Clear the result label
    g_print("\nGame has been reset.\n");
}

void exitGame(GtkWidget *widget, gpointer window) {
    resetBoard(widget,window);
    clear_window(window);
    gameMenu(window);
    gtk_widget_set_visible(window, TRUE);
    g_print("\nExit Game.\n");
}

void on_minimize_clicked(GtkWidget *widget, gpointer window) {
    gtk_window_minimize(GTK_WINDOW(window));
}

void on_maximize_clicked(GtkWidget *widget, gpointer window) {
    if (gtk_window_is_maximized(window)) {
        gtk_window_unmaximize(window);
    } else {
        gtk_window_maximize(window);
    }
}

void on_close_clicked(GtkWidget *widget, gpointer window) {
    gtk_window_close(GTK_WINDOW(window));
}

GtkWidget* headerbar(GtkWidget *window) {
   GtkWidget *header_bar = gtk_header_bar_new();
    gtk_header_bar_set_show_title_buttons(GTK_HEADER_BAR(header_bar), FALSE);  // Hide default buttons

    // Application icon
    GtkWidget *app_icon = gtk_image_new_from_icon_name("applications-games-symbolic");
    gtk_header_bar_pack_start(GTK_HEADER_BAR(header_bar), app_icon);

    // Close button
    GtkWidget *close_button = gtk_button_new_from_icon_name("window-close-symbolic");
    g_signal_connect(close_button, "clicked", G_CALLBACK(on_close_clicked), window);
    gtk_header_bar_pack_end(GTK_HEADER_BAR(header_bar), close_button);

    // Maximize button
    GtkWidget *maximize_button = gtk_button_new_from_icon_name("window-maximize-symbolic");
    g_signal_connect(maximize_button, "clicked", G_CALLBACK(on_maximize_clicked), window);
    gtk_header_bar_pack_end(GTK_HEADER_BAR(header_bar), maximize_button);

    // Minimize button
    GtkWidget *minimize_button = gtk_button_new_from_icon_name("window-minimize-symbolic");
    g_signal_connect(minimize_button, "clicked", G_CALLBACK(on_minimize_clicked), window);
    gtk_header_bar_pack_end(GTK_HEADER_BAR(header_bar), minimize_button);



    return header_bar;
}



static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;

    // Create a new window associated with the application
    window = gtk_application_window_new(app);
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 900);

    GtkWidget *header_bar = headerbar(window);
    gtk_window_set_titlebar(GTK_WINDOW(window), header_bar);

    load_css();
    gameMenu(window);
    g_signal_connect(window, "destroy", G_CALLBACK(g_application_quit), app);
    gtk_widget_set_visible(window, TRUE);
    
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







