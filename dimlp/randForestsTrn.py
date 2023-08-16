import time
import sys
from sklearn import metrics
from .trnFun import get_data, output_pred, output_stats, check_parameters_common, check_int, check_strictly_positive
from sklearn.ensemble import RandomForestClassifier
from sklearn.tree import _tree
import numpy as np


def recurse(tree, node, parent_path, feature_names, output_rules_file, k_dict): # parent_path : path taken until current node
    if tree.feature[node] != _tree.TREE_UNDEFINED: # Check if this is a real node
        feature_name = feature_names[node] # Get node's feature name
        threshold = tree.threshold[node] # Get node threshold
        if node == 0:
            left_path = f"{feature_name} <= {threshold}"
        else:
            left_path = f"{parent_path} & {feature_name} <= {threshold}"
        recurse(tree, tree.children_left[node], left_path, feature_names, output_rules_file, k_dict) # Check left child node

        if node == 0:
            right_path = f"{feature_name} > {threshold}"
        else:
            right_path = f"{parent_path} & {feature_name} > {threshold}"
        recurse(tree, tree.children_right[node], right_path, feature_names, output_rules_file, k_dict) # Check right child node
    else: # If this is a leaf
        k = k_dict["value"]
        k += 1
        k_dict["value"] = k
        cover_value = tree.value[node] # Get cover values for this rule
        output_rules_file.write(f"Rule {k}: {parent_path} -> class {np.argmax(cover_value)} Covering: {[int(num) for num in cover_value[0]]}\n") # Write rule

def trees_to_rules(trees, rules_file):
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
                recurse(tree_, 0, "", feature_names, output_rules_file, k_dict)
            output_rules_file.close()
    except FileNotFoundError:
        raise ValueError(f"Error: File for rules extraction ({rules_file}) not found.")
    except IOError:
        raise ValueError(f"Error: Couldn't open rules extraction file {rules_file}.")

def randForestsTrn(*args, **kwargs):
    try:

        if args or not kwargs:
            print("---------------------------------------------------------------------")
            print("Please specify arguments using named parameters.")
            print("Warning! The files are localised with respect to root folder dimlpfidex.")
            print("----------------------------")
            print("Obligatory parameters :")
            print("train_data : train data file")
            print("train_class : train class file")
            print("test_data : test data file")
            print("test_class : test class file")
            print("----------------------------")
            print("Optional parameters :")
            print("save_folder : Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder.")
            print("train_pred : output train prediction file name without extension(predTrain by default)")
            print("test_pred : output test prediction file name without extension(predTest by default)")
            print("stats : output file name without extension with train and test accuracy")
            print("output_file : file where you redirect console result")
            print("rules_file : random forests rules file without extension (RF_rules.rls by default)")
            print("----------------------------")
            print("SVM parameters (optional)")
            print("n_estimators : Number of generated trees in the forest(100 by default)")
            print("----------------------------")
            print("----------------------------")
            print("Here is an example, keep same parameter names :")
            print('randForestsTrn(train_data="datanormTrain",train_class="dataclass2Train", test_data="datanormTest",test_class="dataclass2Test", stats = "rf/stats.txt", train_pred = "rf/predTrain", test_pred = "rf/predTest", rules_file = "rf/RF_rules", save_folder = "dimlp/datafiles")')
            print("---------------------------------------------------------------------")
        else:

            start_time = time.time()

            # Get parameters
            save_folder = kwargs.get('save_folder')
            train_data_file = kwargs.get('train_data')
            train_class_file = kwargs.get('train_class')
            test_data_file = kwargs.get('test_data')
            test_class_file = kwargs.get('test_class')
            output_file = kwargs.get('output_file')

            train_pred_file = kwargs.get('train_pred')
            test_pred_file = kwargs.get('test_pred')
            stats_file = kwargs.get('stats')
            rules_file = kwargs.get('rules_file')

            n_estimators_var = kwargs.get('n_estimators')

            # Redirect output in file
            if output_file != None:
                try:
                    if (save_folder is not None):
                        output_file = save_folder + "/" + output_file
                    sys.stdout = open(output_file, 'w+')
                except (FileNotFoundError):
                    raise ValueError(f"Error : File {output_file} not found.")
                except (IOError):
                    raise ValueError(f"Error : Couldn't open file {output_file}.")

            # Check parameters

            valid_args = ['train_data', 'train_class', 'test_data', 'test_class', 'train_pred', 'test_pred',
                        'stats', 'save_folder', 'output_file', 'rules_file', 'n_estimators']

            # Check if wrong parameters are given
            for arg_key in kwargs.keys():
                if arg_key not in valid_args:
                    raise ValueError(f"Invalid argument : {arg_key}")

            save_folder, train_data_file, train_class_file, test_data_file, test_class_file, train_pred_file, test_pred_file, stats_file  = check_parameters_common(save_folder, train_data_file, train_class_file, test_data_file, test_class_file, train_pred_file, test_pred_file, stats_file)

            if rules_file is None:
                rules_file = "RF_rules"
            elif not isinstance(rules_file, str):
                raise ValueError('Error : parameter rules_file has to be a name contained in quotation marks "".')
            rules_file += ".rls"

            if n_estimators_var is None:
                n_estimators_var = 100
            elif (not check_strictly_positive(n_estimators_var) or not check_int(n_estimators_var)):
                raise ValueError('Error, parameter n_estimators is not a strictly positive number')

            if (save_folder is not None):
                train_data_file = save_folder + "/" + train_data_file
                train_class_file = save_folder + "/" + train_class_file
                test_data_file = save_folder + "/" + test_data_file
                test_class_file = save_folder + "/" + test_class_file
                train_pred_file = save_folder + "/" + train_pred_file
                test_pred_file = save_folder + "/" + test_pred_file
                rules_file = save_folder + "/" + rules_file
                if (stats_file is not None):
                    stats_file = save_folder + "/" + stats_file

            # Get data
            train_data = get_data(train_data_file)
            train_class = get_data(train_class_file)
            train_class = [cl.index(max(cl)) for cl in train_class]
            test_data = get_data(test_data_file)
            test_class = get_data(test_class_file)
            test_class = [cl.index(max(cl)) for cl in test_class]

            # Train RF
            model = RandomForestClassifier(n_estimators=n_estimators_var, verbose=1, oob_score=False) # Create RF Classifier

            model.fit(train_data, train_class)   # Train the model using the training sets
            train_pred = model.predict(train_data)    # Predict the response for train dataset
            test_pred = model.predict(test_data)    # Predict the response for test dataset

            # Output predictions
            nb_classes = len(model.classes_)
            output_pred(train_pred, train_pred_file, nb_classes)
            output_pred(test_pred, test_pred_file, nb_classes)

            # Compute model Accuracy
            acc_train = metrics.accuracy_score(train_class, train_pred)*100
            acc_test = metrics.accuracy_score(test_class, test_pred)*100
            acc_train = "{:.6f}".format(acc_train).rstrip("0").rstrip(".")
            acc_test = "{:.6f}".format(acc_test).rstrip("0").rstrip(".")
            print("Training accuracy : {}%.".format(acc_train))
            print("Testing accuracy : {}%.".format(acc_test))

            # Output accuracy statistics
            output_stats(stats_file, acc_train, acc_test)
            trees_to_rules(model.estimators_, rules_file)

            end_time = time.time()
            full_time = end_time - start_time
            full_time = "{:.6f}".format(full_time).rstrip("0").rstrip(".")

            print(f"\nFull execution time = {full_time} sec")


            # Redirect output to terminal
            if output_file != None:
                sys.stdout = sys.__stdout__

            return 0

    except ValueError as error:
        print(error)
        return -1


    # Ajout du .out, et des autres checks de paramètres
