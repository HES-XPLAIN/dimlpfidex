import time
import sys
import numpy as np
import os
import math
import re

from trainings.trnFun import get_data, get_attribute_file
from trainings.parameters import get_initial_parser, get_args, sanitizepath, CustomArgumentParser, CustomHelpFormatter, TaggableAction, int_type, float_type, bool_type, list_type, print_parameters



def gaussian_normalization(data_files, normalization_indices, normalized_file, fill_missing_values, normalization_file=None, attributes=None, missing_value=None, with_median=False, mus=None, sigmas=None):

    """
    Perform Gaussian normalization on specified attributes of the given data.

    This function normalizes the data based on the Gaussian distribution, where each selected attribute is
    adjusted to have a mean (or median) of zero and a standard deviation of one. It handles missing values,
    writes the normalization statistics to a file if provided, and saves the normalized data to a specified file.

    :param data_files: The dataset to be normalized.
    :type data_files: list of lists
    :param normalization_indices: Indices of the attributes to be normalized.
    :type normalization_indices: list
    :param normalized_file: File path to save the normalized data.
    :type normalized_file: str
    :param fill_missing_values: Flag to fill missing values with the mean/median.
    :type fill_missing_values: bool
    :param normalization_file: Optional file path to save the normalization statistics.
    :type normalization_file: str, optional
    :param attributes: Optional list of attribute names corresponding to indices.
    :type attributes: list, optional
    :param missing_value: Representation of missing values in the data.
    :type missing_value: str or None
    :param with_median: Flag to use median for normalization instead of mean.
    :type with_median: bool
    :param mus: Predefined list of mean (or median) values for each attribute, defaults to None.
    :type mus: list of float, optional
    :param sigmas: Predefined list of standard deviation values for each attribute, defaults to None.
    :type sigmas: list of float, optional
    :raises ValueError: If mus, sigmas, and normalization_indices are not of the same length or other existing conditions.
    :return: Tuple containing the mean (or median) and standard deviation used for normalization.
    :rtype: (float, float)
    """

    if mus and len(mus) != len(normalization_indices):
            raise ValueError("Error: The length of 'mus' and 'normalization_indices' must be the same.")

    if sigmas and len(sigmas) != len(normalization_indices):
            raise ValueError("Error: The length of 'sigmas' and 'normalization_indices' must be the same.")

    has_mus = True
    if not mus:
        has_mus = False
        mus = []
    has_sigmas = True
    if not sigmas:
        has_sigmas = False
        sigmas = []

    # Erase everything in the normalization file in case it already exists
    if normalization_file is not None:
        try:
            with open(normalization_file, 'w') as myfile:
                # File opened in write mode to clear its contents
                pass
        except FileNotFoundError:
                raise ValueError(f"Error : File {normalization_file} not found.")
        except IOError:
            raise ValueError(f"Error : Couldn't open file {normalization_file}.")

    for idx, indice in enumerate(normalization_indices):
        name_id = str(attributes[indice]) if attributes else str(indice)
        attribut = []
        for data in data_files:
            if data[indice] != missing_value:
                try:
                    value = float(data[indice])
                    if math.isnan(value):
                        raise ValueError
                    attribut.append(value)
                except ValueError:
                    raise ValueError("Error during gaussian normalization : Found a Non-numeric value found which is not the missing value.")
        if len(attribut) != 0:
            attribut = np.array(attribut)
            current_mu = mus[idx] if has_mus else None
            current_sigma = sigmas[idx] if has_sigmas else None
            if current_sigma is None:
                current_sigma = np.std(attribut)
                sigmas.append(current_sigma)
            if current_mu is None:
                if with_median:
                    current_mu = np.median(attribut)
                else:
                    current_mu = np.mean(attribut)
                mus.append(current_mu)

            if normalization_file is not None:
                try:
                    with open(normalization_file, 'a') as myfile:

                        if with_median:
                            myfile.write(name_id + " : original median: " + str(current_mu) + ", original std: " + str(current_sigma) + '\n')
                        else:
                            myfile.write(name_id + " : original mean: " + str(current_mu) + ", original std: " + str(current_sigma) + '\n')
                except (FileNotFoundError):
                    raise ValueError(f"Error : File {normalization_file} not found.")
                except (IOError):
                    raise ValueError(f"Error : Couldn't open file {normalization_file}.")

            if fill_missing_values:
                for data in data_files:
                    data[indice] = np.where(data[indice] == missing_value, str(current_mu), data[indice])
                    data[indice] = str(data[indice])
            for i in range(len(data_files)):
                if data_files[i][indice] != missing_value:
                    if current_sigma == 0:
                        data_files[i][indice] = str(0)
                    else:
                        data_files[i][indice] = str(round((float(data_files[i][indice]) - current_mu) / current_sigma, 6))
        else:
            raise ValueError(f"Error : no value for attribute {name_id}. Impossible to normalize.")

    try:
        with open(normalized_file, 'w') as norm_file:
            for data in data_files:
                norm_file.write(' '.join(data) + '\n')
    except IOError:
        raise ValueError(f"Error: Could not write to file {normalized_file}.")

    return mus, sigmas


def parse_normalization_file(file_name, nb_attributes, attributes=None):
    """
    Parse a file containing normalization statistics previously generated using the normalization function.

    :param file_name: Name of the file containing the normalization statistics.
                      The expected format for each line in the file is either:
                      "[attribute_name] : original (mean|median): [value], original std: [value]"
                      or
                      "[attribute_index] : original (mean|median): [value], original std: [value]"
                      where [attribute_name] is a string from the 'attributes' list (if provided),
                      [attribute_index] is an integer (if 'attributes' is not provided),
                      and [value] is a floating-point number.
    :param nb_attributes: Number of attributes.
    :param attributes: List of attribute names (optional).
    :raises ValueError: If the file is not found, cannot be opened, is not in the correct format,
                        or if there is an inconsistency in using mean or median across different lines.
    :return: Tuple of (indices_list, with_median, mean_median_values, std_values).
    """

    with_median = None

    # Check if each line is on good format, either with attribute names or with attribute indices
    patterns = []
    float_pattern = r'-?\d+(\.\d+)?'
    if attributes is not None:
        if len(attributes) != nb_attributes:
            raise ValueError(f"Error during parsing of {file_name} : the number of attributes is not equal to the length of attributes list.")

        attr_pattern = "|".join(map(re.escape, attributes))
        attr_pattern = re.compile(fr'({attr_pattern}) : original (mean|median): ({float_pattern}), original std: ({float_pattern})') # Regular expression pattern to match for each line
        patterns.append(attr_pattern)
    index_pattern = "|".join(map(str, range(nb_attributes)))
    index_pattern = re.compile(fr'({index_pattern}) : original (mean|median): ({float_pattern}), original std: ({float_pattern})') # Regular expression pattern to match for each line
    patterns.append(index_pattern)

    for pattern in patterns: # We test if it works with attribute names, and if not, with indices
        indices_list = []
        mus = []
        sigmas = []
        pattern_error = False

        try:
            with open(file_name, 'r') as file:
                for line in file:
                    if line.strip(): # Check if line is not empty
                        # Check if line is on good format.
                        match = re.match(pattern, line)
                        if not match:
                            pattern_error = True
                            break

                        if pattern == index_pattern:
                            indices_list.append(int(match.group(1)))
                        else:
                            indices_list.append(attributes.index(match.group(1)))

                        if with_median is None:
                            with_median = match.group(2) == "median"
                        else:
                            if match.group(2) != ("median" if with_median else "mean"):
                                raise ValueError(f"Error in {file_name} : Inconsistency in using mean or median.")

                        mus.append(float(match.group(3)))
                        sigmas.append(float(match.group(5)))
        except FileNotFoundError:
            raise ValueError(f"Error: File {file_name} not found.")
        except IOError:
            raise ValueError(f"Error: Couldn't open file {file_name}.")
        if not pattern_error:
            break

    if pattern_error:
        if attributes is None:
            raise ValueError(f"Error in {file_name} : File not in the correct format, maybe you forgot to add the attribute file.")
        else:
            raise ValueError(f"Error in {file_name} : File not in the correct format.")

    if len(indices_list) != len(set(indices_list)):
                    raise ValueError(f"Error in {file_name} : The same attribute is specified twice.")

    return indices_list, with_median, mus, sigmas

def get_pattern_from_rule_file(rule_file, possible_patterns):
    """
    This function reads through a given rule file and identifies the pattern that matches the rules in the file.
    It narrows down the possible patterns to the one that matches the content of the file.
    If no pattern matches, or if multiple patterns match different rules, an error is raised.

    :param rule_file: The path to the rule file to be analyzed.
    :type rule_file: str
    :param possible_patterns: A list of regular expression patterns to check against the lines in the file.
    :type possible_patterns: list of str
    :raises ValueError: If no rule matches the provided patterns, or if multiple patterns match different rules.
    :raises ValueError: If the file is not found or cannot be opened.
    :return: The pattern that matches the rules in the file.
    :rtype: str
    """
    try:
        with open(rule_file, "r") as file:

            has_rules = False
            for line in file:
                matches = [pattern for pattern in possible_patterns if re.search(pattern, line)]
                # We keep the pattern that match the line, if there is some
                if matches:
                    has_rules = True
                    possible_patterns = matches
                if len(possible_patterns) == 0:
                    raise ValueError(f"Error in file {rule_file}: At least 2 different rules match 2 different patterns, maybe some rules have attribute or class names and some others don't.")
                if len(possible_patterns) == 1:
                    break
            if not has_rules:
                raise ValueError(f"Error in file {rule_file}: There is no rule with correct format in the file. Maybe you forgot to add an attribute file.")
            pattern = possible_patterns[0]
    except FileNotFoundError:
            raise ValueError(f"Error : File {rule_file} not found.")
    except IOError:
        raise ValueError(f"Error : Couldn't open file {rule_file}.")

    return pattern

def denormalize_rule(line, pattern, antecedent_pattern, dimlp_rule, with_attribute_names, normalization_indices, attributes, sigmas, mus):
    """
    This function denormalizes a given rule line based on specified rule patterns and normalization parameters.
    If the line don't correspond to the pattern, it's left unchanged.
    Otherwise, it parses the rule line using the provided regular expression patterns, identifies each antecedent corresponding to indices that we need to denormalize,
    and applies denormalization to the numeric values using the provided sigma (standard deviation) and mu (mean) values.
    The function reconstructs the rule with the denormalized values.

    :param line: The rule line to be denormalized.
    :type line: str
    :param pattern: The regular expression pattern for identifying and parsing the entire rule line.
    :type pattern: str
    :param antecedent_pattern: The regular expression pattern for identifying each antecedent in the rule.
    :type antecedent_pattern: str
    :param dimlp_rule: A flag indicating whether the rule is a DIMLP rule or a Fidex rule, which affects attribute indexing and rule pattern.
    :type dimlp_rule: bool
    :param with_attribute_names: A flag indicating whether the rules use attribute names instead of numeric IDs.
    :type with_attribute_names: bool
    :param normalization_indices: A list of indices of the attributes to be denormalized.
    :type normalization_indices: list of int
    :param attributes: A list of attribute names or numeric IDs of the attributes.
    :type attributes: list of str or int
    :param sigmas: A list of standard deviation values for denormalization, corresponding to each attribute of normalization_indices.
    :type sigmas: list of float
    :param mus: A list of mean or median values for denormalization, corresponding to each attribute of normalization_indices.
    :type mus: list of float
    :return: The denormalized rule line.
    :rtype: str
    """
    match = re.search(pattern, line) # Get each rule
    if match:
        new_line = match.group("first_part")
        # Get each antecedent
        antecedents = match.group('antecedents')
        antecedents_matches = re.finditer(antecedent_pattern, antecedents)
        for antecedent_match in antecedents_matches:
            attribute = antecedent_match.group('attribute')
            attribute_id = attributes.index(attribute) if with_attribute_names else int(attribute)
            if dimlp_rule and not with_attribute_names:
                attribute_id -= 1 # indexes start at 1 in dimlp rules
            value = float(antecedent_match.group('value'))
            if attribute_id in normalization_indices:
                #Denormalize this antecedent
                idx = normalization_indices.index(attribute_id)
                value = value*sigmas[idx]+mus[idx]
            # Reconstuct rule
            new_line += f"{antecedent_match.group(1)}{attribute}{antecedent_match.group('inequality')}{value:.6f}{antecedent_match.group('last_part')}"
        new_line += match.group("last_part")
        return new_line
    else: # Rewrite the line
        return line

def get_and_check_parameters(init_args):
    """
    Processes and validates command-line arguments for a data normalization application.
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
    parser = CustomArgumentParser(description="This is a parser for normalization", parents=[initial_parser], formatter_class=formatter, add_help=True)
    parser.add_argument("--nb_attributes", type=lambda x: int_type(x, min=1), help="Number of attributes in the dataset", metavar="<int [1,inf[>", required=True)
    parser.add_argument("--nb_classes", type=lambda x: int_type(x, min=1), help="Number of classes in the dataset", metavar="<int [1,inf[>")
    parser.add_argument("--data_files", type=lambda x: [sanitizepath(args.root_folder, file) for file in list_type(x, dict(func=str))], metavar="<list<str>>", help="List of paths to data files to normalize, they are normalized with respect to the first one if normalization_file is not specified", action=TaggableAction, tag="Normalization")
    parser.add_argument("--rule_files", type=lambda x: [sanitizepath(args.root_folder, file) for file in list_type(x, dict(func=str))], metavar="<list<str>>", help="List of paths to rule files to denormalize, denormalization is possible only if a normalization_file file or mus, sigmas and normalization_indices are given. Either 'data_files' or 'rule_files' must be specified", action=TaggableAction, tag="Normalization")
    parser.add_argument("--missing_values", type=str, help="String representing a missing value in your data, put 'NaN' (or any string not present in your data) if you do not have any missing value, mandatory for normalization", metavar="<str>", action=TaggableAction, tag="Normalization")
    parser.add_argument("--normalization_file", type=lambda x: sanitizepath(args.root_folder, x), help="Path to the file containing the mean and standard deviation of some attributes. Used to denormalize the rules if specified", metavar="<str>", action=TaggableAction, tag="Normalization")
    parser.add_argument("--mus", type=lambda x: list_type(x, dict(func=float_type)), metavar="<list<float>>", help="Mean or median of each attribute index of interest", action=TaggableAction, tag="Normalization")
    parser.add_argument("--sigmas", type=lambda x: list_type(x, dict(func=float_type)), metavar="<list<float>>", help="Standard deviation of each attribute index of interest", action=TaggableAction, tag="Normalization")
    parser.add_argument("--normalization_indices", type=lambda x: list_type(x, dict(func=int_type, min=0)), metavar="<list<int [0,nb_attributes-1]>>", help="Attribute indices to be normalized or denormalized, index starts at 0, only used when no normalization_file is given (default: [0,...,nb_attributes-1])", action=TaggableAction, tag="Normalization")
    parser.add_argument("--output_normalization_file", type=lambda x: sanitizepath(args.root_folder, x, "w"), help="Path to the file where the mean and standard deviation of the normalized attributes will be stored if normalization_file is not specified.", metavar="<str>", default="normalization_stats.txt", action=TaggableAction, tag="Normalization")
    parser.add_argument("--output_data_files", type=lambda x: [sanitizepath(args.root_folder, file, "w") for file in list_type(x, dict(func=str))], help="List of paths where the normalized files will be saved, it is mandatory to specify each of them if one is specified (default: <original_name>_normalized<original_extension>)", metavar="<list<str>>", action=TaggableAction, tag="Normalization")
    parser.add_argument("--output_rule_files", type=lambda x: [sanitizepath(args.root_folder, file, "w") for file in list_type(x, dict(func=str))], help="List of paths where the normalized rule files will be saved, it is mandatory to specify each of them if one is specified (default: <original_name>_denormalized<original_extension>)", metavar="<list<str>>", action=TaggableAction, tag="Normalization")
    parser.add_argument("--attributes_file", type=lambda x: sanitizepath(args.root_folder, x), help="Path to the file containing the labels of attributes and classes, mandatory during denormalization if rules or normalization stats are written with attribute names", metavar="<str>")
    parser.add_argument("--with_median", type=bool_type, help="Whether to use median instead of mean to normalize", metavar="<bool>", default=False, action=TaggableAction, tag="Normalization")
    parser.add_argument("--fill_missing_values", type=bool_type, help="Whether to fill missing values with mean (or median) during normalization", metavar="<bool>", default=True, action=TaggableAction, tag="Normalization")

    return get_args(args, cleaned_args, parser) # Return attributes

def normalization(args: str = None):

    """
    This function serves two primary purposes: to normalize data files and to denormalize rule files.
    It offers flexibility in the normalization process through various options.

    Normalization can be performed in several ways:
    1. Using a 'normalization_file' file containing normalization parameters along with one or more data files.
    2. Providing data files directly, where the first file is normalized to determine mean/median and standard deviation, which are then applied to other files.
    3. Supplying mean/median (mus) and standard deviations (sigmas) as lists, along with the data files.
    In the last two cases, indices of attributes to normalize must be provided, and a 'normalization_file' file is generated for future use.

    Denormalization can also be done in multiple ways:
    1. Using a 'normalization_file' file with one or more rule files.
    2. Directly providing mean/median (mus) and standard deviations (sigmas) along with the rule files. Attribute indices to be denormalized must be provided in this case.

    The function generates new normalized and/or denormalized files.

    Notes:
    - Each file is located with respect to the root folder dimlpfidex or to the content of the 'root_folder' parameter if specified.
    - It's mandatory to specify the number of attributes in the data and the symbol representing missing data.
    - Choose whether to replace missing data or not.
    - If normalizing training data, it is advisable to normalize test/validation files simultaneously for consistency.
    - Providing no command-line arguments or using -h/--help displays usage instructions, detailing both required and optional parameters for user guidance.

    When to use :
    - It's good to normalize data before training with Dimlp and dimlpBT.
    - It's not necessary to normalize data before training with cnnTrn, MLP and SVM because a normalization is done during the process.
    - It's not necessary to normalize data before training with GradientBoosting and RandomForests because decision trees don't need normalization.

    Outputs :
    - output_normalization_file : File containing the mean and std of the normalized attributes.
    - output_data_files : Files containing the original data files normalized.
    - output_rule_files : Files containing the original rule files denormalized.

    File formats:
    - **Normalization file**: Each line contains the mean/median and standard deviation for an attribute.
      Format: '2 : original mean: 0.8307, original std: 0.0425'
      Attribute indices (index 2 here) can be replaced with attribute names, then an attribute file is required.
    - **Data files**: These files should contain one sample per line, with numbers separated either by spaces, tabs, semicolons or commas. Supported formats:
      1. Only attributes (floats).
      2. Attributes (floats) followed by an integer class ID.
      3. Attributes (floats) followed by one-hot encoded class.
    - **Rule files**: Contain rules in Dimlp or Fidex format. Formats:
      Dimlp: 'Rule 1: (x2 > 0.785787) (x5 > 0.591247) (x8 < 0.443135) Class = 1 (187)'
      Fidex: 'X1>=0.414584 X10<0.507982 X5>=0.314835 X6>=0.356158 -> class 0'
      In both formats, attribute indices (e.g., X1, x2) and class identifiers can be replaced with attribute names and class names, respectively, then an attribute file is required.
    - **Attribute file**: Each line corresponds to an attribute's name, with optional class names at the end. Names can't have spaces inbetween (replace by _).

    Examples of how to call the function:
    from trainings.normalization import normalization
    - For data files: normalization("--data_files [datanormTrain.txt,datanormTest.txt] --normalization_indices [0,2,4] --nb_attributes 16 --missing_values NaN --root_folder dimlp/datafiles")
    - For rule files: normalization("--normalization_file normalization_stats.txt --rule_files globalRulesDatanorm.txt --nb_attributes 16 --root_folder dimlp/datafiles")

    :param args: A single string containing either the path to a JSON configuration file with all specified arguments, or all arguments for the function formatted like command-line input.
                This includes file paths for the normalization/denormalization process and other options.
    :return: Returns 0 for successful execution, -1 for errors.

    """

    try:
        if not args:
            args = ""
        start_time = time.time()

        # Get parameters
        split_args = []
        if len(args) != 0:
            split_args = args.split()  # string command to list
        else:
            split_args= ["-h"]
        args = get_and_check_parameters(split_args)

        # Check parameters

        if args.mus is not None :
            if args.normalization_file is not None:
                raise ValueError("Error : mus are given but normalization_file is also given.")
            if args.sigmas is None:
                raise ValueError("Error : mus are given but sigmas are not given.")
            if not args.mus:
                raise ValueError("Error : parameter mus is an empty list")
        if args.sigmas is not None :
            if args.normalization_file is not None:
                raise ValueError("Error : sigmas are given but normalization_file is also given.")
            if args.mus is None:
                raise ValueError("Error : sigmas are given but mus are not given.")
            if not args.sigmas:
                raise ValueError("Error : parameter sigmas is an empty list")

        if args.normalization_indices is not None:
            if args.normalization_file is not None:
                raise ValueError("Error : normalization_indices are given but normalization_file is also given.")
            if not all(index < args.nb_attributes for index in args.normalization_indices):
                raise ValueError("Error : parameter normalization_indices has to be a list of positive integers smaller than the number of attributes.")
            if len(args.normalization_indices) != len(set(args.normalization_indices)):
                raise ValueError("Error : The same attribute is specified twice in normalization_indices.")
            if not args.normalization_indices:
                raise ValueError("Error : parameter normalization_indices is an empty list")
        else:
            if args.normalization_file is None:
                args.normalization_indices = list(range(args.nb_attributes))

        if args.mus is not None and args.sigmas is not None:
            if len(args.mus) != len(args.sigmas):
                raise ValueError("Error : mus and sigmas have not the same amount of values.")
            if len(args.mus) != len(args.normalization_indices):
                raise ValueError("Error : normalization_indices has not the same amount of values as mus and sigmas. Maybe you forgot to specify it.")

        if args.data_files is not None:
            if args.missing_values is None:
                raise ValueError("Error : parameter missing_values missing.")

            if args.output_data_files is None:
                args.output_data_files = []
                for data in args.data_files:
                    base, ext = os.path.splitext(data)
                    if ext:
                        args.output_data_files.append(f"{base}_normalized{ext}")
                    else:
                        args.output_data_files.append(f"{data}_normalized")
            if len(args.output_data_files) != len(args.data_files):
                raise ValueError("Error : the size of output_data_files is not equal to the size of data_files")

        if args.rule_files is not None:
            if args.normalization_file is None and args.mus is None:
                raise ValueError("Error : rule files specified but there is no normalization_file or mus, sigmas and normalization_indices")
            if args.output_rule_files is None:
                args.output_rule_files = []
                for rule_file in args.rule_files:
                    base, ext = os.path.splitext(rule_file)
                    if ext:
                        args.output_rule_files.append(f"{base}_denormalized{ext}")
                    else:
                        args.output_rule_files.append(f"{rule_file}_denormalized")
            if len(args.output_rule_files) != len(args.rule_files):
                raise ValueError("Error : the size of output_rule_files is not equal to the size of rule_files")

        if (args.rule_files is None and args.data_files is None):
            raise ValueError("Error : rule_files or data_files must be specified.")

        print_parameters(args)

        # Get attributes
        attributes = None
        has_classes = False
        if (args.attributes_file is not None):
            attributes = []
            attributes, classes = get_attribute_file(args.attributes_file, args.nb_attributes, args.nb_classes)
            if (len(classes) != 0):
                has_classes = True
        if args.normalization_file is not None:
            # Get normalization_indices, mean/median, std and with_median from normalization file
            args.normalization_indices, args.with_median, args.mus, args.sigmas = parse_normalization_file(args.normalization_file, args.nb_attributes, attributes)

        elif args.mus is None:
            #Normalize first file and get mean/median and std
            main_file = args.data_files.pop(0)
            output_main_file = args.output_data_files.pop(0)
            if args.nb_classes is not None:
                main_datas = get_data(main_file, args.nb_attributes, args.nb_classes, keep_string=True)[0]
            else:
                main_datas = get_data(main_file, args.nb_attributes, keep_string=True)[0]

            args.mus, args.sigmas = gaussian_normalization(main_datas, args.normalization_indices, output_main_file, args.fill_missing_values, args.output_normalization_file, attributes, args.missing_values, args.with_median)


        if args.data_files is not None:
            #Normalize each data file with mean/median and std obtained above
            if args.normalization_file is not None:
                args.output_normalization_file = None
            for i in range(len(args.data_files)):
                if args.nb_classes is not None:
                    data = get_data(args.data_files[i], args.nb_attributes, args.nb_classes, keep_string=True)[0]
                else:
                    data = get_data(args.data_files[i], args.nb_attributes, keep_string=True)[0]

                gaussian_normalization(data, args.normalization_indices, args.output_data_files[i], args.fill_missing_values, args.output_normalization_file, attributes, args.missing_values, args.with_median, args.mus, args.sigmas)


        if args.rule_files is not None:
            #Denormalize each rule file with mean/median and std obtained above

            #Patterns corresponding to rules, there are 6 possible patterns :
            float_pattern = r'-?\d+(\.\d+)?'
            int_pattern = r'\d+'
            if args.attributes_file is not None:
                attr_pattern = "|".join(map(re.escape, attributes))
                pattern_fidex_attributes = fr'(?P<first_part>.*?)(?P<antecedents>(({attr_pattern})[<>]=?{float_pattern} )+)(?P<last_part>-> class {int_pattern}.*)'
                pattern_dimlp_attributes = fr'(?P<first_part>.*?)(?P<antecedents>(\(({attr_pattern}) [<>] {float_pattern}\) )+)(?P<last_part>Class = {int_pattern}.*)'
                if has_classes:
                    class_pattern = "|".join(map(re.escape, classes))
                    pattern_fidex_attributes_and_classes = fr'(?P<first_part>.*?)(?P<antecedents>(({attr_pattern})[<>]=?{float_pattern} )+)(?P<last_part>-> ({class_pattern}).*)'
                    pattern_dimlp_attributes_and_classes = fr'(?P<first_part>.*?)(?P<antecedents>(\(({attr_pattern}) [<>] {float_pattern}\) )+)(?P<last_part>Class = ({class_pattern}).*)'

            pattern_fidex_id = fr'(?P<first_part>.*?)(?P<antecedents>(X{int_pattern}[<>]=?{float_pattern} )+)(?P<last_part>-> class {int_pattern}.*)'
            pattern_dimlp_id = fr'(?P<first_part>.*?)(?P<antecedents>(\(x{int_pattern} [<>] {float_pattern}\) )+)(?P<last_part>Class = {int_pattern}.*)'

            for r in range(len(args.rule_files)): # For each rule file
                rule_file = args.rule_files[r]
                output_rule_file = args.output_rule_files[r]

                possible_patterns = [pattern_fidex_id, pattern_dimlp_id]
                possible_dimlp_patters = [pattern_dimlp_id]
                possible_patterns_with_attribute_names = []
                if args.attributes_file is not None:
                    possible_patterns.append(pattern_fidex_attributes)
                    possible_patterns.append(pattern_dimlp_attributes)
                    possible_dimlp_patters.append(pattern_dimlp_attributes)
                    possible_patterns_with_attribute_names.append(pattern_fidex_attributes)
                    possible_patterns_with_attribute_names.append(pattern_dimlp_attributes)
                    if has_classes:
                        possible_patterns.append(pattern_fidex_attributes_and_classes)
                        possible_patterns.append(pattern_dimlp_attributes_and_classes)
                        possible_dimlp_patters.append(pattern_dimlp_attributes_and_classes)
                        possible_patterns_with_attribute_names.append(pattern_fidex_attributes_and_classes)
                        possible_patterns_with_attribute_names.append(pattern_dimlp_attributes_and_classes)

                # Check which pattern is in the file
                pattern = get_pattern_from_rule_file(rule_file, possible_patterns)

                dimlp_rules = pattern in possible_dimlp_patters
                with_attribute_names = pattern in possible_patterns_with_attribute_names

                # Build antecedent pattern
                if dimlp_rules:
                    if with_attribute_names:
                        dimlp_attr_pattern = fr'(\()(?P<attribute>{attr_pattern})'
                    else:
                        dimlp_attr_pattern = fr'(\(x)(?P<attribute>{int_pattern})'
                    antecedent_pattern = fr'{dimlp_attr_pattern}(?P<inequality> [<>] )(?P<value>{float_pattern})(?P<last_part>\) )'
                else:
                    if with_attribute_names:
                        fidex_attr_pattern = fr'()(?P<attribute>{attr_pattern})'
                    else:
                        fidex_attr_pattern = fr'(X)(?P<attribute>{int_pattern})'
                    antecedent_pattern = fr'{fidex_attr_pattern}(?P<inequality>[<>]=?)(?P<value>{float_pattern})(?P<last_part> )'

                # Denormalize each rules
                try:
                    with open(rule_file, "r") as file:
                        try:
                            with open(output_rule_file, "w") as output_file:
                                for line in file:
                                    #Denormalize rule
                                    new_line = denormalize_rule(line.rstrip("\n"), pattern, antecedent_pattern, dimlp_rules, with_attribute_names, args.normalization_indices, attributes, args.sigmas, args.mus) + "\n"
                                    output_file.write(new_line) # Write line in output file

                        except FileNotFoundError:
                                raise ValueError(f"Error : File {output_file} not found.")
                        except IOError:
                            raise ValueError(f"Error : Couldn't open file {output_file}.")
                except FileNotFoundError:
                        raise ValueError(f"Error : File {rule_file} not found.")
                except IOError:
                    raise ValueError(f"Error : Couldn't open file {rule_file}.")


        end_time = time.time()
        full_time = end_time - start_time
        full_time = "{:.6f}".format(full_time).rstrip("0").rstrip(".")

        print(f"\nFull execution time = {full_time} sec")

        return 0

    except ValueError as error:
        print(error)
        return -1


if __name__ == "__main__":
    """
    Entry point for the normalization script. Parses command-line arguments and initiates the normalization or denormalization process for the specified data or rule files.

    The script is designed to be run from the command line with various arguments that control the normalization and denormalization process,
    including file paths for data, rules, normalization statistics, and other parameters.

    Usage:
        python script.py [options]

    Example:
        python script.py --data_files [datanormTrain.txt,datanormTest.txt] --normalization_indices [0,2,4] \
                         --nb_attributes 16 --missing_values NaN --root_folder dimlp/datafiles

    :param sys.argv: List of command-line arguments passed to the script.
    :type sys.argv: list
    """
    cmdline_args = " ".join(sys.argv[1:])
    normalization(cmdline_args)
