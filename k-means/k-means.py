import pandas as pd
from sklearn.cluster import KMeans
from sklearn.preprocessing import LabelEncoder
import numpy as np

# Alternatively, the colab link is https://colab.research.google.com/drive/1ngPLnBgOyvdnI0-m8G2lX-H28i8E--FP?usp=sharing

data = pd.read_csv("/content/drive/MyDrive/TicTacToe Datasets/tic-tac-toe.data", header = None)

# Check data read is successful
print(data.head())

# Convert 'b', 'o', 'x' into numeric values: 'b' = 0, 'o' = 1, 'x' = 2
# Map the board states to numeric values (0 = 'b', 1 = 'o', 2 = 'x')
label_encoder = LabelEncoder()
data[0] = label_encoder.fit_transform(data[0])
data[1] = label_encoder.transform(data[1])
data[2] = label_encoder.transform(data[2])
data[3] = label_encoder.transform(data[3])
data[4] = label_encoder.transform(data[4])
data[5] = label_encoder.transform(data[5])
data[6] = label_encoder.transform(data[6])
data[7] = label_encoder.transform(data[7])
data[8] = label_encoder.transform(data[8])

# Convert into numeric labels (1 = 'positive', 0 = 'negative')
data[9] = label_encoder.fit_transform(data[9])

# Separate features (board states) and labels (positive/negative)
X = data.iloc[:, :-1].values  # Features: Board states
y = data.iloc[:, -1].values   # Labels: Positive or negative

# Train a KMeans model to cluster the board states into two clusters
# Set K = 2 for two types: 'positive' and 'negative'
kmeans = KMeans(n_clusters=2, random_state=42)

# Fit the KMeans model to the data
kmeans.fit(X)

# Predict the clusters (i.e., which cluster each board state belongs to)
clusters = kmeans.predict(X)

# Output the results
print("\nCluster assignments (0 or 1):")
print(clusters)

# Check how many samples were assigned to each cluster
print("\nCluster counts:")
print(np.bincount(clusters))

# Check the centroids of the clusters
print("\nCluster centroids:")
print(kmeans.cluster_centers_)

# Check the accuracy of clustering by comparing to true labels
accuracy = (clusters == y).mean()
print(f"\nAccuracy of clustering: {accuracy * 100:.2f}%")