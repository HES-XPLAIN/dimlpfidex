# -*- coding: utf-8 -*-
"""
Created on Fri Mar 19 11:54:46 2021

@author: guido.bologna
"""

###############################################################

import os
os.environ["CUDA_DEVICE_ORDER"] = "PCI_BUS_ID"
os.environ["CUDA_VISIBLE_DEVICES"] = ""

import numpy as np
np.random.seed(seed=None)

from keras.models     import Sequential
from keras.layers     import Dense, Dropout, Activation, Flatten
from keras.layers     import Convolution2D, DepthwiseConv2D, LocallyConnected2D, MaxPooling2D
from keras.models     import load_model

from keras.callbacks  import ModelCheckpoint
from keras.datasets import mnist

###############################################################

size1D = 28    # for images
nbIt   = 80

###############################################################

print("Loading data...")

(X_train, Y_train), (X_test, Y_test) = mnist.load_data()
#train   = np.loadtxt("trainMnist784WC")

X_train = X_train.reshape(X_train.shape[0], 1, size1D, size1D)
# X_train = train.reshape(train.shape[0], size1D, size1D, 1)
X_train = X_train.astype('float32') / 255
print(X_train.shape[0])

#test   = np.loadtxt("testMnist2")

X_test = X_test.reshape(X_test.shape[0], 1, size1D, size1D)
# X_test = test.reshape(test.shape[0], size1D, size1D, 1)
X_test = X_test.astype('float32') / 255
print(X_test.shape[0])

#Y_train = np.loadtxt("mnistTrainClass")
Y_train = Y_train.astype('int32')

#Y_test  = np.loadtxt("mnistTestClass")
Y_test  = Y_test.astype('int32')

##############################################################################

x_train = X_train[0:50000]
x_val   = X_train[50000:]
y_train = Y_train[0:50000]
y_val   = Y_train[50000:]

##############################################################################

model = Sequential()

model.add(Convolution2D(32, (5, 5), activation='relu', data_format="channels_first", input_shape=(1, size1D, size1D)))
model.add(Dropout(0.3))
model.add(MaxPooling2D(pool_size=(2, 2), data_format="channels_first"))

# # model.add(Convolution2D(32, (3, 3), activation='sigmoid'))
# model.add(DepthwiseConv2D((5, 5), data_format="channels_first", activation='relu'))

model.add(Convolution2D(32, (5, 5), data_format="channels_first", activation='relu'))
model.add(Dropout(0.3))
model.add(MaxPooling2D(pool_size=(2, 2), data_format="channels_first"))

# model.add(Convolution2D(32, (5, 5), activation='relu', input_shape=(size1D, size1D, 1)))
# model.add(Dropout(0.3))
# model.add(MaxPooling2D(pool_size=(2, 2)))

# model.add(DepthwiseConv2D((5, 5), activation='relu'))
# model.add(Dropout(0.3))
# model.add(MaxPooling2D(pool_size=(2, 2)))


# model.add(Convolution2D(32, (3, 3), activation='relu'))

model.add(Flatten())

model.add(Dense(256, activation='sigmoid'))
model.add(Dropout(0.3))
# model.add(Dense(128, activation='sigmoid'))

model.add(Dense(10, activation='sigmoid'))

model.summary()

model.compile(loss='mse', optimizer='adam', metrics=['accuracy'])

##############################################################################

checkpointer = ModelCheckpoint(filepath='./weights.hdf5', verbose=1, save_best_only=True)

model.fit(x_train, y_train, batch_size=32, epochs=nbIt, validation_data=(x_val, y_val), callbacks=[checkpointer], verbose=2)

# model.fit(X_train, Y_train, batch_size=32, epochs=nbIt, validation_data=(X_test, Y_test), callbacks=[checkpointer], verbose=2)

##############################################################################

score = model.evaluate(X_train, Y_train)
print(score)

score = model.evaluate(X_test, Y_test)
print(score)

##############################################################################

modelBest = load_model('weights.hdf5')

score = modelBest.evaluate(X_train, Y_train)
print(score)

score = modelBest.evaluate(X_test, Y_test)
print(score)
