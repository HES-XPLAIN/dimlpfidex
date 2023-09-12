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

from tensorflow import keras
from keras.models     import Sequential
from keras.layers     import Dense, Dropout, Activation, Flatten
from keras.layers     import Convolution2D, DepthwiseConv2D, LocallyConnected2D, MaxPooling2D
from keras.models     import load_model

from keras.callbacks  import ModelCheckpoint
from keras.datasets import mnist
from keras.datasets import cifar100


from .trnFun import compute_first_hidden_layer, output_stats, check_parameters_dimlp_layer

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

def convKeras(*args, **kwargs):

    try:
        if args or not kwargs:
            print("Obligatory parameters :")
            print("dataset : mnist or cifar")
            print("Optional parameters :")
            print("save_folder : Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder.")
            print("train_pred : output train prediction file name without extension(predTrain by default)")
            print("test_pred : output test prediction file name without extension(predTest by default)")
            print("weights : output weights file name without extension (weights by default)")
            print("stats : output file name with train and test accuracy (stats.txt by default)")
            print("output_file : file where you redirect console result")
            print("nb_stairs : number of stairs in staircase activation function (50 by default)")
            print("hiknot : high side of the interval (5 by default)")
            print("K : Parameter to improve dynamics (1 by default)")
        else:

            start_time = time.time()

            # Get parameters
            dataset = kwargs.get('dataset')
            save_folder = kwargs.get('save_folder')
            output_file = kwargs.get('output_file')
            train_pred_file = kwargs.get('train_pred')
            test_pred_file = kwargs.get('test_pred')
            weights_file = kwargs.get('weights')
            stats_file = kwargs.get('stats')
            K = kwargs.get('K')
            quant = kwargs.get('nb_stairs')
            hiknot = kwargs.get('hiknot')

            # Redirect output in file
            if output_file != None:
                try:
                    if (save_folder is not None):
                        output_file = save_folder + "/" + output_file
                    sys.stdout = open(output_file, 'w+')
                except (FileNotFoundError):
                    raise ValueError(f"Error : File {output_file} not found.")
                except (IOError):
                    raise ValueError(f"Error : Couldn't open file {output_file}.")

            valid_args = ['dataset', 'save_folder', 'output_file', 'train_pred', 'test_pred', 'weights', 'stats', 'K', 'nb_stairs', 'hiknot']

            # Check if wrong parameters are given
            for arg_key in kwargs.keys():
                if arg_key not in valid_args:
                    raise ValueError(f"Invalid argument : {arg_key}")

            if dataset is None :
                raise ValueError('Error : dataset name missing, add it with option dataset and choose "mnist" or "cifar".')
            elif dataset not in {"mnist", "cifar"}:
                raise ValueError('Error : parameter dataset is not "mnist" or "cifar".')

            if (save_folder is not None and (not isinstance(save_folder, str))):
                raise ValueError('Error : parameter save_folder has to be a name contained in quotation marks "".')

            if train_pred_file is None:
                train_pred_file = "predTrain"
            elif not isinstance(train_pred_file, str):
                raise ValueError('Error : parameter predTrain has to be a name contained in quotation marks "".')
            train_pred_file += ".out"

            if test_pred_file is None:
                test_pred_file = "predTest"
            elif not isinstance(test_pred_file, str):
                raise ValueError('Error : parameter predTestn has to be a name contained in quotation marks "".')
            test_pred_file += ".out"

            if stats_file is None:
                stats_file = "stats.txt"
            elif stats_file is not None and not isinstance(stats_file, str):
                raise ValueError('Error : parameter stats_file has to be a name contained in quotation marks "".')

            weights_file, K, quant, hiknot = check_parameters_dimlp_layer(weights_file, K, quant, hiknot)

            if (save_folder is not None):
                train_pred_file = save_folder + "/" + train_pred_file
                test_pred_file = save_folder + "/" + test_pred_file
                weights_file = save_folder + "/" + weights_file
                if (stats_file is not None):
                    stats_file = save_folder + "/" + stats_file
                model_checkpoint_weights = save_folder + "/weights.hdf5"

            ###############################################################

            nb_it   = 80

            ###############################################################

            print("Loading data...")

            if dataset == "mnist":
                (x_train, y_train), (x_test, y_test) = mnist.load_data()

                #x_train = x_train[0:1000]
                #y_train = y_train[0:1000]
                #x_test = x_test[0:300]
                #y_test = y_test[0:300]

                nb_classes = 10

            elif dataset == "cifar":
                (x_train, y_train), (x_test, y_test) = cifar100.load_data(label_mode="fine")

                #x_train = x_train[0:1000]
                #y_train = y_train[0:1000]
                #x_test = x_test[0:300]
                #y_test = y_test[0:300]

                nb_classes = 100

            size1d = x_train.shape[1] # Size of image is size1d x size1d (Ex Mnist : 28x28)
            nb_chanels = x_train.shape[3]

            x_train = (x_train.astype('float32') / 255) * 10 - 5
            x_test = (x_test.astype('float32') / 255) * 10 - 5

            x_train_flattened = x_train.reshape(x_train.shape[0], -1)
            x_test_flattened = x_test.reshape(x_test.shape[0], -1)

            x_train_h1, mu, sigma = compute_first_hidden_layer("train", x_train_flattened, K, quant, hiknot, weights_file)
            x_train_h1 = x_train_h1.reshape(x_train.shape)

            x_test_h1 = compute_first_hidden_layer("test", x_test_flattened, K, quant, hiknot, mu=mu, sigma=sigma)
            x_test_h1 = x_test_h1.reshape(x_test.shape)

            #y_train_h1 = compute_first_hidden_layer("test", y_train_flattened, K, quant, hiknot, mu=mu, sigma=sigma)
            #print("change Ytrain to array")
            #y_train_h1 = np.array(y_train_h1)
            #print("changed")
            #y_train_h1 = y_train_h1.reshape(y_train.shape)

            #train   = np.loadtxt("trainMnist784WC")

            # x_train = train.reshape(train.shape[0], 1, size1d, size1d)
            x_train_h1 = x_train_h1.reshape(x_train_h1.shape[0], size1d, size1d, nb_chanels)

            #test   = np.loadtxt("testMnist2")

            # x_test = test.reshape(test.shape[0], 1, size1d, size1d)
            x_test_h1 = x_test_h1.reshape(x_test_h1.shape[0], size1d, size1d, nb_chanels)

            #y_train = np.loadtxt("mnistTrainClass")
            y_train = y_train.astype('int32')

            #y_test  = np.loadtxt("mnistTestClass")
            y_test  = y_test.astype('int32')


            y_train = keras.utils.to_categorical(y_train, nb_classes)
            y_test = keras.utils.to_categorical(y_test, nb_classes)
            ##############################################################################

            if dataset == "mnist":
                x_val   = x_train_h1[50000:]
                x_train = x_train_h1[0:50000]
                y_val   = y_train[50000:]
                y_train = y_train[0:50000]

            elif dataset == "cifar":
                x_val   = x_train_h1[40000:]
                x_train = x_train_h1[0:40000]
                y_val   = y_train[40000:]
                y_train = y_train[0:40000]

            ##############################################################################

            model = Sequential()

            # model.add(Convolution2D(32, (5, 5), activation='relu', data_format="channels_first", input_shape=(1, size1d, size1d)))
            # model.add(MaxPooling2D(pool_size=(2, 2), data_format="channels_first"))

            # # model.add(Convolution2D(32, (3, 3), activation='sigmoid'))
            # model.add(DepthwiseConv2D((5, 5), data_format="channels_first", activation='relu'))
            # model.add(MaxPooling2D(pool_size=(2, 2), data_format="channels_first"))

            model.add(Convolution2D(32, (5, 5), activation='relu', input_shape=(size1d, size1d, nb_chanels)))
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

            model.fit(x_train, y_train, batch_size=32, epochs=nb_it, validation_data=(x_val, y_val), callbacks=[checkpointer], verbose=2)
            #model.fit(x_train_h1, y_train, batch_size=32, epochs=nb_it, validation_data=(x_test_h1, y_test), callbacks=[checkpointer], verbose=2)

            ##############################################################################

            model_best = load_model(model_checkpoint_weights)

            train_pred = model_best.predict(x_train)    # Predict the response for train dataset
            test_pred = model_best.predict(x_test_h1)    # Predict the response for test dataset

            # Output predictions
            output_pred(train_pred, train_pred_file)
            output_pred(test_pred, test_pred_file)

            ##############################################################################

            print("\nResult :")

            score = model.evaluate(x_train, y_train)
            print("Train score : ", score)

            score = model.evaluate(x_test_h1, y_test)
            print("Test score : ", score)

            ##############################################################################

            print("\nBest result :")

            score = model_best.evaluate(x_train, y_train)
            print("Train score : ", score)

            score = model_best.evaluate(x_test_h1, y_test)
            print("Test score : ", score)

            acc_train = model_best.evaluate(x_train, y_train, verbose=0)[1]
            acc_test = model_best.evaluate(x_test_h1, y_test, verbose=0)[1]

            formatted_acc_train = "{:.6f}".format(acc_train*100)
            formatted_acc_test = "{:.6f}".format(acc_test*100)
            output_stats(stats_file, formatted_acc_train, formatted_acc_test)

            end_time = time.time()
            full_time = end_time - start_time
            full_time = "{:.6f}".format(full_time).rstrip("0").rstrip(".")

            print(f"\nFull execution time = {full_time} sec")

            # Redirect output to terminal
            if output_file != None:
                sys.stdout = sys.__stdout__

            return 0

    except ValueError as error:
        print(error)
        return -1
