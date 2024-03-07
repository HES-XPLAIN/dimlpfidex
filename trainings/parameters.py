import argparse
import json
import os
import sys

def get_tag_value(actions):
    for action in actions:
        tag_value = getattr(action, 'tag', None)
        if tag_value is not None:
            return tag_value

class CustomHelpFormatter(argparse.ArgumentDefaultsHelpFormatter):

    def add_arguments(self, actions):
        # Separate actions
        required_actions = [action for action in actions if action.required and getattr(action, 'tag', None) is None]
        optional_actions = [action for action in actions if not action.required and getattr(action, 'tag', None) is None]
        tag_actions = [action for action in actions if getattr(action, 'tag', None) is not None]

        # Add required parameters
        if required_actions:
            self.add_text("")
            self.add_text("---------------------------------------------------------------------")
            self.add_text("Warning! The files are localised with respect to root folder dimlpfidex.")
            self.add_text("----------------------------")
            self.add_text("Required parameters:")
            super(CustomHelpFormatter, self).add_arguments(required_actions)
            self.add_text("")

        # Add optionnal parameters
        if optional_actions:
            self.add_text("----------------------------")
            self.add_text("Optional parameters:")
            super(CustomHelpFormatter, self).add_arguments(optional_actions)
            self.add_text("")

        if tag_actions:
            tag = get_tag_value(actions)
            self.add_text("----------------------------")
            self.add_text(tag + " parameters (optional):")
            super(CustomHelpFormatter, self).add_arguments(tag_actions)
            self.add_text("")
            self.add_text("----------------------------")
            self.add_text("----------------------------")
            self.add_text("Execution example :")
            if tag == "MLP":
                self.add_text("mlpTrn('--train_data_file datanormTrain.txt --train_class_file dataclass2Train.txt --test_data_file datanormTest.txt --test_class_file dataclass2Test.txt --weights_outfile mlp/weights.wts --stats_file mlp/stats.txt --train_pred_outfile mlp/predTrain.out --test_pred_outfile mlp/predTest.out --nb_attributes 16 --nb_classes 2 --root_folder dimlp/datafiles')")

        self.add_text("---------------------------------------------------------------------")


    # Show default value in help if necessary
    def _get_help_string(self, action):
        help_string = action.help

        if '%(default)' not in action.help:
            if action.default is not argparse.SUPPRESS and action.default is not None:
                help_string += ' (default: %(default)s)'

        return help_string

    def _format_action_invocation(self, action):
        parts = []
        # Add only options (ex: --train_class_file) and metavar (ex: TRAIN_CLASS_FILE)
        for option_string in action.option_strings:
            parts.append(option_string)
        if action.nargs != 0 and action.metavar is not None:
            parts.append(action.metavar)
        return ' '.join(parts)

    def _format_usage(self, _, actions, groups, prefix):
        if prefix is None:
            prefix = 'Usage: \n'

        # Divide actions between required and optionnals
        required_actions = [action for action in actions if action.required and getattr(action, 'tag', None) is None]
        optional_actions = [action for action in actions if not action.required and getattr(action, 'tag', None) is None]
        tag_actions = [action for action in actions if getattr(action, 'tag', None) is not None]

        # Formatting
        format_action = super()._format_action_invocation
        required_parts = [format_action(action) for action in required_actions]
        optional_parts = ['[%s]' % format_action(action) for action in optional_actions]
        tag_parts = ['[%s]' % format_action(action) for action in tag_actions]

        # Build usage display
        usage = prefix + ' '.join(required_parts + optional_parts + tag_parts) + '\n\n'

        return usage
    # Remove positional arguments and options lines
    def format_help(self):
        help_message = super().format_help()

        lines = help_message.split('\n')
        cleaned_lines = []
        skip_line = False
        for line in lines:
            if 'positional arguments:' in line:
                skip_line = True
            elif 'options:' in line:
                skip_line = True
                cleaned_lines.append("Parameters:")
            elif skip_line and '---' in line:
                skip_line = False
            else:
                cleaned_lines.append(line)
                skip_line = False
        cleaned_help = '\n'.join(cleaned_lines)
        return cleaned_help

# Add possibility to add a tag
class TaggableAction(argparse.Action):
    def __init__(self, *args, **kwargs):
        self.tag = kwargs.pop('tag', None)
        super(TaggableAction, self).__init__(*args, **kwargs)

    def __call__(self, parser, namespace, values, option_string=None):
        setattr(namespace, self.dest, values)

class CustomArgumentParser(argparse.ArgumentParser):
    # Remove sys.exit
    def exit(self, status=0, message=None):
        if message:
            self._print_message("\nError:\n" + message, sys.stderr)
        raise ValueError("")

def directory(path:str):
    if path == "":
        return ""

    if os.path.isdir(path):
        return path
    else:
        raise argparse.ArgumentTypeError(f"'{path}' is not a valid directory.")


def sanitizepath(path:str, file:str, access_type : str = 'r'):
    # Error ff file is an empty string
    if not file.strip():
        raise argparse.ArgumentTypeError(f"'{file}' is not a valid filename.")
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

def int_type(value:str, min=float('-inf'), max=float('inf'), allow_none=False):
    if allow_none and value == "None":
        return None
    try:
        ivalue = int(value)
    except ValueError:
        if allow_none:
            raise argparse.ArgumentTypeError(f"{value} is not an integer or None")
        raise argparse.ArgumentTypeError(f"{value} is not an integer")
    ivalue = int(value)
    if ivalue < min or ivalue > max:
        raise argparse.ArgumentTypeError(f"{ivalue} must be between [{min},{max}]")

    return ivalue

def float_type(value:str, min=float('-inf'), max=float('inf'), min_inclusive=True, max_inclusive=True):
    try:
        fvalue = float(value)
    except ValueError:
        raise argparse.ArgumentTypeError(f"{value} is not an float")
    fvalue = float(value)
    if min_inclusive and max_inclusive and (fvalue < min or fvalue > max):
        raise argparse.ArgumentTypeError(f"{fvalue} must be between [{min},{max}]")
    if min_inclusive and not max_inclusive and (fvalue < min or fvalue >= max):
        raise argparse.ArgumentTypeError(f"{fvalue} must be between [{min},{max}[")
    if not min_inclusive and max_inclusive and (fvalue <= min or fvalue > max):
        raise argparse.ArgumentTypeError(f"{fvalue} must be between ]{min},{max}]")
    if not min_inclusive and not max_inclusive and (fvalue <= min or fvalue >= max):
        raise argparse.ArgumentTypeError(f"{fvalue} must be between ]{min},{max}[")

    return fvalue

def bool_type(value):
    if isinstance(value, bool):
        return value
    if value.lower() in ('yes', 'true', 't', 'y', '1'):
        return True
    elif value.lower() in ('no', 'false', 'f', 'n', '0'):
        return False
    else:
        raise argparse.ArgumentTypeError(f"{value} is not a boolean.")


def enum_type(value:str, *valid_strings, **valid_types):
    """
    Attempts to match the given 'value' against a set of valid string values or some specified types.

    :param value: The value to be validated or converted.
    :param valid_strings: A variable number of string arguments representing valid values for 'value'.
    :param valid_types: Keyword arguments where each key is a type identifier and its value is a dict containing the
                        type function under the key 'func' and any additional keyword arguments for the function.

    :return: The original 'value' if it matches one of the 'valid_strings', or the type converted 'value' if it matches
             one of the type constraints specified in 'valid_types'.

    :raises argparse.ArgumentTypeError: If 'value' does not match any of the 'valid_strings' or cannot be converted
                                         to match the specified type constraints, with a detailed error message.
    """
    if value in valid_strings:
        return value
    # Try each type
    errors = []
    for type_key, type_info in valid_types.items():
        type_func = type_info['func']  # Get the type conversion function
        kwargs = {k: v for k, v in type_info.items() if k != 'func'}  # Get additional arguments, excluding the function itself
        try:
            result = type_func(value, **kwargs)  # Call the function with kwargs
            return result
        except (ValueError, argparse.ArgumentTypeError) as e:
            errors.append(e)
            continue  # Try next type

    # Error if the value belongs to no type
    valid_types_str = '\n'.join([f"Type {type_info['func'].__name__}, error : {errors[idx]}" for idx, (type_key, type_info) in enumerate(valid_types.items())])
    if len(valid_strings)==0:
        raise argparse.ArgumentTypeError(f"Value must match type constraints:\n {valid_types_str}")
    if len(valid_strings)==1:
        raise argparse.ArgumentTypeError(f"Value must be '{valid_strings[0]}' or match one of the types:\n {valid_types_str}")
    else:
        raise argparse.ArgumentTypeError('Value must be one of {0} or match one of the types:\n{1}'.format(', '.join([f"'{v}'" for v in valid_strings]), valid_types_str))


def get_initial_parser(init_args):
    initial_parser = CustomArgumentParser(description="This is a parser for root folder and json handling", add_help=False)
    initial_parser.add_argument("--json_config_file", type=lambda x: sanitizepath("", x), help="JSON config file", metavar="<str>")
    initial_parser.add_argument("--root_folder", type=directory, help="Folder based on main folder dimlpfidex(default folder) containg all used files and where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder>", metavar="<str>", default="")

    args = initial_parser.parse_known_args(init_args)[0]

    # verifies if args are coming from JSON config file
    if args.json_config_file is not None:
        if len(init_args) != 2:
            raise ValueError("Error : option --json_config_file has to be the only option in the command if specified.")
        to_parse = json_to_args(args.json_config_file)
        args = initial_parser.parse_known_args(to_parse)[0]

    return args, initial_parser

def get_common_parser(args, initial_parser):
    common_parser = CustomArgumentParser(description="This is a parser for common training arguments", parents=[initial_parser], add_help=False)
    common_parser.add_argument("--train_data_file", type=lambda x: sanitizepath(args.root_folder, x), help="Train data file", metavar="<str>", required=True)
    common_parser.add_argument("--train_class_file", type=lambda x: sanitizepath(args.root_folder, x), help="Train class file, mendatory if classes are not specified in train_data_file", metavar="<str>")
    common_parser.add_argument("--train_pred_outfile", type=lambda x: sanitizepath(args.root_folder, x, 'w'), help="Output train prediction file name", metavar="<str>", default="predTrain.out")
    common_parser.add_argument("--test_data_file", type=lambda x: sanitizepath(args.root_folder, x), help="Test data file", metavar="<str>", required=True)
    common_parser.add_argument("--test_class_file", type=lambda x: sanitizepath(args.root_folder, x), help="Test class file, mendatory if classes are not specified in test_data_file", metavar="<str>")
    common_parser.add_argument("--test_pred_outfile", type=lambda x: sanitizepath(args.root_folder, x, 'w'), help="Output test prediction file name", metavar="<str>", default="predTest.out")
    common_parser.add_argument("--console_file", type=lambda x: sanitizepath(args.root_folder, x, 'w'), help="File with console logs redirection", metavar="<str>")
    common_parser.add_argument("--stats_file", type=lambda x: sanitizepath(args.root_folder, x, 'w'), help="Output file name with train and test accuracy", metavar="<str>", default="stats.txt")
    common_parser.add_argument("--nb_attributes", type=lambda x: int_type(x, min=1), help="Number of attributes in dataset", metavar="<int [1,inf[>", required=True)
    common_parser.add_argument("--nb_classes", type=lambda x: int_type(x, min=1), help="Number of classes in dataset", metavar="<int [1,inf[>", required=True)

    return common_parser

def get_args(args, init_args, parser):
    # verifies if args are coming from JSON config file or CLI
    if args.json_config_file is not None:
        to_parse = json_to_args(args.json_config_file)
        args = parser.parse_args(to_parse)
    else:
        args = parser.parse_args(init_args)
    return args
