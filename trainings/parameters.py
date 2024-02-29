import argparse
import json
import os

def directory(string:str):
    if string == "":
        return ""

    if os.path.isdir(string):
        return string
    else:
        raise argparse.ArgumentTypeError(f"'{string}' is not a valid directory.")


def sanitizepath(path:str, file:str, access_type : str = 'r'):
    result = os.path.join(path, file)

    if (access_type == 'r'):
        to_verify = result
    else:
        to_verify = os.path.dirname(result)

        if to_verify == "":
            return result

    if os.path.exists(to_verify):
        return result
    else:
        raise argparse.ArgumentTypeError(f"'{result}' is not a valid file path.")


def int_type(value:str, min=float('-inf'), max=float('inf')):
    try:
        ivalue = int(value)
    except ValueError:
        raise argparse.ArgumentTypeError(f"{value} is not an integer")
    ivalue = int(value)
    if ivalue < min or ivalue > max:
        raise argparse.ArgumentTypeError(f"{ivalue} must be between [{min},{max}]")

    return ivalue



if __name__ == "__main__":
    initial_parser = argparse.ArgumentParser(description="This is a parser for root folder and json handling", add_help=False)
    initial_parser.add_argument("--root_folder", type=directory, help="Folder containing all used files", default="")
    args = initial_parser.parse_known_args()[0]

    common_parser = argparse.ArgumentParser(description="This is a parser for common training arguments", parents=[initial_parser])
    common_parser.add_argument("--train_data_file", type=lambda x: sanitizepath(args.root_folder, x), help="File with train data", required=True)
    common_parser.add_argument("--train_class_file", type=lambda x: sanitizepath(args.root_folder, x), help="File with train classes")
    common_parser.add_argument("--train_pred_outfile", type=lambda x: sanitizepath(args.root_folder, x, 'w'), help="File with train predictions", default="predTrain.out")
    common_parser.add_argument("--test_data_file", type=lambda x: sanitizepath(args.root_folder, x), help="File with test data", required=True)
    common_parser.add_argument("--test_class_file", type=lambda x: sanitizepath(args.root_folder, x), help="File with test classes")
    common_parser.add_argument("--test_pred_outfile", type=lambda x: sanitizepath(args.root_folder, x, 'w'), help="File with test predictions", default="predTest.out")
    common_parser.add_argument("--console_file", type=lambda x: sanitizepath(args.root_folder, x, 'w'), help="File with terminal logs redirection")
    common_parser.add_argument("--stats_file", type=lambda x: sanitizepath(args.root_folder, x, 'w'), help="File with output statistics", default="stats.txt")
    common_parser.add_argument("--nb_attributes", type=lambda x: int_type(x, min=1), help="Number of attributes in dataset", metavar=">0", required=True)
    common_parser.add_argument("--nb_classes", type=lambda x: int_type(x, min=1), help="Number of classes in dataset", metavar=">0", required=True)
    args = common_parser.parse_args()
    print(args)
