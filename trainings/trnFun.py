import numpy as np
import os
import sys
from .stairObj import StairObj
from sklearn.tree import _tree
import time
import matplotlib.pyplot as plt
from sklearn.metrics import roc_curve, auc, RocCurveDisplay

def check_strictly_positive(variable):
    """
    Checks whether the provided variable is a strictly positive number (either float or integer).

    :param variable: The variable to be checked.
    :type variable: any type
    :return: True if the variable is a strictly positive number, False otherwise.
    :rtype: bool
    """
    if isinstance(variable, (float,int)) and variable > 0:
        return True
    return False

def check_int(variable):
    """
    Checks whether the provided variable is an integer.

    :param variable: The variable to be checked.
    :type variable: any type
    :return: True if the variable is an integer, False otherwise.
    :rtype: bool
    """
    if isinstance(variable, int):
        return True
    return False

def check_positive(variable):
    """
    Checks whether the provided variable is a positive number (either float or integer).

    :param variable: The variable to be checked.
    :type variable: any type
    :return: True if the variable is a positive number, False otherwise.
    :rtype: bool
    """
    if isinstance(variable, (float,int)) and variable >= 0:
        return True
    return False

def check_bool(variable):
    """
    Checks whether the provided variable is a boolean.

    :param variable: The variable to be checked.
    :type variable: any type
    :return: True if the variable is a boolean, False otherwise.
    :rtype: bool
    """
    if isinstance(variable, bool):
        return True
    return False

def delete_file(file):
    """
    Delete file

    :param file: The name of the file to be deleted.
    :type file: str
    """
    try:
        os.remove(file)
    except FileNotFoundError:
        print(f"Error : File '{file}' not found.")
    except Exception:
        print(f"Error during delete of file {file}")


def get_data_pred(file_name, nb_classes):
    """
    Add predictions to the dataset using a prediction file.
    The prediction file should contain one line per data sample, each line consisting of a series of numerical values separated by spaces
    representing the prediction scores scores for each class.
    The number of values per line should match the specified number of classes.

    :param file_name: The path of the file to read data from.
    :type file_name: str
    :param nb_classes: The expected number of float values per line.
    :type nb_classes: int
    :return: Predictions, a list of lists, where each inner list contains float values from one line of the file.
    :rtype: list[list[float]]
    :raises ValueError: If the file is not found, cannot be opened, if a line does not contain 'nb_classes' floats,
                        or if any value in a line is not a float.
    """
    try:
        with open(file_name, "r") as my_file:
            data = []
            for line in my_file:
                line = line.strip()
                if line:
                    elements = line.split()

                    # Check if all elements are floats and the line has the correct number of predictions
                    if len(elements) != nb_classes:
                        raise ValueError(f"Error in {file_name}: Incorrect number of predictions per line.")

                    try:
                        di = [float(elt) for elt in elements]
                        data.append(di)
                    except ValueError:
                        raise ValueError(f"Error in {file_name}: Non-numeric value found.")
            return data
    except (FileNotFoundError):
        raise ValueError(f"Error : File {file_name} not found.")
    except (IOError):
        raise ValueError(f"Error : Couldn't open file {file_name}.")

def get_data(file_name, nb_attributes, nb_classes=0, keep_string=False):
    """
    Get data from file and separate it into attributes and classes(if there are some classes).

    The file should contain one sample per line. Each number in line is separated by a space. Each sample can be in one of the following formats:
    1. Attributes only: Each line contains each float attribute.
    2. Attributes with Class ID: Each line contains all the float attributes followed by an integer class ID.
    3. Attributes with One-Hot Class Encoding: Each line contains all the float attributes followed by a one-hot encoding of the class.
       The number of elements in this encoding should match the total number of classes, with exactly one '1' and the rest '0's.

    :param file_name: The name of the file to read data from.
    :type file_name: str
    :param nb_attributes: The number of attributes.
    :type nb_attributes: int
    :param nb_classes: The number of classes, defaults to 0.
    :type nb_classes: int, optional
    :param keep_string : Whether to keep data on string format (allows non numericals)
    :type keep_string : bool
    :raises ValueError: If the file is not found, cannot be opened, or if the data format is incorrect.
    :return: A tuple containing two lists of list of float, one for data (attributes) and one for classes.
            Each element in the data list is a list of floats representing the attributes of a sample.
            Each element in the classes list is an integer representing the class ID.
    :rtype: (list[list[float]], list[int])
    """
    try:
        with open(file_name, "r") as my_file:
            data = []
            classes = []
            expected_length = None
            for line in my_file:
                line = line.strip()
                if line:
                    elements = line.split()

                    # Check if all elements are numbers (int or float)
                    if not keep_string:
                        try:
                            elements = [float(elt) for elt in elements]
                        except ValueError:
                            raise ValueError(f"Error in {file_name}: Non-numeric value found.")

                    # Determine expected length based on the first line and check consistency
                    if expected_length is None:
                        if len(elements) in [nb_attributes, nb_attributes + 1, nb_attributes + nb_classes]:
                            expected_length = len(elements)
                        else:
                            raise ValueError(f"Error in {file_name}: Incorrect number of values per line.")

                    if len(elements) != expected_length:
                        raise ValueError(f"Error in {file_name}: Inconsistent number of values per line.")

                    # Determine the format based on the number of elements
                    if len(elements) == nb_attributes:
                        # Only attributes
                        data.append(elements)
                    elif len(elements) == nb_attributes + 1:
                        # Class ID format
                        try:
                            class_id = int(elements[-1])
                            if class_id < 0 or class_id >= nb_classes:
                                raise ValueError
                            data.append(elements[:nb_attributes])
                            classes.append(class_id)
                        except ValueError:
                            raise ValueError(f"Error in {file_name}: Invalid class ID, the number of classes is set to {nb_classes}.")
                    elif len(elements) == nb_attributes + nb_classes:
                        # One-hot format
                        try:
                            one_hot = [float(elt) for elt in elements[nb_attributes:]]
                            if one_hot.count(1.0) != 1 or one_hot.count(0.0) != nb_classes - 1:
                                raise ValueError
                            data.append(elements[:nb_attributes])
                            classes.append(one_hot.index(1.0))
                        except ValueError:
                            raise ValueError(f"Error in {file_name}: Invalid one-hot format.")
                    else:
                        raise ValueError(f"Error in {file_name}: Incorrect number of values per line.")

                    # Check if when there is classes, there is a class for every data
                    if classes and len(classes) != len(data):
                        raise ValueError(f"Error in {file_name}: Inconsistent data across lines.")

            return data, classes

    except FileNotFoundError:
        raise ValueError(f"Error: File {file_name} not found.")
    except IOError:
        raise ValueError(f"Error: Couldn't open file {file_name}.")

def get_data_class(file_name, nb_classes):
    """
    Get class data from file. The file can contain class data in two formats:

    1. Class ID format: Each line contains a single integer representing the class ID.
       The class ID must be a non-negative integer less than the number of classes.
    2. One-Hot Encoding format: Each line contains numbers separated by spaces, representing a one-hot encoding of the class.
       The number of elements in each line should match the total number of classes, with exactly one '1' and the rest '0's.
    Each number in line is separated by a space.

    :param file_name: The name of the file to read class data from.
    :type file_name: str
    :param nb_classes: The number of classes.
    :type nb_classes: int
    :raises ValueError: If the file is not found, cannot be opened, or if the data format is incorrect.
    :return: A list containing class data.
    :rtype: list[int]
    """
    try:
        with open(file_name, "r") as my_file:
            classes = []
            expected_elements = None

            for line in my_file:
                line = line.strip()
                if line:
                    elements = line.split()

                    # Determine the format based on the first line
                    if expected_elements is None:
                        expected_elements = len(elements)

                    # Check consistency of number of elements per line
                    if len(elements) != expected_elements:
                        raise ValueError(f"Error in {file_name}: Inconsistent number of elements per line.")

                    # Check format and convert to class labels
                    if expected_elements == 1:
                        # Single element per line (class ID)
                        try:
                            class_id = int(elements[0])
                            if class_id < 0 or class_id >= nb_classes:
                                raise ValueError
                            classes.append(class_id)
                        except ValueError:
                            raise ValueError(f"Error in {file_name}: Invalid class ID.")
                    else:
                        # One-hot format
                        if expected_elements != nb_classes:
                            raise ValueError(f"Error in {file_name}: Invalid one-hot format, the number of classes in line is not {nb_classes}.")
                        try:
                            one_hot = [float(elt) for elt in elements]
                            if one_hot.count(1.0) != 1 or one_hot.count(0.0) != expected_elements - 1:
                                raise ValueError
                            classes.append(one_hot.index(1.0))
                        except ValueError:
                            raise ValueError(f"Error in {file_name}: Invalid one-hot format.")

            return classes

    except FileNotFoundError:
        raise ValueError(f"Error: File {file_name} not found.")
    except IOError:
        raise ValueError(f"Error: Couldn't open file {file_name}.")

def get_attribute_file(attribute_file, nb_attributes, nb_classes=None):
    """
    Reads an attribute file and splits its content into two lists: 'attributes' and 'classes'.
    The first 'nb_attributes' non-empty lines are stored in 'attributes', and the
    remaining non-empty lines in 'classes'. Raises an error if the file does not
    contain at least 'nb_attributes' non-empty lines and if there is not exactly
    nb_attributes + nb_classes lines when nb_classes is specified.

    Format of attribute_file : one attribute per line followed eventually by one class per line

    :param attribute_file: Path to the file to be read
    :param nb_attributes: Number of non-empty lines to be included in 'attributes'
    :return: A tuple of two lists: (attributes, classes)
    """
    attributes = []
    classes = []
    has_classes = True
    if nb_classes is None:
        has_classes = False

    try:
        with open(attribute_file, "r") as attr_file:
            # Read all lines and filter out empty ones
            lines = [line.strip() for line in attr_file if line.strip()]
            for line in lines:
                if len(line.split()) > 1:
                    raise ValueError(f"Error in file {attribute_file} : an attribute has spaces inbetween.")

            # Check if there is the good amount of attributes and classes
            if (not has_classes and len(lines) < nb_attributes): # Number of classes not specified, if there is too many attributes, it's stored in classes
                raise ValueError(f"Error in file {attribute_file} : There is not enough attributes. Required at least : {nb_attributes}, found: {len(lines)}")
            if (has_classes and len(lines) != nb_attributes + nb_classes): # Number of classes specified, need to be precise
                raise ValueError(f"Error in file {attribute_file} : The number of classes and attributes don't correspond to the number of lines. Required : {nb_attributes + nb_classes}, found: {len(lines)}")

            # Splitting the lines into attributes and classes
            attributes = lines[:nb_attributes]
            classes = lines[nb_attributes:]

    except FileNotFoundError:
        raise ValueError(f"Error: File {attribute_file} not found.")
    except IOError:
        raise ValueError(f"Error: Couldn't open file {attribute_file}.")

    return attributes, classes

def output_pred(pred, pred_file, nb_classes):
    """
    Save the predictions in one-hot encoded format to a specified file.

    This function takes a list of predicted class indices and converts them into one-hot encoded vectors,
    where each vector has a length equal to the number of classes (`nb_classes`). Each vector is a row in the output file,
    with '1' in the position of the predicted class and '0's elsewhere. The output is saved to a file specified by `pred_file`.

    :param pred: A list of predicted class indices.
    :type pred: list[int]
    :param pred_file: The path of the file where the one-hot encoded predictions will be saved.
    :type pred_file: str
    :param nb_classes: The total number of classes, determining the length of the one-hot encoded vectors.
    :type nb_classes: int
    :raises ValueError: If the specified file cannot be found or opened.
    """
    try:
        pred_vector = np.zeros((len(pred), nb_classes), dtype=int)

        for idx, cl in enumerate(pred):
            pred_vector[idx, cl] = 1 # 1 in the predicted class

        np.savetxt(pred_file, pred_vector, fmt="%d", delimiter=" ")
    except (FileNotFoundError):
        raise ValueError(f"Error : File {pred_file} not found.")
    except (IOError):
        raise ValueError(f"Error : Couldn't open file {pred_file}.")

def output_data(data, data_file):
    """
    Write the provided data to a specified file.

    This function takes a list of lists (`data`) where each inner list represents a series of values,
    and writes these values to a file specified by `data_file`. Each inner list is written to a new line
    in the file with values separated by spaces.

    :param data: A list of lists, where each inner list contains values to be written to the file.
    :type data: list[list[float]] or list[list[int]]
    :param data_file: The path of the file where the data will be saved.
    :type data_file: str
    :raises ValueError: If the specified file cannot be found or opened.
    """
    try:
        with open(data_file, "w") as file:
            for var in data:
                for val in var:
                    file.write(str(val) + " ")
                file.write("\n")
            file.close()

    except (FileNotFoundError):
        raise ValueError(f"Error : File {data_file} not found.")
    except (IOError):
        raise ValueError(f"Error : Couldn't open file {data_file}.")

def compute_first_hidden_layer(step, input_data, k, nb_stairs, hiknot, weights_outfile=None, mu=None, sigma=None):
    """
    Compute the output of the first hidden layer in a neural network model, apply a staircase activation function, and optionally save weights.

    This function normalizes the input data and applies a linear transformation based on provided or calculated
    mean (mu) and standard deviation (sigma). If in 'train' mode, it calculates and optionally saves the weights
    and biases. The transformed data is then passed through a staircase activation function.

    :param step: 'train' for training step, otherwise for testing step.
    :type step: str
    :param input_data: Input data to be processed.
    :type input_data: list[list[float]] or np.ndarray
    :param k: Scaling factor for normalization.
    :type k: float
    :param nb_stairs: Number of stairs in the staircase activation function.
    :type nb_stairs: int
    :param hiknot: Knots for the staircase activation function.
    :type hiknot: list[float]
    :param weights_outfile: File path to save weights, defaults to None, mendatory for training step.
    :type weights_outfile: str, optional
    :param mu: Mean for normalization, calculated if None. Defaults to None.
    :type mu: np.ndarray, optional
    :param sigma: Standard deviation for normalization, calculated if None. Defaults to None.
    :type sigma: np.ndarray, optional
    :return: Transformed data, and mu and sigma if in 'train' mode.
    :rtype: tuple (np.ndarray, np.ndarray, np.ndarray) or np.ndarray
    :raises ValueError: If file operations fail.
    """
    input_data = np.array(input_data)
    if step == "train": # Train datas
        if weights_outfile is None:
            raise ValueError("Error : weights file is None during computation of first hidden layer with train data.")
        mu = np.mean(input_data, axis=0) if mu is None else mu  # mean over variables
        sigma = np.std(input_data, axis=0) if sigma is None else sigma # std over variables
        sigma[sigma == 0] = 0.001  # Prevent division by zero

        weights = k/sigma
        biais = -k*mu/sigma

        # Save weights and bias
        try:
            with open(weights_outfile, "w") as my_file:
                for b in biais:
                    my_file.write(str(b))
                    my_file.write(" ")
                my_file.write("\n")
                for w in weights:
                    my_file.write(str(w))
                    my_file.write(" ")
                my_file.close()
        except (FileNotFoundError):
            raise ValueError(f"Error : File {weights_outfile} not found.")
        except (IOError):
            raise ValueError(f"Error : Couldn't open file {weights_outfile}.")

    # Compute new data after first hidden layer
    h = k*(input_data-mu)/sigma # With indices : hij=K*(xij-muj)/sigmaj
    stair = StairObj(nb_stairs, hiknot)
    output_data = np.vectorize(stair.funct)(h) # Apply staircase activation function

    return (output_data, mu, sigma) if step == "train" else output_data

def output_stats(stats_file, acc_train, acc_test):
    """
    Write training and testing accuracy statistics to a specified file.

    This function takes the training and testing accuracy values and writes them to the file specified by `stats_file`.
    The accuracies are written in percentage format.

    :param stats_file: The path of the file where the accuracy statistics will be saved.
    :type stats_file: str
    :param acc_train: The training accuracy percentage.
    :type acc_train: float
    :param acc_test: The testing accuracy percentage.
    :type acc_test: float
    :raises ValueError: If the specified file cannot be found or opened.
    """
    if stats_file is not None:
        try:
            with open(stats_file, "w") as my_file:
                my_file.write("Training accuracy : {}%.\n".format(acc_train))
                my_file.write("Testing accuracy : {}%.".format(acc_test))
                my_file.close()
        except (FileNotFoundError):
            raise ValueError(f"Error : File {stats_file} not found.")
        except (IOError):
            raise ValueError(f"Error : Couldn't open file {stats_file}.")

def validate_string_param(param, param_name, default=None, allow_none=False):
    """
    Validate a string parameter. Raises a ValueError if the parameter is invalid.

    :param param: The parameter to validate.
    :param param_name: The name of the parameter (for error messages).
    :param default: The default value to return if the parameter is None.
    :param allow_none: Set to True to allow None as a valid value.
    :return: The validated parameter, default value, or None.
    :raises ValueError: If the parameter is invalid.
    """
    if param is None:
        if allow_none:
            return None
        elif default is not None:
            return default
        else:
            raise ValueError(f'Error: {param_name} missing, add it with option {param_name}="your_{param_name}_file".')
    elif not isinstance(param, str):
        raise ValueError(f'Error: parameter {param_name} has to be a name contained in quotation marks "".')
    return param

def check_parameters_common(root_folder, train_data_file, test_data_file, train_pred_outfile, test_pred_outfile, stats_file, nb_attributes, nb_classes):
    """
    Validate the common parameters used in a machine learning process.

    This function checks if the provided file names and numerical parameters are valid. It raises a ValueError
    with an appropriate message if any parameter is invalid. The function also sets default values for prediction
    and statistics file names if they are not provided.

    :param save_folder: The folder path for saving files.
    :type save_folder: str, optional
    :param train_data_file: The file name for training data.
    :type train_data_file: str
    :param train_class_file: The file name for training class labels.
    :type train_class_file: str
    :param test_data_file: The file name for testing data.
    :type test_data_file: str
    :param test_class_file: The file name for testing class labels.
    :type test_class_file: str
    :param train_pred_outfile: The file name for saving training predictions, defaults to "predTrain.out".
    :type train_pred_outfile: str, optional
    :param test_pred_outfile: The file name for saving testing predictions, defaults to "predTest.out".
    :type test_pred_outfile: str, optional
    :param stats_file: The file name for saving statistics, defaults to "stats.txt".
    :type stats_file: str, optional
    :param nb_attributes: The number of attributes.
    :type nb_attributes: int
    :param nb_classes: The number of classes.
    :type nb_classes: int
    :return: A tuple of validated and possibly modified parameters.
    :rtype: tuple
    :raises ValueError: If any parameter is invalid.
    """
    root_folder = validate_string_param(root_folder, "root_folder", allow_none=True)
    train_data_file = validate_string_param(train_data_file, "train_data_file")
    test_data_file = validate_string_param(test_data_file, "test_data_file")
    train_pred_outfile = validate_string_param(train_pred_outfile, "train_pred_outfile", default="predTrain.out")
    test_pred_outfile = validate_string_param(test_pred_outfile, "test_pred_outfile", default="predTest.out")
    stats_file = validate_string_param(stats_file, "stats_file", default="stats.txt")

    # Check nb_attributes and nb_classes

    if nb_attributes is None:
        raise ValueError('Error : number of attributes missing, add it with option nb_attributes="your_number_of_attributes".')
    elif not check_strictly_positive(nb_attributes) or not check_int(nb_attributes):
        raise ValueError('Error : parameter nb_attributes has to be a strictly positive integer.')

    if nb_classes is None:
        raise ValueError('Error : number of classes missing, add it with option nb_classes="your_number_of_classes".')
    elif not check_strictly_positive(nb_classes) or not check_int(nb_classes):
        raise ValueError('Error : parameter nb_classes has to be a strictly positive integer.')

    return root_folder, train_data_file, test_data_file, train_pred_outfile, test_pred_outfile, stats_file, nb_attributes, nb_classes

def check_parameters_dimlp_layer(weights_outfile, k, quant):
    """
    Validate and set default values for the parameters of a DIMLP layer.

    This function checks if the provided parameters for the DIMLP layer are valid and sets default values
    if they are not provided. It specifically checks the weights file name, scaling factor 'k',
    and number of stairs in staircase activation function ('quant').

    :param weights_outfile: The file name for saving weights. Defaults to "weights.wts" if not provided.
    :type weights_outfile: str, optional
    :param k: The scaling factor for the layer. Defaults to 1 if not provided.
    :type k: float, optional
    :param quant: The number of stairs in staircase activation function. Defaults to 50 if not provided.
    :type quant: int, optional
    :return: A tuple of the validated and possibly modified parameters weights_outfile, k, and quant.
    :rtype: tuple
    :raises ValueError: If any provided parameter is invalid.
    """
    weights_outfile = validate_string_param(weights_outfile, "weights_outfile", default="weights.wts")

    if k is None:
        k = 1
    elif not check_strictly_positive(k):
        raise ValueError('Error, parameter K is not a strictly positive number.')

    if quant is None:
        quant = 50
    elif not check_strictly_positive(quant):
        raise ValueError('Error, parameter nb_quant_levels is not a strictly positive number.')

    return weights_outfile, k, quant

def check_parameters_decision_trees(n_estimators, min_samples_split, min_samples_leaf, min_weight_fraction_leaf, min_impurity_decrease, seed, max_features, verbose, max_leaf_nodes, warm_start, ccp_alpha):
    """
    Validate and set default values for the parameters used in decision tree models.

    :param n_estimators: The number of trees in the forest, defaults to 100.
    :type n_estimators: int, optional
    :param min_samples_split: The minimum number of samples required to split an internal node, defaults to 2.
    :type min_samples_split: int or float, optional
    :param min_samples_leaf: The minimum number of samples required to be at a leaf node, defaults to 1.
    :type min_samples_leaf: int or float, optional
    :param min_weight_fraction_leaf: The minimum weighted fraction of the sum total of weights required to be at a leaf node, defaults to 0.0.
    :type min_weight_fraction_leaf: float, optional
    :param min_impurity_decrease: A node will be split if this split induces a decrease of the impurity greater than or equal to this value, defaults to 0.0.
    :type min_impurity_decrease: float, optional
    :param seed: Controls the randomness of the estimator, can be None.
    :type seed: int, optional
    :param max_features: The number of features to consider when looking for the best split, can be int, float, "sqrt", "log2", or None, defaults to "sqrt".
    :type max_features: int, float, str or None, optional
    :param verbose: Controls the verbosity when fitting and predicting, defaults to 0.
    :type verbose: int or bool, optional
    :param max_leaf_nodes: Grow trees with max_leaf_nodes in best-first fashion, can be None.
    :type max_leaf_nodes: int, optional
    :param warm_start: When set to True, reuse the solution of the previous call to fit and add more estimators to the ensemble, defaults to False.
    :type warm_start: bool, optional
    :param ccp_alpha: Complexity parameter used for Minimal Cost-Complexity Pruning, defaults to 0.0.
    :type ccp_alpha: float, optional
    :return: A tuple of validated and possibly modified parameters.
    :rtype: tuple
    :raises ValueError: If any parameter is invalid.
    """
    if n_estimators is None:
        n_estimators = 100
    elif (not check_strictly_positive(n_estimators) or not check_int(n_estimators)):
        raise ValueError('Error, parameter n_estimators is not a strictly positive integer.')

    if min_samples_split is None:
        min_samples_split = 2
    elif ((check_int(min_samples_split) and  min_samples_split < 2) or (isinstance(min_samples_split, float) and ((min_samples_split <= 0) or min_samples_split > 1.0)) or not check_positive(min_samples_split)):
        raise ValueError('Error, parameter min_samples_split is not an integer bigger than 1 or a float in ]0,1.0]. For 1, put 1.0.')

    if min_samples_leaf is None:
        min_samples_leaf = 1
    elif ((isinstance(min_samples_leaf, float) and ((min_samples_leaf <= 0) or min_samples_leaf >= 1)) or not check_strictly_positive(min_samples_leaf)):
        raise ValueError('Error, parameter min_samples_leaf is not a strictly positive integer or a float in ]0,1[.')

    if min_weight_fraction_leaf is None:
        min_weight_fraction_leaf = 0.0
    elif not check_positive(min_weight_fraction_leaf) or min_weight_fraction_leaf > 0.5:
        raise ValueError('Error, parameter min_weight_fraction_leaf is not a float in [0,0.5].')

    if min_impurity_decrease is None:
        min_impurity_decrease = 0.0
    elif not check_positive(min_impurity_decrease):
        raise ValueError('Error, parameter min_impurity_decrease is not a positive float.')

    if seed is not None and (not check_int(seed) or not check_positive(seed)):
        raise ValueError('Error, parameter seed is not a positive integer.')

    if max_features is None:
        max_features = "sqrt"
    elif max_features == "None":
        max_features = None
    elif max_features not in {"sqrt", "log2", "None"} and not check_strictly_positive(max_features) or (isinstance(max_features, float) and max_features > 1):
        raise ValueError('Error, parameter max_features is not "sqrt", "log2", "None", a float in ]0,1] or a strictly positive integer.')

    if verbose is None:
        verbose = 0
    elif not ((check_int(verbose) and check_positive(verbose)) or check_bool(verbose)):
        raise ValueError('Error, parameter verbose is not a positive integer or a boolean.')

    if max_leaf_nodes is not None and not check_int(max_leaf_nodes) or (check_int(max_leaf_nodes) and max_leaf_nodes < 2):
            raise ValueError('Error, parameter max_leaf_nodes is not an integer bigger than 1.')

    if warm_start is None:
        warm_start = False
    elif not check_bool(warm_start):
        raise ValueError('Error, parameter warm_start is not boolean.')

    if ccp_alpha is None:
        ccp_alpha = 0.0
    elif not check_positive(ccp_alpha):
        raise ValueError('Error, parameter ccp_alpha is not a positive float.')

    return n_estimators, min_samples_split, min_samples_leaf, min_weight_fraction_leaf, min_impurity_decrease, seed, max_features, verbose, max_leaf_nodes, warm_start, ccp_alpha

def recurse(tree, node, parent_path, feature_names, output_rules_file, k_dict, from_grad_boost): # parent_path : path taken until current node
    """
    Recursively traverse the decision tree to generate rules.

    This function walks through the decision tree from a given node down to its leaves. For each node,
    it constructs a rule as a string and writes the rules for leaf nodes to an output file.

    :param tree: The decision tree object.
    :type tree: _tree.Tree
    :param node: The current node index in the tree to start from.
    :type node: int
    :param parent_path: The rule string constructed up to the current node.
    :type parent_path: str
    :param feature_names: List of feature names used in the tree.
    :type feature_names: list[str]
    :param output_rules_file: Opened file object to write the rules.
    :param k_dict: Dictionary holding the count of rules processed.
    :type k_dict: dict
    :param from_grad_boost: Boolean indicating if the tree is from gradient boosting.
    :type from_grad_boost: bool
    """
    if tree.feature[node] != _tree.TREE_UNDEFINED: # Check if this is a real node
        feature_name = feature_names[node] # Get node's feature name
        threshold = tree.threshold[node] # Get node threshold
        if node == 0:
            left_path = f"{feature_name}<={threshold}"
        else:
            left_path = f"{parent_path} {feature_name}<={threshold}"
        recurse(tree, tree.children_left[node], left_path, feature_names, output_rules_file, k_dict, from_grad_boost) # Check left child node

        if node == 0:
            right_path = f"{feature_name}>{threshold}"
        else:
            right_path = f"{parent_path} {feature_name}>{threshold}"
        recurse(tree, tree.children_right[node], right_path, feature_names, output_rules_file, k_dict, from_grad_boost) # Check right child node
    else: # If this is a leaf
        k = k_dict["value"]
        k += 1
        k_dict["value"] = k
        if from_grad_boost:
            output_rules_file.write(f"Rule {k}: {parent_path} -> value {tree.value[node][0]}\n") # Write rule
        else:
            cover_value = tree.value[node] # Get cover values for this rule
            output_rules_file.write(f"Rule {k}: {parent_path} -> class {np.argmax(cover_value)} Covering: {[int(num) for num in cover_value[0]]}\n") # Write rule

def trees_to_rules(trees, rules_file, from_grad_boost=False):
    """
    Convert a list of decision trees into human-readable rules and save them to a file.

    This function takes a list of decision tree objects and converts each tree into a set of rules.
    These rules are then written to a specified output file.

    :param trees: List of decision tree objects.
    :type trees: list[DecisionTreeClassifier or DecisionTreeRegressor]
    :param rules_file: File path where the rules will be saved.
    :type rules_file: str
    :param from_grad_boost: Boolean indicating if the trees are from gradient boosting.
    :type from_grad_boost: bool
    :raises ValueError: If the specified file cannot be found or opened.
    """
    try:
        with open(rules_file, "w") as output_rules_file:
            for t in range(len(trees)):
                output_rules_file.write(f"-------------------\nTree {t + 1}\n-------------------\n")
                tree_ = trees[t].tree_
                feature_names = [
                    "X"+str(i) if i != _tree.TREE_UNDEFINED else "undefined!"
                    for i in tree_.feature
                ]
                k_dict = {"value": 0}
                recurse(tree_, 0, "", feature_names, output_rules_file, k_dict, from_grad_boost)
            output_rules_file.close()
    except FileNotFoundError:
        raise ValueError(f"Error: File for rules extraction ({rules_file}) not found.")
    except IOError:
        raise ValueError(f"Error: Couldn't open rules extraction file {rules_file}.")

def compute_roc(estimator, output_roc, *args): #test_class, test_pred
    """
    Compute and save the ROC curve for a given estimator, test and prediction data.

    This function calculates the false positive rate (fpr) and true positive rate (tpr) for the provided test classes
    and predictions. It then generates a ROC curve plot with the computed AUC (Area Under Curve) score and saves it
    to the specified file.

    :param estimator: The name of the estimator used for generating the predictions.
    :type estimator: str
    :param output_roc: The file path where the ROC curve plot will be saved.
    :type output_roc: str
    :param args: A variable-length argument list containing the test classes and test predictions.
                 args[0] should be the test classes, and args[1] should be the test predictions.
    :type args: tuple
    :return: A list containing the false positive rate, true positive rate, and AUC score.
    :rtype: list
    """
    if len(args) < 2:
        raise ValueError("Error during ROC computation : insufficient arguments, test_class and test_pred required.")

    test_class, test_pred = args[0], args[1]
    fpr, tpr, _ = roc_curve(test_class, test_pred)
    auc_score = auc(fpr, tpr)

    try:
        viz = RocCurveDisplay(fpr=fpr, tpr=tpr, roc_auc=auc_score, estimator_name=estimator)
        viz.plot(color="darkorange", plot_chance_level=True)
        viz.figure_.savefig(output_roc)
        plt.close(viz.figure_)
    except Exception as e:
        raise RuntimeError(f"Error in generating ROC plot: {e}")

    return [fpr, tpr, auc_score]
