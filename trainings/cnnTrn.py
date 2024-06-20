import os
import sys
import time
os.environ["CUDA_DEVICE_ORDER"] = "PCI_BUS_ID"
#os.environ["CUDA_VISIBLE_DEVICES"] = "" // To choose the GPU you want to use
os.environ["KERAS_BACKEND"] = "torch"

import numpy as np
import random
import torch
import keras
from keras import Input
from keras.models import Sequential, load_model
from keras.layers import Dense, Dropout, Flatten, BatchNormalization, Resizing, Convolution2D, DepthwiseConv2D, MaxPooling2D
from keras.applications import ResNet50, VGG16
from keras.callbacks import ModelCheckpoint



from trainings.trnFun import compute_first_hidden_layer, output_stats, output_data, get_data, get_data_class
from trainings.parameters import get_initial_parser, get_args, sanitizepath, CustomArgumentParser, CustomHelpFormatter, TaggableAction, int_type, float_type, print_parameters, pair_type

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
    parser = CustomArgumentParser(description="This is a parser for cnnTrn", parents=[initial_parser], formatter_class=formatter, add_help=True)
    parser.add_argument("--train_data_file", type=lambda x: sanitizepath(args.root_folder, x), help="Path to the file containing the train portion of the dataset", metavar="<str>", required=True)
    parser.add_argument("--train_class_file", type=lambda x: sanitizepath(args.root_folder, x), help="Path to the file containing the train true classes of the dataset, mandatory if classes are not specified in train_data_file", metavar="<str>")
    parser.add_argument("--test_data_file", type=lambda x: sanitizepath(args.root_folder, x), help="Path to the file containing the test portion of the dataset", metavar="<str>", required=True)
    parser.add_argument("--test_class_file", type=lambda x: sanitizepath(args.root_folder, x), help="Path to the file containing the test true classes of the dataset, mandatory if classes are not specified in test_data_file", metavar="<str>")
    parser.add_argument("--original_input_size", type=lambda x: pair_type(x, dict(func=int_type, min=1)), metavar="<pair<int [1, inf[>>", help="Original input size size", required=True)
    parser.add_argument("--nb_channels", type=lambda x: int_type(x, min=1), metavar="<int [1,inf[>", help="Number of channels in the input (3 for RGB image, 1 for B&W image)", required=True)
    parser.add_argument("--model", choices=["small", "large", "vgg", "resnet"], metavar="<{small, large, vgg, resnet}>", help="Training model", required=True)
    parser.add_argument("--data_format", choices=["normalized_01", "classic", "other"], metavar="<{normalized_01, classic, other}>", help="Format of the values of the data, normalized_01 if the data are normalized between 0 and 1, classic if they are between 0 and 255", required=True)
    parser.add_argument("--nb_classes", type=lambda x: int_type(x, min=1), help="Number of classes in the dataset", metavar="<int [1,inf[>", required=True)
    parser.add_argument("--train_valid_pred_outfile", type=lambda x: sanitizepath(args.root_folder, x, "w"), help="Path to the file where the output train and validation (in this order) prediction will be stored", metavar="<str>", default="predTrain.out")
    parser.add_argument("--test_pred_outfile", type=lambda x: sanitizepath(args.root_folder, x, "w"), help="Path to the file where the test predictions will be stored", metavar="<str>", default="predTest.out")
    parser.add_argument("--valid_ratio", type=lambda x: float_type(x, min=0, min_inclusive=False, max=1, max_inclusive=False), metavar="<float ]0,inf[>", help="Percentage of train data taken for validation (default: 0.1)")
    parser.add_argument("--valid_data_file", type=lambda x: sanitizepath(args.root_folder, x), help="Path to the file containing the validation portion of the dataset", metavar="<str>")
    parser.add_argument("--valid_class_file", type=lambda x: sanitizepath(args.root_folder, x), help="Path to the file containing the validation true classes of the dataset, mandatory if classes are not specified in valid_data_file. BE CAREFUL if there is validation files, and you want to use Fidex algorithms later, you will have to use both train and validation datas given here in the train datas and classes of Fidex", metavar="<str>")
    parser.add_argument("--weights_outfile", type=lambda x: sanitizepath(args.root_folder, x, "w"), help="Path to the file where the output trained weights of the model will be stored", metavar="<str>", default="weights.wts")
    parser.add_argument("--stats_file", type=lambda x: sanitizepath(args.root_folder, x, "w"), help="Path to the file where the train and test accuracy will be stored", metavar="<str>", default="stats.txt")
    parser.add_argument("--console_file", type=lambda x: sanitizepath(args.root_folder, x, 'w'), help="Path to the file where the terminal output will be redirected. If not specified, all output will be shown on your terminal", metavar="<str>")
    parser.add_argument("--nb_epochs", type=lambda x: int_type(x, min=1), metavar="<int [1,inf[>", help="Number of model training epochs", default=80)
    parser.add_argument("--model_input_size", type=lambda x: pair_type(x, dict(func=int_type, min=1)), metavar="<pair<int [1, inf[>>", help="Input size in the model. A small size is recommended to speed up the process. The size is modified if necessary", action=TaggableAction, tag="CNN")
    parser.add_argument("--nb_quant_levels", type=lambda x: int_type(x, min=3), metavar="<int [3,inf[>", help="Number of stairs in the staircase activation function", default=50)
    parser.add_argument("--K", type=lambda x: float_type(x, min=0, min_inclusive=False), metavar="<float ]0,inf[>", help="Parameter to improve dynamics by normalizing input data", default=1.0)
    parser.add_argument("--seed", type=lambda x: int_type(x, min=0), metavar="<{int [0,inf[}>", help="Seed for random number generation, 0=random. Anything else than 0 is an arbitrary seed that can be reused to obtain the same randomly generated sequence and therefore getting same results", action=TaggableAction, tag="CNN", default=0)

    return get_args(args, cleaned_args, parser) # Return attributes

def cnnTrn(args: str = None):
    """
    Trains a convolutional neural network (CNN) model using the Keras library with optional support for popular architectures like ResNet and VGG.
    This function processes data preprocessing that includes resizing, normalization and a staircase activation function that allows for the characterization
    of discriminating hyperplanes, which are used in Fidex. This allows us to then use Fidex for comprehensible rule extraction. It accommodates
    various types of image datasets including MNIST, CIFAR-10, and CIFAR-100, and allows for extensive customization through command-line arguments.
    It's also possible to use other data types.

    Notes:
    - Each file is located with respect to the root folder dimlpfidex or to the content of the 'root_folder' parameter if specified.
    - It's mandatory to specify the number of classes in the data, as well as the train and test datasets.
    - Validation data can either be specified directly or split from the training data based on a provided ratio.
    - If validation files are given, and you want to use Fidex algorithms later, you will have to use both train and
      validation datas given here in the train datas and classes of Fidex.
    - It's mandatory to specify the size of the original inputs as well as the number of channels (it should be 3 for RGB and 1 for B&W). The number of attributes is inferred from it.
    - It's mandatory to chose a model. There is a large model, a small one, a VGG16 and a Resnet50 available. You can add any other model you want by modifying the code.
    - It's mandatory to specify the format of the data values: 'normalized_01' if the data are normalized between 0 and 1, 'classic' if they are between 0 and 255, and 'other' otherwise.
    - Data is reshaped in 3-channels shape if there is only one and usinf VGG or Resnet.
    - If Fidex is meant to be executed afterward for rule extraction, resizing inputs beforhand to a smaller size is recommended as it will take a lot of time because of the number of parameters.
    - It is also possible to resize the inputs just for training with the model_input_size parameter. Training with smaller inputs will be worst but will save a lot of time.
    - Parameters can be specified using the command line or a JSON configuration file.
    - Providing no command-line arguments or using -h/--help displays usage instructions, detailing both required and optional parameters for user guidance.
    - It's not necessary to normalize data before training because a normalization is done during the process.

    Outputs:
    - train_valid_pred_outfile : File containing the model's train and validation (in this order) predictions.
    - test_pred_outfile : File containing the model's test predictions.
    - weights_outfile : File containing the model's trained weights.
    - stats_file : File containing train and test accuracy.
    - console_file : If specified, contains the console output.

    File formats:
    - **Data files**: These files should contain one sample (input/image) per line, with numbers separated either by spaces, tabs, semicolons or commas. Each pixel must be given one after the other. Supported formats:
      1. Only attributes (floats).
      2. Attributes (floats) followed by an integer class ID.
      3. Attributes (floats) followed by one-hot encoded class.
    - **Class files**: These files should contain one class sample per line, with integers separated either by spaces, tabs, semicolons or commas. Supported formats:
      1. Integer class ID.
      2. One-hot encoded class.

    Example of how to call the function:
    from trainings.cnnTrn import cnnTrn
    cnnTrn('--model small --train_data_file trainData.txt --train_class_file trainClass.txt --test_data_file testData.txt --test_class_file testClass.txt --valid_data_file validData.txt --valid_class_file validClass.txt --original_input_size (28,28) --nb_channels 1 --data_format classic --nb_classes 10 --root_folder dimlp/datafiles/Mnist')

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

        # Generate random seed
        if args.seed != 0:
            os.environ['PYTHONHASHSEED'] = str(args.seed)
            random.seed(args.seed)
            np.random.seed(args.seed)
            torch.manual_seed(args.seed)
            torch.cuda.manual_seed(args.seed)
            torch.cuda.manual_seed_all(args.seed)  # if you are using multi-GPU.
            torch.backends.cudnn.deterministic = True
            torch.backends.cudnn.benchmark = False

        if args.valid_ratio is None:
            if (args.valid_data_file is None and args.valid_class_file is not None):
                raise ValueError('Error : parameter valid_data_file missing because valid_class_file was given.')
        else:
            if args.valid_data_file is not None:
                raise ValueError('Error : parameter valid_data_file is given but there is already a validation ratio given explicitly.')
        if args.valid_ratio is None and args.valid_data_file is None:
            args.valid_ratio = 0.1

        img_size = args.model_input_size if args.model_input_size else args.original_input_size
        if args.model == "vgg":
            if img_size[0] < 32 or img_size[1] < 32:
                raise ValueError('Error : The VGG model do not accept an input size smaller than 32 in each coordinate. Use model_input_size to increase the input size in the model.')
        elif args.model == "small":
            if img_size[0] < 16 or img_size[1] < 16:
                raise ValueError('Error : The small model do not accept an input size smaller than 16 in each coordinate. Use model_input_size to increase the input size in the model.')
        elif args.model == "large":
            if img_size[0] < 36 or img_size[1] < 36:
                raise ValueError('Error : The large model do not accept an input size smaller than 36 in each coordinate. Use model_input_size to increase the input size in the model.')

        model_checkpoint_weights = "weightsModel.keras"
        if (args.root_folder is not None):
            model_checkpoint_weights = os.path.join(args.root_folder, model_checkpoint_weights)

        print_parameters(args)

        ###############################################################

        print("Loading data...")

        mu = None
        sigma = None

        # Get data
        nb_attributes = args.original_input_size[0]*args.original_input_size[1]*args.nb_channels
        x_train_full_temp, y_train_full_temp = get_data(args.train_data_file, nb_attributes, args.nb_classes)
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

        x_test, y_test = get_data(args.test_data_file, nb_attributes, args.nb_classes)
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
            x_val, y_val = get_data(args.valid_data_file, nb_attributes, args.nb_classes)
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

        nb_train_samples = len(x_train[0])
        # (x-mu)/sigma between -5 and 5
        if args.data_format == "normalized_01":
            mu_val = 0.5
            sigma_val = (1-0.5)/hiknot
            mu = np.full(nb_train_samples, mu_val)
            sigma = np.full(nb_train_samples, sigma_val)
        elif args.data_format == "classic":
            mu_val = 127.5
            sigma_val = (255-127.5)/hiknot
            mu = np.full(nb_train_samples, mu_val)
            sigma = np.full(nb_train_samples, sigma_val)

        print("Data loaded")
        # Data are flattend : nbSamples x nbAttributes (not nbSamples x 32x32x3 for example)
        x_train_h1, mu, sigma = compute_first_hidden_layer("train", x_train, args.K, args.nb_quant_levels, hiknot, args.weights_outfile, mu=mu, sigma=sigma)
        x_test_h1 = compute_first_hidden_layer("test", x_test, args.K, args.nb_quant_levels, hiknot, mu=mu, sigma=sigma)
        x_val_h1 = compute_first_hidden_layer("test", x_val, args.K, args.nb_quant_levels, hiknot, mu=mu, sigma=sigma)

        x_train_h1 = x_train_h1.reshape(x_train_h1.shape[0], args.original_input_size[0], args.original_input_size[1], args.nb_channels)
        x_test_h1 = x_test_h1.reshape(x_test_h1.shape[0], args.original_input_size[0], args.original_input_size[1], args.nb_channels)
        x_val_h1 = x_val_h1.reshape(x_val_h1.shape[0], args.original_input_size[0], args.original_input_size[1], args.nb_channels)

        # VGG and Resnet need 3 channels
        if (args.nb_channels == 1 and (args.model == "vgg" or args.model == "resnet")):
            # B&W to RGB
            x_train_h1 = np.repeat(x_train_h1, 3, axis=-1)
            x_test_h1 = np.repeat(x_test_h1, 3, axis=-1)
            x_val_h1 = np.repeat(x_val_h1, 3, axis=-1)
            args.nb_channels = 3

        y_train = y_train.astype('int32')

        y_test  = y_test.astype('int32')

        y_val = y_val.astype('int32')


        y_train = keras.utils.to_categorical(y_train, args.nb_classes)
        y_test = keras.utils.to_categorical(y_test, args.nb_classes)
        y_val = keras.utils.to_categorical(y_val, args.nb_classes)

        ##############################################################################
        print("Training model...")

        if args.model == "resnet":

            input_tensor = keras.Input(shape=(img_size[0], img_size[1], 3))
            model_base = ResNet50(include_top=False, weights="imagenet", input_tensor=input_tensor)
            model = Sequential()
            if args.model_input_size is not None:
                model.add(Resizing(img_size[0], img_size[1]))
            model.add(model_base)
            model.add(keras.layers.Flatten())
            model.add(Dropout(0.5))
            model.add(BatchNormalization())
            model.add(Dense(args.nb_classes, activation='softmax'))

            model.build((None, img_size[0], img_size[1], 3))  # Build the model with the input shape

            model.compile(optimizer=keras.optimizers.Adam(learning_rate=0.00001), loss='categorical_crossentropy', metrics=['acc'])
            model.summary()

        elif args.model == "vgg":
            input_tensor = keras.Input(shape=(img_size[0], img_size[1], 3))

            # charge pre-trained model vgg with imageNet weights
            model_base = VGG16(include_top=False, weights="imagenet", input_tensor=input_tensor)

            # Freeze layers of VGG
            for layer in model_base.layers:
                layer.trainable = False

            model = Sequential()
            if args.model_input_size is not None:
                model.add(Resizing(img_size[0], img_size[1]))
            model.add(model_base)
            model.add(keras.layers.Flatten())
            model.add(Dense(256, activation='relu'))
            model.add(Dropout(0.3))
            model.add(BatchNormalization())
            model.add(Dense(args.nb_classes, activation='softmax'))

            model.build((None, img_size[0], img_size[1], 3))  # Build the model with the input shape

            model.compile(optimizer=keras.optimizers.Adam(learning_rate=0.00001), loss='categorical_crossentropy', metrics=['acc'])
            model.summary()


        elif args.model =="large":
            # Define the model architecture
            model = Sequential()

            model.add(Input(shape=(img_size[0], img_size[1], args.nb_channels)))
            if args.model_input_size is not None:
                model.add(Resizing(img_size[0], img_size[1]))
            # Add a convolutional layer with 32 filters, 3x3 kernel size, and relu activation function
            model.add(Convolution2D(32, kernel_size=(3, 3), activation='relu'))
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
            model.add(Dense(args.nb_classes, activation='softmax'))

            model.summary()
            # Compile the model with categorical cross-entropy loss, adam optimizer, and accuracy metric
            model.compile(loss="categorical_crossentropy", optimizer= keras.optimizers.Adam(learning_rate=0.0001), metrics=['accuracy'])

        elif args.model == "small":

            model = Sequential()

            model.add(Input(shape=(img_size[0], img_size[1], args.nb_channels)))
            if args.model_input_size is not None:
                model.add(Resizing(img_size[0], img_size[1]))

            model.add(Convolution2D(32, (5, 5), activation='relu'))
            model.add(Dropout(0.3))
            model.add(MaxPooling2D(pool_size=(2, 2)))

            model.add(DepthwiseConv2D((5, 5), activation='relu'))
            model.add(Dropout(0.3))
            model.add(MaxPooling2D(pool_size=(2, 2)))

            model.add(Flatten())

            model.add(Dense(256, activation='sigmoid'))
            model.add(Dropout(0.3))

            model.add(Dense(args.nb_classes, activation='sigmoid'))

            model.summary()

            model.compile(loss='mse', optimizer='adam', metrics=['accuracy'])

        else:
            raise ValueError(f"Internal error : No model has been executed, given model : {args.model}.")

        ##############################################################################

        checkpointer = ModelCheckpoint(filepath=model_checkpoint_weights, verbose=1, save_best_only=True)
        model.fit(x_train_h1, y_train, batch_size=32, epochs=args.nb_epochs, validation_data=(x_val_h1, y_val), callbacks=[checkpointer], verbose=2)

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
    """
    Entry point for the script. Parses command-line arguments and initiates the training process for the convolutional neural network (CNN) model.

    The script is designed to be run from the command line with various arguments that control the CNN training process,
    including dataset selection, file paths for training and testing data, the number of attributes and classes, and options for model architecture and output files.

    Usage:
        python script.py [options]

    Example:
        python script.py --model small --train_data_file trainData.txt --train_class_file trainClass.txt \
                         --test_data_file testData.txt --test_class_file testClass.txt \
                         --valid_data_file validData.txt --valid_class_file validClass.txt \
                         --original_input_size (28,28) --nb_channels 1 --data_format classic --nb_classes 10 --root_folder dimlp/datafiles/Mnist

    :param sys.argv: List of command-line arguments passed to the script.
    :type sys.argv: list
    """
    cmdline_args = " ".join(sys.argv[1:])
    cnnTrn(cmdline_args)
