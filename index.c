#define SDL_MAIN_HANDLED
#include <gtk/gtk.h>
#include <gtk/gtkmain.h>
#include <stdbool.h>
#include <cairo.h>
#include <limits.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <time.h>
#include <unistd.h>
#include "header.h"


// Game State Variables
char board[3][3];
char output[3][3];
GtkWidget *players[2]; // 0
GtkWidget *buttons[9];
int game_mode = 0;
int player_turn = 1; // 1 for X, 2 for O
char player = 'X', opponent ='O'; //Identify player and opponent
GtkWidget *switch_button;
GtkWidget *status_label;
GtkWidget *result_label;
const char *selected_difficulty = "Easy"; 
Mix_Music *bgMusic = NULL;            // Background music
Mix_Chunk *clickSound = NULL;         // Click sound effect
Mix_Chunk *buttonClicks = NULL;
Mix_Chunk *gamewinSound = NULL;
Mix_Chunk *gamedrawSound = NULL;


struct WinnerResult {
    char winner;
    int winning_cells[3];
};

struct WinnerResult result;

void gameMenu();
void main_page();
void gameBoard_mode1();
void gameBoard_mode2();
void getGtkBoardState();
struct WinnerResult checkWinner();
bool checkTie();
void printWinner();
void endGame();
void resetBoard();
void exitGame();

void load_sounds() {
    bgMusic = Mix_LoadMUS("sounds/background.wav");
    if (!bgMusic) {
        printf("Failed to load background music: %s\n", Mix_GetError());
    }

    clickSound = Mix_LoadWAV("sounds/clicks.wav");
    if (!clickSound) {
        printf("Failed to load click sound: %s\n", Mix_GetError());
    }

    buttonClicks = Mix_LoadWAV("sounds/button-clicks.wav");
    if(!buttonClicks){
         printf("Failed to load button-clicks sound: %s\n", Mix_GetError());
    }

    gamewinSound = Mix_LoadWAV("sounds/game-win.wav");
    if (!gamewinSound) {
        printf("Failed to load game-win sound: %s\n", Mix_GetError());
    }
    gamedrawSound = Mix_LoadWAV("sounds/game-draw.wav");
    if (!gamedrawSound) {
        printf("Failed to load game-win sound: %s\n", Mix_GetError());
    }

}

// Function to initialize background music
void background_music() {
        Mix_PlayMusic(bgMusic, -1); // Loop indefinitely
}

void click_sounds() {
     Mix_PlayChannel(-1, clickSound, 0); // Play click sound
     Mix_Volume(-1, MIX_MAX_VOLUME); // Set for all channels
}

void buttonClicks_sounds(){
     Mix_PlayChannel(-1, buttonClicks, 0); // Play click sound
     Mix_Volume(-1, MIX_MAX_VOLUME); // Set for all channels
}

void gamewin_sounds(){
    if (Mix_PlayingMusic()) {
        Mix_HaltMusic();
    }
   Mix_PlayChannel(-1, gamewinSound, 0); // Play win sound
   Mix_Volume(-1, MIX_MAX_VOLUME); // Set for all channels

    if (bgMusic != NULL) {
        Mix_PlayMusic(bgMusic, -1); // Loop the background music
    }
}

void gamedraw_sounds(){
    if (Mix_PlayingMusic()) {
        Mix_HaltMusic();
    }
   Mix_PlayChannel(-1, gamedrawSound, 0); // Play Draw sound
   Mix_Volume(-1, MIX_MAX_VOLUME); // Set for all channels

    if (bgMusic != NULL) {
        Mix_PlayMusic(bgMusic, -1); // Loop the background music
    }
}

void cleanup_sounds() {
    if (clickSound != NULL) {
        Mix_FreeChunk(clickSound);
        clickSound = NULL;
    }
    if(gamewinSound != NULL){
        Mix_FreeChunk(gamewinSound);
        gamewinSound = NULL;
    }
    if(gamedrawSound != NULL){
        Mix_FreeChunk(gamewinSound);
        gamewinSound = NULL;
    }
    if (bgMusic != NULL) {
        Mix_FreeMusic(bgMusic);
        bgMusic = NULL;
    }
    Mix_CloseAudio();
}

void load_css() {
    GtkCssProvider *provider = gtk_css_provider_new();
    GdkDisplay *display = gdk_display_get_default();

    // Load the external CSS file
    gtk_css_provider_load_from_path(provider, "style.css");

    gtk_style_context_add_provider_for_display(display, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(provider);  // Free the provider after use
}

gboolean force_style_update(gpointer data) {
    // No operation, just ensure the main loop processes pending events
    return FALSE; // Return FALSE to remove the idle callback after execution
}

void on_difficulty_changed(GtkDropDown *dropdown, GParamSpec *pspec, gpointer user_data) {
    click_sounds();
    resetBoard();
    GtkStringObject *selected_item = GTK_STRING_OBJECT(gtk_drop_down_get_selected_item(dropdown));
    if (selected_item != NULL) {
        const char *difficulty = gtk_string_object_get_string(selected_item);
        
        selected_difficulty = difficulty; 
        g_print("Difficulty changed to: %s\n", difficulty); 
    }
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


bool win_found(char board[3][3]) {
    int score = eval_board(board);
    
    if (score == 10) {
        return true;
    } else if (score == -10) {
        return true;  // Opponent wins
    } else if (score == 0 && !MovesLeft(board)) {
        return true;  // It's a draw
    }

    return false;  // Game is still ongoing
}

void active_player(GtkWidget *playerX, GtkWidget *playerO){
    g_print("Switching active player. Current player_turn: %d\n", player_turn);
    if (player_turn == 1){
        // Highlight Player X
        gtk_widget_remove_css_class(playerO, "active-player");
        gtk_widget_add_css_class(playerX, "active-player");
    }
    else if (player_turn == 2) {
        // Highlight Player O
        gtk_widget_remove_css_class(playerX, "active-player");
        gtk_widget_add_css_class(playerO, "active-player");

    }
    else{}

    g_idle_add(force_style_update, NULL);
}
void computer_Move() {
    struct Move best_move;
   // Check the selected difficulty and use the appropriate method
    if (strcmp(selected_difficulty, "Naive") == 0) {
        // Use the Naive Bayes model to determine the move
        best_move = get_naive_bayes_move();

    } else if (strcmp(selected_difficulty, "Epsilon Greedy") == 0) {
        //Use reinforcement learning with Epsilon Greedy model to determine move
        loadQ_Table("RL-epsilon-greedy/q_table_100k.txt");
        best_move = get_epsilonGreedy_move(board, player_turn);

    }else if (strcmp(selected_difficulty, "Hard") == 0){
        // Use the Minimax algorithm for other difficulty levels
        best_move = find_best_move(board);
    }
    else if (strcmp(selected_difficulty, "Easy") == 0) {
        // Use the Minimax algorithm for other difficulty levels
        best_move = diff_find_best_move(board);
    }
    else if (strcmp(selected_difficulty, "Normal") == 0) {
        // Use the Minimax algorithm for other difficulty levels
        best_move = diff_find_best_move(board);
    }
    else if (strcmp(selected_difficulty, "K-Means") == 0) {
        // Load centroids from the file
        load_centroids("k-means/centroids.csv");
        // Use the K-Means algorithm to determine move
        best_move = kmeans_find_best_move(board);
    }
    else if (strcmp(selected_difficulty, "SVM") == 0) {
        // Use the Support Vector Machine model to determine the move
        best_move = get_SVM_move();
    }    

     if (!win_found(board)) {
        if (best_move.row != -1 && best_move.col != -1) {
            // double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
            // printf("Time taken by AI to make move: %f seconds\n", time_taken);
            
            // srand(time(NULL));
            // int delay_seconds = (rand() % 3) + 1;
            // printf("Delaying for %d seconds to simulate thinking\n", delay_seconds);
            // sleep(delay_seconds);

            board[best_move.row][best_move.col] = opponent;  // Bot's symbol
            gtk_button_set_label(GTK_BUTTON(buttons[best_move.row * 3 + best_move.col]), 
                                 (opponent == 'X') ? "X" : "O");

            // Check for a winner after the bot's move
            struct WinnerResult winner = checkWinner();
            printWinner(winner);

            // Switch back to the player's turn
            player_turn = (opponent == 'X') ? 2 : 1;

            // Update the active player visually
            active_player(players[0], players[1]);
        }
    }
}



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
    gtk_widget_set_name(button_1p, "player-mode");

    //"2 Players" button
    button_2p = gtk_button_new_with_label("2 Player");
    g_signal_connect(button_2p, "clicked", G_CALLBACK(main_page), GINT_TO_POINTER(2));
    gtk_box_append(GTK_BOX(box), button_2p);
    gtk_widget_set_name(button_2p, "player-mode");

}

void clear_window(GtkWidget *window) {
    GtkWidget *child = gtk_window_get_child(GTK_WINDOW(window));
    if (child != NULL) {
        gtk_window_set_child(GTK_WINDOW(window), NULL);  // Remove the child safely
    }
}

void set_player_turn(GtkWidget *widget, gpointer data) {
    click_sounds();
    player_turn = GPOINTER_TO_INT(data); // Set the starting player: 1 for X, 2 for O

    // Disable Player X and Player O buttons
    gtk_widget_set_sensitive(players[0], FALSE);
    gtk_widget_set_sensitive(players[1], FALSE);

    // Highlight the active player
    active_player(players[0], players[1]);

    if (player_turn == 2) {
        computer_Move(); // If the bot is O, make the first move
    }
}



void main_page(GtkWidget *widget, gpointer data) {
    click_sounds();
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

    if (board[row][col] == '\0') {  // If the cell is empty
        char player_symbol = (player_turn == 1) ? 'X' : 'O';
        gtk_button_set_label(GTK_BUTTON(widget), (player_turn == 1) ? "X" : "O");
        board[row][col] = player_symbol;
        buttonClicks_sounds();

        // Switch turn
        player_turn = (player_turn == 1) ? 2 : 1;

        // Update the active player visually
        active_player(players[0], players[1]);

        // Check for a winner
        struct WinnerResult winner = checkWinner();
        printWinner(winner);

        // Trigger bot move if in player mode = 1
        if (game_mode == 1 && player_turn == 2) {
            computer_Move();
        }
    }
}


void on_switch_clicked(GtkWidget *button, gpointer data) {
    g_print("Switching player turn.\n");

    // Toggle the current player's turn
    player_turn = (player_turn == 1) ? 2 : 1;

    // Reset the board while keeping the current player_turn
    resetBoard(NULL, NULL);

    // Update active player indicator
    GtkWidget **boxes = (GtkWidget **)data;
    active_player(boxes[0], boxes[1]);

    // If it's the bot's turn, let the bot make the move
    if (game_mode == 1 && player_turn == 2) {
        g_print("Bot makes the first move as Player O.\n");
        computer_Move();
    }
}


void gameBoard_mode1(GtkWidget *window) {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_window_set_child(GTK_WINDOW(window), vbox); 

    // Difficulty dropdown
    GtkStringList *string_list = gtk_string_list_new((const char *[]) {"Easy", "Normal", "Hard", "Naive", "Epsilon Greedy", "K-Means", "SVM", NULL});
    GtkWidget *dropdown = gtk_drop_down_new(G_LIST_MODEL(string_list), NULL);

    // Set the ID for CSS styling and alignment
    gtk_widget_set_name(dropdown, "dropdown");
    gtk_widget_set_size_request(dropdown, 70, 30);  // Set dropdown size
    gtk_widget_set_halign(dropdown, GTK_ALIGN_START);  // Align to the left

    // Connect the signal to handle selection changes
    g_signal_connect(dropdown, "notify::selected", G_CALLBACK(on_difficulty_changed), NULL);

    // Add dropdown to the vbox
    gtk_box_append(GTK_BOX(vbox), dropdown);

     GtkWidget *status_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,20);
    gtk_widget_set_halign(status_hbox, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(vbox), status_hbox);
    gtk_widget_set_name(status_hbox, "player-section");

  // Player X section
    GtkWidget *playerX_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5); // Vertical box for "Player" and "X"
    GtkWidget *playerX = gtk_button_new_with_label("Player\n    X");
    gtk_widget_set_name(playerX, "player-box");
    gtk_box_append(GTK_BOX(playerX_box), playerX);
    gtk_box_append(GTK_BOX(status_hbox), playerX_box);
    g_signal_connect(playerX, "clicked", G_CALLBACK(set_player_turn), GINT_TO_POINTER(1)); // Set X as the starting player

    // Player O section
    GtkWidget *playerO_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5); // Vertical box for "Player" and "O"
    GtkWidget *playerO = gtk_button_new_with_label("Player\n    O");
    gtk_widget_set_name(playerO, "player-box");
    gtk_box_append(GTK_BOX(playerO_box), playerO);
    gtk_box_append(GTK_BOX(status_hbox), playerO_box);
    g_signal_connect(playerO, "clicked", G_CALLBACK(set_player_turn), GINT_TO_POINTER(2)); // Set O as the starting player


     // Store Player Widgets for Active Highlighting
    players[0] = playerX; // Player X
    players[1] = playerO; // Player O
    active_player(playerX, playerO);
 
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

    result_label = gtk_label_new("");  // Empty label initially
    gtk_widget_set_name(result_label, "result-label");
    gtk_box_append(GTK_BOX(vbox), result_label);

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
    gtk_widget_set_name(vbox, "main-vbox");
    gtk_window_set_child(GTK_WINDOW(window), vbox);  

    GtkWidget *status_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,20);
    gtk_widget_set_halign(status_hbox, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(vbox), status_hbox);
    gtk_widget_set_name(status_hbox, "player-section");

    GtkWidget *playerX = gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
    GtkWidget *x_label = gtk_label_new("Player");
    GtkWidget *x_sublabel = gtk_label_new("X");
    gtk_box_append(GTK_BOX(playerX), x_label);
    gtk_box_append(GTK_BOX(playerX), x_sublabel);
    gtk_widget_set_name(playerX, "player-box");
    gtk_box_append(GTK_BOX(status_hbox), playerX);

    GtkWidget *switch_button = gtk_button_new_with_label("↔"); 
    gtk_widget_set_name(switch_button, "switch-button");
     gtk_widget_set_size_request(switch_button, 30, 20);
     gtk_box_append(GTK_BOX(status_hbox), switch_button);

    GtkWidget *playerO = gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
    GtkWidget *o_label = gtk_label_new("Player");
    GtkWidget *o_sublabel = gtk_label_new("O");
    gtk_box_append(GTK_BOX(playerO), o_label);
    gtk_box_append(GTK_BOX(playerO), o_sublabel);
    gtk_widget_set_name(playerO, "player-box");
    gtk_box_append(GTK_BOX(status_hbox), playerO);

    // Dynamically allocate memory for player_boxes
    GtkWidget **player = g_malloc(2 * sizeof(GtkWidget *));
    players[0] = playerX;
    players[1] = playerO;

    // Connect the button signal with the dynamically allocated player_boxes
    g_signal_connect(switch_button, "clicked", G_CALLBACK(on_switch_clicked), players);

    // Initialize active player
    active_player(playerX, playerO); 

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

    result_label = gtk_label_new("");  // Empty label initially
    gtk_widget_set_name(result_label, "result-label");
    gtk_box_append(GTK_BOX(vbox), result_label);

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
        gamedraw_sounds();
    }   
    else if (result.winner == 'X' || result.winner == 'O') {
        g_print("\nPlayer %c Wins!\n", (result.winner == 'X') ? 'X' : 'O');
        gtk_label_set_text(GTK_LABEL(result_label), (result.winner == 'X') ? "Player X Wins!" : "Player O Wins!");
        highlight_winning_cells(result.winning_cells[0], result.winning_cells[1], result.winning_cells[2]); 
        endGame();
        gamewin_sounds();
    }
    else{}
}

void endGame(){
    for(int i = 0; i < 9; i++){
        gtk_widget_set_sensitive(buttons[i], FALSE); //disable all buttons 
    }

    if (GTK_IS_WIDGET(switch_button)) {
        gtk_widget_set_sensitive(switch_button, FALSE); // Disable the switch button
    }
}

void resetBoard(GtkWidget *widget, gpointer window) {
    click_sounds();

    // Clear the game board array
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            board[row][col] = '\0';  // Reset each cell to empty
        }
    }

    // Reset button labels and enable all buttons
    for (int i = 0; i < 9; i++) {
        gtk_button_set_label(GTK_BUTTON(buttons[i]), "");  // Clear button text
        gtk_widget_remove_css_class(buttons[i], "winning-cells");
        gtk_widget_set_sensitive(buttons[i], TRUE);
    }

    // Enable Player X and Player O buttons
    gtk_widget_set_sensitive(players[0], TRUE);
    gtk_widget_set_sensitive(players[1], TRUE);

    player_turn = 1;
    active_player(players[0], players[1]); // Default back to Player X 

    result.winner = '\0';
    for (int i = 0; i < 3; i++) {
        result.winning_cells[i] = -1;
    }

    gtk_label_set_text(GTK_LABEL(result_label), "");
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

void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;

    // Create a new window associated with the application
    window = gtk_application_window_new(app);
    gtk_window_set_default_size(GTK_WINDOW(window), 700, 800);

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
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
    load_sounds();
    

    // Create a new application
    app = gtk_application_new("com.example.tictactoe", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    background_music();

    // Run the application
    status = g_application_run(G_APPLICATION(app), argc, argv);
    

    // Clean up the application object
    g_object_unref(app);
    cleanup_sounds();
    g_free(players);

    return status;
}







