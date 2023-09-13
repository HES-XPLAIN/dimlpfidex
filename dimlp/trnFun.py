import numpy as np
import os
import sys
from .stairObj import StairObj
from sklearn.tree import _tree
import time

def check_strictly_positive(variable):
    if isinstance(variable, (float,int)) and variable > 0:
        return True
    return False

def check_int(variable):
    if isinstance(variable, int):
        return True
    return False

def check_positive(variable):
    if isinstance(variable, (float,int)) and variable >= 0:
        return True
    return False

def check_bool(variable):
    if isinstance(variable, bool):
        return True
    return False

def get_data(file_name): # Get data from file
    try:
        with open(file_name, "r") as my_file:
            data = []
            line = my_file.readline()
            while line:
                line = line.strip()  # Remove the line break at the end of the line
                di = [float(elt) for elt in line.split(" ")]
                data.append(di)
                line = my_file.readline()
            my_file.close()
        return data
    except (FileNotFoundError):
        raise ValueError(f"Error : File {file_name} not found.")
    except (IOError):
        raise ValueError(f"Error : Couldn't open file {file_name}.")

def output_pred(pred, pred_file, nb_classes):
    try:
        pred_vector = np.zeros((len(pred), nb_classes), dtype=int)

        for idx, cl in enumerate(pred):
            pred_vector[idx, cl] = 1 # 1 in the predicted class

        # Écrire le tableau dans un fichier avec des espaces entre les nombres
        np.savetxt(pred_file, pred_vector, fmt="%d", delimiter=" ")
    except (FileNotFoundError):
        raise ValueError(f"Error : File {pred_file} not found.")
    except (IOError):
        raise ValueError(f"Error : Couldn't open file {pred_file}.")

def output_pred_proba(pred, pred_file):
    try:
        with open(pred_file, "a") as predFile:
            for var in pred:
                for val in var:
                    predFile.write(str(val) + " ")
                predFile.write("\n")
            predFile.close()

    except (FileNotFoundError):
        raise ValueError(f"Error : File {pred_file} not found.")
    except (IOError):
        raise ValueError(f"Error : Couldn't open file {pred_file}.")

def compute_first_hidden_layer(step, input_data, k, nb_stairs, hiknot, weights_file=None, mu=None, sigma=None):
    input_data = np.array(input_data)
    if step == "train": # Train datas
        mu = np.mean(input_data, axis=0) # mean over variables
        sigma = np.std(input_data, axis=0)
        sigma[sigma == 0] = 0.001
        weights = k/sigma
        biais = -k*mu/sigma

        # Output weights and biais
        try:
            with open(weights_file, "w") as my_file:
                for b in biais:
                    my_file.write(str(b))
                    my_file.write(" ")
                my_file.write("\n")
                for w in weights:
                    my_file.write(str(w))
                    my_file.write(" ")
                my_file.close()
        except (FileNotFoundError):
            raise ValueError(f"Error : File {weights_file} not found.")
        except (IOError):
            raise ValueError(f"Error : Couldn't open file {weights_file}.")
    # Compute new data after first hidden layer
    h = k*(input_data-mu)/sigma # With indices : hij=K*(xij-muj)/sigmaj
    stair = StairObj(nb_stairs, hiknot)
    output_data = np.vectorize(stair.funct)(h)

    if step == "train": # Train data
        return output_data, mu, sigma
    else: # Test data
        return output_data

def output_stats(stats_file, acc_train, acc_test):
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


def check_parameters_common(save_folder, train_data_file, train_class_file, test_data_file, test_class_file, train_pred_file, test_pred_file, stats_file):
    if (save_folder is not None and (not isinstance(save_folder, str))):
        raise ValueError('Error : parameter save_folder has to be a name contained in quotation marks "".')

    if train_data_file is None :
        raise ValueError('Error : train data file missing, add it with option train_data="your_train_data_file"')
    elif not isinstance(train_data_file, str):
        raise ValueError('Error : parameter train_data has to be a name contained in quotation marks "".')

    if train_class_file is None :
        raise ValueError('Error : train class file missing, add it with option train_class="your_train_class_file"')
    elif not isinstance(train_class_file, str):
        raise ValueError('Error : parameter train_class has to be a name contained in quotation marks "".')

    if test_data_file is None :
        raise ValueError('Error : test data file missing, add it with option test_data="your_test_data_file"')
    elif not isinstance(test_data_file, str):
        raise ValueError('Error : parameter test_data has to be a name contained in quotation marks "".')

    if test_class_file is None :
        raise ValueError('Error : test class file missing, add it with option test_class="your_test_class_file"')
    elif not isinstance(test_class_file, str):
        raise ValueError('Error : parameter test_class has to be a name contained in quotation marks "".')

    if train_pred_file is None:
        train_pred_file = "predTrain"
    elif not isinstance(train_pred_file, str):
        raise ValueError('Error : parameter predTrain has to be a name contained in quotation marks "".')
    train_pred_file += ".out"

    if test_pred_file is None:
        test_pred_file = "predTest"
    elif not isinstance(test_pred_file, str):
        raise ValueError('Error : parameter predTestn has to be a name contained in quotation marks "".')
    test_pred_file += ".out"

    if stats_file is None:
        stats_file = "stats.txt"
    elif stats_file is not None and not isinstance(stats_file, str):
        raise ValueError('Error : parameter stats_file has to be a name contained in quotation marks "".')

    return save_folder, train_data_file, train_class_file, test_data_file, test_class_file, train_pred_file, test_pred_file, stats_file

def check_parameters_dimlp_layer(weights_file, k, quant, hiknot):

    if weights_file is None:
        weights_file = "weights"
    elif not isinstance(weights_file, str):
            raise ValueError('Error : parameter weights has to be a name contained in quotation marks "".')
    weights_file += ".wts"

    if k is None:
        k = 1
    elif not check_strictly_positive(k):
        raise ValueError('Error, parameter K is not a strictly positive number')

    if quant is None:
        quant = 50
    elif not check_strictly_positive(quant):
        raise ValueError('Error, parameter quant is not a strictly positive number')

    if hiknot is None:
        hiknot = 5
    elif not check_int(hiknot):
        raise ValueError('Error, parameter hiknot is not a number')

    return weights_file, k, quant, hiknot

def check_parameters_decision_trees(n_estimators, min_samples_split, min_samples_leaf, min_weight_fraction_leaf, min_impurity_decrease, random_state, max_features, verbose, max_leaf_nodes, warm_start, ccp_alpha):

    if n_estimators is None:
        n_estimators = 100
    elif (not check_strictly_positive(n_estimators) or not check_int(n_estimators)):
        raise ValueError('Error, parameter n_estimators is not a strictly positive integer')

    if min_samples_split is None:
        min_samples_split = 2
    elif ((check_int(min_samples_split) and  min_samples_split < 2) or (isinstance(min_samples_split, float) and ((min_samples_split <= 0) or min_samples_split > 1.0)) or not check_positive(min_samples_split)):
        raise ValueError('Error, parameter min_samples_split is not an integer bigger than 1 or a float in ]0,1.0]. For 1, put 1.0.')

    if min_samples_leaf is None:
        min_samples_leaf = 1
    elif ((isinstance(min_samples_leaf, float) and ((min_samples_leaf <= 0) or min_samples_leaf >= 1)) or not check_strictly_positive(min_samples_leaf)):
        raise ValueError('Error, parameter min_samples_leaf is not a strictly positive integer or a float in ]0,1[')

    if min_weight_fraction_leaf is None:
        min_weight_fraction_leaf = 0.0
    elif not check_positive(min_weight_fraction_leaf) or min_weight_fraction_leaf > 0.5:
        raise ValueError('Error, parameter min_weight_fraction_leaf is not a float in [0,0.5]')

    if min_impurity_decrease is None:
        min_impurity_decrease = 0.0
    elif not check_positive(min_impurity_decrease):
        raise ValueError('Error, parameter min_impurity_decrease is not a positive float')

    if random_state is not None and (not check_int(random_state) or not check_positive(random_state)):
        raise ValueError('Error, parameter random_state is not a positive integer')

    if max_features is None:
        max_features = "sqrt"
    elif max_features == "None":
        max_features = None
    elif max_features not in {"sqrt", "log2", "None"} and not check_strictly_positive(max_features) or (isinstance(max_features, float) and max_features > 1):
        raise ValueError('Error, parameter max_features is not "sqrt", "log2", "None", a float in ]0,1] or a strictly positive integer')

    if verbose is None:
        verbose = 0
    elif not ((check_int(verbose) and check_positive(verbose)) or check_bool(verbose)):
        raise ValueError('Error, parameter verbose is not a positive integer or a boolean')

    if max_leaf_nodes is not None and not check_int(max_leaf_nodes) or (check_int(max_leaf_nodes) and max_leaf_nodes < 2):
            raise ValueError('Error, parameter max_leaf_nodes is not an integer bigger than 1')

    if warm_start is None:
        warm_start = False
    elif not check_bool(warm_start):
        raise ValueError('Error, parameter warm_start is not boolean')

    if ccp_alpha is None:
        ccp_alpha = 0.0
    elif not check_positive(ccp_alpha):
        raise ValueError('Error, parameter ccp_alpha is not a positive float')

    return n_estimators, min_samples_split, min_samples_leaf, min_weight_fraction_leaf, min_impurity_decrease, random_state, max_features, verbose, max_leaf_nodes, warm_start, ccp_alpha

def recurse(tree, node, parent_path, feature_names, output_rules_file, k_dict, from_grad_boost): # parent_path : path taken until current node
    if tree.feature[node] != _tree.TREE_UNDEFINED: # Check if this is a real node
        feature_name = feature_names[node] # Get node's feature name
        threshold = tree.threshold[node] # Get node threshold
        if node == 0:
            left_path = f"{feature_name} <= {threshold}"
        else:
            left_path = f"{parent_path} & {feature_name} <= {threshold}"
        recurse(tree, tree.children_left[node], left_path, feature_names, output_rules_file, k_dict, from_grad_boost) # Check left child node

        if node == 0:
            right_path = f"{feature_name} > {threshold}"
        else:
            right_path = f"{parent_path} & {feature_name} > {threshold}"
        recurse(tree, tree.children_right[node], right_path, feature_names, output_rules_file, k_dict, from_grad_boost) # Check right child node
    else: # If this is a leaf
        k = k_dict["value"]
        k += 1
        k_dict["value"] = k
        cover_value = tree.value[node] # Get cover values for this rule
        if from_grad_boost:
            output_rules_file.write(f"Rule {k}: {parent_path} -> value {tree.value[node][0]}\n") # Write rule
        else:
            output_rules_file.write(f"Rule {k}: {parent_path} -> class {np.argmax(cover_value)} Covering: {[int(num) for num in cover_value[0]]}\n") # Write rule

def trees_to_rules(trees, rules_file, from_grad_boost=False):
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
