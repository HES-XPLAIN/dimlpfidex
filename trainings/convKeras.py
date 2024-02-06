# -*- coding: utf-8 -*-
"""
Created on Fri Mar 19 11:54:46 2021

@author: guido.bologna
"""

###############################################################

import os
import sys
import time
os.environ["CUDA_DEVICE_ORDER"] = "PCI_BUS_ID"
os.environ["CUDA_VISIBLE_DEVICES"] = ""

import numpy as np
np.random.seed(seed=None)

import tensorflow as tf
from tensorflow import keras
from keras.models     import Sequential
from keras.layers     import Dense, Dropout, Activation, Flatten, BatchNormalization, Lambda
from keras.layers     import Convolution2D, DepthwiseConv2D, LocallyConnected2D, MaxPooling2D, GlobalAveragePooling2D
from keras.models     import load_model, Model
from keras.applications import ResNet50, VGG16

from keras.callbacks  import ModelCheckpoint
from keras.datasets import mnist
from keras.datasets import cifar100
from keras.datasets import cifar10
import colorsys



from .trnFun import compute_first_hidden_layer, output_stats, output_data, check_parameters_dimlp_layer, check_parameters_common, get_data, get_data_class, check_strictly_positive, check_int, check_bool, validate_string_param


def convKeras(*args, **kwargs):

    try:
        if not kwargs:
            print("Obligatory parameters :")
            print("dataset : mnist, cifar100, cifar10 or fer")
            print("train_data_file : train data file")
            print("train_class_file : train class file, not mendatory if classes are specified in train_data_file")
            print("test_data_file : test data file")
            print("test_class_file : test class file, not mendatory if classes are specified in test_data_file")
            print("nb_attributes : number of attributes")
            print("nb_classes : number of classes")
            print("----------------------------")
            print("Optional parameters :")
            print("valid_ratio : porcentage(]0,1[) of train data taken for validation (0.1 by default if no valid data)")
            print("valid_data_file : validation data file")
            print("valid_class_file : validation class file")
            print("BE CAREFUL if there is validation files, and you want to use fidex algorithms you have to use both train and validation datas for train datas and classes")
            print("normalized : whether image datas are normalized between 0 and 1 (false by default, true if with_hsl)")
            print("root_folder : Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder.")
            print("nb_epochs : number of epochs during training(80 by default)")
            print("train_valid_pred_file : output train and validation (in this order) prediction file name (predTrain.out by default)")
            print("test_pred_file : output test prediction file name (predTest.out by default)")
            print("weights_file : output weights file name (weights.wts by default)")
            print("stats_file : output file name with train and test accuracy (stats.txt by default)")
            print("console_file : file where you redirect console result")
            print("nb_quant_levels : number of stairs in staircase activation function (50 by default)")
            print("K : Parameter to improve dynamics (1 by default)")
            print("with_hsl : If you want to change 3-channels data from RGB to HSL format (False by default)")
            print("with_resnet : Training with ResNet (False by default)")
            print("with_vgg : Training with ResNet (False by default)")
        else:

            start_time = time.time()

            # Get parameters
            dataset = kwargs.get('dataset')
            root_folder = kwargs.get('root_folder')
            train_data_file = kwargs.get('train_data_file')
            train_class_file = kwargs.get('train_class_file')
            test_data_file = kwargs.get('test_data_file')
            test_class_file = kwargs.get('test_class_file')
            nb_attributes = kwargs.get('nb_attributes')
            nb_classes = kwargs.get('nb_classes')
            valid_ratio = kwargs.get('valid_ratio')
            valid_data_file = kwargs.get('valid_data_file')
            valid_class_file = kwargs.get('valid_class_file')
            normalized = kwargs.get('normalized')
            console_file = kwargs.get('console_file')
            train_valid_pred_file = kwargs.get('train_valid_pred_file')
            test_pred_file = kwargs.get('test_pred_file')
            weights_file = kwargs.get('weights_file')
            stats_file = kwargs.get('stats_file')
            K = kwargs.get('K')
            quant = kwargs.get('nb_quant_levels')
            nb_epochs = kwargs.get('nb_epochs')
            with_hsl = kwargs.get('with_hsl')
            with_resnet = kwargs.get('with_resnet')
            with_vgg = kwargs.get('with_vgg')
            hiknot = 5
            # Redirect output in file
            if console_file != None:
                try:
                    if (root_folder is not None):
                        console_file = root_folder + "/" + console_file
                    sys.stdout = open(console_file, 'w+')
                except (FileNotFoundError):
                    raise ValueError(f"Error : File {console_file} not found.")
                except (IOError):
                    raise ValueError(f"Error : Couldn't open file {console_file}.")

            valid_args = ['dataset', 'train_data_file', 'train_class_file', 'test_data_file', 'test_class_file', 'nb_attributes', 'nb_classes', 'valid_ratio', 'valid_data_file', 'valid_class_file', 'normalized', 'root_folder', 'console_file', 'train_valid_pred_file', 'test_pred_file', 'weights_file',
                          'stats_file', 'K', 'nb_quant_levels', 'nb_epochs', 'with_hsl', 'with_resnet', 'with_vgg']

            # Check if wrong parameters are given
            for arg_key in kwargs.keys():
                if arg_key not in valid_args:
                    raise ValueError(f"Invalid argument : {arg_key}.")

            root_folder, train_data_file, test_data_file, train_valid_pred_file, test_pred_file, stats_file, nb_attributes, nb_classes  = check_parameters_common(root_folder, train_data_file, test_data_file, train_valid_pred_file, test_pred_file, stats_file, nb_attributes, nb_classes)

            if valid_ratio is None:
                if (valid_data_file is None and valid_class_file is not None) or (valid_data_file is not None and valid_class_file is None):
                    raise ValueError('Error : parameter valid_data_file or parameter valid_class_file missing.')
            else:
                if valid_data_file is not None or valid_class_file is not None:
                    raise ValueError('Error : parameter valid_data_file or parameter valid_class_file is given but there is already a validation ratio.')
            if valid_ratio is None and valid_data_file is None and valid_class_file is None:
                valid_ratio = 0.1
            elif valid_ratio is not None and (valid_ratio <= 0 or valid_ratio >= 1):
                raise ValueError('Error : parameter valid_ratio has to be strictly bigger than 0 and strictly smaller than 1.')

            valid_data_file = validate_string_param(valid_data_file, "valid_data_file", allow_none=True)
            valid_class_file = validate_string_param(valid_class_file, "valid_class_file", allow_none=True)

            if dataset is None :
                raise ValueError('Error : dataset name missing, add it with option dataset and choose "mnist" or "cifar100" or "cifar10".')
            elif dataset not in {"mnist", "cifar100", "cifar10", "fer"}:
                raise ValueError('Error : parameter dataset is not "mnist" or "cifar100" or "cifar10".')

            if nb_epochs is None:
                nb_epochs = 80
            elif not check_int(nb_epochs) or not check_strictly_positive(nb_epochs):
                raise ValueError('Error : parameter nb_epochs is not a strictly positive integer.')

            if normalized is None:
                normalized = False
            elif not check_bool(normalized):
                raise ValueError('Error, parameter normalized is not a boolean.')

            if with_hsl is None:
                with_hsl = False
            elif not check_bool(with_hsl):
                raise ValueError('Error, parameter with_hsl is not a boolean.')
            if with_hsl:
                normalized = True

            if with_resnet is None:
                with_resnet = False
            elif not check_bool(with_resnet):
                raise ValueError('Error, parameter with_resnet is not a boolean.')

            if with_vgg is None:
                with_vgg = False
            elif not check_bool(with_vgg):
                raise ValueError('Error, parameter with_vgg is not a boolean.')
            if with_vgg == True and with_resnet == True:
                raise ValueError('Error, parameter with_resnet and with_vgg are both True, choose one.')

            weights_file, K, quant = check_parameters_dimlp_layer(weights_file, K, quant)

            model_checkpoint_weights = "weights.hdf5"
            if (root_folder is not None):
                train_data_file = root_folder + "/" + train_data_file
                test_data_file = root_folder + "/" + test_data_file
                if valid_ratio is None:
                    valid_data_file = root_folder + "/" + valid_data_file
                train_valid_pred_file = root_folder + "/" + train_valid_pred_file
                test_pred_file = root_folder + "/" + test_pred_file
                weights_file = root_folder + "/" + weights_file
                if (stats_file is not None):
                    stats_file = root_folder + "/" + stats_file
                model_checkpoint_weights = root_folder + "/" + model_checkpoint_weights

            ###############################################################

            print("Loading data...")

            mu = None
            sigma = None

            # Get data
            x_train_full_temp, y_train_full_temp = get_data(train_data_file, nb_attributes, nb_classes)
            x_train_full = np.array(x_train_full_temp)

            del x_train_full_temp
            if len(y_train_full_temp) == 0:
                train_class_file = validate_string_param(train_class_file, "train_class_file")
                if (root_folder is not None):
                    train_class_file = root_folder + "/" + train_class_file
                y_train_full_temp = get_data_class(train_class_file, nb_classes)
            y_train_full = np.array(y_train_full_temp)
            del y_train_full_temp

            if len(x_train_full) != len(y_train_full):
                raise ValueError('Error, there is not the same amount of train data and train class.')

            x_test, y_test = get_data(test_data_file, nb_attributes, nb_classes)
            if len(y_test) == 0:
                test_class_file = validate_string_param(test_class_file, "test_class_file")
                if (root_folder is not None):
                    test_class_file = root_folder + "/" + test_class_file
                y_test = get_data_class(test_class_file, nb_classes)
            x_test, y_test = np.array(x_test), np.array(y_test)

            if len(x_test) != len(y_test):
                raise ValueError('Error, there is not the same amount of test data and test class.')

            if valid_ratio is None:
                x_train = x_train_full
                y_train = y_train_full
                x_val, y_val = get_data(valid_data_file, nb_attributes, nb_classes)
                if len(y_val) == 0:
                    valid_class_file = validate_string_param(valid_class_file, "valid_class_file")
                    if (root_folder is not None):
                        valid_class_file = root_folder + "/" + valid_class_file
                    y_val = get_data_class(valid_class_file, nb_classes)
                x_val, y_val = np.array(x_val), np.array(y_val)

                if len(x_val) != len(y_val):
                    raise ValueError('Error, there is not the same amount of validation data and validation class.')
            else:
                cut_off = int(len(x_train_full)*(1-valid_ratio))
                if cut_off == 0 or cut_off == len(x_train_full):
                    raise ValueError('Error, problem during separation of data in train and validation sets. There is maybe not enough train data or a too big or too low validation ratio')
                x_train = x_train_full[:cut_off]
                x_val   = x_train_full[cut_off:]
                y_train = y_train_full[:cut_off]
                y_val   = y_train_full[cut_off:]

            classes = set(range(nb_classes))
            miss_train_classes = classes - set(y_train) # Check if a class is not represented during training


            if dataset == "mnist":
                #(x_train, y_train), (x_test, y_test) = mnist.load_data()

                #x_train = x_train[0:1000]
                #y_train = y_train[0:1000]
                #x_test = x_test[0:300]
                #y_test = y_test[0:300]

                size1d = 28
                nb_channels = 1

            elif dataset == "cifar100":
                #(x_train, y_train), (x_test, y_test) = cifar100.load_data(label_mode="fine")

                #x_train = x_train[0:1000]
                #y_train = y_train[0:1000]
                #x_test = x_test[0:300]
                #y_test = y_test[0:300]

                size1d = 32
                nb_channels = 3

            elif dataset == "cifar10":
                size1d = 32
                nb_channels = 3


            elif dataset == "fer":
                size1d = 48
                nb_channels = 1

            if dataset in {"fer", "cifar10"}:
                nb_var = len(x_train[0])
                # (x-mu)/sigma entre -5 et 5
                if normalized:
                    mu_val = 0.5
                    sigma_val = (1-0.5)/hiknot
                    mu = np.full(nb_var, mu_val)
                    sigma = np.full(nb_var, sigma_val)
                else:
                    mu_val = 127.5
                    sigma_val = (255-127.5)/hiknot
                    mu = np.full(nb_var, mu_val)
                    sigma = np.full(nb_var, sigma_val)

            print("Data loaded")

            x_train_h1, mu, sigma = compute_first_hidden_layer("train", x_train, K, quant, hiknot, weights_file, mu=mu, sigma=sigma)
            x_test_h1 = compute_first_hidden_layer("test", x_test, K, quant, hiknot, mu=mu, sigma=sigma)
            x_val_h1 = compute_first_hidden_layer("test", x_val, K, quant, hiknot, mu=mu, sigma=sigma)

            #y_train_h1 = compute_first_hidden_layer("test", y_train_flattened, K, quant, hiknot, mu=mu, sigma=sigma)
            #print("change Ytrain to array")
            #y_train_h1 = np.array(y_train_h1)
            #print("changed")
            #y_train_h1 = y_train_h1.reshape(y_train.shape)

            #train   = np.loadtxt("trainMnist784WC")

            # x_train = train.reshape(train.shape[0], 1, size1d, size1d)
            x_train_h1 = x_train_h1.reshape(x_train_h1.shape[0], size1d, size1d, nb_channels)

            #test   = np.loadtxt("testMnist2")

            # x_test = test.reshape(test.shape[0], 1, size1d, size1d)
            x_test_h1 = x_test_h1.reshape(x_test_h1.shape[0], size1d, size1d, nb_channels)
            x_val_h1 = x_val_h1.reshape(x_val_h1.shape[0], size1d, size1d, nb_channels)

            #y_train = np.loadtxt("mnistTrainClass")
            y_train = y_train.astype('int32')

            #y_test  = np.loadtxt("mnistTestClass")
            y_test  = y_test.astype('int32')

            y_val = y_val.astype('int32')


            y_train = keras.utils.to_categorical(y_train, nb_classes)
            y_test = keras.utils.to_categorical(y_test, nb_classes)
            y_val = keras.utils.to_categorical(y_val, nb_classes)

            ##############################################################################

            """
            if dataset == "mnist":
                x_val   = x_train_h1[50000:]
                x_train_h1_train = x_train_h1[0:50000]
                y_val   = y_train[50000:]
                y_train = y_train[0:50000]

            elif dataset == "cifar":
                x_val   = x_train_h1[40000:]
                x_train_h1_train = x_train_h1[0:40000]
                y_val   = y_train[40000:]
                y_train = y_train[0:40000]
            """
            ##############################################################################
            print("Training model...")

            if with_resnet:

                input_tensor = keras.Input(shape=(224, 224, 3))
                target_size = (224, 224)
                model_base = ResNet50(include_top=False, weights="imagenet", input_tensor=input_tensor)
                model = Sequential()
                model.add(Lambda(lambda image: tf.image.resize(image, target_size)))
                model.add(model_base)
                model.add(keras.layers.Flatten())
                model.add(Dropout(0.5))
                model.add(BatchNormalization())
                model.add(Dense(nb_classes, activation='softmax'))

                model.build((None, size1d, size1d, nb_channels))  # Build the model with the input shape

                model.compile(optimizer=keras.optimizers.Adam(learning_rate=0.00001), loss='categorical_crossentropy', metrics=['acc'])
                model.summary()


                """# charge pre-trained model ResNet50 with imageNet weights
                base_model = ResNet50(weights='imagenet', include_top=False, input_shape=(size1d, size1d, nb_channels))

                x = base_model.output
                x = GlobalAveragePooling2D()(x)
                x = Dense(256, activation='relu')(x)
                x = Dropout(0.3)(x)
                predictions = Dense(nb_classes, activation='softmax')(x)

                model = Model(inputs=base_model.input, outputs=predictions)

                # Frees layers of Resnet
                for layer in base_model.layers:
                    layer.trainable = False

                model.summary()

                model.compile(optimizer='adam', loss='mse', metrics=['accuracy'])"""

            elif with_vgg:
                # charge pre-trained model vgg with imageNet weights
                base_model = VGG16(weights='imagenet', include_top=False)

                x = base_model.output
                x = GlobalAveragePooling2D()(x)
                x = Dense(256, activation='relu')(x)
                x = Dropout(0.3)(x)
                predictions = Dense(nb_classes, activation='softmax')(x)

                model = Model(inputs=base_model.input, outputs=predictions)

                # Frees layers of Resnet
                for layer in base_model.layers:
                    layer.trainable = False

                model.summary()

                model.compile(optimizer='adam', loss='categorical_crossentropy', metrics=['acc'])

            elif dataset == "fer":
                # Define the model architecture
                model = Sequential()

                # Add a convolutional layer with 32 filters, 3x3 kernel size, and relu activation function
                model.add(Convolution2D(32, kernel_size=(3, 3), activation='relu', input_shape=(48,48,1)))
                # Add a batch normalization layer
                model.add(BatchNormalization())
                # Add a second convolutional layer with 64 filters, 3x3 kernel size, and relu activation function
                model.add(Convolution2D(64, kernel_size=(3, 3), activation='relu'))
                # Add a second batch normalization layer
                model.add(BatchNormalization())
                # Add a max pooling layer with 2x2 pool size
                model.add(MaxPooling2D(pool_size=(2, 2)))
                # Add a dropout layer with 0.25 dropout rate
                model.add(Dropout(0.25))

                # Add a third convolutional layer with 128 filters, 3x3 kernel size, and relu activation function
                model.add(Convolution2D(128, kernel_size=(3, 3), activation='relu'))
                # Add a third batch normalization layer
                model.add(BatchNormalization())
                # Add a fourth convolutional layer with 128 filters, 3x3 kernel size, and relu activation function
                model.add(Convolution2D(128, kernel_size=(3, 3), activation='relu'))
                # Add a fourth batch normalization layer
                model.add(BatchNormalization())
                # Add a max pooling layer with 2x2 pool size
                model.add(MaxPooling2D(pool_size=(2, 2)))
                # Add a dropout layer with 0.25 dropout rate
                model.add(Dropout(0.25))

                # Add a fifth convolutional layer with 256 filters, 3x3 kernel size, and relu activation function
                model.add(Convolution2D(256, kernel_size=(3, 3), activation='relu'))
                # Add a fifth batch normalization layer
                model.add(BatchNormalization())
                # Add a sixth convolutional layer with 256 filters, 3x3 kernel size, and relu activation function
                model.add(Convolution2D(256, kernel_size=(3, 3), activation='relu'))
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

                model.summary()
                # Compile the model with categorical cross-entropy loss, adam optimizer, and accuracy metric
                model.compile(loss="categorical_crossentropy", optimizer= tf.keras.optimizers.Adam(learning_rate=0.0001), metrics=['accuracy'])

            else:

                model = Sequential()

                # model.add(Convolution2D(32, (5, 5), activation='relu', data_format="channels_first", input_shape=(1, size1d, size1d)))
                # model.add(MaxPooling2D(pool_size=(2, 2), data_format="channels_first"))

                # # model.add(Convolution2D(32, (3, 3), activation='sigmoid'))
                # model.add(DepthwiseConv2D((5, 5), data_format="channels_first", activation='relu'))
                # model.add(MaxPooling2D(pool_size=(2, 2), data_format="channels_first"))

                model.add(Convolution2D(32, (5, 5), activation='relu', input_shape=(size1d, size1d, nb_channels)))
                model.add(Dropout(0.3))
                model.add(MaxPooling2D(pool_size=(2, 2)))

                model.add(DepthwiseConv2D((5, 5), activation='relu'))
                model.add(Dropout(0.3))
                model.add(MaxPooling2D(pool_size=(2, 2)))


                # model.add(Convolution2D(32, (3, 3), activation='relu'))

                model.add(Flatten())

                model.add(Dense(256, activation='sigmoid'))
                model.add(Dropout(0.3))
                # model.add(Dense(128, activation='sigmoid'))

                model.add(Dense(nb_classes, activation='sigmoid'))

                model.summary()

                model.compile(loss='mse', optimizer='adam', metrics=['accuracy'])

            ##############################################################################

            checkpointer = ModelCheckpoint(filepath=model_checkpoint_weights, verbose=1, save_best_only=True)
            #x_train_h1 = keras.applications.resnet50.preprocess_input(x_train_h1)
            #x_val_h1 = keras.applications.resnet50.preprocess_input(x_val_h1)
            model.fit(x_train_h1, y_train, batch_size=32, epochs=nb_epochs, validation_data=(x_val_h1, y_val), callbacks=[checkpointer], verbose=2)
            #model.fit(x_train_h1_train, y_train, batch_size=32, epochs=nb_epochs, validation_data=(x_test_h1, y_test), callbacks=[checkpointer], verbose=2)

            ##############################################################################

            model_best = load_model(model_checkpoint_weights)

            print("model trained")

            train_pred = model_best.predict(x_train_h1)    # Predict the response for train dataset
            test_pred = model_best.predict(x_test_h1)    # Predict the response for test dataset
            valid_pred = model_best.predict(x_val_h1)   # Predict the response for validation dataset
            train_valid_pred = np.concatenate((train_pred,valid_pred)) # We output predictions of both validation and training sets


            # If a class is missing, we adapt predictions
            if len(miss_train_classes)!=0:
                miss_train_classes = sorted(list(miss_train_classes))
                train_valid_pred = [pred.tolist() for pred in train_valid_pred]
                test_pred = [pred.tolist() for pred in test_pred]
                for train_pred_list in [train_valid_pred, test_pred]:
                    for pred in train_pred_list:
                        for classe in miss_train_classes:
                            pred.insert(classe, 0.0) # Prediction 0 for the missing class

            # Output predictions
            output_data(train_valid_pred, train_valid_pred_file)
            output_data(test_pred, test_pred_file)

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

            # Redirect output to terminal
            if console_file != None:
                sys.stdout = sys.__stdout__

            return 0

    except ValueError as error:
        # Redirect output to terminal
        if console_file != None:
            sys.stdout = sys.__stdout__
        print(error)
        return -1
