import argparse
import json
import os

class CustomHelpFormatter(argparse.ArgumentDefaultsHelpFormatter):

    """
    def add_arguments(self, actions):
        # Separation of arguments between optional and required
        optional_actions = [action for action in actions if not action.required]
        required_actions = [action for action in actions if action.required]

        if required_actions:
            self.start_section("Obligatory parameters :")
            self.add_argument_group(required_actions)
            self.end_section()

        if optional_actions:
            self.start_section("Optional parameters :")
            self.add_argument_group(optional_actions)
            self.end_section()
            """
    def _get_help_string(self, action):
        help_string = action.help
        if '%(default)' not in action.help:
            if action.default is not argparse.SUPPRESS and action.default is not None:
                help_string += ' (default: %(default)s)'
        return help_string


def directory(path:str):
    if path == "":
        return ""

    if os.path.isdir(path):
        return path
    else:
        raise argparse.ArgumentTypeError(f"'{path}' is not a valid directory.")


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

# Reads a JSON file and parses it in order to transform it in a list of string
# that is digestible by the argparse parser
#! this has not been tested with every datatype possible yet
def json_to_args(jsonfile: str):
    args = ["--json_config_file", jsonfile]

    with open(jsonfile, "r") as f:
        json_dict = json.load(f)

        for k, v in json_dict.items():
            if isinstance(v, list):
                lst_str = [str(i) for i in v]
                args.extend((f"--{k}",))
                args += lst_str
            else:
                args.extend((f"--{k}", str(v)))

    return args

def int_type(value:str, min=float('-inf'), max=float('inf')):
    try:
        ivalue = int(value)
    except ValueError:
        raise argparse.ArgumentTypeError(f"{value} is not an integer")
    ivalue = int(value)
    if ivalue < min or ivalue > max:
        raise argparse.ArgumentTypeError(f"{ivalue} must be between [{min},{max}]")

    return ivalue

def get_initial_parser(init_args):
    initial_parser = argparse.ArgumentParser(description="This is a parser for root folder and json handling", add_help=False)
    initial_parser.add_argument("--json_config_file", type=lambda x: sanitizepath("", x), help="JSON config file")
    initial_parser.add_argument("--root_folder", type=directory, help="Folder containing all used files", default="")

    args = initial_parser.parse_known_args(init_args)[0]

    # verifies if args are coming from JSON config file
    if args.json_config_file is not None:
        to_parse = json_to_args(args.json_config_file)
        args = initial_parser.parse_known_args(to_parse)[0]

    return args, initial_parser

def get_common_parser(args, initial_parser):
    common_parser = argparse.ArgumentParser(description="This is a parser for common training arguments", parents=[initial_parser], add_help=False)
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

    return common_parser

def get_args(args, init_args, parser):
    # verifies if args are coming from JSON config file or CLI
    if args.json_config_file is not None:
        to_parse = json_to_args(args.json_config_file)
        args = parser.parse_args(to_parse)
    else:
        args = parser.parse_args(init_args)
    return args
