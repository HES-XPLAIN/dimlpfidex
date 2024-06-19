import time
import sys
from sklearn.neural_network import MLPClassifier
import warnings
from sklearn.exceptions import ConvergenceWarning
from sklearn import metrics

from trainings.trnFun import get_data, get_data_class, output_data, compute_first_hidden_layer, output_stats
from trainings.parameters import get_common_parser, get_initial_parser, get_args, sanitizepath, CustomArgumentParser, CustomHelpFormatter, TaggableAction, int_type, float_type, bool_type, list_type, enum_type, print_parameters

warnings.filterwarnings("ignore", category=ConvergenceWarning)

def get_and_check_parameters(init_args):
    """
    Processes and validates command-line arguments for a model training with MLP.
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

    # Add common attributes
    common_parser = get_common_parser(args, initial_parser)

    # set custom formatter printing boundaries
    formatter = lambda prog: CustomHelpFormatter(prog, width=150, max_help_position=60)

    # Add new attributes
    parser = CustomArgumentParser(description="This is a parser for mlpTrn", parents=[common_parser], formatter_class=formatter, add_help=True)
    parser.add_argument("--weights_outfile", type=lambda x: sanitizepath(args.root_folder, x, "w"), help="Path to the file where the output trained weights of the model will be stored", metavar="<str>", default="weights.wts")
    parser.add_argument("--nb_quant_levels", type=lambda x: int_type(x, min=3), metavar="<int [3,inf[>", help="Number of stairs in the staircase activation function", default=50)
    parser.add_argument("--K", type=lambda x: float_type(x, min=0, min_inclusive=False), metavar="<float ]0,inf[>", help="Parameter to improve dynamics by normalizing input data", default=1.0)
    parser.add_argument("--hidden_layer_sizes", type=lambda x: list_type(x, dict(func=int_type, min=1)), metavar="<list<int [1, inf[>>", help="Size of each hidden layers", default=100, action=TaggableAction, tag="MLP")
    parser.add_argument("--activation", choices=["identity", "logistic", "tanh", "relu"], metavar="<{identity, logistic, tanh, relu}>", help="Activation function", default="relu", action=TaggableAction, tag="MLP")
    parser.add_argument("--solver", choices=["lbfgs", "sgd", "adam"], metavar="<{lbfgs, sgd, adam}>", help="Solver for weight optimization", default="adam", action=TaggableAction, tag="MLP")
    parser.add_argument("--alpha", type=lambda x: float_type(x, min=0), metavar="<float [0,inf[>", help="Strength of the L2 regularization term", default=0.0001, action=TaggableAction, tag="MLP")
    parser.add_argument("--batch_size", type=lambda x: enum_type(x, "auto", int_type=dict(func=int_type, min=1)), metavar="<{auto, int [1,inf[}>", help="Size of minibatches for stochastic optimizers for adam and stochastic gradient descent", default="auto", action=TaggableAction, tag="MLP")
    parser.add_argument("--learning_rate", choices=["constant", "invscaling", "adaptive"], metavar="<{constant, invscaling, adaptive}>", help="Learning rate schedule for weight updates for stochastic gradient descent solver", default="constant", action=TaggableAction, tag="MLP")
    parser.add_argument("--learning_rate_init", type=lambda x: float_type(x, min=0, min_inclusive=False), metavar="<float ]0,inf[>", help="Initial learning rate for adam and stochastic gradient descent", default=0.001, action=TaggableAction, tag="MLP")
    parser.add_argument("--power_t", type=lambda x: float_type(x, min=0), metavar="<float [0,inf[>", help="Exponent for inverse scaling learning rate for stochastic gradient descent", default=0.5, action=TaggableAction, tag="MLP")
    parser.add_argument("--max_iterations", type=lambda x: int_type(x, min=1), metavar="<int [1,inf[>", help="Maximum number of training iterations", default=200, action=TaggableAction, tag="MLP")
    parser.add_argument("--shuffle", type=bool_type, metavar="<bool>", help="Whether to shuffle samples in each iteration for stochastic gradient descent and adam", default=True, action=TaggableAction, tag="MLP")
    parser.add_argument("--seed", type=lambda x: int_type(x, min=0), metavar="<{int [0,inf[}>", help="Seed for random number generation for stochastic gradient descent and adam", action=TaggableAction, tag="MLP")
    parser.add_argument("--tol", type=lambda x: float_type(x, min=0, min_inclusive=False), metavar="<float ]0,inf[>", help="Tolerance for optimization", default=0.0001, action=TaggableAction, tag="MLP")
    parser.add_argument("--verbose", type=bool_type, metavar="<bool>", help="Enable verbose output", default=False, action=TaggableAction, tag="MLP")
    parser.add_argument("--warm_start", type=bool_type, metavar="<bool>", help="Whether to reuse the previous solution to fit initialization", default=False, action=TaggableAction, tag="MLP")
    parser.add_argument("--momentum", type=lambda x: float_type(x, min=0, max=1), metavar="<float [0,1]>", help="Momentum for gradient descent update for stochastic gradient descent", default=0.9, action=TaggableAction, tag="MLP")
    parser.add_argument("--nesterovs_momentum", type=bool_type, metavar="<bool>", help="Whether to use the Nesterov’s momentum for stochastic gradient descent and momentum", default=True, action=TaggableAction, tag="MLP")
    parser.add_argument("--early_stopping", type=bool_type, metavar="<bool>", help="Whether to use early stopping to terminate training when validation score is not improving for stochastic gradient descent and adam", default=False, action=TaggableAction, tag="MLP")
    parser.add_argument("--validation_fraction", type=lambda x: float_type(x, min=0, max=1, max_inclusive=False), metavar="<float [0,1[>", help="Proportion of training data to set aside as validation set for early stopping", default=0.1, action=TaggableAction, tag="MLP")
    parser.add_argument("--beta_1", type=lambda x: float_type(x, min=0, max=1, max_inclusive=False), metavar="<float [0,1[>", help="Exponential decay rate for estimates of first moment vector in adam", default=0.9, action=TaggableAction, tag="MLP")
    parser.add_argument("--beta_2", type=lambda x: float_type(x, min=0, max=1, max_inclusive=False), metavar="<float [0,1[>", help="Exponential decay rate for estimates of second moment vector in adam", default=0.999, action=TaggableAction, tag="MLP")
    parser.add_argument("--epsilon", type=lambda x: float_type(x, min=0, min_inclusive=False), metavar="<float ]0,inf[>", help="Value for numerical stability in adam", default=0.00000001, action=TaggableAction, tag="MLP")
    parser.add_argument("--n_iter_no_change", type=lambda x: int_type(x, min=1), metavar="<int [1,inf[>", help="Maximum number of epochs to not meet tol improvement for stochastic gradient descent and adam", default=10, action=TaggableAction, tag="MLP")
    parser.add_argument("--max_fun", type=lambda x: int_type(x, min=1), metavar="<int [1,inf[>", help="Maximum number of loss function calls for lbfgs", default=15000, action=TaggableAction, tag="MLP")
    return get_args(args, cleaned_args, parser) # Return attributes

def mlpTrn(args: str = None):
    """
    Trains an MLP model with data preprocessing that includes normalization and a staircase activation function that allows
    for the characterization of discriminating hyperplanes, which are used in Fidex. This allows us to then use Fidex for comprehensible rule extraction.
    The function offers a wide range of customization through command-line arguments, allowing for the specification of MLP parameters, output options, and more.

    Notes:
    - Each file is located with respect to the root folder dimlpfidex or to the content of the 'root_folder' parameter if specified.
    - It's mandatory to specify the number of attributes and classes in the data, as well as the train and test datasets.
    - True train and test class labels must be provided, either within the data file or separately through a class file.
    - Parameters can be defined directly via the command line or through a JSON configuration file.
    - Providing no command-line arguments or using -h/--help displays usage instructions, detailing both required and optional parameters for user guidance.
    - It's not necessary to normalize data before training because a normalization is done during the process.

    Outputs:
    - train_pred_outfile : File containing the model's train predictions.
    - test_pred_outfile : File containing the model's test predictions.
    - weights_outfile : File containing the model's trained weights.
    - stats_file : File containing train and test accuracy.
    - console_file : If specified, contains the console output.

    File formats:
    - **Data files**: These files should contain one sample per line, with numbers separated either by spaces, tabs, semicolons or commas. Supported formats:
      1. Only attributes (floats).
      2. Attributes (floats) followed by an integer class ID.
      3. Attributes (floats) followed by one-hot encoded class.
    - **Class files**: These files should contain one class sample per line, with integers separated either by spaces, tabs, semicolons or commas. Supported formats:
      1. Integer class ID.
      2. One-hot encoded class.

    Example of how to call the function:
    from trainings.mlpTrn import mlpTrn
    mlpTrn('--train_data_file datanormTrain.txt --train_class_file dataclass2Train.txt --test_data_file datanormTest.txt --test_class_file dataclass2Test.txt --weights_outfile mlp/weights.wts --stats_file mlp/stats.txt --train_pred_outfile mlp/predTrain.out --test_pred_outfile mlp/predTest.out --nb_attributes 16 --nb_classes 2 --root_folder dimlp/datafiles')

    :param args: A single string containing either the path to a JSON configuration file with all specified arguments, or all arguments for the function formatted like command-line input.
                 This includes file paths, MLP parameters, and options for output and for the staircase activation process.
    :return: Returns 0 for successful execution, -1 for errors encountered during the process.
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

        print_parameters(args)

        # Get data
        train_data, train_class = get_data(args.train_data_file, args.nb_attributes, args.nb_classes)
        if len(train_class) == 0:
            if args.train_class_file is None:
                raise ValueError("Error : parameter --train_class_file is required because classes are not contained in train_data_file.")
            train_class = get_data_class(args.train_class_file, args.nb_classes)
        if len(train_data) != len(train_class):
            raise ValueError('Error, there is not the same amount of train data and train class.')
        classes = set(range(args.nb_classes))
        miss_train_classes = classes - set(train_class) # Check if a class is not represented during training

        test_data, test_class = get_data(args.test_data_file, args.nb_attributes, args.nb_classes)
        if len(test_class) == 0:
            if args.test_class_file is None:
                raise ValueError("Error : parameter --test_class_file is required because classes are not contained in test_data_file.")
            test_class = get_data_class(args.test_class_file, args.nb_classes)
        if len(test_data) != len(test_class):
            raise ValueError('Error, there is not the same amount of test data and test class.')

        # Get weights and biais from first hidden layer as well as data transformed in first hidden layer

        train_data_h1, mu, sigma = compute_first_hidden_layer("train", train_data, args.K, args.nb_quant_levels, hiknot, args.weights_outfile)
        test_data_h1 = compute_first_hidden_layer("test", test_data, args.K, args.nb_quant_levels, hiknot, mu=mu, sigma=sigma)

        # Train mlp
        model = MLPClassifier(hidden_layer_sizes = args.hidden_layer_sizes, activation = args.activation, solver = args.solver, alpha = args.alpha, batch_size = args.batch_size,
                                learning_rate = args.learning_rate, learning_rate_init = args.learning_rate_init, power_t = args.power_t, max_iter = args.max_iterations, shuffle = args.shuffle,
                                random_state = args.seed, tol = args.tol, verbose = args.verbose, warm_start = args.warm_start, momentum = args.momentum, nesterovs_momentum = args.nesterovs_momentum,
                                early_stopping = args.early_stopping, validation_fraction = args.validation_fraction, beta_1 = args.beta_1, beta_2 = args.beta_2, epsilon = args.epsilon,
                                n_iter_no_change = args.n_iter_no_change, max_fun = args.max_fun)

        model.fit(train_data_h1, train_class)    # Train the model using the training sets

        # Compute predictions
        train_pred_proba = model.predict_proba(train_data_h1)  # Predict the response for train dataset
        test_pred_proba = model.predict_proba(test_data_h1)    # Predict the response for test dataset
        train_pred = model.predict(train_data_h1)
        test_pred = model.predict(test_data_h1)

        # If a class is missing, we adapt predictions
        if len(miss_train_classes)!=0:
            miss_train_classes = sorted(list(miss_train_classes))
            train_pred_proba = [pred.tolist() for pred in train_pred_proba]
            test_pred_proba = [pred.tolist() for pred in test_pred_proba]
            for train_pred_list in [train_pred_proba, test_pred_proba]:
                for pred in train_pred_list:
                    for classe in miss_train_classes:
                        pred.insert(classe, 0.0) # Prediction 0 for the missing class

        # Output predictions
        output_data(train_pred_proba, args.train_pred_outfile)
        output_data(test_pred_proba, args.test_pred_outfile)

        # Calcul de l'exactitude (accuracy) du modèle
        acc_train = metrics.accuracy_score(train_class, train_pred) * 100
        acc_test = metrics.accuracy_score(test_class, test_pred) * 100
        acc_train = "{:.6f}".format(acc_train).rstrip("0").rstrip(".")
        acc_test = "{:.6f}".format(acc_test).rstrip("0").rstrip(".")
        print("Training accuracy : {}%.".format(acc_train))
        print("Testing accuracy : {}%.".format(acc_test))

        # Output accuracy statistics
        output_stats(args.stats_file, acc_train, acc_test)

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
    Entry point for the script. Parses command-line arguments and initiates the training process for the MLP model.

    The script is designed to be run from the command line with various arguments that control the training process,
    including data file paths, model hyperparameters, and output options.

    Usage:
        python script.py [options]

    Example:
        python script.py --train_data_file datanormTrain.txt --train_class_file dataclass2Train.txt \
                         --test_data_file datanormTest.txt --test_class_file dataclass2Test.txt \
                         --weights_outfile mlp/weights.wts --stats_file mlp/stats.txt \
                         --train_pred_outfile mlp/predTrain.out --test_pred_outfile mlp/predTest.out \
                         --nb_attributes 16 --nb_classes 2 --root_folder dimlp/datafiles

    :param sys.argv: List of command-line arguments passed to the script.
    :type sys.argv: list
    """
    cmdline_args = " ".join(sys.argv[1:])
    mlpTrn(cmdline_args)
