import numpy as np
import sys

# Process data function (replacing sklearn's data validation and preprocessing)
def process_data(data):
    processed_data = []
    for item in data:  # Iterate over entries in the dataset
        items = item.strip().split(",")
        if len(items) == 10:  # 9 features and 1 label
            processed_data.append(items)  # Store valid dataset entry
        else:
            raise ValueError("Dataset entries must contain 9 features and 1 label.")
    return processed_data

# Load and preprocess the dataset
with open("SVM/tic-tac-toe.data", "r") as f:
    raw_data = f.readlines()

# Process the dataset to ensure valid entries
processed_data = process_data(raw_data)

# Replace symbols with numeric values
data = [
    [1 if cell == "x" else -1 if cell == "o" else 0 for cell in row[:-1]] +
    [1 if row[-1] == "positive" else -1]
    for row in processed_data
]

# Convert to NumPy array
data = np.array(data, dtype=int)

# Manually split into training and testing sets
np.random.seed(42)  # Set seed for reproducibility
np.random.shuffle(data)  # Shuffle the dataset

split_index = int(0.8 * len(data))  # 80% for training, 20% for testing
values_train, values_test = data[:split_index, :-1], data[split_index:, :-1]
target_train, target_test = data[:split_index, -1], data[split_index:, -1]

# Define the PolynomialSVMClassifier class
# Equivalent to polynomial kernel classifier
class OptimizedPolynomialSVMClassifier:
    def __init__(self, degree=2, learning_rate=0.001, no_of_iterations=1000, lambda_parameter=0.01):
        self.degree = degree
        self.learning_rate = learning_rate
        self.no_of_iterations = no_of_iterations
        self.lambda_parameter = lambda_parameter

    def polynomial_kernel(self, X):
        X = np.array(X, dtype=np.float64)  # Ensure consistent data type
        return (1 + np.dot(X, X.T)) ** self.degree

    def fit(self, X, y):
        X = np.array(X, dtype=np.float64)
        self.training_data = X
        self.kernel_matrix = self.polynomial_kernel(X)

        self.m, self.n = self.kernel_matrix.shape
        self.w = np.zeros(self.n, dtype=np.float64)
        self.b = 0
        self.Y = np.where(y <= 0, -1, 1).astype(np.float64)

        for _ in range(self.no_of_iterations):
            margins = self.Y * (np.dot(self.kernel_matrix, self.w) - self.b)
            condition = margins >= 1
            condition = condition.astype(np.float64)

            dw = 2 * self.lambda_parameter * self.w - np.dot(self.kernel_matrix.T, self.Y * (1 - condition))
            db = -np.sum(self.Y * (1 - condition))

            self.w -= self.learning_rate * dw
            self.b -= self.learning_rate * db

    def decision_function(self, X):
        X = np.array(X, dtype=np.float64)
        X_poly = (1 + np.dot(X, self.training_data.T)) ** self.degree
        return np.dot(X_poly, self.w) - self.b

    def predict(self, X):
        return np.sign(self.decision_function(X))


# Helper function to check for a winning move
def is_winning_move(board, player):
    win_conditions = [
        [0, 1, 2], [3, 4, 5], [6, 7, 8],  # Rows
        [0, 3, 6], [1, 4, 7], [2, 5, 8],  # Columns
        [0, 4, 8], [2, 4, 6]              # Diagonals
    ]
    # If player has a move on every cell in any of the win_conditions array, counted win
    for condition in win_conditions:
        values = [board[i] for i in condition]
        if values.count(player) == 3:
            return True
    return False


#  Employing critical moves to prioritize win follow by win whenever possible
def prioritymoves(board_state, valid_moves):
    for move in valid_moves:
        test_board = board_state.copy()
        test_board[move] = -1  # Simulate AI's move
        if is_winning_move(test_board, -1): # Means AI can win at this move this round
            return move, "win"
    
    for move in valid_moves:
        test_board = board_state.copy()
        test_board[move] = 1  # Simulate player's move
        if is_winning_move(test_board, 1): # Means player can win at this move this round, need to block
            return move, "block"
    return None, None


# Main execution block
if __name__ == "__main__":
    # Instantiate and train the polynomial SVM model
    model = OptimizedPolynomialSVMClassifier(degree=3, learning_rate=0.001, no_of_iterations=2000, lambda_parameter=0.01)
    model.fit(values_train, target_train)

    # Read board state from command-line arguments
    if len(sys.argv) > 9:
        board_state = list(map(int, sys.argv[1:10]))
    else:
        print("-1 -1")  # Invalid board state input
        sys.exit()

    # Identify all valid moves on board 
    valid_moves = [i for i, cell in enumerate(board_state) if cell == 0]

    # Determine if there is a winning move/block required
    critical_move, critical_reason = prioritymoves(board_state, valid_moves)

    # No critial moves required use trained SVM model to evaluate each valid move
    if critical_move is not None:
        best_move = critical_move
    else:
        decision_scores = [
            (move, model.decision_function([board_state[:move] + [1] + board_state[move + 1:]])[0])
            for move in valid_moves
        ]
        best_move = max(decision_scores, key=lambda x: x[1])[0] if decision_scores else None

    # Output the best move as row and column to return as move 
    if best_move is not None:
        row = best_move // 3 
        col = best_move % 3 
        print(f"{row} {col}")
    else:
        print("-1 -1")
