import time
import sys
from trainings.trnFun import get_data, get_data_class, output_data, output_stats, trees_to_rules
from trainings.parameters import get_common_parser, get_initial_parser, get_args, sanitizepath, CustomArgumentParser, CustomHelpFormatter, TaggableAction, int_type, float_type, bool_type, dict_type, enum_type, print_parameters
from sklearn.ensemble import RandomForestClassifier
from sklearn import metrics

def get_and_check_parameters(init_args):
    """
    Processes and validates command-line arguments for a model training with random forests.
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
    parser = CustomArgumentParser(description="This is a parser for randForestsTrn", parents=[common_parser], formatter_class=formatter, add_help=True)
    parser.add_argument("--rules_outfile", type=lambda x: sanitizepath(args.root_folder, x, "w"), help="Output random forests rules file", metavar="<str>", default="RF_rules.rls")
    parser.add_argument("--n_estimators", type=lambda x: int_type(x, min=1), metavar="<int [1,inf[>", help="Number of generated trees in the forest", default=100, action=TaggableAction, tag="RF")
    parser.add_argument("--criterion", choices=["gini", "entropy", "log_loss"], metavar="<{gini, entropy, log_loss}>", help="Function to measure split quality", default="gini", action=TaggableAction, tag="RF")
    parser.add_argument("--max_depth", type=lambda x: int_type(x, min=1), metavar="<int [1,inf[>", help="Maximum depth of the tree", action=TaggableAction, tag="RF")
    parser.add_argument("--min_samples_split", type=lambda x: enum_type(x, int_type=dict(func=int_type, min=2), float_type=dict(func=float_type, min=0, min_inclusive=False, max=1)), metavar="<int [2,inf[ U float]0,1.0]>", help="Minimum number of samples required to split an internal node, if float, it is a fraction of the number of samples.", default=2, action=TaggableAction, tag="RF")
    parser.add_argument("--min_samples_leaf", type=lambda x: enum_type(x, int_type=dict(func=int_type, min=1), float_type=dict(func=float_type, min=0, min_inclusive=False, max=1, max_inclusive=False)), metavar="<int [1,inf[ U float]0,1[>", help="Minimum number of samples required to be at a leaf node, if float, it is a fraction of the number of samples", default=1, action=TaggableAction, tag="RF")
    parser.add_argument("--min_weight_fraction_leaf", type=lambda x: float_type(x, min=0, max=0.5), metavar="<float [0,0.5]>", help="Minimum weighted fraction of the sum total of input samples weights required to be at a leaf node", default=0.0, action=TaggableAction, tag="RF")
    parser.add_argument("--max_features", type=lambda x: enum_type(x, "sqrt", "log2", "all", _=dict(func=float_type, min=0, min_inclusive=False, max=1, max_inclusive=False), int_type=dict(func=int_type, min=1)), metavar="<{sqrt, log2, all, float ]0,1[, int [1,inf[}>", help="Number of features to consider when looking for the best splitif float, it is a fraction of the number of features. 1 stands for 1 feature, for all features put 'all', not 1.0", default="sqrt", action=TaggableAction, tag="RF")
    parser.add_argument("--max_leaf_nodes", type=lambda x: int_type(x, min=2), metavar="<int [2,inf[>", help="Grow trees with max_leaf_nodes in best-first fashion", action=TaggableAction, tag="RF")
    parser.add_argument("--min_impurity_decrease", type=lambda x: float_type(x, min=0), metavar="<float [0,inf[>", help="A node will be split if this split induces a decrease of the impurity greater than or equal to this value", default=0.0, action=TaggableAction, tag="RF")
    parser.add_argument("--bootstrap", type=bool_type, metavar="<bool>", help="Whether bootstrap samples are used when building trees", default=True, action=TaggableAction, tag="RF")
    parser.add_argument("--oob_score", type=bool_type, metavar="<bool>", help="Whether to use out-of-bag samples to estimate the generalization score", default=False, action=TaggableAction, tag="RF")
    parser.add_argument("--n_jobs", type=int_type, metavar="<int>", help="Number of jobs to run in parallel, -1 = using all processors", default=1, action=TaggableAction, tag="RF")
    parser.add_argument("--seed", type=lambda x: int_type(x, min=0), metavar="<{int [0,inf[}>", help="Random seed", action=TaggableAction, tag="RF")
    parser.add_argument("--verbose", type=lambda x: int_type(x, min=0), metavar="<int [0,inf[>", help="Controls the verbosity when fitting and predicting", default=0, action=TaggableAction, tag="RF")
    parser.add_argument("--warm_start", type=bool_type, metavar="<bool>", help="Whether to reuse the solution of the previous call to fit and add more estimators to the ensemble", default=False, action=TaggableAction, tag="RF")
    parser.add_argument("--class_weight", type=lambda x: enum_type(x, "balanced", "balanced_subsample", _=dict(func=dict_type)), metavar="<{balanced, dict}>", help="Class balance, for exemple with a dictionnary and 2 classes : {0:1.2, 1:3.5}", action=TaggableAction, tag="RF")
    parser.add_argument("--ccp_alpha", type=lambda x: float_type(x, min=0), metavar="<float [0,inf[>", help="Complexity parameter used for Minimal Cost-Complexity Pruning", default=0.0, action=TaggableAction, tag="RF")
    parser.add_argument("--max_samples", type=lambda x: enum_type(x, int_type=dict(func=int_type, min=1), float_type=dict(func=float_type, min=0, min_inclusive=False, max=1)), metavar="<int [1,inf[ U float]0,1.0]>", help="Number of samples to draw to train each base estimator for bootstrap, if float, it is a fraction of the number of samples", action=TaggableAction, tag="RF")

    return get_args(args, cleaned_args, parser) # Return attributes

def randForestsTrn(args: str = None):
    """
    Trains a random forests decision trees model. The nodes of the trees represent the discriminating hyperplanes used in Fidex. This allows
    us to then use Fidex for comprehensible rule extraction. The function offers a wide range of customization through command-line arguments,
    allowing for the specification of random forests parameters, output options, and more.

    Notes:
    - Each file is located with respect to the root folder dimlpfidex or to the content of the 'root_folder' parameter if specified.
    - It's mandatory to specify the number of attributes and classes in the data, as well as the train and test datasets.
    - True train and test class labels must be provided, either within the data file or separately through a class file.
    - Parameters can be defined directly via the command line or through a JSON configuration file.
    - Providing no command-line arguments or using -h/--help displays usage instructions, detailing both required and optional parameters for user guidance.
    - It's not necessary to normalize data before training because decision trees don't need normalization.

    File formats:
    - Data files should contain one sample per line, with numbers separated either by spaces, tabs, semicolons or commas. Supported formats:
      1. Only attributes (floats).
      2. Attributes (floats) followed by an integer class ID.
      3. Attributes (floats) followed by one-hot encoded class.
    - Class files should contain one class sample per line, with integers separated either by spaces, tabs, semicolons or commas. Supported formats:
      1. Integer class ID.
      2. One-hot encoded class.

    Example of how to call the function:
    from trainings.randForestsTrn import randForestsTrn
    randForestsTrn('--train_data_file datanormTrain.txt --train_class_file dataclass2Train.txt --test_data_file datanormTest.txt --test_class_file dataclass2Test.txt --stats_file rf/stats.txt --train_pred_outfile rf/predTrain.out --test_pred_outfile rf/predTest.out --rules_outfile rf/RF_rules.rls --nb_attributes 16 --nb_classes 2 --root_folder dimlp/datafiles')

    :param args: A single string containing either the path to a JSON configuration file with all specified arguments, or all arguments for the function formatted like command-line input.
                 This includes file paths, random forests parameters, and options for output.
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

        if args.max_features == "all":
            args.max_features = None

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

        # Train RF
        model = RandomForestClassifier(n_estimators=args.n_estimators, criterion=args.criterion, max_depth=args.max_depth, # Create RF Classifier
                                        min_samples_split=args.min_samples_split, min_samples_leaf=args.min_samples_leaf, min_weight_fraction_leaf=args.min_weight_fraction_leaf,
                                        max_features=args.max_features, max_leaf_nodes=args.max_leaf_nodes, min_impurity_decrease=args.min_impurity_decrease, bootstrap=args.bootstrap,
                                        oob_score=args.oob_score, n_jobs=args.n_jobs, random_state=args.seed, verbose=args.verbose, warm_start=args.warm_start, class_weight=args.class_weight,
                                        ccp_alpha=args.ccp_alpha, max_samples=args.max_samples)

        model.fit(train_data, train_class)   # Train the model using the training sets

        # Compute predictions
        train_pred_proba = model.predict_proba(train_data)  # Predict the response for train dataset
        test_pred_proba = model.predict_proba(test_data)    # Predict the response for test dataset
        train_pred = model.predict(train_data)    # Predict the response for train dataset
        test_pred = model.predict(test_data)    # Predict the response for test dataset

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

        # Compute model Accuracy
        acc_train = metrics.accuracy_score(train_class, train_pred) * 100
        acc_test = metrics.accuracy_score(test_class, test_pred) * 100
        acc_train = "{:.6f}".format(acc_train).rstrip("0").rstrip(".")
        acc_test = "{:.6f}".format(acc_test).rstrip("0").rstrip(".")
        print("Training accuracy : {}%.".format(acc_train))
        print("Testing accuracy : {}%.".format(acc_test))

        # Output accuracy statistics
        output_stats(args.stats_file, acc_train, acc_test)
        trees_to_rules(model.estimators_, args.rules_outfile)

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
    Entry point for the script. Parses command-line arguments and initiates the training process for the random forests decision trees model.

    The script is designed to be run from the command line with various arguments that control the random forests training process,
    including file paths for training and testing data, the number of attributes and classes, and options for model parameters and output files.

    Usage:
        python script.py [options]

    Example:
        python script.py --train_data_file datanormTrain.txt --train_class_file dataclass2Train.txt \
                         --test_data_file datanormTest.txt --test_class_file dataclass2Test.txt \
                         --stats_file rf/stats.txt --train_pred_outfile rf/predTrain.out --test_pred_outfile rf/predTest.out \
                         --rules_outfile rf/RF_rules.rls --nb_attributes 16 --nb_classes 2 --root_folder dimlp/datafiles

    :param sys.argv: List of command-line arguments passed to the script.
    :type sys.argv: list
    """
    cmdline_args = " ".join(sys.argv[1:])
    randForestsTrn(cmdline_args)
