import pandas as pd
import numpy as np
import sys
import time
from sklearn.model_selection import train_test_split
sys.path.append('../../trainings')
from trainings.trnFun import compute_first_hidden_layer, output_data, output_stats
from tensorflow import keras
from keras.models import Sequential, load_model
from keras.layers import Conv1D, MaxPooling1D, Flatten, Dense, Dropout, BatchNormalization
from keras.callbacks import ModelCheckpoint, ReduceLROnPlateau

start_time = time.time()

K = 1.0
nb_quant_levels = 100
hiknot = 5
weights_outfile = "weights.wts"
segment_size = 250
nb_channels = 6
nb_classes = 8
nb_epochs = 80
model_checkpoint_weights = "weightsModel.keras"
stats_file = "stats.txt"
train_valid_pred_outfile = "predTrain.out"
test_pred_outfile = "predTest.out"



print("Data loading ...")
data_folder = "Data/"
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

# Split in train(70%), test(20%) and validation sets(10%)
x_train, x_temp, y_train, y_temp = train_test_split(data_segments, class_labels, test_size=0.3, random_state=42)
x_val, x_test, y_val, y_test = train_test_split(x_temp, y_temp, test_size=(2/3), random_state=42)

print(f"Training set: {x_train.shape}, {y_train.shape}")
print(f"Validation set: {x_val.shape}, {y_val.shape}")
print(f"Test set: {x_test.shape}, {y_test.shape}")

x_train = x_train.reshape(x_train.shape[0],x_train.shape[1]*x_train.shape[2])
x_val = x_val.reshape(x_val.shape[0],x_val.shape[1]*x_val.shape[2])
x_test = x_test.reshape(x_test.shape[0],x_test.shape[1]*x_test.shape[2])

print(f"Training set flattened: {x_train.shape}, {y_train.shape}")
print(f"Validation set flattened: {x_val.shape}, {y_val.shape}")
print(f"Test set flattened: {x_test.shape}, {y_test.shape}")


print("Data loaded.")

# Compute first hidden layer
x_train_h1, mu, sigma = compute_first_hidden_layer("train", x_train, K, nb_quant_levels, hiknot, weights_outfile)
x_test_h1 = compute_first_hidden_layer("test", x_test, K, nb_quant_levels, hiknot, mu=mu, sigma=sigma)
x_val_h1 = compute_first_hidden_layer("test", x_val, K, nb_quant_levels, hiknot, mu=mu, sigma=sigma)

x_train_h1 = x_train_h1.reshape(x_train_h1.shape[0], segment_size, nb_channels)

x_test_h1 = x_test_h1.reshape(x_test_h1.shape[0], segment_size, nb_channels)
x_val_h1 = x_val_h1.reshape(x_val_h1.shape[0], segment_size, nb_channels)

print(f"Training set: {x_train_h1.shape}, {y_train.shape}")
print(f"Validation set: {x_val_h1.shape}, {y_val.shape}")
print(f"Test set: {x_test_h1.shape}, {y_test.shape}")

y_train = keras.utils.to_categorical(y_train, nb_classes)
y_test = keras.utils.to_categorical(y_test, nb_classes)
y_val = keras.utils.to_categorical(y_val, nb_classes)

##############################################################################
print("Training model...")

model = Sequential()

model.add(Conv1D(filters=64, kernel_size=3, activation='relu', input_shape=(250, 6)))
model.add(BatchNormalization())
model.add(MaxPooling1D(pool_size=2))


model.add(Conv1D(filters=128, kernel_size=3, activation='relu'))
model.add(BatchNormalization())
model.add(MaxPooling1D(pool_size=2))

model.add(Dropout(0.5))

model.add(Flatten())
model.add(Dense(128, activation='relu'))
model.add(BatchNormalization())
model.add(Dense(nb_classes, activation='softmax'))

model.compile(loss="categorical_crossentropy", optimizer= keras.optimizers.Adam(learning_rate=0.0001), metrics=['accuracy'])

model.summary()

checkpointer = ModelCheckpoint(filepath=model_checkpoint_weights, verbose=1, save_best_only=True)
model.fit(x_train_h1, y_train, batch_size=32, epochs=nb_epochs, validation_data=(x_val_h1, y_val), callbacks=[checkpointer], verbose=2)

model_best = load_model(model_checkpoint_weights)

print("model trained")

train_pred = model_best.predict(x_train_h1)    # Predict the response for train dataset
test_pred = model_best.predict(x_test_h1)    # Predict the response for test dataset
valid_pred = model_best.predict(x_val_h1)   # Predict the response for validation dataset
train_valid_pred = np.concatenate((train_pred,valid_pred)) # We output predictions of both validation and training sets

# Output predictions
output_data(train_valid_pred, train_valid_pred_outfile)
output_data(test_pred, test_pred_outfile)

##############################################################################

print("\nResult :")

score = model.evaluate(x_train_h1, y_train)
print("Train score : ", score)

score = model.evaluate(x_test_h1, y_test)
print("Test score : ", score)

##############################################################################

print("\nBest result :")

score = model_best.evaluate(x_train_h1, y_train)
print("Train score : ", score)

score = model_best.evaluate(x_test_h1, y_test)
print("Test score : ", score)

acc_train = model_best.evaluate(x_train_h1, y_train, verbose=0)[1]
acc_test = model_best.evaluate(x_test_h1, y_test, verbose=0)[1]

formatted_acc_train = "{:.6f}".format(acc_train*100)
formatted_acc_test = "{:.6f}".format(acc_test*100)
output_stats(stats_file, formatted_acc_train, formatted_acc_test)

end_time = time.time()
full_time = end_time - start_time
full_time = "{:.6f}".format(full_time).rstrip("0").rstrip(".")

print(f"\nFull execution time = {full_time} sec")
