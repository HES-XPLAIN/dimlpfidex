# -*- coding: utf-8 -*-
"""
Created on Fri Mar 19 11:54:46 2021

@author: guido.bologna
"""

###############################################################

import os
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

def convKeras():

    try:

        start_time = time.time()

        ###############################################################

        size1d = 28    # for images
        nb_it   = 80

        ###############################################################

        print("Loading data...")

        (x_train, y_train), (x_test, y_test) = mnist.load_data()

        x_train = x_train[0:1000]
        y_train = y_train[0:1000]
        x_test = x_test[0:300]
        y_test = y_test[0:300]

        x_train = (x_train.astype('float32') / 255) * 10 - 5
        x_test = (x_test.astype('float32') / 255) * 10 - 5

        x_train_flattened = x_train.reshape(x_train.shape[0], -1)
        x_test_flattened = x_test.reshape(x_test.shape[0], -1)

        K = 1
        quant = 50
        hiknot = 5
        weights_file = "dimlp/weights.wts"
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
        x_train_h1 = x_train_h1.reshape(x_train_h1.shape[0], size1d, size1d, 1)

        #test   = np.loadtxt("testMnist2")

        # x_test = test.reshape(test.shape[0], 1, size1d, size1d)
        x_test_h1 = x_test_h1.reshape(x_test_h1.shape[0], size1d, size1d, 1)

        #y_train = np.loadtxt("mnistTrainClass")
        y_train = y_train.astype('int32')

        #y_test  = np.loadtxt("mnistTestClass")
        y_test  = y_test.astype('int32')


        y_train = keras.utils.to_categorical(y_train, 10)
        y_test = keras.utils.to_categorical(y_test, 10)
        ##############################################################################

        x_val   = x_train_h1[800:]
        x_train = x_train_h1[0:800]
        y_val   = y_train[800:]
        y_train = y_train[0:800]

        ##############################################################################

        model = Sequential()

        # model.add(Convolution2D(32, (5, 5), activation='relu', data_format="channels_first", input_shape=(1, size1d, size1d)))
        # model.add(MaxPooling2D(pool_size=(2, 2), data_format="channels_first"))

        # # model.add(Convolution2D(32, (3, 3), activation='sigmoid'))
        # model.add(DepthwiseConv2D((5, 5), data_format="channels_first", activation='relu'))
        # model.add(MaxPooling2D(pool_size=(2, 2), data_format="channels_first"))

        model.add(Convolution2D(32, (5, 5), activation='relu', input_shape=(size1d, size1d, 1)))
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

        model.add(Dense(10, activation='sigmoid'))

        model.summary()

        model.compile(loss='mse', optimizer='adam', metrics=['accuracy'])

        ##############################################################################

        checkpointer = ModelCheckpoint(filepath='./dimlp/weights.hdf5', verbose=1, save_best_only=True)

        model.fit(x_train, y_train, batch_size=32, epochs=nb_it, validation_data=(x_val, y_val), callbacks=[checkpointer], verbose=2)
        #model.fit(x_train_h1, y_train, batch_size=32, epochs=nb_it, validation_data=(x_test_h1, y_test), callbacks=[checkpointer], verbose=2)

        ##############################################################################

        model_best = load_model('dimlp/weights.hdf5')

        train_pred = model_best.predict(x_train)    # Predict the response for train dataset
        test_pred = model_best.predict(x_test_h1)    # Predict the response for test dataset

        # Output predictions
        output_pred(train_pred, "dimlp/predTrain.out")
        output_pred(test_pred, "dimlp/predTest.out")

        ##############################################################################

        print("Result :")

        score = model.evaluate(x_train, y_train)
        print(score)

        score = model.evaluate(x_test_h1, y_test)
        print(score)

        ##############################################################################

        print("Best result :")

        score = model_best.evaluate(x_train, y_train)
        print(score)

        score = model_best.evaluate(x_test_h1, y_test)
        print(score)

        acc_train = model_best.evaluate(x_train, y_train, verbose=0)[1]
        acc_test = model_best.evaluate(x_test_h1, y_test, verbose=0)[1]

        formatted_acc_train = "{:.6f}".format(acc_train*100)
        formatted_acc_test = "{:.6f}".format(acc_test*100)
        output_stats("dimlp/stats.txt", formatted_acc_train, formatted_acc_test)

        end_time = time.time()
        full_time = end_time - start_time
        full_time = "{:.6f}".format(full_time).rstrip("0").rstrip(".")

        print(f"\nFull execution time = {full_time} sec")

    except ValueError as error:
        print(error)
        return -1
