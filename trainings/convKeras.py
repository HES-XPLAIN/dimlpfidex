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
np.random.seed(seed=None) #Seed not working

import tensorflow as tf
from tensorflow import keras
from keras.models     import Sequential
from keras.layers     import Dense, Dropout, Activation, Flatten, BatchNormalization, Lambda
from keras.layers     import Convolution2D, DepthwiseConv2D, MaxPooling2D, GlobalAveragePooling2D
from keras.models     import load_model, Model
from keras.applications import ResNet50, VGG16

from keras.callbacks  import ModelCheckpoint
from keras.datasets import mnist
from keras.datasets import cifar100
from keras.datasets import cifar10
import colorsys



from trainings.trnFun import compute_first_hidden_layer, output_stats, output_data, get_data, get_data_class
from trainings.parameters import get_initial_parser, get_args, sanitizepath, CustomArgumentParser, CustomHelpFormatter, TaggableAction, int_type, float_type, bool_type, print_parameters

def get_and_check_parameters(init_args):
    """
    Processes and validates command-line arguments for a convolution model training.
    This function cleans the input arguments by removing None values ensuring no unnecessary
    arguments are passed to the parser. It initializes the argument parser with basic
    configurations and adds various arguments required for the normalization process.
    It deternines which arguments are required or not and defines their default values.

    :param init_args: A list of command-line arguments passed to the program.
    :type init_args: list

    :return: A namespace object containing all the arguments that have been parsed and validated.
    :rtype: argparse.Namespace
    """
    # Remove None values and his -- attribute
    cleaned_args = [arg for i, arg in enumerate(init_args[:-1]) if ((not arg.startswith("--") or init_args[i+1] not in ["None", "none", None]) and arg not in ["None", "none", None])]
    if init_args and init_args[-1] not in ["None", "none", None]:
        cleaned_args.append(init_args[-1])

    # Get initial attributes with root_folder and json file information
    args, initial_parser = get_initial_parser(cleaned_args)

    # set custom formatter printing boundaries
    formatter = lambda prog: CustomHelpFormatter(prog, width=150, max_help_position=60)

    # Add new attributes
    parser = CustomArgumentParser(description="This is a parser for convKeras", parents=[initial_parser], formatter_class=formatter, add_help=True)
    parser.add_argument("--dataset", choices=["mnist", "cifar100", "cifar10", "fer"], metavar="<{mnist, cifar100, cifar10, fer}>", help="Dataset")
    parser.add_argument("--train_data_file", type=lambda x: sanitizepath(args.root_folder, x), help="Train data file", metavar="<str>", required=True)
    parser.add_argument("--train_class_file", type=lambda x: sanitizepath(args.root_folder, x), help="Train class file, mandatory if classes are not specified in train_data_file", metavar="<str>")
    parser.add_argument("--test_data_file", type=lambda x: sanitizepath(args.root_folder, x), help="Test data file", metavar="<str>", required=True)
    parser.add_argument("--test_class_file", type=lambda x: sanitizepath(args.root_folder, x), help="Test class file, mandatory if classes are not specified in test_data_file", metavar="<str>")
    parser.add_argument("--nb_attributes", type=lambda x: int_type(x, min=1), help="Number of attributes in dataset", metavar="<int [1,inf[>", required=True)
    parser.add_argument("--nb_classes", type=lambda x: int_type(x, min=1), help="Number of classes in dataset", metavar="<int [1,inf[>", required=True)
    parser.add_argument("--valid_ratio", type=lambda x: float_type(x, min=0, min_inclusive=False, max=1, max_inclusive=False), metavar="<float ]0,inf[>", help="Percentage of train data taken for validation (default: 0.1)")
    parser.add_argument("--valid_data_file", type=lambda x: sanitizepath(args.root_folder, x), help="Validation data file", metavar="<str>")
    parser.add_argument("--valid_class_file", type=lambda x: sanitizepath(args.root_folder, x), help="Validation class file, mandatory if classes are not specified in valid_data_file. BE CAREFUL if there is validation files, and you want to use Fidex algorithms later, you will have to use both train and validation datas given here in the train datas and classes of Fidex", metavar="<str>")
    parser.add_argument("--normalized", type=bool_type, metavar="<bool>", help="Whether input image datas are normalized between 0 and 1 (default: False, default if with_hsl: True)")
    parser.add_argument("--nb_epochs", type=lambda x: int_type(x, min=1), metavar="<int [1,inf[>", help="Number of training epochs", default=80)
    parser.add_argument("--train_valid_pred_outfile", type=lambda x: sanitizepath(args.root_folder, x, "w"), help="Output train and validation (in this order) prediction file name", metavar="<str>", default="predTrain.out")
    parser.add_argument("--test_pred_outfile", type=lambda x: sanitizepath(args.root_folder, x, "w"), help="Output test prediction file name", metavar="<str>", default="predTest.out")
    parser.add_argument("--weights_outfile", type=lambda x: sanitizepath(args.root_folder, x, "w"), help="Output weights file name", metavar="<str>", default="weights.wts")
    parser.add_argument("--stats_file", type=lambda x: sanitizepath(args.root_folder, x, "w"), help="Output statistic file name with train and test accuracy", metavar="<str>", default="stats.txt")
    parser.add_argument("--console_file", type=lambda x: sanitizepath(args.root_folder, x, 'w'), help="File with console logs redirection", metavar="<str>")
    parser.add_argument("--nb_quant_levels", type=lambda x: int_type(x, min=3), metavar="<int [3,inf[>", help="Number of stairs in staircase activation function", default=50)
    parser.add_argument("--K", type=lambda x: float_type(x, min=0, min_inclusive=False), metavar="<float ]0,inf[>", help="Parameter to improve dynamics", default=1.0)
    parser.add_argument("--with_hsl", type=bool_type, metavar="<bool>", help="Whether to change 3-channels data from RGB to HSL format", default=False, action=TaggableAction, tag="convKeras")
    parser.add_argument("--with_resnet", type=bool_type, metavar="<bool>", help="Whether to train with ResNet", default=False, action=TaggableAction, tag="convKeras")
    parser.add_argument("--with_vgg", type=bool_type, metavar="<bool>", help="Whether to train with VGG", default=False, action=TaggableAction, tag="convKeras")

    return get_args(args, cleaned_args, parser) # Return attributes

def convKeras(args: str = None):
    """
    Trains a convolutional neural network (CNN) model using the Keras library with optional support for popular architectures like ResNet and VGG.
    This function processes data preprocessing that includes normalization and a staircase activation function that allows for the characterization
    of discriminating hyperplanes, which are used in Fidex. This allows us to then use Fidex for comprehensible rule extraction. It accommodates
    various datasets including MNIST, CIFAR-10, and CIFAR-100, and allows for extensive customization through command-line arguments.

    Note:
    - It's mandatory to specify the number of attributes and classes in the data, as well as the train and test datasets.
    - Validation data can either be specified directly or split from the training data based on a provided ratio.
    - if validation files are given, and you want to use Fidex algorithms later, you will have to use both train and
      validation datas given here in the train datas and classes of Fidex.
    - Parameters can be specified using the command line or a JSON configuration file.
    - Providing no command-line arguments or using -h/--help displays usage instructions, detailing both required and optional parameters for user guidance.

    Formats:
    - Data files should contain one sample per line, with numbers separated either by spaces, tabs, semicolons or commas. Supported formats:
      1. Only attributes (floats).
      2. Attributes (floats) followed by an integer class ID.
      3. Attributes (floats) followed by one-hot encoded class.
    - Class files should contain one class sample per line, with integers separated either by spaces, tabs, semicolons or commas. Supported formats:
      1. Integer class ID.
      2. One-hot encoded class.

    Example of how to call the function:
    convKeras('--dataset mnist --train_data_file mnistTrainData.txt --train_class_file mnistTrainClass.txt --test_data_file mnistTestData.txt --test_class_file mnistTestClass.txt --valid_data_file mnistValidData.txt --valid_class_file mnistValidClass.txt --nb_attributes 784 --nb_classes 10 --root_folder dimlp/datafiles/Mnist')

    :param args: A single string containing either the path to a JSON configuration file with all specified arguments or all arguments for the function, formatted like command-line input.
                 This includes dataset selection, file paths, training parameters, and options for model architecture and output files.
    :return: Returns 0 for successful execution, -1 for any errors encountered during the process.
    """
    try:
        if not args:
            args = ""
        console_file = None # In case a ValueError is raised before initialization of console_file
        start_time = time.time()

        # Get parameters
        split_args = []
        if len(args) != 0:
            split_args = args.split()  # string command to list
        else:
            split_args= ["-h"]
        args = get_and_check_parameters(split_args)
        hiknot = 5
        console_file = args.console_file

        # Redirect output in file
        if console_file != None:
            try:
                sys.stdout = open(console_file, 'w+')
            except (FileNotFoundError):
                raise ValueError(f"Error : File {console_file} not found.")
            except (IOError):
                raise ValueError(f"Error : Couldn't open file {console_file}.")

        if args.valid_ratio is None:
            if (args.valid_data_file is None and args.valid_class_file is not None):
                raise ValueError('Error : parameter valid_data_file missing because valid_class_file was given.')
        else:
            if args.valid_data_file is not None:
                raise ValueError('Error : parameter valid_data_file is given but there is already a validation ratio given explicitly.')
        if args.valid_ratio is None and args.valid_data_file is None:
            args.valid_ratio = 0.1

        if args.normalized is None:
            args.normalized = False

        if args.with_hsl:
            args.normalized = True

        if args.with_vgg == True and args.with_resnet == True:
            raise ValueError('Error, parameter with_resnet and with_vgg are both True, choose one.')

        model_checkpoint_weights = "weights.keras"
        if (args.root_folder is not None):
            model_checkpoint_weights = os.path.join(args.root_folder, model_checkpoint_weights)

        print_parameters(args)

        ###############################################################

        print("Loading data...")

        mu = None
        sigma = None

        # Get data
        x_train_full_temp, y_train_full_temp = get_data(args.train_data_file, args.nb_attributes, args.nb_classes)
        x_train_full = np.array(x_train_full_temp)

        del x_train_full_temp
        if len(y_train_full_temp) == 0:
            if args.train_class_file is None:
                raise ValueError("Error : parameter --train_class_file is required because classes are not contained in train_data_file.")
            y_train_full_temp = get_data_class(args.train_class_file, args.nb_classes)
        y_train_full = np.array(y_train_full_temp)
        del y_train_full_temp

        if len(x_train_full) != len(y_train_full):
            raise ValueError('Error, there is not the same amount of train data and train class.')

        x_test, y_test = get_data(args.test_data_file, args.nb_attributes, args.nb_classes)
        if len(y_test) == 0:
            if args.test_class_file is None:
                raise ValueError("Error : parameter --test_class_file is required because classes are not contained in test_data_file.")
            y_test = get_data_class(args.test_class_file, args.nb_classes)
        x_test, y_test = np.array(x_test), np.array(y_test)

        if len(x_test) != len(y_test):
            raise ValueError('Error, there is not the same amount of test data and test class.')

        if args.valid_ratio is None:
            x_train = x_train_full
            y_train = y_train_full
            x_val, y_val = get_data(args.valid_data_file, args.nb_attributes, args.nb_classes)
            if len(y_val) == 0:
                if args.valid_class_file is None:
                    raise ValueError("Error : parameter --valid_class_file is required because classes are not contained in valid_data_file.")
                y_val = get_data_class(args.valid_class_file, args.nb_classes)
            x_val, y_val = np.array(x_val), np.array(y_val)

            if len(x_val) != len(y_val):
                raise ValueError('Error, there is not the same amount of validation data and validation class.')
        else:
            cut_off = int(len(x_train_full)*(1-args.valid_ratio))
            if cut_off == 0 or cut_off == len(x_train_full):
                raise ValueError('Error, problem during separation of data in train and validation sets. There is maybe not enough train data or a too big or too low validation ratio')
            x_train = x_train_full[:cut_off]
            x_val   = x_train_full[cut_off:]
            y_train = y_train_full[:cut_off]
            y_val   = y_train_full[cut_off:]

        classes = set(range(args.nb_classes))
        miss_train_classes = classes - set(y_train) # Check if a class is not represented during training


        if args.dataset == "mnist":
            #(x_train, y_train), (x_test, y_test) = mnist.load_data()

            #x_train = x_train[0:1000]
            #y_train = y_train[0:1000]
            #x_test = x_test[0:300]
            #y_test = y_test[0:300]

            size1d = 28
            nb_channels = 1

        elif args.dataset == "cifar100":
            #(x_train, y_train), (x_test, y_test) = cifar100.load_data(label_mode="fine")

            #x_train = x_train[0:1000]
            #y_train = y_train[0:1000]
            #x_test = x_test[0:300]
            #y_test = y_test[0:300]

            size1d = 32
            nb_channels = 3

        elif args.dataset == "cifar10":
            size1d = 32
            nb_channels = 3


        elif args.dataset == "fer":
            size1d = 48
            nb_channels = 1


        if args.dataset in {"fer", "cifar10"}:
            nb_var = len(x_train[0])
            # (x-mu)/sigma entre -5 et 5
            if args.normalized:
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

        x_train_h1, mu, sigma = compute_first_hidden_layer("train", x_train, args.K, args.nb_quant_levels, hiknot, args.weights_outfile, mu=mu, sigma=sigma)
        x_test_h1 = compute_first_hidden_layer("test", x_test, args.K, args.nb_quant_levels, hiknot, mu=mu, sigma=sigma)
        x_val_h1 = compute_first_hidden_layer("test", x_val, args.K, args.nb_quant_levels, hiknot, mu=mu, sigma=sigma)

        #y_train_h1 = compute_first_hidden_layer("test", y_train_flattened, args.K, args.nb_quant_levels, mu=mu, sigma=sigma)
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

        if (args.dataset == "fer" and args.with_vgg):
            # B&W to RGB
            x_train_h1 = np.repeat(x_train_h1, 3, axis=-1)
            x_test_h1 = np.repeat(x_test_h1, 3, axis=-1)
            x_val_h1 = np.repeat(x_val_h1, 3, axis=-1)
            nb_channels = 3

        #y_train = np.loadtxt("mnistTrainClass")
        y_train = y_train.astype('int32')

        #y_test  = np.loadtxt("mnistTestClass")
        y_test  = y_test.astype('int32')

        y_val = y_val.astype('int32')


        y_train = keras.utils.to_categorical(y_train, args.nb_classes)
        y_test = keras.utils.to_categorical(y_test, args.nb_classes)
        y_val = keras.utils.to_categorical(y_val, args.nb_classes)

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

        if args.with_resnet:

            input_tensor = keras.Input(shape=(224, 224, 3))
            target_size = (224, 224)
            model_base = ResNet50(include_top=False, weights="imagenet", input_tensor=input_tensor)
            model = Sequential()
            model.add(Lambda(lambda image: tf.image.resize(image, target_size)))
            model.add(model_base)
            model.add(keras.layers.Flatten())
            model.add(Dropout(0.5))
            model.add(BatchNormalization())
            model.add(Dense(args.nb_classes, activation='softmax'))

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

        elif args.with_vgg:
            # charge pre-trained model vgg with imageNet weights
            base_model = VGG16(weights='imagenet', include_top=False)

            x = base_model.output
            x = GlobalAveragePooling2D()(x)
            x = Dense(256, activation='relu')(x)
            x = Dropout(0.3)(x)
            predictions = Dense(args.nb_classes, activation='softmax')(x)

            model = Model(inputs=base_model.input, outputs=predictions)

            # Frees layers of Resnet
            for layer in base_model.layers:
                layer.trainable = False

            model.summary()

            model.compile(optimizer='adam', loss='categorical_crossentropy', metrics=['acc'])

        elif args.dataset == "fer":
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

            model.add(Dense(args.nb_classes, activation='sigmoid'))

            model.summary()

            model.compile(loss='mse', optimizer='adam', metrics=['accuracy'])

        ##############################################################################

        checkpointer = ModelCheckpoint(filepath=model_checkpoint_weights, verbose=1, save_best_only=True)
        #x_train_h1 = keras.applications.resnet50.preprocess_input(x_train_h1)
        #x_val_h1 = keras.applications.resnet50.preprocess_input(x_val_h1)
        model.fit(x_train_h1, y_train, batch_size=32, epochs=args.nb_epochs, validation_data=(x_val_h1, y_val), callbacks=[checkpointer], verbose=2)
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
        output_data(train_valid_pred, args.train_valid_pred_outfile)
        output_data(test_pred, args.test_pred_outfile)

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
        output_stats(args.stats_file, formatted_acc_train, formatted_acc_test)

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

if __name__ == "__main__":
    cmdline_args = " ".join(sys.argv[1:])
    convKeras(cmdline_args)
