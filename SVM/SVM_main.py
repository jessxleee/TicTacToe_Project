import numpy as np
import pandas as pd
import sys
import joblib
# from sklearn.svm import SVC
# from sklearn.model_selection import train_test_split

MODEL_PATH = 'SVM/svm_model.pkl'

def load_model_or_train(data_path, model_path):
    try:
        # Attempt to load the saved model
        model = joblib.load(model_path)
    except FileNotFoundError:
        # If no saved model, train a new one
        df = pd.read_csv(data_path, header=None)
        df.iloc[:, :-1] = df.iloc[:, :-1].replace({'x': 1, 'o': -1, 'b': 0})
        df[9] = df[9].replace({'positive': 1, 'negative': -1})

        values = df.iloc[:, :-1]
        target = df[9]

        values_train, values_test, target_train, target_test = train_test_split(
            values, target, test_size=0.2, random_state=42
        )

        model = SVC(kernel='poly', C=1, gamma=0.1, decision_function_shape='ovo')
        model.fit(values_train, target_train)

        # Save the model for future use
        joblib.dump(model, model_path)
    return model

if __name__ == "__main__":
    DATA_PATH = "tic-tac-toe.data"

    # Load or train the model
    model = load_model_or_train(DATA_PATH, MODEL_PATH)

    # Check if board state is provided via command-line arguments
    if len(sys.argv) > 1:
        board_state = list(map(int, sys.argv[1:10]))  # Read board state as command-line arguments
    else:
        print("-1 -1")  # Return no move if no valid board state provided
        sys.exit()

    # Identify valid moves
    valid_moves = [i for i, cell in enumerate(board_state) if cell == 0]

    best_move = None
    best_prediction_score = float('-inf')

    for move in valid_moves:
        # Simulate player move on all valid moves
        temp_board_state = board_state.copy()
        temp_board_state[move] = 1 

        # Evaluate the moves using the trained model
        prediction_score = model.decision_function([temp_board_state])[0]

        # Update best move and prediction score if a better move is found
        if prediction_score > best_prediction_score:
            best_prediction_score = prediction_score
            best_move = move

    # Return best move as row index and column index
    if best_move is not None:
        row = (best_move // 3) 
        col = (best_move % 3) 
        print(f"Predicted best move: Index {best_move} (Row {row}, Column {col})")
        print(f"{row} {col}")  # Output row and column as space-separated values
    else:
        print("-1 -1")  # No valid moves available