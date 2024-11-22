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

def is_winning_move(board, player, move):
    if board[move] != 0:  # Ignore occupied cells
        return 0
    
    # Simulate move
    board_copy = board[:]
    board_copy[move] = player

    # Check rows, columns, and diagonals for a win
    win_patterns = [
        [0, 1, 2], [3, 4, 5], [6, 7, 8],  # Rows
        [0, 3, 6], [1, 4, 7], [2, 5, 8],  # Columns
        [0, 4, 8], [2, 4, 6]              # Diagonals
    ]
    for pattern in win_patterns:
        if all(board_copy[cell] == player for cell in pattern):
            return 1
    return 0

def is_blocking_move(board, opponent, move):
    if board[move] != 0:  # Ignore occupied cells
        return 0

    # Check if placing the opponent's marker results in a winning outcome
    return is_winning_move(board, opponent, move)

def creates_fork(board, player, move):
    if board[move] != 0:  # Ignore occupied cells
        return 0
    
    # Simulate move and count the number of potential winning patterns
    board_copy = board[:]
    board_copy[move] = player

    win_patterns = [
        [0, 1, 2], [3, 4, 5], [6, 7, 8],  # Rows
        [0, 3, 6], [1, 4, 7], [2, 5, 8],  # Columns
        [0, 4, 8], [2, 4, 6]              # Diagonals
    ]
    winning_patterns = 0
    for pattern in win_patterns:
        if sum(board_copy[cell] == player for cell in pattern) == 2 and \
           sum(board_copy[cell] == 0 for cell in pattern) == 1:
            winning_patterns += 1
    return 1 if winning_patterns > 1 else 0

def extend_features(data): 
    extended_data = []
    for idx, instance in enumerate(data):
        try:
            # Extract board and label
            board = [1 if cell == 'x' else -1 if cell == 'o' else 0 for cell in instance[:9]]
            label = instance[9]
            
            # Initialize extended features with board state
            extended_features = board[:]
            
            # Count strategic features
            winning_moves = sum(
                is_winning_move(board, 1, move)
                for move in range(9)
                if board[move] == 0
            )

            blocking_moves = sum(
                is_blocking_move(board, -1, move)
                for move in range(9)
                if board[move] == 0
            )

            forks_created = sum(
                creates_fork(board, 1, move)
                for move in range(9)
                if board[move] == 0
            )

            center_control = 1 if board[4] == 1 else 0

            corner_occupation = sum(
                1 for move in [0, 2, 6, 8] if board[move] == 1
            )
            
            # Add strategic features to the extended feature set
            extended_features.extend([
                winning_moves,        # Total possible winning moves
                blocking_moves,       # Total possible blocking moves
                forks_created,        # Total forks created
                center_control,       # Is center occupied by 'x'
                corner_occupation     # Number of corners occupied by 'x'
            ])
            
            # Append the label to feature set
            extended_features.append(label)
            
            # Debugging
            if len(extended_features) != 15:
                print(f"Issue with instance {idx}: Expected 20 features, got {len(extended_features)}")
                print(f"Extended features: {extended_features}")

            extended_data.append(extended_features)

        except Exception as e:
            print(f"Error processing instance {idx}: {e}")

    return extended_data

def train_model(dataset):
    # Initialize dictionaries for feature counts 
    feature_counts = [{} for _ in range(14)]  # Expected 9 board + 5 extended features
    label_counts = {}

    for instance in dataset:
        # Extract board and label
        board = [1 if cell == 'x' else -1 if cell == 'o' else 0 for cell in instance[:9]]
        label = instance[-1]

        # Count original features (board state)
        for i, feature in enumerate(board):
            if feature not in feature_counts[i]:
                feature_counts[i][feature] = {'positive': 0, 'negative': 0}  # Separate counts for each label
            feature_counts[i][feature][label] += 1

        # Count extended features
        for move in range(9):
            if board[move] == 0:  # Only evaluate unoccupied cells
                extended_features = [
                    is_winning_move(board, 1, move),     # Check for winning move
                    is_blocking_move(board, -1, move),   # Check for blocking move
                    creates_fork(board, 1, move),        # Check for fork
                    1 if move == 4 else 0,               # Center control
                    1 if move in [0, 2, 6, 8] else 0     # Corner control
                ]
                for j, feature in enumerate(extended_features, start=9):
                    if feature not in feature_counts[j]:
                        feature_counts[j][feature] = {'positive': 0, 'negative': 0}
                    feature_counts[j][feature][label] += 1

        # Label counts
        label_counts[label] = label_counts.get(label, 0) + 1

    return feature_counts, label_counts

# Predict "positive" or "negative" outcome for a new instance
def predict_outcome(feature_counts, label_counts, instance):
    label_probabilities = {}  # Dictionary to store probabilities for each label
    smoothing_factor = 1

    for label in label_counts:
        probability = label_counts[label]  # Initialize with prior probability of the label
        for i, feature in enumerate(instance):
                    if feature in feature_counts[i]:  # Check if the feature exists in feature_counts
                        # Handle nested dictionary: feature_counts[i][feature] is itself a dictionary
                        label_counts_for_feature = feature_counts[i][feature]
                        numerator = label_counts_for_feature.get(label, 0) + smoothing_factor
                        denominator = sum(label_counts_for_feature.values()) + smoothing_factor * len(label_counts_for_feature)
                    else:
                        # Feature not present: Apply smoothing over the entire feature space
                        numerator = smoothing_factor
                        denominator = sum(sum(label_counts_for_feature.values()) for label_counts_for_feature in feature_counts[i].values()) + smoothing_factor * len(feature_counts[i])

                    # Update probability for this label
                    probability *= numerator / denominator
        
        label_probabilities[label] = probability  # Store the calculated probability for this label
    
    # Return the label with the highest probability and the probabilities for all labels
    return max(label_probabilities, key=label_probabilities.get), label_probabilities

# Move selection logic
# Move selection logic
def predict_move(board, feature_counts, label_counts):
    # Convert board to numeric representation
    board = [1 if cell == 'x' else -1 if cell == 'o' else 0 for cell in board]

    # Step 1: Check for a winning move 80% of the time
    for move in range(9):
        if board[move] == 0:  # Only evaluate empty cells
            if is_winning_move(board, 1, move):  # Check if Naive Bayes can win
                if random.random() < 0.8:  # 80% chance to prioritize winning move
                    return move // 3, move % 3  # Return index of move (row, col)

    probabilities = []  # List to store probabilities for each move

    for move in range(9):
        if board[move] == 0:  # Only evaluate empty cells
            # Calculate higher-order feature values
            features = [
                is_winning_move(board, 1, move),
                is_blocking_move(board, -1, move),
                creates_fork(board, 1, move),
                1 if move == 4 else 0,  # Center control
                1 if move in [0, 2, 6, 8] else 0  # Corner control
            ]

            # Compute the probability for this move
            prob = label_counts.get('positive', 1) / sum(label_counts.values())
            for i, feature in enumerate(features):
                feature_dict = feature_counts[i].get(feature, {"positive": 0, "negative": 0})
                total_count = sum(feature_dict.values())  # Sum across all labels
                prob *= (feature_dict.get('positive', 0) + 1) / (total_count + 2)  # Add smoothing

            probabilities.append((move, prob))

    # Ensure probabilities list is not empty
    if probabilities:
        best_move = max(probabilities, key=lambda x: x[1])[0]
        return best_move // 3, best_move % 3  # Return index of move (row, col)

    # No valid moves left
    return None

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

# Train model based on processed data from dataset
with open("naive-bayes/tic-tac-toe.data") as file:
    raw_data = file.readlines() # Read from dataset file

data = process_data(raw_data) # Process data
extended_data = extend_features(data)
random.shuffle(extended_data) # Randomise order of data to prevent bias
# Split dataset into traning and testing sets in 80:20 ratio
split_index = int(0.8 * len(extended_data))
train_data = extended_data[:split_index]
test_data = extended_data[split_index:]
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
            board = [0] * 9  # Start game with all cells empty
            model_marker = "o"
            player_marker = "x"
            
            # Randomize who goes first
            current_player = random.choice(["model", "player"])
            
            while 0 in board:  # Continue game until no empty cells remain
                if current_player == "model": # Model's turn
                    prediction, _ = predict_outcome(feature_probabilities, label_probabilities, board)
                    move = predict_move(board, feature_probabilities, label_probabilities)
                    if move:
                        board[move[0] * 3 + move[1]] = model_marker # Convert coordinates of position into index to place model's marker on board
                        if check_winner(board, model_marker): # Check if winning move was made
                            return "model"  # Model wins
                    current_player = "player"  # Swap to player's turn if game continues
                else: # Simulated player's turn
                    empty_cells = [move for move in range(9) if board[move] == 0] # Create list for all blank cells
                    if empty_cells:
                        move = random.choice(empty_cells) # Player's move is randomly chosen from remaining blank cells 
                        board[move] = player_marker # Place player's marker at position
                        if check_winner(board, player_marker): # Check if winning move was made
                            return "player"  # Player wins
                    current_player = "model"  # Swap to model's turn if game continues

            return "draw"  # Draw if no winner is identified and all cells are occupied

        # Simulate multiple games
        for _ in range(1000):
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
        move = predict_move(board_instance, feature_probabilities, label_probabilities)
        
        if move:
            print(move[0], move[1])  # Output the row and column of the AI's move
  # Output the row and column of the AI's move
        else:
            print("-1 -1")  # If no valid move is found, output -1 -1

    else:
        # Just train the model if no arguments are provided
        print("Naive Bayes model trained using the provided dataset.")