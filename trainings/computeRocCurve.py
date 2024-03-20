# Not working with SVM because process is different to get Roc curve

from trainings.trnFun import get_data_class, get_data_pred, compute_roc
from trainings.parameters import get_initial_parser, get_args, sanitizepath, CustomArgumentParser, CustomHelpFormatter, TaggableAction, int_type, bool_type, print_parameters
import numpy as np
import sys

def get_and_check_parameters(init_args):
    # Remove None values and his -- attribute
    cleaned_args = [arg for i, arg in enumerate(init_args[:-1]) if ((not arg.startswith("--") or init_args[i+1] not in ["None", "none", None]) and arg not in ["None", "none", None])]
    if init_args and init_args[-1] not in ["None", "none", None]:
        cleaned_args.append(init_args[-1])

    # Get initial attributes with root_folder and json file information
    args, initial_parser = get_initial_parser(cleaned_args)

    # set custom formatter printing boundaries
    formatter = lambda prog: CustomHelpFormatter(prog, width=150, max_help_position=60)

    # Add new attributes
    parser = CustomArgumentParser(description="This is a parser for gradBoostTrn", parents=[initial_parser], formatter_class=formatter, add_help=True)
    parser.add_argument("--test_class_file", type=lambda x: sanitizepath(args.root_folder, x), help="Test class file", metavar="<str>", required=True)
    parser.add_argument("--test_pred_file", type=lambda x: sanitizepath(args.root_folder, x), help="Test prediction file", metavar="<str>", required=True)
    parser.add_argument("--nb_classes", type=lambda x: int_type(x, min=1), help="Number of classes in dataset", metavar="<int [1,inf[>", required=True)
    parser.add_argument("--positive_class_index", type=lambda x: int_type(x, min=1), help="Index of positive class, index starts at 0", metavar="<int [1,nb_classes-1]>", required=True)
    parser.add_argument("--estimator", type=str, help="Name of estimator", metavar="<str>", action=TaggableAction, tag="ROC")
    parser.add_argument("--output_roc", type=lambda x: sanitizepath(args.root_folder, x, "w"), help="Output ROC curve file name", metavar="<str>", default="roc_curve.png", action=TaggableAction, tag="ROC")
    parser.add_argument("--stats_file", type=lambda x: sanitizepath(args.root_folder, x, "w"), help="Output statistic file name with AUC score", metavar="<str>")
    parser.add_argument("--show_params", type=bool_type, help="Whether to show parameters", metavar="<bool>", default=True)

    return get_args(args, cleaned_args, parser) # Return attributes

def computeRocCurve(args: str = None):

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
            with open(args.stats_file, 'r') as file:
                lines = file.readlines()
                file.close()
            last_line = lines[-1]
            if last_line.startswith("AUC score on testing set"):
                lines[-1] = f"AUC score on testing set : {auc_score}" # Replace the line
            else:
                lines.append(f"\nAUC score on testing set : {auc_score}")# Add new line

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
    cmdline_args = " ".join(sys.argv[1:])
    computeRocCurve(cmdline_args)
