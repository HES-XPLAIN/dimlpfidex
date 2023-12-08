from tensorflow.keras.preprocessing.image import ImageDataGenerator
from tensorflow.keras.layers import Dense, Dropout, Flatten, BatchNormalization
from tensorflow.keras.layers import Conv2D, MaxPooling2D
from tensorflow.keras.models import load_model, Sequential
import tensorflow as tf
from tensorflow.keras.callbacks import ModelCheckpoint
import time
import numpy as np

from .trnFun import compute_first_hidden_layer, output_stats

def output_pred(pred, pred_file):
    try:
        with open(pred_file, 'w') as file:
            for prediction in pred:
                # Convertir les prédictions en chaîne de caractères avec des espaces entre les valeurs
                prediction_str = ' '.join(map(str, prediction))
                # Écrire la ligne dans le fichier
                file.write(prediction_str + '\n')
    except (FileNotFoundError):
        raise ValueError(f"Error : File {pred_file} not found.")
    except (IOError):
        raise ValueError(f"Error : Couldn't open file {pred_file}.")

def fer():
    try:
        start_time = time.time()

        save_folder = "dimlp/datafiles/FER"
        train_dir = save_folder + "/train"
        test_dir = save_folder + "/test"
        model_checkpoint_weights = save_folder + "/weights.h5"

        # Preprocessing

        train_datagen = ImageDataGenerator(
            width_shift_range = 0.1,        # Randomly shift the width of images by up to 10%
            height_shift_range = 0.1,       # Randomly shift the height of images by up to 10%
            horizontal_flip = True,         # Flip images horizontally at random
            rescale = 1./255,               # Rescale pixel values to be between 0 and 1
            validation_split = 0.2          # Set aside 20% of the data for validation
        )

        #test_datagen = ImageDataGenerator(
        #    rescale = 1./255,               # Rescale pixel values to be between 0 and 1
        #)

        train_generator = train_datagen.flow_from_directory(
            directory = train_dir,           # Directory containing the training data
            target_size = (48, 48),          # Resizes all images to 48x48 pixels
            batch_size = 64,                 # Number of images per batch
            color_mode = "grayscale",        # Converts the images to grayscale
            class_mode = "categorical",      # Classifies the images into 7 categories
            subset = "training"              # Uses the training subset of the data
        )

        validation_generator = train_datagen.flow_from_directory(
            directory = train_dir,            # Directory containing the training data
            target_size = (48, 48),          # Resizes all images to 48x48 pixels
            batch_size = 64,                 # Number of images per batch
            color_mode = "grayscale",        # Converts the images to grayscale
            class_mode = "categorical",      # Classifies the images into 7 categories
            subset = "validation"            # Uses the validation subset of the data
        )

        print("Data loaded")
        print("Training model...")
        # Model

        # Define the model architecture
        model = Sequential()

        # Add a convolutional layer with 32 filters, 3x3 kernel size, and relu activation function
        model.add(Conv2D(32, kernel_size=(3, 3), activation='relu', input_shape=(48,48,1)))
        # Add a batch normalization layer
        model.add(BatchNormalization())
        # Add a second convolutional layer with 64 filters, 3x3 kernel size, and relu activation function
        model.add(Conv2D(64, kernel_size=(3, 3), activation='relu'))
        # Add a second batch normalization layer
        model.add(BatchNormalization())
        # Add a max pooling layer with 2x2 pool size
        model.add(MaxPooling2D(pool_size=(2, 2)))
        # Add a dropout layer with 0.25 dropout rate
        model.add(Dropout(0.25))

        # Add a third convolutional layer with 128 filters, 3x3 kernel size, and relu activation function
        model.add(Conv2D(128, kernel_size=(3, 3), activation='relu'))
        # Add a third batch normalization layer
        model.add(BatchNormalization())
        # Add a fourth convolutional layer with 128 filters, 3x3 kernel size, and relu activation function
        model.add(Conv2D(128, kernel_size=(3, 3), activation='relu'))
        # Add a fourth batch normalization layer
        model.add(BatchNormalization())
        # Add a max pooling layer with 2x2 pool size
        model.add(MaxPooling2D(pool_size=(2, 2)))
        # Add a dropout layer with 0.25 dropout rate
        model.add(Dropout(0.25))

        # Add a fifth convolutional layer with 256 filters, 3x3 kernel size, and relu activation function
        model.add(Conv2D(256, kernel_size=(3, 3), activation='relu'))
        # Add a fifth batch normalization layer
        model.add(BatchNormalization())
        # Add a sixth convolutional layer with 256 filters, 3x3 kernel size, and relu activation function
        model.add(Conv2D(256, kernel_size=(3, 3), activation='relu'))
        # Add a sixth batch normalization layer
        model.add(BatchNormalization())
        # Add a max pooling layer with 2x2 pool size
        model.add(MaxPooling2D(pool_size=(2, 2)))
        # Add a dropout layer with 0.25 dropout rate
        model.add(Dropout(0.25))

        # Flatten the output of the convolutional layers
        model.add(Flatten())
        # Add a dense layer with 256 neurons and relu activation function
        model.add(Dense(256, activation='relu'))
        # Add a seventh batch normalization layer
        model.add(BatchNormalization())
        # Add a dropout layer with 0.5 dropout rate
        model.add(Dropout(0.5))
        # Add a dense layer with 7 neurons (one for each class) and softmax activation function
        model.add(Dense(7, activation='softmax'))

        # Compile the model with categorical cross-entropy loss, adam optimizer, and accuracy metric
        model.compile(loss="categorical_crossentropy", optimizer= tf.keras.optimizers.Adam(lr=0.0001), metrics=['accuracy'])
        model.summary()

        # Define the callback
        checkpoint_callback = ModelCheckpoint(
            filepath=model_checkpoint_weights,
            monitor='val_accuracy',
            save_best_only=True,
            save_weights_only=True,
            mode='max',
            verbose=1
        )

        # Train the model with the callback
        history = model.fit(
            train_generator,
            steps_per_epoch=len(train_generator),
            epochs=2,
            validation_data=validation_generator,
            validation_steps=len(validation_generator),
            callbacks=[checkpoint_callback]
        )


        model_best = load_model(model_checkpoint_weights)

        print("model trained")

        train_pred = model_best.predict(x_train_h1)    # Predict the response for train dataset
        test_pred = model_best.predict(x_test_h1)    # Predict the response for test dataset
        valid_pred = model_best.predict(x_val_h1)   # Predict the response for validation dataset
        train_valid_pred = np.concatenate((train_pred,valid_pred)) # We output predictions of both validation and training sets

        # Output predictions
        test_pred_file = save_folder + "/predTest.out"
        train_valid_pred_file = save_folder + "/predTrain.out"
        output_pred(train_valid_pred, train_valid_pred_file)
        output_pred(test_pred, test_pred_file)

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
        stats_file = save_folder+"/stats.txt"
        output_stats(stats_file, formatted_acc_train, formatted_acc_test)

        end_time = time.time()
        full_time = end_time - start_time
        full_time = "{:.6f}".format(full_time).rstrip("0").rstrip(".")

        print(f"\nFull execution time = {full_time} sec")



        end_time = time.time()
        full_time = end_time - start_time
        full_time = "{:.6f}".format(full_time).rstrip("0").rstrip(".")

        print(f"\nFull execution time = {full_time} sec")

        return 0

    except ValueError as error:
        print(error)
        return -1



fer()
