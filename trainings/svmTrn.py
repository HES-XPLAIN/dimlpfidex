import time
import sys
from sklearn import svm
from sklearn import metrics
import numpy as np

from trainings.trnFun import get_data, get_data_class, output_pred, compute_first_hidden_layer, output_stats, compute_roc
from trainings.parameters import get_common_parser, get_initial_parser, get_args, sanitizepath, CustomArgumentParser, CustomHelpFormatter, TaggableAction, int_type, float_type, bool_type, dict_type, enum_type, print_parameters

def get_and_check_parameters(init_args):
    """
    Processes and validates command-line arguments for a model training with SVM.
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
    parser = CustomArgumentParser(description="This is a parser for svmTrn", parents=[common_parser], formatter_class=formatter, add_help=True)
    parser.add_argument("--weights_outfile", type=lambda x: sanitizepath(args.root_folder, x, "w"), help="Path to the file where the output trained weights of the model will be stored", metavar="<str>", default="weights.wts")
    parser.add_argument("--output_roc", type=lambda x: sanitizepath(args.root_folder, x, "w"), help="Path to the file where the output ROC curve will be saved", metavar="<str>", default="roc_curve.png")
    parser.add_argument("--return_roc", type=bool_type, metavar="<bool>", help="Whether to return ROC statistics", default=False)
    parser.add_argument("--positive_class_index", type=lambda x: int_type(x, min=0), metavar="<int [0,nb_class-1]>", help="Index of the positive class for the roc curve calculation, index starts at 0")
    parser.add_argument("--nb_quant_levels", type=lambda x: int_type(x, min=3), metavar="<int [3,inf[>", help="Number of stairs in the staircase activation function", default=50)
    parser.add_argument("--K", type=lambda x: float_type(x, min=0, min_inclusive=False), metavar="<float ]0,inf[>", help="Parameter to improve dynamics by normalizing input data", default=1.0)
    parser.add_argument("--C", type=lambda x: float_type(x, min=0, min_inclusive=False), metavar="<float ]0,inf[>", help="Regularization parameter", default=1.0, action=TaggableAction, tag="SVM")
    parser.add_argument("--kernel", choices=["linear", "poly", "rbf", "sigmoid"], metavar="<{linear, poly, rbf, sigmoid}>", help="Kernel type", default="rbf", action=TaggableAction, tag="SVM")
    parser.add_argument("--degree", type=lambda x: int_type(x, min=0), metavar="<int [0, inf[>", help="Polynomial degree", default=3, action=TaggableAction, tag="SVM")
    parser.add_argument("--gamma", type=lambda x: enum_type(x, "scale", "auto", _=dict(func=float_type, min=0)), metavar="<{scale, auto, float [0,inf[}>", help="Gamma value", default="scale", action=TaggableAction, tag="SVM")
    parser.add_argument("--coef0", type=float_type, metavar="<float>", help="Term in the kernel function", default=0.0, action=TaggableAction, tag="SVM")
    parser.add_argument("--shrinking", type=bool_type, metavar="<bool>", help="Whether to use the shrinking heuristic", default=True, action=TaggableAction, tag="SVM")
    parser.add_argument("--tol", type=lambda x: float_type(x, min=0, min_inclusive=False), metavar="<float ]0,inf[>", help="Tolerance for stopping criterion", default=0.001, action=TaggableAction, tag="SVM")
    parser.add_argument("--cache_size", type=lambda x: float_type(x, min=0, min_inclusive=False), metavar="<float ]0,inf[>", help="Kernel cache size(MB)", default=200, action=TaggableAction, tag="SVM")
    parser.add_argument("--class_weight", type=lambda x: enum_type(x, "balanced", _=dict(func=dict_type)), metavar="<{balanced, dict}>", help="Class balance, for exemple with a dictionnary and 2 classes : {0:1.2, 1:3.5}", action=TaggableAction, tag="SVM")
    parser.add_argument("--verbose", type=bool_type, metavar="<bool>", help="Whether to enable verbose output", default=False, action=TaggableAction, tag="SVM")
    parser.add_argument("--max_iterations", type=lambda x: int_type(x, min=1, allow_value=-1), metavar="<int [1, inf[>", help="Maximal number of training iterations, -1 for no limit", default=-1, action=TaggableAction, tag="SVM")
    parser.add_argument("--decision_function_shape", choices=["ovo", "ovr"], metavar="<{ovo(one-vs-one), ovr(one-vs-rest)}>", help="Decision function shape", default="ovr", action=TaggableAction, tag="SVM")
    parser.add_argument("--break_ties", type=bool_type, metavar="<bool>", help="Whether to break tie decision for ovr with more than 2 classes", default=False, action=TaggableAction, tag="SVM")

    return get_args(args, cleaned_args, parser) # Return attributes

def svmTrn(args: str = None):
    """
    Trains an SVM model with data preprocessing that includes normalization and a staircase activation function that allows
    for the characterization of discriminating hyperplanes, which are used in Fidex. This allows us to then use Fidex for comprehensible rule extraction.
    The function offers a wide range of customization through command-line arguments, allowing for the specification of SVM parameters, output options, and more.

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
    - output_roc : PNG file containing the ROC curve.

    File formats:
    - **Data files**: These files should contain one sample per line, with numbers separated either by spaces, tabs, semicolons or commas. Supported formats:
      1. Only attributes (floats).
      2. Attributes (floats) followed by an integer class ID.
      3. Attributes (floats) followed by one-hot encoded class.
    - **Class files**: These files should contain one class sample per line, with integers separated either by spaces, tabs, semicolons or commas. Supported formats:
      1. Integer class ID.
      2. One-hot encoded class.

    Example of how to call the function:
    from trainings.svmTrn import svmTrn
    svmTrn('--train_data_file datanormTrain.txt --train_class_file dataclass2Train.txt --test_data_file datanormTest.txt --test_class_file dataclass2Test.txt --weights_outfile svm/weights.wts --stats_file svm/stats.txt --train_pred_outfile svm/predTrain.out --test_pred_outfile svm/predTest.out --nb_attributes 16 --nb_classes 2 --root_folder dimlp/datafiles')

    :param args: A single string containing either the path to a JSON configuration file with all specified arguments, or all arguments for the function formatted like command-line input.
                 This includes file paths, SVM parameters, and options for output and for the staircase activation process.
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

        test_data, test_class = get_data(args.test_data_file, args.nb_attributes, args.nb_classes)
        if len(test_class) == 0:
            if args.test_class_file is None:
                raise ValueError("Error : parameter --test_class_file is required because classes are not contained in test_data_file.")
            test_class = get_data_class(args.test_class_file, args.nb_classes)
        if len(test_data) != len(test_class):
            raise ValueError('Error, there is not the same amount of test data and test class.')

        with_roc = True
        if args.positive_class_index is None:
            with_roc = False
        elif args.positive_class_index >= args.nb_classes:
            raise ValueError(f'Error : parameter positive_class_index has to be a positive integer smaller than {args.nb_classes}.')


        # Get weights and biais from first hidden layer as well as data transformed in first hidden layer

        train_data_h1, mu, sigma = compute_first_hidden_layer("train", train_data, args.K, args.nb_quant_levels, hiknot, args.weights_outfile)
        test_data_h1 = compute_first_hidden_layer("test", test_data, args.K, args.nb_quant_levels, hiknot, mu=mu, sigma=sigma)

        # Train svm
        model = svm.SVC(C = args.C, kernel = args.kernel, degree = args.degree, gamma = args.gamma, coef0 = args.coef0, shrinking = args.shrinking,
                        tol = args.tol, cache_size = args.cache_size, class_weight = args.class_weight, verbose = args.verbose, max_iter = args.max_iterations,
                        decision_function_shape = args.decision_function_shape, break_ties = args.break_ties, probability=True) # Create svm Classifier

        model.fit(train_data_h1, train_class)   # Train the model using the training sets
        train_pred = model.predict(train_data_h1)    # Predict the response for train dataset
        test_pred = model.predict(test_data_h1)    # Predict the response for test dataset

        # Output predictions
        output_pred(train_pred, args.train_pred_outfile, args.nb_classes)
        output_pred(test_pred, args.test_pred_outfile, args.nb_classes)

        # Compute model Accuracy
        acc_train = metrics.accuracy_score(train_class, train_pred) * 100
        acc_test = metrics.accuracy_score(test_class, test_pred) * 100
        acc_train = "{:.6f}".format(acc_train).rstrip("0").rstrip(".")
        acc_test = "{:.6f}".format(acc_test).rstrip("0").rstrip(".")
        print("Training accuracy : {}%.".format(acc_train))
        print("Testing accuracy : {}%.".format(acc_test))

        # Output accuracy statistics
        output_stats(args.stats_file, acc_train, acc_test)

        print("Weights : ", model.class_weight_)
        if with_roc:
            test_class_roc = [1 if cl == args.positive_class_index else 0 for cl in test_class]
            test_pred_proba = model.predict_proba(test_data_h1)
            test_pred_roc = test_pred_proba[:, args.positive_class_index]
            fpr, tpr, auc_score = compute_roc("svm", args.output_roc, test_class_roc, test_pred_roc)

            # Save AUC in stats file
            with open(args.stats_file, 'a') as file:
                file.write(f"\nAUC score on testing set : {auc_score}")

        end_time = time.time()
        full_time = end_time - start_time
        full_time = "{:.6f}".format(full_time).rstrip("0").rstrip(".")

        print(f"\nFull execution time = {full_time} sec")


        # Redirect output to terminal
        if console_file != None:
            sys.stdout = sys.__stdout__

        if with_roc and args.return_roc:
            # Interpolation to get 1000 points (necessary for crossValidation)
            fpr_interp = np.linspace(0, 1, 1000)
            tpr_interp = np.interp(fpr_interp, fpr, tpr)
            tpr_interp[0] = 0
            return [fpr_interp, tpr_interp, auc_score]

        return 0

    except ValueError as error:
        # Redirect output to terminal
        if console_file != None:
            sys.stdout = sys.__stdout__
        print(error)
        return -1

if __name__ == "__main__":
    """
    Entry point for the script. Parses command-line arguments and initiates the training process for the SVM model.

    The script is designed to be run from the command line with various arguments that control the SVM training process,
    including file paths for training and testing data, the number of attributes and classes, and options for model parameters and output files.

    Usage:
        python script.py [options]

    Example:
        python script.py --train_data_file datanormTrain.txt --train_class_file dataclass2Train.txt \
                         --test_data_file datanormTest.txt --test_class_file dataclass2Test.txt \
                         --weights_outfile svm/weights.wts --stats_file svm/stats.txt --train_pred_outfile svm/predTrain.out \
                         --test_pred_outfile svm/predTest.out --nb_attributes 16 --nb_classes 2 --root_folder dimlp/datafiles

    :param sys.argv: List of command-line arguments passed to the script.
    :type sys.argv: list
    """
    cmdline_args = " ".join(sys.argv[1:])
    svmTrn(cmdline_args)
