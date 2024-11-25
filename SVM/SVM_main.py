import numpy as np
import sys
#import matplotlib.pyplot as plt
#from sklearn.metrics import confusion_matrix
#from matplotlib.colors import LinearSegmentedColormap

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

# Construct the SVM model using matrix operations
# Equivalent to polynomial kernel classifier
class OptimizedPolynomialSVMClassifier:
    def __init__(self, degree=2, learning_rate=0.001, no_of_iterations=1000, lambda_parameter=0.01):
        # Initialize the SVM model with given hyperparameters
        self.degree = degree                # Degree of the polynomial kernel
        self.learning_rate = learning_rate  # Learning rate for gradient descent
        self.no_of_iterations = no_of_iterations  # Number of iterations for training
        self.lambda_parameter = lambda_parameter  # Regularization parameter to prevent overfitting

    def polynomial_kernel(self, X):
        # Compute the polynomial kernel for the input data
        X = np.array(X, dtype=np.float64)  # Ensure consistent data type
        return (1 + np.dot(X, X.T)) ** self.degree  # Polynomial kernel formula

    def fit(self, X, y):
        # Train the SVM model using gradient descent
        X = np.array(X, dtype=np.float64)  # Ensure consistent input format
        self.training_data = X             # Store the training data
        self.kernel_matrix = self.polynomial_kernel(X)  # Compute the kernel matrix

        # Initialize parameters for optimization
        self.m, self.n = self.kernel_matrix.shape  # Dimensions of the kernel matrix
        self.w = np.zeros(self.n, dtype=np.float64)  # Initialize weights to zeros
        self.b = 0                                  # Initialize bias to zero
        self.Y = np.where(y <= 0, -1, 1).astype(np.float64)  # Convert labels to -1 and 1

        # Perform gradient descent for the specified number of iterations
        for _ in range(self.no_of_iterations):
            # Calculate margins for each training example
            margins = self.Y * (np.dot(self.kernel_matrix, self.w) - self.b)
            condition = margins >= 1  # SVM hinge loss condition
            condition = condition.astype(np.float64)

            # Compute gradients for weights and bias
            dw = 2 * self.lambda_parameter * self.w - np.dot(self.kernel_matrix.T, self.Y * (1 - condition))
            db = -np.sum(self.Y * (1 - condition))

            # Update weights and bias using gradient descent
            self.w -= self.learning_rate * dw
            self.b -= self.learning_rate * db

    def decision_function(self, X):
        # Compute the decision score for input data
        X = np.array(X, dtype=np.float64)  # Ensure consistent input format
        X_poly = (1 + np.dot(X, self.training_data.T)) ** self.degree  # Compute polynomial transformation
        return np.dot(X_poly, self.w) - self.b  # Compute decision scores

    def predict(self, X):
        # Predict class labels for input data based on decision scores
        return np.sign(self.decision_function(X))  # Return class labels (-1 or 1)


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
    model = OptimizedPolynomialSVMClassifier(degree=1, learning_rate=0.001, no_of_iterations=2000, lambda_parameter=0.01)
    model.fit(values_train, target_train)

    # Read board state from command-line arguments
    if len(sys.argv) > 9: # must read in at least 10 command line arguments for program name and 9 integers
        board_state = list(map(int, sys.argv[1:10])) # slice extracts 9 arguments after program name, make sure integer type
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

        # Print move scores to stderr
        print("Move Evaluation Scores:", file=sys.stderr)
        for move, score in decision_scores:
            print(f"Move index: {move}, Score: {score}", file=sys.stderr)

        best_move = max(decision_scores, key=lambda x: x[1])[0] if decision_scores else None

    # Output the best move as row and column to return as move 
    if best_move is not None:
        row = best_move // 3 
        col = best_move % 3 
        print(f"{row} {col}")
    else:
        print("-1 -1")

"""Comments for model accuracy tests and confusion matrix

# Generate a custom colormap
def create_custom_colormap():
    colors = ["#4059AD", "#6B9AC4", "#97D8C4", "#EFF2F1"]  # Sapphire, Blue Gray, Tiffany Blue, Anti-flash white
    return LinearSegmentedColormap.from_list("CustomColormap", colors, N=256)


# Generate a simplified confusion matrix with custom colors
# Generate a simplified confusion matrix with custom colors
def plot_confusion_matrix(y_true, y_pred):
    cm = confusion_matrix(y_true, y_pred, labels=[1, -1])  # 1: Positive, -1: Negative

    # Use the custom colormap
    cmap = create_custom_colormap()

    # Plot the confusion matrix
    plt.figure(figsize=(6, 5))
    plt.imshow(cm, interpolation='nearest', cmap=cmap)
    plt.title("Confusion Matrix", fontsize=14)
    plt.colorbar()

    tick_marks = [0, 1]
    plt.xticks(tick_marks, ["Positive", "Negative"], fontsize=10)
    plt.yticks(tick_marks, ["Positive", "Negative"], fontsize=10)

    # Annotate matrix with counts
    for i in range(2):
        for j in range(2):
            plt.text(j, i, f"{cm[i, j]}",
                     horizontalalignment="center",
                     color="black",
                     fontsize=12)

    plt.ylabel("Actual Label", fontsize=12)
    plt.xlabel("Predicted Label", fontsize=12)
    plt.tight_layout()
    plt.show()

    # Return TP, TN, FP, FN values
    TP = cm[0, 0]  # True Positive
    FP = cm[1, 0]  # False Positive
    FN = cm[0, 1]  # False Negative
    TN = cm[1, 1]  # True Negative

    return TP, TN, FP, FN


# Function to calculate error probability
def calculate_error_probability(TP, TN, FP, FN):

    total = TP + TN + FP + FN  # Total number of samples
    errors = FP + FN  # Total incorrect predictions
    error_probability = errors / total  # Error probability
    return error_probability

# Function to calculate accuracy
def calculate_accuracy(TP, TN, FP, FN):

    total = TP + TN + FP + FN  # Total number of samples
    correct_predictions = TP + TN  # Total correct predictions
    accuracy = correct_predictions / total  # Accuracy
    return accuracy

# Predict on the test set
test_predictions = model.predict(values_test)
TP_test, TN_test, FP_test, FN_test = plot_confusion_matrix(target_test, test_predictions)

# Predict on the training set
train_predictions = model.predict(values_train)
TP_train, TN_train, FP_train, FN_train = plot_confusion_matrix(target_train, train_predictions)

# Display counts for the training set
print("\nTraining Set Metrics:")
print(f"True Positive (TP): {TP_train}")
print(f"True Negative (TN): {TN_train}")
print(f"False Positive (FP): {FP_train}")
print(f"False Negative (FN): {FN_train}")

# Calculate and display error probability and accuracy for the training set
train_error_probability = calculate_error_probability(TP_train, TN_train, FP_train, FN_train)
train_accuracy = calculate_accuracy(TP_train, TN_train, FP_train, FN_train)
print(f"Training Set Error Probability: {train_error_probability:.2f}")
print(f"Training Set Accuracy: {train_accuracy:.2f}")

# Display counts for the test set
print("\nTest Set Metrics:")
print(f"True Positive (TP): {TP_test}")
print(f"True Negative (TN): {TN_test}")
print(f"False Positive (FP): {FP_test}")
print(f"False Negative (FN): {FN_test}")

# Calculate and display error probability and accuracy for the test set
test_error_probability = calculate_error_probability(TP_test, TN_test, FP_test, FN_test)
test_accuracy = calculate_accuracy(TP_test, TN_test, FP_test, FN_test)
print(f"Test Set Error Probability: {test_error_probability:.2f}")
print(f"Test Set Accuracy: {test_accuracy:.2f}")

Not ran during Tic tac Toe game"""