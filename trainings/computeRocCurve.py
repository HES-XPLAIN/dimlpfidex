# Not working with SVM because process is different to get Roc curve

from trainings.trnFun import get_data_class, get_data_pred, compute_roc
from trainings.parameters import get_initial_parser, get_args, sanitizepath, CustomArgumentParser, CustomHelpFormatter, TaggableAction, int_type, bool_type, print_parameters
import numpy as np
import sys

def get_and_check_parameters(init_args):
    """
    Processes and validates command-line arguments for a ROC curve computation.
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
    parser = CustomArgumentParser(description="This is a parser for computeRocCurve", parents=[initial_parser], formatter_class=formatter, add_help=True)
    parser.add_argument("--test_class_file", type=lambda x: sanitizepath(args.root_folder, x), help="Path to the file containing the test true classes of the dataset", metavar="<str>", required=True)
    parser.add_argument("--test_pred_file", type=lambda x: sanitizepath(args.root_folder, x), help="Test prediction file", metavar="<str>", required=True)
    parser.add_argument("--positive_class_index", type=lambda x: int_type(x, min=0), help="Index of the positive class, index starts at 0", metavar="<int [0,nb_classes-1]>", required=True)
    parser.add_argument("--nb_classes", type=lambda x: int_type(x, min=1), help="Number of classes in the dataset", metavar="<int [1,inf[>", required=True)
    parser.add_argument("--output_roc", type=lambda x: sanitizepath(args.root_folder, x, "w"), help="Path to the file where the output ROC curve will be saved", metavar="<str>", default="roc_curve.png", action=TaggableAction, tag="ROC")
    parser.add_argument("--stats_file", type=lambda x: sanitizepath(args.root_folder, x, "w"), help="Path to the file where the AUC score will be added, it can be the training stats file", metavar="<str>")
    parser.add_argument("--estimator", type=str, help="Name of the estimator", metavar="<str>", action=TaggableAction, tag="ROC")
    parser.add_argument("--show_params", type=bool_type, help="Whether to show the parameters", metavar="<bool>", default=True)

    return get_args(args, cleaned_args, parser) # Return attributes

def computeRocCurve(args: str = None):
    """
    Computes and plots the Receiver Operating Characteristic (ROC) curve for a given set of test predictions and true class labels.
    The function supports various customizations through command-line arguments, including specifying input files, choosing the
    positive class index, and output options.

    Notes:
    - Each file is located with respect to the root folder dimlpfidex or to the content of the 'root_folder' parameter if specified.
    - The function is not compatible with SVM models directly due to the different process required for generating ROC curves for them.
    - It's mandatory to specify the number of classes, the index of the positive class, and provide the test class labels and prediction scores.
    - Parameters can be specified using the command line or a JSON configuration file.
    - Providing no command-line arguments or using -h/--help displays usage instructions, detailing both required and optional parameters for user guidance.

    Outputs:
    - stats_file : If specified, contains AUC scores.
    - output_roc : PNG file containing the ROC curve.

    File formats:
    - **Class file**: Thesen files should contain one class sample per line, with integers separated either by spaces, tabs, semicolons or commas. Supported formats:
      1. Integer class ID.
      2. One-hot encoded class.
    - **Prediction file** : These files should contain the prediction scores for the test set, with one sample per line, with scores (float) for each class separated either by spaces, tabs, semicolons or commas.

    Example of how to call the function:
    from trainings.computeRocCurve import computeRocCurve
    computeRocCurve('--test_class_file dataclass2Test.txt --test_pred_file predTest.out --positive_class_index 1 --output_roc roc_curve.png --stats_file stats.txt --root_folder dimlp/datafiles --nb_classes 2')

    :param args: A single string containing either the path to a JSON configuration file with all specified arguments or all arguments for the function, formatted like command-line input.
                 This includes file paths, the positive class index, and options for the output and statistical analysis.
    :return: Returns 0 for successful execution, -1 for any errors encountered during the process. Additionally, it returns an array containing interpolated false positive rates (FPR), true positive rates (TPR), and the area under the ROC curve (AUC) for further analysis or cross-validation purposes.
    """
    try:
        if not args:
            args = ""

        # Get parameters
        split_args = []
        if len(args) != 0:
            split_args = args.split()  # string command to list
        else:
            split_args= ["-h"]
        args = get_and_check_parameters(split_args)

        if args.show_params:
            print_parameters(args)

        # Get data
        test_class = get_data_class(args.test_class_file, args.nb_classes)
        test_pred = get_data_pred(args.test_pred_file, args.nb_classes)

        if len(test_class) != len(test_pred):
            raise ValueError('Error, there is not the same amount of test predictions and test class.')

        if args.positive_class_index is not None and args.positive_class_index >= args.nb_classes:
            raise ValueError(f'Error : parameter positive_class_index has to be a positive integer smaller than {args.nb_classes}.')

        test_class_roc = [int(cl == args.positive_class_index) for cl in test_class]
        test_pred = [p[args.positive_class_index] for p in test_pred]
        fpr, tpr, auc_score = compute_roc(args.estimator, args.output_roc, test_class_roc, test_pred)

        # Save AUC result in stats file
        if args.stats_file is not None:
            try:
                with open(args.stats_file, 'r') as file:
                    lines = file.readlines()
                    file.close()
            except FileNotFoundError:
                lines = []
            if lines and lines[-1].startswith("AUC score on testing set"):
                lines[-1] = f"AUC score on testing set : {auc_score}" # Replace the line
            elif lines:
                lines.append(f"\nAUC score on testing set : {auc_score}")# Add new line
            else:
                lines.append(f"AUC score on testing set : {auc_score}")# Add new line

            with open(args.stats_file, "w") as file:
                file.writelines(lines)
                file.close()

        # Interpolation to get 1000 points (necessary for crossValidation)
        fpr_interp = np.linspace(0, 1, 1000)
        tpr_interp = np.interp(fpr_interp, fpr, tpr)
        tpr_interp[0] = 0
        return [fpr_interp, tpr_interp, auc_score]

    except ValueError as error:
        print(error)
        return -1

if __name__ == "__main__":
    """
    Entry point for the script. Parses command-line arguments and initiates the ROC curve computation process.

    The script is designed to be run from the command line with various arguments that control the ROC curve computation,
    including file paths for test class labels and predictions, the index of the positive class, the number of classes,
    and output options.

    Usage:
        python script.py [options]

    Example:
        python script.py --test_class_file dataclass2Test.txt --test_pred_file predTest.out \
                         --positive_class_index 1 --output_roc roc_curve.png --stats_file stats.txt \
                         --root_folder dimlp/datafiles --nb_classes 2

    :param sys.argv: List of command-line arguments passed to the script.
    :type sys.argv: list
    """
    cmdline_args = " ".join(sys.argv[1:])
    computeRocCurve(cmdline_args)
