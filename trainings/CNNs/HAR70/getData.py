import pandas as pd
import numpy as np
from sklearn.model_selection import train_test_split


def output_data(data, data_file):
    try:
        with open(data_file, "w") as dtaFile:
            for var in data:
                for val in var:
                    dtaFile.write(str(val) + " ")
                dtaFile.write("\n")
            dtaFile.close()

    except (FileNotFoundError):
        raise ValueError(f"Error : File {data_file} not found.")
    except (IOError):
        raise ValueError(f"Error : Couldn't open file {data_file}.")

def output_class(classes, class_file):
    try:
        with open(class_file, "w") as dtaFile:
            for val in classes:
                dtaFile.write(str(val) + " ")
                dtaFile.write("\n")
            dtaFile.close()

    except (FileNotFoundError):
        raise ValueError(f"Error : File {class_file} not found.")
    except (IOError):
        raise ValueError(f"Error : Couldn't open file {class_file}.")

segment_size = 250

print("Data loading ...")
data_folder = "datafiles/"
file_paths = [data_folder+str(i)+".csv" for i in range(501,519)]

data_segments = []
class_labels = []

# Read each file
for file_path in file_paths:
    df = pd.read_csv(file_path)

    # Get attributes and classes, ignore timestamp
    classes = df.iloc[:, -1]
    attributes = df.iloc[:, 1:-1]

    # Create segments of segment_size datas with same class
    current_class = None
    segment = []

    for index, row in attributes.iterrows():
        row_class = classes.iloc[index]
        if current_class is None:
            current_class = row_class

        # Finished segment
        if row_class != current_class or len(segment) == segment_size:
            if len(segment) < segment_size:
                # Zero Padding if necessary
                while len(segment) < segment_size:
                    segment.append([0]*6)
            # Add class and segment to the lists
            data_segments.append(segment)
            class_labels.append(current_class)
            # Reinitialize
            segment = []
            current_class = row_class

        # Add row to actual segment
        segment.append(row.tolist())

    # Add last segment if necessary
    if len(segment) > 0:
        if len(segment) < segment_size:
            while len(segment) < segment_size:
                segment.append([0]*6)  # Zero Padding
        data_segments.append(segment)
        class_labels.append(current_class)

data_segments = np.array(data_segments)
class_labels = np.array(class_labels)

print("Nombre of samples : ", len(data_segments))

# Shuffle data
indices = np.arange(data_segments.shape[0])
np.random.shuffle(indices)
data_segments = data_segments[indices]
class_labels = class_labels[indices] -1 # We start class indices at 0

# Split in train(80%) and test(20%)
x_train, x_test, y_train, y_test = train_test_split(data_segments, class_labels, test_size=0.2, random_state=42)

print(f"Training set: {x_train.shape}, {y_train.shape}")
print(f"Test set: {x_test.shape}, {y_test.shape}")

x_train = x_train.reshape(x_train.shape[0],x_train.shape[1]*x_train.shape[2])
x_test = x_test.reshape(x_test.shape[0],x_test.shape[1]*x_test.shape[2])

print(f"Training set flattened: {x_train.shape}, {y_train.shape}")
print(f"Test set flattened: {x_test.shape}, {y_test.shape}")

print("Data loaded.")

output_data(x_train, "trainData.txt")
output_class(y_train, "trainClass.txt")
output_data(x_test, "testData.txt")
output_class(y_test, "testClass.txt")
