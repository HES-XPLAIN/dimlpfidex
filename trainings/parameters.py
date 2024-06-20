import argparse
import json
import os
import sys
import re
import ast

def print_parameters(args):
    """
    Prints the list of parameters passed to the program.

    This function iterates over all arguments contained within `args` (expected to be an argparse Namespace)
    and prints them to the standard output along with their values, provided they are not None.

    :param args: A Namespace containing the program's arguments.
    """
    print("Parameters list:")
    for k in args.__dict__:
        if args.__dict__[k] is not None:
            print(" -",k,(60 - len(k))*" ",args.__dict__[k])
    print("End of Parameters list.", "\n")

def get_tag_value(actions):
    """
    Retrieves the 'tag' attribute from a list of argparse actions.

    :param actions: A list of argparse actions.
    :return: The first 'tag' attribute value found among the actions, or None if no tag is present.
    """
    for action in actions:
        tag_value = getattr(action, 'tag', None)
        if tag_value is not None:
            return tag_value

class CustomHelpFormatter(argparse.ArgumentDefaultsHelpFormatter):
    """
    A custom help formatter for argparse that categorizes arguments into required, optional, and tagged parameters
    and enhances help message formatting.
    """
    def add_arguments(self, actions):
        """
        Organizes and adds argument descriptions to the help message.

        :param actions: A list of argparse actions.
        """
        # Separate actions
        required_actions = [action for action in actions if action.required and getattr(action, 'tag', None) is None]
        optional_actions = [action for action in actions if not action.required and getattr(action, 'tag', None) is None]
        tag_actions = [action for action in actions if getattr(action, 'tag', None) is not None]

        # Add required parameters
        if required_actions:
            self.add_text("")
            self.add_text("---------------------------------------------------------------------")
            self.add_text("Warning! The files are located with respect to the root folder dimlpfidex.")
            self.add_text("The arguments can be specified in the command or in a json configuration file with --json_config_file your_config_file.json.")
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
            self.add_text("Execution example :", raw=True)
            if tag == "MLP":
                self.add_text("mlpTrn('--train_data_file datanormTrain.txt --train_class_file dataclass2Train.txt --test_data_file datanormTest.txt --test_class_file dataclass2Test.txt --weights_outfile mlp/weights.wts --stats_file mlp/stats.txt --train_pred_outfile mlp/predTrain.out --test_pred_outfile mlp/predTest.out --nb_attributes 16 --nb_classes 2 --root_folder dimlp/datafiles')", raw=True)
            elif tag == "SVM":
                self.add_text("svmTrn('--train_data_file datanormTrain.txt --train_class_file dataclass2Train.txt --test_data_file datanormTest.txt --test_class_file dataclass2Test.txt --weights_outfile svm/weights.wts --stats_file svm/stats.txt --train_pred_outfile svm/predTrain.out --test_pred_outfile svm/predTest.out --nb_attributes 16 --nb_classes 2 --root_folder dimlp/datafiles')", raw=True)
            elif tag == "RF":
                self.add_text("randForestsTrn('--train_data_file datanormTrain.txt --train_class_file dataclass2Train.txt --test_data_file datanormTest.txt --test_class_file dataclass2Test.txt --stats_file rf/stats.txt --train_pred_outfile rf/predTrain.out --test_pred_outfile rf/predTest.out --rules_outfile rf/RF_rules.rls --nb_attributes 16 --nb_classes 2 --root_folder dimlp/datafiles')", raw=True)
            elif tag == "GB":
                self.add_text("gradBoostTrn('--train_data_file datanormTrain.txt --train_class_file dataclass2Train.txt --test_data_file datanormTest.txt --test_class_file dataclass2Test.txt --stats_file gb/stats.txt --train_pred_outfile gb/predTrain.out --test_pred_outfile gb/predTest.out --rules_outfile gb/RF_rules.rls --nb_attributes 16 --nb_classes 2 --root_folder dimlp/datafiles')", raw=True)
            elif tag == "ROC":
                self.add_text("computeRocCurve('--test_class_file dataclass2Test.txt --test_pred_file predTest.out --positive_class_index 1 --output_roc roc_curve.png --stats_file stats.txt --root_folder dimlp/datafiles --nb_classes 2')", raw=True)
            elif tag == "CNN":
                self.add_text("cnnTrn('--model small --train_data_file trainData.txt --train_class_file trainClass.txt --test_data_file testData.txt --test_class_file testClass.txt --valid_data_file validData.txt --valid_class_file validClass.txt --original_input_size (28,28) --nb_channels 1 --data_format classic --nb_classes 10 --root_folder dimlp/datafiles/Mnist')", raw=True)
            elif tag == "Normalization":
                self.add_text("For datafile normalization :", raw=True)
                self.add_text("normalization('--data_files [datanormTrain.txt,datanormTest.txt] --normalization_indices [0,2,4] --nb_attributes 16 --missing_values NaN --root_folder dimlp/datafiles')", raw=True)
                self.add_text("For rulefile denormalization :", raw=True)
                self.add_text("normalization('--normalization_file normalization_stats.txt --rule_files globalRulesDatanorm.txt --nb_attributes 16 --root_folder dimlp/datafiles')", raw=True)
        self.add_text("---------------------------------------------------------------------", raw=True)

    # To display execution examples on one line
    def add_text(self, text, raw=False):
        """
        Adds custom text to the help message. This method allows for raw text addition,
        bypassing the standard formatting applied by argparse.

        :param text: The text to be added to the help message.
        :type text: str
        :param raw: If True, adds the text without any formatting. If False, the standard formatting is applied.
        :type raw: bool
        """
        if raw:
            self._add_item(lambda x: x + '\n\n', [text])
        else:
            super(CustomHelpFormatter, self).add_text(text)

    # Show default value in help if necessary
    def _get_help_string(self, action):
        """
        Retrieves and optionally appends the default value to the help string of an argparse action.

        :param action: An argparse action.
        :return: The help string with default value information if applicable.
        """
        help_string = action.help

        if '%(default)' not in action.help:
            if action.default is not argparse.SUPPRESS and action.default is not None:
                if action.default == "":
                    help_string += ' (default: \"\")'
                else:
                    help_string += ' (default: %(default)s)'

        return help_string

    def _format_action_invocation(self, action):
        """
        Formats the display of argument invocation in the help message (e.g., how to use the argument).

        :param action: An argparse action.
        :return: A string representing how to invoke the action.
        """
        parts = []
        # Add only options (ex: --train_class_file) and metavar (ex: TRAIN_CLASS_FILE)
        for option_string in action.option_strings:
            parts.append(option_string)
        if action.nargs != 0 and action.metavar is not None:
            parts.append(action.metavar)
        return ' '.join(parts)

    def _format_usage(self, _, actions, groups, prefix):
        """
        Customizes the usage message, categorizing actions into required, optional, and tagged for clarity.

        :param actions: A list of argparse actions.
        :param groups: A list of argument groups.
        :param prefix: A prefix string for the usage message.
        :return: A formatted usage string.
        """
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
        """
        Cleans up and formats the overall help message, removing unnecessary lines and organizing content.

        :return: A cleaned and formatted help message string.
        """
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
    """
    A custom argparse action that supports tagging, allowing for additional metadata to be associated with arguments.

    :param tag: An optional tag to associate with the action.
    """
    def __init__(self, *args, **kwargs):
        """
        Initializes the TaggableAction with optional tag metadata.
        """
        self.tag = kwargs.pop('tag', None)
        super(TaggableAction, self).__init__(*args, **kwargs)

    def __call__(self, parser, namespace, values, option_string=None):
        """
        Sets the argument value on the parser namespace, handling the custom action execution.
        """
        setattr(namespace, self.dest, values)

class CustomArgumentParser(argparse.ArgumentParser):
    """
    A custom argument parser that overrides the default exit behavior to raise an exception instead of exiting.
    """

    # Remove sys.exit
    def exit(self, status=0, message=None):
        """
        Overrides the default 'exit' method to prevent system exit on parsing errors.

        :param status: The exit status code.
        :param message: The error message to print.
        :raises ValueError: Always raised to avoid exiting the script.
        """
        if message:
            self._print_message(message, sys.stderr)
        raise ValueError("")

def directory(path:str):
    """
    An argparse type function that validates if the provided path is a directory.

    :param path: The input path string.
    :return: The validated directory path.
    :raises argparse.ArgumentTypeError: If the path is not a valid directory.
    """
    if path == "":
        return ""

    if os.path.isdir(path):
        return path
    else:
        raise argparse.ArgumentTypeError(f"'{path}' is not a valid directory.")


def sanitizepath(path:str, file:str, access_type : str = 'r'):
    """
    Validates and constructs a full file path based on a base path and a filename, checking for file existence based on access type.

    :param path: The base path.
    :param file: The filename to append to the base path.
    :param access_type: The file access type ('r' for read, 'w' for write).
    :return: The constructed and validated file path.
    :raises argparse.ArgumentTypeError: If the path does not exist or the file is not valid.
    """
    # Error if file is an empty string
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
    """
    Parses a JSON file and converts it into a list of command-line arguments.

    :param jsonfile: The path to the JSON configuration file.
    :return: A list of command-line arguments derived from the JSON file.
    :raises ValueError: If there is an error parsing the JSON file.
    """
    args = ["--json_config_file", jsonfile]

    with open(jsonfile, "r") as f:
        try:
            json_dict = json.load(f)
        except ValueError as e:
            raise ValueError(f"Error : {e}\nPS: This error means that your configuration file {jsonfile} has the error specified above in it.")

        for k, v in json_dict.items():
            if isinstance(v, list):
                lst_str = [str(i) for i in v]
                args.extend((f"--{k}",))
                args += lst_str
            else:
                args.extend((f"--{k}", str(v)))

    return args

def int_type(value:str, min=float('-inf'), max=float('inf'), allow_value=None):
    """
    Validates and converts a string to an integer, with optional range constraints and the option to allow None as a value.

    :param value: The input string to convert.
    :param min: The minimum acceptable value.
    :param max: The maximum acceptable value.
    :return: The converted integer.
    :raises argparse.ArgumentTypeError: If the input is invalid or out of the specified range.
    """
    try:
        ivalue = int(value)
    except ValueError:
        raise argparse.ArgumentTypeError(f"{value} is not an integer")
    if ivalue < min or ivalue > max:
        if allow_value:
            if ivalue == allow_value:
                return ivalue
            else:
                raise argparse.ArgumentTypeError(f"{ivalue} must be between [{min},{max}] or {allow_value}")
        raise argparse.ArgumentTypeError(f"{ivalue} must be between [{min},{max}]")

    return ivalue

def float_type(value:str, min=float('-inf'), max=float('inf'), min_inclusive=True, max_inclusive=True):
    """
    Validates and converts a string to a float, with optional range constraints.

    :param value: The input string to convert.
    :param min: The minimum acceptable value.
    :param max: The maximum acceptable value.
    :param min_inclusive: Whether the minimum value is inclusive.
    :param max_inclusive: Whether the maximum value is inclusive.
    :return: The converted float.
    :raises argparse.ArgumentTypeError: If the input is invalid or out of the specified range.
    """
    try:
        fvalue = float(value)
    except ValueError:
        raise argparse.ArgumentTypeError(f"{value} is not an float")
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
    """
    Converts a string to a boolean value, recognizing various true/false representations.

    :param value: The input string to convert.
    :return: The converted boolean value.
    :raises argparse.ArgumentTypeError: If the input cannot be interpreted as a boolean.
    """
    if isinstance(value, bool):
        return value
    if value.lower() in ('yes', 'true', 't', 'y', '1'):
        return True
    elif value.lower() in ('no', 'false', 'f', 'n', '0'):
        return False
    else:
        raise argparse.ArgumentTypeError(f"{value} is not a boolean.")

def dict_type(value:str):
    """
    Validates and converts a string representation of a dictionary into a dictionary.

    :param value_str: The string representation of a dictionary.
    :return: The dictionary represented by the input string.
    :raises argparse.ArgumentTypeError: If the input string does not represent a valid dictionary.
    """
    try:
        # Ensure the result is a dictionary
        result = ast.literal_eval(value)
        if not isinstance(result, dict):
            raise ValueError("Input is not a valid dictionary. Use format {0:1.5,1:2}.")

        return result
    except (ValueError, SyntaxError) as e:
        # Raise a specific error if conversion fails
        raise argparse.ArgumentTypeError(f"Could not convert '{value}' to a dictionary. Use format {{0:1.5,1:2}}. Error: {e}")



def list_type(str_list:str, valid_type:dict):
    """
    Converts a string representation of a list into a list of values of a specified type,
    checking this type.

    :param str_list: The string representation of the list to be converted. The string should be
                     delimited by commas or spaces, and optionally enclosed in brackets or parentheses.
    :param valid_type: A dictionary specifying the type to which each element of the list should be converted.
                       It must contain a 'func' key with a function for type conversion, and can include
                       additional keys for type-specific constraints.

    :return: A list of values of the specified type, with each element having passed the defined constraints.

    Example usage:
    list_type("[1, 2, 3]", valid_type={'func': int}) -> [1, 2, 3]
    list_type("4 5 6", valid_type={'func': int, 'min': 3}) -> [4, 5, 6] with min value constraint applied
    """
    # Get type
    type_func = valid_type['func']  # La fonction de conversion
    constraints = {k: v for k, v in valid_type.items() if k != 'func'}

    # String to list
    res_temp = str_list.removeprefix('[').removesuffix(']')
    res_temp = res_temp.removeprefix('(').removesuffix(')')
    res_temp_lst = list(filter(None, re.split(r'[,\s]+', res_temp)))

    # Check constraints
    res_lst = []
    for v in res_temp_lst:
        res_lst.append(type_func(v, **constraints))
    return res_lst

def pair_type(str_list: str, valid_type: dict):
    """
    Converts a string representation of a pair into a pair of values of a specified type,
    ensuring that the pair contains exactly 2 elements.

    :param str_list: The string representation of the pair to be converted. The string should be
                     delimited by commas or spaces, and optionally enclosed in brackets or parentheses.
    :param valid_type: A dictionary specifying the type to which each element of the pair should be converted.
                       It must contain a 'func' key with a function for type conversion, and can include
                       additional keys for type-specific constraints.

    :return: A pair of exactly 2 values of the specified type, with each element having passed the defined constraints.
             Raises a ValueError if the list does not contain exactly 2 elements.

    Example usage:
    pair_type("[1, 2]", valid_type={'func': int}) -> (1, 2)
    pair_type("(4 5)", valid_type={'func': int, 'min': 3}) -> (4, 5) with min value constraint applied
    """
    res_lst = list_type(str_list, valid_type)

    if len(res_lst) != 2:
        raise ValueError("The list must contain exactly 2 elements.")

    return (res_lst[0], res_lst[1])

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
    for _, type_info in valid_types.items():
        type_func = type_info['func']  # Get the type conversion function
        constraints = {k: v for k, v in type_info.items() if k != 'func'}  # Get additional arguments, excluding the function itself
        try:
            result = type_func(value, **constraints)  # Call the function with constraints
            return result
        except (ValueError, argparse.ArgumentTypeError) as e:
            errors.append(str(e))
            continue  # Try next type

    # Error if the value belongs to no type
    valid_types_str = '\n'.join([f"Type {type_info['func'].__name__}, error : {errors[idx]}" for idx, (_, type_info) in enumerate(valid_types.items())])
    if len(valid_strings)==0:
        raise argparse.ArgumentTypeError(f"Value must match one of the type constraints:\n{valid_types_str}")
    if len(valid_strings)==1:
        raise argparse.ArgumentTypeError(f"Value must be '{valid_strings[0]}' or match one of the types:\n{valid_types_str}")
    else:
        raise argparse.ArgumentTypeError('Value must be one of {0} or match one of the types:\n{1}'.format(', '.join([f"'{v}'" for v in valid_strings]), valid_types_str))


def get_initial_parser(init_args):
    """
    Creates and returns an initial argument parser for handling the root folder and JSON configuration file options.

    :param init_args: The initial command-line arguments.
    :return: A tuple containing the parsed arguments and the initial parser instance.
    """
    initial_parser = CustomArgumentParser(description="This is a parser for root folder and json handling", add_help=False)
    initial_parser.add_argument("--json_config_file", type=lambda x: sanitizepath("", x), help="Path to the JSON file that configures all parameters. If used, this must be the sole argument and must specify the file's relative path", metavar="<str>")
    initial_parser.add_argument("--root_folder", type=directory, help="Path to the folder, based on main default folder dimlpfidex, containing all used files and where generated files will be saved. If a file name is specified with another option, its path will be relative to this root folder>", metavar="<str>", default="")

    args = initial_parser.parse_known_args(init_args)[0]

    # verifies if args are coming from JSON config file
    if args.json_config_file is not None:
        if len(init_args) != 2:
            raise ValueError("Error : option --json_config_file has to be the only option in the command if specified.")
        to_parse = json_to_args(args.json_config_file)
        args = initial_parser.parse_known_args(to_parse)[0]

    return args, initial_parser

def get_common_parser(args, initial_parser):
    """
    Creates and returns a common argument parser for handling shared training arguments.

    :param args: The arguments previously parsed by the initial parser.
    :param initial_parser: The instance of the initial parser to use as a parent for common arguments.
    :return: The instance of the common argument parser.
    """
    common_parser = CustomArgumentParser(description="This is a parser for common training arguments", parents=[initial_parser], add_help=False)
    common_parser.add_argument("--train_data_file", type=lambda x: sanitizepath(args.root_folder, x), help="Path to the file containing the train portion of the dataset", metavar="<str>", required=True)
    common_parser.add_argument("--train_class_file", type=lambda x: sanitizepath(args.root_folder, x), help="Path to the file containing the train true classes of the dataset, mandatory if classes are not specified in train_data_file", metavar="<str>")
    common_parser.add_argument("--test_data_file", type=lambda x: sanitizepath(args.root_folder, x), help="Path to the file containing the test portion of the dataset", metavar="<str>", required=True)
    common_parser.add_argument("--test_class_file", type=lambda x: sanitizepath(args.root_folder, x), help="Path to the file containing the test true classes of the dataset, mandatory if classes are not specified in test_data_file", metavar="<str>")
    common_parser.add_argument("--nb_attributes", type=lambda x: int_type(x, min=1), help="Number of attributes in the dataset", metavar="<int [1,inf[>", required=True)
    common_parser.add_argument("--nb_classes", type=lambda x: int_type(x, min=1), help="Number of classes in the dataset", metavar="<int [1,inf[>", required=True)
    common_parser.add_argument("--train_pred_outfile", type=lambda x: sanitizepath(args.root_folder, x, 'w'), help="Path to the file where the train predictions will be stored", metavar="<str>", default="predTrain.out")
    common_parser.add_argument("--test_pred_outfile", type=lambda x: sanitizepath(args.root_folder, x, 'w'), help="Path to the file where the test predictions will be stored", metavar="<str>", default="predTest.out")
    common_parser.add_argument("--stats_file", type=lambda x: sanitizepath(args.root_folder, x, 'w'), help="Path to the file where the train and test accuracy will be stored", metavar="<str>", default="stats.txt")
    common_parser.add_argument("--console_file", type=lambda x: sanitizepath(args.root_folder, x, 'w'), help="Path to the file where the terminal output will be redirected. If not specified, all output will be shown on your terminal", metavar="<str>")

    return common_parser

def get_args(args, init_args, parser):
    """
    Finalizes argument parsing, either from the JSON configuration file or command-line input, using a given parser.

    :param args: The arguments previously parsed by the initial parser.
    :param init_args: The initial command-line arguments.
    :param parser: The argparse parser instance to use for final argument parsing.
    :return: The fully parsed arguments.
    """
    # verifies if args are coming from JSON config file or CLI
    if args.json_config_file is not None:
        to_parse = json_to_args(args.json_config_file)
        args = parser.parse_args(to_parse)
    else:
        args = parser.parse_args(init_args)
    return args
