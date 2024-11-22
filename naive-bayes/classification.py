import random
import sys
# from sklearn.metrics import confusion_matrix, ConfusionMatrixDisplay
# import matplotlib.pyplot as plt

# Process the dataset for use by the model
def process_data(data):
    processed_data = []
    for item in data: # Iterate over entries in dataset
        items = item.strip().split(",")
        if len(items) == 10:  # 9 features and 1 label
            processed_data.append(items) # Store data from dataset
        else:
            raise ValueError("Dataset entries must contain 9 features and 1 label.")
    return processed_data # Return data from dataset

# Train the Naive Bayes model
def train_model(dataset):
    feature_counts = [{} for _ in range(9)] # Create 9 dictionaries (1 for each feature)
    label_counts = {} # Store the counts for each label ("positive" and "negative")

    for i in range(len(dataset)): # Iterate over entries in dataset
        instance = dataset[i]
        label = instance[-1] # Set label to last item in instance
        label_counts[label] = label_counts.get(label, 0) + 1 # Count "positive" or "negative" labels in the dataset
        for i, feature in enumerate(instance[:-1]): # Exclude the last element (label)
            feature_counts[i].setdefault(feature, {}) # Ensure feature key exists, if not initialize it to 0
            feature_counts[i][feature].setdefault(label, 0) # Ensure label key exists, if not initialize it to 0
            feature_counts[i][feature][label] += 1 # Increment the count

    # Apply Laplace smoothing and normalize
    for i in range(len(feature_counts)): 
        feature_dict = feature_counts[i] # Feature counts of a single cell
        for _, label_dict in feature_dict.items():
            for label in label_counts: # For label="positive" or label="negative"
                label_count = label_counts[label]
                label_dict[label] = (label_dict.get(label, 0) + 1) / (label_count + len(feature_dict)) # Remove zero possiblities through Laplace smoothing by adding 1 to each count, follwed by normalisation

    # Normalisation of label counts
    total_instances = len(dataset) # 958 lines in dataset
    for label in label_counts: 
        label_counts[label] /= total_instances # Normalisation of each label

    return feature_counts, label_counts

# Predict "positive" or "negative" outcome for a new instance
def predict_outcome(feature_counts, label_counts, instance):
    label_probabilities = {}
    for label in label_counts:
        probability = label_counts[label] # Initialized to previous base probability
        for i, feature in enumerate(instance):
            if feature in feature_counts[i] and label in feature_counts[i][feature]: # Ensure feature and current label exist in respective dictionaries
                probability *= feature_counts[i][feature][label] # Update probability for feature
            else:
                probability *= 1 / (label_counts[label] + len(feature_counts[i])) # Prevent zero probability if no recorded probability exists
        label_probabilities[label] = probability # Store probability in dictionary
    return max(label_probabilities, key=label_probabilities.get), label_probabilities # Return the label with the highest probability i.e. predicted outcome

# Function to check for a winning pattern
def check_winner(board, player):
    win_conditions = [ # All possible winning patterns in Tic Tac Toe
        [0, 1, 2], [3, 4, 5], [6, 7, 8],  # Rows
        [0, 3, 6], [1, 4, 7], [2, 5, 8],  # Columns
        [0, 4, 8], [2, 4, 6]              # Diagonals
    ]
    for condition in win_conditions:
            all_match = True  # Assume all positions match a winning pattern
            for i in condition:
                if board[i] != player: # If player does not occupy the position
                    all_match = False  # Set to False if any one position doesn't match
                    break 
            if all_match:
                return True  # Return True if a winning position is matched
    return False  # Return False if no winning pattern is matched

# Select best move based on prediction
def select_best_move(board, prediction):
    # Check for a winning move for the player
    def check_winning_move(board, player, index):
        temp_board = board[:] # Create a copy of board state
        temp_board[index] = player # Simulate the predicted move
        return check_winner(temp_board, player) # Check if move would lead to a winning outcome

    # Check for a winning move on the model's turn
    for i, cell in enumerate(board): 
        if cell == "b":  # Check for empty cell
            if check_winning_move(board, "o", i):  # Check if "o" move leads to a winning outcome
                return i // 3, i % 3  # Return row and column indexes of selected move

    # Check for a blocking move to prevent the "x" player from a winning outcome
    for i, cell in enumerate(board):
        if cell == "b":  # Check for empty cell
            if check_winning_move(board, "x", i):  # Check if "x" move leads to a winning outcome
                return i // 3, i % 3  # Return row and column indexes of position to block

    # Use Naive Bayes model prediction if no strategic move is immediately identified
    for i, cell in enumerate(board):
        if cell == "b":  # Check for empty cell
            temp_board = board[:] # Create a copy of board state
            temp_board[i] = prediction # Simulate the predicted move
            if predict_outcome(feature_probabilities, label_probabilities, temp_board[:-1])[0] == prediction:
                return i // 3, i % 3  # Return row and column of the selected move

    # Return None if no stragetic move is identified
    return None


# Train model based on processed data from dataset
with open("naive-bayes/tic-tac-toe.data") as file:
    raw_data = file.readlines() # Read from dataset file

data = process_data(raw_data) # Process data
random.shuffle(data) # Randomise order of data to prevent bias
# Split dataset into traning and testing sets in 80:20 ratio
split_index = int(0.8 * len(data))
train_data = data[:split_index]
test_data = data[split_index:]
feature_probabilities, label_probabilities = train_model(train_data) # Train model to calculate feature and label probabilities

if __name__ == "__main__": # Only run code if executed directly in file
    if len(sys.argv) > 1 and sys.argv[1] == "--plot": # Check for 'plot' flag
        # Calculate accuracy of model's predictions
        def calculate_accuracy(data, feature_probabilities, label_probabilities):
            correct_predictions = 0
            for instance in data: # Iterate over each instance in dataset
                prediction, _ = predict_outcome(feature_probabilities, label_probabilities, instance[:-1])
                if prediction == instance[-1]: # Check if prediction matches actual label
                    correct_predictions += 1 
            return correct_predictions / len(data) # Calculate accuracy

        train_accuracy = calculate_accuracy(train_data, feature_probabilities, label_probabilities)
        test_accuracy = calculate_accuracy(test_data, feature_probabilities, label_probabilities)
    
        print(f"Training Accuracy: {train_accuracy:.2f}")
        print(f"Testing Accuracy: {test_accuracy:.2f}")

        # Generate confusion matrices
        y_true_train = [instance[-1] for instance in train_data]
        y_pred_train = [predict_outcome(feature_probabilities, label_probabilities, instance[:-1])[0] for instance in train_data]

        y_true_test = [instance[-1] for instance in test_data]
        y_pred_test = [predict_outcome(feature_probabilities, label_probabilities, instance[:-1])[0] for instance in test_data]

        # Define the label names
        labels = ["Lose", "Win"]

        # Plot confusion matrices
        cm_train = confusion_matrix(y_true_train, y_pred_train)
        ConfusionMatrixDisplay(cm_train, display_labels=labels).plot()
        plt.title("Training Confusion Matrix")
        plt.show()

        cm_test = confusion_matrix(y_true_test, y_pred_test)
        ConfusionMatrixDisplay(cm_test, display_labels=labels).plot()
        plt.title("Testing Confusion Matrix")
        plt.show()

    # Simulate Tic Tac Toe games for testing
    elif len(sys.argv) > 1 and sys.argv[1] == "--simulate":  # Check for 'simulate' flag          
        model_wins = 0
        player_wins = 0
        draws = 0

        # Simulate Tic Tac Toe game
        def simulate_game():
            board = ["b"] * 9  # Start game with all cells empty
            model_marker = "o"
            player_marker = "x"
            
            # Randomize who goes first
            current_player = random.choice(["model", "player"])
            
            while "b" in board:  # Continue game until no empty cells remain
                if current_player == "model": # Model's turn
                    prediction, _ = predict_outcome(feature_probabilities, label_probabilities, board)
                    move = select_best_move(board, prediction)
                    if move:
                        board[move[0] * 3 + move[1]] = model_marker # Convert coordinates of position into index to place model's marker on board
                        if check_winner(board, model_marker): # Check if winning move was made
                            return "model"  # Model wins
                    current_player = "player"  # Swap to player's turn if game continues
                else: # Simulated player's turn
                    empty_cells = [i for i, cell in enumerate(board) if cell == "b"] # Create list for all blank cells
                    if empty_cells:
                        move = random.choice(empty_cells) # Player's move is randomly chosen from remaining blank cells 
                        board[move] = player_marker # Place player's marker at position
                        if check_winner(board, player_marker): # Check if winning move was made
                            return "player"  # Player wins
                    current_player = "model"  # Swap to model's turn if game continues

            return "draw"  # Draw if no winner is identified and all cells are occupied

        # Simulate multiple games
        for _ in range(777): # Angel number
            result = simulate_game()
            if result == "model":
                model_wins += 1
            elif result == "player":
                player_wins += 1
            else:
                draws += 1

        # Display the number of times the computer wins
        print("The Naive Bayes model won " + str(model_wins) + " times.")
        print("The player won " + str(player_wins) + " times.")
        print("There were " + str(draws) + " draws.")
        # Simulate Tic Tac Toe games for testing

    
    elif len(sys.argv) == 10:  # Check for 'simulate' flag          
        # Extract the board state from the command-line arguments
        board_instance = sys.argv[1:]  # Get the 9 board positions from arguments

        # Use the Naive Bayes model to predict the AI's move
        prediction, _ = predict_outcome(feature_probabilities, label_probabilities, board_instance)
        
        # Select the best move for the AI based on the prediction
        move = select_best_move(board_instance, prediction)
        
        if move:
            print(move[0], move[1])  # Output the row and column of the AI's move
        else:
            print("-1 -1")  # If no valid move is found, output -1 -1

        # Example usage:
        # Run your script in the command line with a board state like:
        # python classification.py x o b b x b o b x

    else:
        # Just train the model if no arguments are provided
        print("Naive Bayes model trained using the provided dataset.")