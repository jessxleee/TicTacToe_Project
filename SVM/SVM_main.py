import numpy as np
import pandas as pd
from time import time
import random
import matplotlib.pyplot as plt

from sklearn.svm import SVC
from sklearn.metrics import confusion_matrix, ConfusionMatrixDisplay
from sklearn.model_selection import train_test_split, cross_val_score

# Load and preprocess the dataset
data = pd.read_csv("tic-tac-toe.data", header=None)
df = pd.read_csv("tic-tac-toe.data", header=None)

# Replace symbols with numeric values
df.iloc[:, :-1] = df.iloc[:, :-1].replace({'x': 1, 'o': -1, 'b': 0})
df[9] = df[9].replace({'positive': 1, 'negative': -1})

# Split data into features (values) and labels (target)
values = df.iloc[:, :-1]
target = df[9]

# Split into training and testing sets
values_train, values_test, target_train, target_test = train_test_split(
    values, target, test_size=0.2, random_state=42
)

# Cross-validation
scores = cross_val_score(SVC(kernel='poly', C=1, gamma=0.1), values_train, target_train, cv=5)
print("Cross-Validation Accuracy:", scores.mean())

# Train the SVM model
model = SVC(kernel='poly', C=1, gamma=0.1, decision_function_shape='ovo')  # One-vs-One strategy
model.fit(values_train, target_train)

# Test the model
predictions = model.predict(values_test)

# Evaluate the model
accuracy = np.mean(predictions == target_test.values)
print("Predictions: ", predictions)
print("Actual: ", target_test.values)
print("Accuracy: ", accuracy)

# Generate and visualize the confusion matrix
cm = confusion_matrix(target_test, predictions, labels=[1, -1])
labels = ["Positive (1)", "Negative (-1)"]

fig, ax = plt.subplots(figsize=(8, 6))
ConfusionMatrixDisplay(confusion_matrix=cm, display_labels=labels).plot(cmap="Greens", ax=ax)
ax.set_title("Confusion Matrix", fontsize=16)
ax.set_xlabel("Predicted Labels", fontsize=12)
ax.set_ylabel("Actual Labels", fontsize=12)
plt.xticks(fontsize=10)
plt.yticks(fontsize=10)
plt.grid(False)
plt.show()

# New board state in numeric representation directly matching the dataset format
new_board_state = [1, 0, 0, 0, 0, -1, 1, 0, 1]  # Numeric representation: 1 for 'x', -1 for 'o', 0 for empty

# Identify valid moves (cells that are empty)
valid_moves = [i for i, cell in enumerate(new_board_state) if cell == 0]
print(f"Valid moves: {valid_moves}")

best_move = None
best_prediction_score = float('-inf')  # Initialize the best prediction score to a very low value

# Iterate over all valid moves
for move in valid_moves: 
    # Simulate the move by updating the board state
    temp_board_state = new_board_state.copy()
    temp_board_state[move] = 1  # Simulate player move

    # Use the SVM model to evaluate the new board state
    prediction_score = model.decision_function([temp_board_state])[0]
    print(f"Evaluating move at index {move}: Prediction = {prediction_score}")

    # Compare the prediction score and update the best move if necessary
    if prediction_score > best_prediction_score:
        best_prediction_score = prediction_score
        best_move = move

# Ensure a valid move was selected
if best_move is not None:
    print(f"Optimal move chosen: Index {best_move}")
else:
    print("No valid optimal move found. This should not happen if there are valid moves.")

# Convert the predicted move to row and column (1-based indexing)
row = (best_move // 3) + 1
col = (best_move % 3) + 1

# Output the results
print("New Board State:", new_board_state)
print("Predicted Move Index:", best_move)
print(f"Predicted Move: Row {row}, Column {col}")

# Simulate 100 games to determine AI or player wins
ai_wins = 0
player_wins = 0
draws = 0

for game in range(1000):
    # Start with an empty board
    board_state = [0] * 9  # 1 for 'x', -1 for 'o', 0 for empty
    turn = 1  # 1 for AI's turn ('x'), -1 for player's turn ('o')
    winner = None

    while 0 in board_state:  # Continue until the board is full
        if turn == 1:  # AI's turn
            valid_moves = [i for i, cell in enumerate(board_state) if cell == 0]
            best_move = None
            best_prediction_score = float('-inf')

            for move in valid_moves:
                test_board_state = board_state.copy()
                test_board_state[move] = 1  # AI plays as 'x'

                prediction_score = model.decision_function([test_board_state])[0]

                if prediction_score > best_prediction_score:
                    best_prediction_score = prediction_score
                    best_move = move

            if best_move is not None:
                board_state[best_move] = -1
        else:  # Player's turn (random move on top of svm)
            valid_moves = [i for i, cell in enumerate(board_state) if cell == 0]
            best_move = np.random.choice(valid_moves)
            board_state[best_move] = 1

        # Check for a winner after each move
        for line in [(0, 1, 2), (3, 4, 5), (6, 7, 8),  # Rows
                     (0, 3, 6), (1, 4, 7), (2, 5, 8),  # Columns
                     (0, 4, 8), (2, 4, 6)]:            # Diagonals
            if abs(board_state[line[0]] + board_state[line[1]] + board_state[line[2]]) == 3:
                winner = turn
                break

        if winner is not None:
            break

        # Switch turns
        turn *= -1

    # Check for the outcome
    if winner == 1:
        ai_wins += 1
    elif winner == -1:
        player_wins += 1
    else:
        draws += 1

# Print the results
print(f"AI Wins: {ai_wins}")
print(f"Player Wins: {player_wins}")
print(f"Draws: {draws}")

