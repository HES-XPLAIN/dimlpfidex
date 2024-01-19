import time
import sys
from sklearn import svm
from sklearn import metrics
import numpy as np
import matplotlib.pyplot as plt
import os
import math
import re

from trainings.trnFun import get_data, validate_string_param, check_int, check_strictly_positive, check_bool, get_attribute_file


def gaussian_normalization(datas, attribute_indices, normalized_file, fill_missing_values, normalization_stats=None, attributes=None, missing_value=None, with_median=False, mus=None, sigmas=None):

    """
    Perform Gaussian normalization on specified attributes of the given data.

    This function normalizes the data based on the Gaussian distribution, where each selected attribute is
    adjusted to have a mean (or median) of zero and a standard deviation of one. It handles missing values,
    writes the normalization statistics to a file if provided, and saves the normalized data to a specified file.

    :param datas: The dataset to be normalized.
    :type datas: list of lists
    :param attribute_indices: Indices of the attributes to be normalized.
    :type attribute_indices: list
    :param normalized_file: File path to save the normalized data.
    :type normalized_file: str
    :param fill_missing_values: Flag to fill missing values with the mean/median.
    :type fill_missing_values: bool
    :param normalization_stats: Optional file path to save the normalization statistics.
    :type normalization_stats: str, optional
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
    :raises ValueError: If mus, sigmas, and attribute_indices are not of the same length or other existing conditions.
    :return: Tuple containing the mean (or median) and standard deviation used for normalization.
    :rtype: (float, float)
    """

    if mus and len(mus) != len(attribute_indices):
            raise ValueError("Error: The length of 'mus' and 'attribute_indices' must be the same.")

    if sigmas and len(sigmas) != len(attribute_indices):
            raise ValueError("Error: The length of 'sigmas' and 'attribute_indices' must be the same.")

    has_mus = True
    if not mus:
        has_mus = False
        mus = []
    has_sigmas = True
    if not sigmas:
        has_sigmas = False
        sigmas = []

    # Erase everything in the normalization file in case it already exists
    if normalization_stats is not None:
        try:
            with open(normalization_stats, 'w') as myfile:
                pass
        except FileNotFoundError:
                raise ValueError(f"Error : File {normalization_stats} not found.")
        except IOError:
            raise ValueError(f"Error : Couldn't open file {normalization_stats}.")

    for idx, indice in enumerate(attribute_indices):
        name_id = str(attributes[indice]) if attributes else str(indice)
        attribut = []
        for data in datas:
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

            if normalization_stats is not None:
                try:
                    with open(normalization_stats, 'a') as myfile:

                        if with_median:
                            myfile.write(name_id + " : original median: " + str(current_mu) + ", original std: " + str(current_sigma) + '\n')
                        else:
                            myfile.write(name_id + " : original mean: " + str(current_mu) + ", original std: " + str(current_sigma) + '\n')
                except (FileNotFoundError):
                    raise ValueError(f"Error : File {normalization_stats} not found.")
                except (IOError):
                    raise ValueError(f"Error : Couldn't open file {normalization_stats}.")

            if fill_missing_values:
                for data in datas:
                    data[indice] = np.where(data[indice] == missing_value, str(current_mu), data[indice])
                    data[indice] = str(data[indice])
            for i in range(len(datas)):
                if datas[i][indice] != missing_value:
                    if current_sigma == 0:
                        datas[i][indice] = str(0)
                    else:
                        datas[i][indice] = str(round((float(datas[i][indice]) - current_mu) / current_sigma, 6))
        else:
            raise ValueError(f"Error : no value for attribute {name_id}. Impossible to normalize.")

    try:
        with open(normalized_file, 'w') as norm_file:
            for data in datas:
                norm_file.write(' '.join(data) + '\n')
    except IOError:
        raise ValueError(f"Error: Could not write to file {normalized_file}.")

    return mus, sigmas


def parse_normalization_stats(file_name, nb_attributes, attributes=None):
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
    :param nb_attributes: Number of attributes
    :param attributes: List of attribute names (optional)
    :raises ValueError: If the file is not found, cannot be opened, is not in the correct format,
                        or if there is an inconsistency in using mean or median across different lines.
    :return: Tuple of (indices_list, with_median, mean_median_values, std_values)
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


def get_and_check_string_data_from_list(datas, name):
    """
    This function checks whether the 'datas' parameter is either a list of strings or a single string.
    The function raises a ValueError if the conditions are not met.

    :param datas: The data to check. It can be either a list of file names (strings) or a single file name (string).
    :type datas: list or str
    :raises ValueError: If 'datas' is a list but contains non-string elements.
    :raises ValueError: If 'datas' is neither a list nor a string.
    :return: The original 'datas' if it's a list, or a list containing 'datas' if it's a single string.
    :rtype: list
    """
    if isinstance(datas, list):
        if not datas:
            raise ValueError(f"Error : parameter {name} is an empty list")
        for data in datas:
            if not isinstance(data, str):
                raise ValueError(f'Error: in parameter {name} : Each file name has to be contained in quotation marks "".')
        return datas
    elif not isinstance(datas, str):
        raise ValueError(f'Error: parameter {name} has to be a list or a name contained in quotation marks "".')
    else:
        return [datas]

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

def denormalize_rule(line, pattern, antecedant_pattern, dimlp_rule, with_attribute_names, attribute_indices, attributes, sigmas, mus):
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
    :param antecedant_pattern: The regular expression pattern for identifying each antecedent in the rule.
    :type antecedant_pattern: str
    :param dimlp_rule: A flag indicating whether the rule is a DIMLP rule or a Fidex rule, which affects attribute indexing and rule pattern.
    :type dimlp_rule: bool
    :param with_attribute_names: A flag indicating whether the rules use attribute names instead of numeric IDs.
    :type with_attribute_names: bool
    :param attribute_indices: A list of indices of the attributes to be denormalized.
    :type attribute_indices: list of int
    :param attributes: A list of attribute names or numeric IDs of the attributes.
    :type attributes: list of str or int
    :param sigmas: A list of standard deviation values for denormalization, corresponding to each attribute of attribute_indices.
    :type sigmas: list of float
    :param mus: A list of mean or median values for denormalization, corresponding to each attribute of attribute_indices.
    :type mus: list of float
    :return: The denormalized rule line.
    :rtype: str
    """
    match = re.search(pattern, line) # Get each rule
    if match:
        new_line = match.group("first_part")
        # Get each antecedant
        antecedants = match.group('antecedants')
        antecedants_matches = re.finditer(antecedant_pattern, antecedants)
        for antecedant_match in antecedants_matches:
            attribute = antecedant_match.group('attribute')
            attribute_id = attributes.index(attribute) if with_attribute_names else int(attribute)
            if dimlp_rule and not with_attribute_names:
                attribute_id -= 1 # indexes start at 1 in dimlp rules
            value = float(antecedant_match.group('value'))
            if attribute_id in attribute_indices:
                #Denormalize this antecedant
                idx = attribute_indices.index(attribute_id)
                value = value*sigmas[idx]+mus[idx]
            # Reconstuct rule
            new_line += f"{antecedant_match.group(1)}{attribute}{antecedant_match.group('inequality')}{value:.6f}{antecedant_match.group('last_part')}"
        new_line += match.group("last_part")
        return new_line
    else: # Rewrite the line
        return line

def normalization(*args, **kwargs):

    """
    This function serves two primary purposes: to normalize data files and to denormalize rule files.
    It offers flexibility in the normalization process through various options.

    Normalization can be performed in several ways:
    1. Using a 'normalization_stats' file containing normalization parameters along with one or more data files.
    2. Providing data files directly, where the first file is normalized to determine mean/median and standard deviation, which are then applied to other files.
    3. Supplying mean/median (mus) and standard deviations (sigmas) as lists, along with the data files.
    In the last two cases, indices of attributes to normalize must be provided, and a 'normalization_stats' file is generated for future use.

    Denormalization can also be done in multiple ways:
    1. Using a 'normalization_stats' file with one or more rule files.
    2. Directly providing mean/median (mus) and standard deviations (sigmas) along with the rule files. Attribute indices to be denormalized must be provided in this case.

    The function generates new normalized and/or denormalized files.

    Note:
    - Specify the number of attributes in the data and the symbol representing missing data.
    - Choose whether to replace missing data or not.
    - If normalizing training data, it is advisable to normalize test/validation files simultaneously for consistency.
    - If no keyword arguments are provided, the function displays the usage instructions, detailing the required and optional parameters.
      This guide assists users in correctly specifying arguments for the function.

    When to use :
    - It's good to normalize data before training with Dimlp and DimlpBT.
    - It's not necessary to normalize data before training with convKeras, MLP and SVM because a normalization is done during the process.
    - It's not necessary to normalize before training with GradientBoosting and RandomForests because decision trees don't need normalization.

    Formats:
    - normalization_stats file: Each line contains the mean/median and standard deviation for an attribute.
      Format: '2 : original mean: 0.8307, original std: 0.0425'
      Attribute indices (index 2 here) can be replaced with attribute names, then an attribute file is required.
    - Data files (datas): Should contain one sample per line, with numbers separated by spaces. Supported formats:
      1. Only attributes (floats).
      2. Attributes (floats) followed by an integer class ID.
      3. Attributes (floats) followed by one-hot encoded class.
    - Rule files (rule_files): Contain rules in Dimlp or Fidex format. Formats:
      Dimlp: 'Rule 1: (x2 > 0.785787) (x5 > 0.591247) (x8 < 0.443135) Class = 1 (187)'
      Fidex: 'X1>=0.414584 X10<0.507982 X5>=0.314835 X6>=0.356158 -> class 0'
      In both formats, attribute indices (e.g., X1, x2) and class identifiers can be replaced with attribute names and class names, respectively, then an attribute file is required.
    - Attribute file (attribute_file): Each line corresponds to an attribute's name, with optional class names at the end.

    Examples of how to call the function:
    - For data files: normalization(datas=["datanormTrain.txt", "datanormTest.txt"], attribute_indices=[0,2,4], nb_attributes=16, missing_values="NaN", save_folder="dimlp/datafiles")
    - For rule files: normalization(normalization_stats="normalization_stats.txt", rule_files="globalRulesDatanorm.txt", nb_attributes=16, save_folder="dimlp/datafiles")

    :param args: Non-keyword arguments are not used.
    :param kwargs: Keyword arguments specifying various options and file paths for the normalization/denormalization process.
    :return: Returns 0 for successful execution, -1 for errors.

    """

    try:

        if not kwargs:
            print("---------------------------------------------------------------------")
            print("Please specify arguments using named parameters.")
            print("Warning! The files are localised with respect to root folder dimlpfidex.")
            print("----------------------------")
            print("Obligatory parameters :")
            print("nb_attributes : number of attributes")
            print('missing_values : string representing a missing value in your data, put "NaN" (or any string not in your data) if you do not have any missing value (only when working on data files)')
            print("----------------------------")
            print("Optional parameters :")
            print("nb_classes : mendatory if classes are also present in data file or attribute file")
            print("save_folder : Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder.")
            print("normalization_stats : file containing the mean and std of some attributes. Used to normalize or denormalize if specified.")
            print("mus : list of float corresponding to mean or median of each attribute index of interest.")
            print("sigmas : list of float corresponding to standard deviation of each attribute index of interest.")
            print("datas : every data files to normalize contained in a list if there is more than one file. Normalize every file with respect to the first one if normalization_stats is not specified.")
            print("rule_files : every rule files to denormalize contained in a list if there is more than one file. Can only be denormalized if a normalization_stats file or mus, sigmas and attribute_indices are given.")
            print("Either 'datas' or 'rule_files' must be specified.")
            print("attribute_file : file containing attribute names (mendatory if rules or normalization stats are written with attribute names)")
            print("attribute_indices : indices of the attributes(columns) to normalize a data file, contained in a list (first column is index 0, all indices by default, only used when no normalization_stats is given).")
            print("output_normalization_stats : name of the output file containing the mean and std of some attributes when normalization_stats is not specified (normalization_stats.txt by default).")
            print("output_datas : name of the normalized files contained in a list if there is more than one file. Need to specify everyone of them if one is specified (<original_name>_normalized<original_extension> by default)")
            print("output_rules : name of the normalized rule files contained in a list if there is more than one file. Need to specify everyone of them if one is specified (<original_name>_denormalized<original_extension> by default)")
            print("with_median : boolean, whether we use median instead of mean to compute normalitzation (False by default, only when working on data files)")
            print("fill_missing_values : boolean, whether we fill missing values with mean or median (True by default, only when working on data files)")
            print("----------------------------")
            print("Here is an example with data files, keep same parameter names :")
            print('normalization(datas = ["datanormTrain.txt", "datanormTest.txt"], attribute_indices = [0,2,4], nb_attributes=16, missing_values="NaN", save_folder = "dimlp/datafiles")')
            print("Another example with rule files :")
            print('normalization(normalization_stats = "normalization_stats.txt", rule_files = "globalRulesDatanorm.txt", nb_attributes=16, save_folder = "dimlp/datafiles")')
            print("---------------------------------------------------------------------")
        else:

            start_time = time.time()

            # Get parameters
            nb_attributes = kwargs.get('nb_attributes')
            nb_classes = kwargs.get('nb_classes')
            missing_values = kwargs.get('missing_values')
            save_folder = kwargs.get('save_folder')
            normalization_stats = kwargs.get('normalization_stats')
            mus = kwargs.get('mus')
            sigmas = kwargs.get('sigmas')
            datas = kwargs.get('datas')
            rule_files = kwargs.get('rule_files')
            attribute_file = kwargs.get('attribute_file')
            attribute_indices = kwargs.get('attribute_indices')
            output_normalization_stats = kwargs.get('output_normalization_stats')
            output_datas = kwargs.get('output_datas')
            output_rules = kwargs.get('output_rules')
            with_median = kwargs.get('with_median')
            fill_missing_values = kwargs.get('fill_missing_values')

            # Check parameters

            valid_args = ['nb_attributes', 'nb_classes', 'missing_values', 'save_folder', 'normalization_stats', 'mus', 'sigmas', 'datas', 'rule_files', 'attribute_file', 'attribute_indices', 'output_normalization_stats',
                          'output_datas', 'output_rules', 'with_median', 'fill_missing_values']

            # Check if wrong parameters are given
            for arg_key in kwargs.keys():
                if arg_key not in valid_args:
                    raise ValueError(f"Invalid argument : {arg_key}.")

            if nb_attributes is None:
                raise ValueError('Error : number of attributes missing, add it with option nb_attributes="your_number_of_attributes".')
            elif not check_strictly_positive(nb_attributes) or not check_int(nb_attributes):
                raise ValueError('Error : parameter nb_attributes has to be a strictly positive integer.')

            if (nb_classes is not None and (not check_strictly_positive(nb_classes) or not check_int(nb_attributes))):
                raise ValueError('Error : parameter nb_classes has to be a strictly positive integer.')

            save_folder = validate_string_param(save_folder, "save_folder", allow_none=True)
            normalization_stats = validate_string_param(normalization_stats, "normalization_stats", allow_none=True)

            if mus is not None :
                if normalization_stats is not None:
                    raise ValueError("Error : mus are given but normalization_stats is also given.")
                if sigmas is None:
                    raise ValueError("Error : mus are given but sigmas are not given.")
                if not isinstance(mus, list) or not all(isinstance(mu, (float,int)) for mu in mus):
                    raise ValueError("Error : parameter mus has to be a list of float.")
                if not mus:
                    raise ValueError("Error : parameter mus is an empty list")
            if sigmas is not None :
                if normalization_stats is not None:
                    raise ValueError("Error : sigmas are given but normalization_stats is also given.")
                if mus is None:
                    raise ValueError("Error : sigmas are given but mus are not given.")
                if not isinstance(sigmas, list) or not all(isinstance(sigma, (float,int)) for sigma in sigmas):
                    raise ValueError("Error : parameter sigmas has to be a list of float.")
                if not sigmas:
                    raise ValueError("Error : parameter sigmas is an empty list")

            if datas:
                datas = get_and_check_string_data_from_list(datas, "datas")
            if rule_files:
                rule_files = get_and_check_string_data_from_list(rule_files, "rule_files")

            attribute_file = validate_string_param(attribute_file, "attribute_file", allow_none=True)

            if attribute_indices is not None:
                if normalization_stats is not None:
                    raise ValueError("Error : attribute_indices are given but normalization_stats is also given.")
                if not isinstance(attribute_indices, list) or not all(check_int(index) and 0 <= index < nb_attributes for index in attribute_indices):
                    raise ValueError("Error : parameter attribute_indices has to be a list of positive integers smaller than the number of attributes.")
                if len(attribute_indices) != len(set(attribute_indices)):
                    raise ValueError("Error : The same attribute is specified twice in attribute_indices.")
                if not attribute_indices:
                    raise ValueError("Error : parameter attribute_indices is an empty list")
            else:
                if normalization_stats is None:
                    attribute_indices = list(range(nb_attributes))

            if mus is not None and sigmas is not None:
                if len(mus) != len(sigmas):
                    raise ValueError("Error : mus and sigmas have not the same amount of values.")
                if len(mus) != len(attribute_indices):
                    raise ValueError("Error : attribute_indices has not the same amount of values as mus and sigmas. Maybe you forgot to specify it.")


            output_normalization_stats = validate_string_param(output_normalization_stats, "output_normalization_stats", default="normalization_stats.txt")

            if datas is not None:

                if missing_values is None:
                    raise ValueError("Error : parameter missing_values missing.")
                else:
                    missing_values = validate_string_param(missing_values, "missing_values")

                if fill_missing_values is None:
                    fill_missing_values = True
                elif not check_bool(fill_missing_values):
                        raise ValueError("Error : parameter fill_missing_values is not a boolean.")

                if with_median is None:
                    with_median = False
                elif not check_bool(with_median):
                        raise ValueError("Error : parameter with_median is not a boolean.")

                if output_datas is not None:
                    output_datas = get_and_check_string_data_from_list(output_datas, "output_datas")
                else:
                    output_datas = []
                    for data in datas:
                        base, ext = os.path.splitext(data)
                        if ext:
                            output_datas.append(f"{base}_normalized{ext}")
                        else:
                            output_datas.append(f"{data}_normalized")
                if len(output_datas) != len(datas):
                    raise ValueError("Error : the size of output_datas is not equal to the size of datas")

            if rule_files is not None:
                if normalization_stats is None and mus is None:
                    raise ValueError("Error : rule files specified but there is no normalization_stats file or mus, sigmas and attribute_indices")
                if output_rules is not None:
                    output_rules = get_and_check_string_data_from_list(output_rules, "output_rules")
                else:
                    output_rules = []
                    for rule_file in rule_files:
                        base, ext = os.path.splitext(rule_file)
                        if ext:
                            output_rules.append(f"{base}_denormalized{ext}")
                        else:
                            output_rules.append(f"{rule_file}_denormalized")
                if len(output_rules) != len(rule_files):
                    raise ValueError("Error : the size of output_rules is not equal to the size of rule_files")

            if (rule_files is None and datas is None):
                raise ValueError("Error : rule_files or datas must be specified.")


            if (save_folder):
                if normalization_stats is not None:
                    normalization_stats = save_folder + "/" + normalization_stats
                if output_normalization_stats is not None:
                    output_normalization_stats = save_folder + "/" + output_normalization_stats
                if datas is not None:
                    datas = [save_folder + "/" + data for data in datas]
                    output_datas = [save_folder + "/" + out_data for out_data in output_datas]
                if rule_files is not None:
                    rule_files = [save_folder + "/" + rule_file for rule_file in rule_files]
                    output_rules = [save_folder + "/" + out_rule_file for out_rule_file in output_rules]
                if attribute_file is not None:
                    attribute_file = save_folder + "/" + attribute_file


            # Get attributes
            attributes = None
            has_classes = False
            if (attribute_file is not None):
                attributes = []
                attributes, classes = get_attribute_file(attribute_file, nb_attributes, nb_classes)
                if (len(classes) != 0):
                    has_classes = True
            if normalization_stats is not None:
                # Get attribute_indices, mean/median, std and withMedian from normalization file
                attribute_indices, with_median, mus, sigmas = parse_normalization_stats(normalization_stats, nb_attributes, attributes)

            elif mus is None:
                #Normalize first file and get mean/median and std
                main_file = datas.pop(0)
                output_main_file = output_datas.pop(0)
                if nb_classes is not None:
                    main_datas = get_data(main_file, nb_attributes, nb_classes, keep_string=True)[0]
                else:
                    main_datas = get_data(main_file, nb_attributes, keep_string=True)[0]

                mus, sigmas = gaussian_normalization(main_datas, attribute_indices, output_main_file, fill_missing_values, output_normalization_stats, attributes, missing_values, with_median)


            if datas is not None:
                #Normalize each data file with mean/median and std obtained above
                if normalization_stats is not None:
                    output_normalization_stats = None
                for i in range(len(datas)):
                    if nb_classes is not None:
                        data = get_data(datas[i], nb_attributes, nb_classes, keep_string=True)[0]
                    else:
                        data = get_data(datas[i], nb_attributes, keep_string=True)[0]

                    gaussian_normalization(data, attribute_indices, output_datas[i], fill_missing_values, output_normalization_stats, attributes, missing_values, with_median, mus, sigmas)


            if rule_files is not None:
                #Denormalize each rule file with mean/median and std obtained above

                #Patterns corresponding to rules, there are 6 possible patterns :
                float_pattern = r'-?\d+(\.\d+)?'
                int_pattern = r'\d+'
                if attribute_file is not None:
                    attr_pattern = "|".join(map(re.escape, attributes))
                    pattern_fidex_attributes = fr'(?P<first_part>.*?)(?P<antecedants>(({attr_pattern})[<>]=?{float_pattern} )+)(?P<last_part>-> class {int_pattern}.*)'
                    pattern_dimlp_attributes = fr'(?P<first_part>.*?)(?P<antecedants>(\(({attr_pattern}) [<>] {float_pattern}\) )+)(?P<last_part>Class = {int_pattern}.*)'
                    if has_classes:
                        class_pattern = "|".join(map(re.escape, classes))
                        pattern_fidex_attributes_and_classes = fr'(?P<first_part>.*?)(?P<antecedants>(({attr_pattern})[<>]=?{float_pattern} )+)(?P<last_part>-> ({class_pattern}).*)'
                        pattern_dimlp_attributes_and_classes = fr'(?P<first_part>.*?)(?P<antecedants>(\(({attr_pattern}) [<>] {float_pattern}\) )+)(?P<last_part>Class = ({class_pattern}).*)'

                pattern_fidex_id = fr'(?P<first_part>.*?)(?P<antecedants>(X{int_pattern}[<>]=?{float_pattern} )+)(?P<last_part>-> class {int_pattern}.*)'
                pattern_dimlp_id = fr'(?P<first_part>.*?)(?P<antecedants>(\(x{int_pattern} [<>] {float_pattern}\) )+)(?P<last_part>Class = {int_pattern}.*)'

                for r in range(len(rule_files)): # For each rule file
                    rule_file = rule_files[r]
                    output_rule_file = output_rules[r]

                    possible_patterns = [pattern_fidex_id, pattern_dimlp_id]
                    possible_dimlp_patters = [pattern_dimlp_id]
                    possible_patterns_with_attribute_names = []
                    if attribute_file is not None:
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

                    # Build antecedant pattern
                    if dimlp_rules:
                        if with_attribute_names:
                            dimlp_attr_pattern = fr'(\()(?P<attribute>{attr_pattern})'
                        else:
                            dimlp_attr_pattern = fr'(\(x)(?P<attribute>{int_pattern})'
                        antecedant_pattern = fr'{dimlp_attr_pattern}(?P<inequality> [<>] )(?P<value>{float_pattern})(?P<last_part>\) )'
                    else:
                        if with_attribute_names:
                            fidex_attr_pattern = fr'()(?P<attribute>{attr_pattern})'
                        else:
                            fidex_attr_pattern = fr'(X)(?P<attribute>{int_pattern})'
                        antecedant_pattern = fr'{fidex_attr_pattern}(?P<inequality>[<>]=?)(?P<value>{float_pattern})(?P<last_part> )'

                    # Denormalize each rules
                    try:
                        with open(rule_file, "r") as file:
                            try:
                                with open(output_rule_file, "w") as output_file:
                                    for line in file:
                                        #Denormalize rule
                                        new_line = denormalize_rule(line.rstrip("\n"), pattern, antecedant_pattern, dimlp_rules, with_attribute_names, attribute_indices, attributes, sigmas, mus) + "\n"
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
