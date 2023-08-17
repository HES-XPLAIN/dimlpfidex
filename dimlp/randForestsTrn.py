import time
import sys
from sklearn import metrics
from .trnFun import get_data, output_pred, output_stats, check_parameters_common, check_int, check_strictly_positive, check_positive, check_bool
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
            print("criterion : function to measure split quality, gini(default), entropy or log_loss,")
            print("max_depth : max depth of the tree, integer (None by default)")
            print("min_samples_split : minimum number of samples required to split an internal node, if float, it is a fraction of the number of samples (2 by default)")
            print("min_samples_leaf : minimum number of samples required to be at a leaf node, if float, it is a fraction of the number of samples (1 by default)")
            print("min_weight_fraction_leaf : minimum weighted fraction of the sum total of input samples weights required to be at a leaf node (0.0 by default)")
            print('max_features : number of features to consider when looking for the best split, sqrt(default), log2, None(specify "None"), integer or float')
            print('max_leaf_nodes : grow trees with max_leaf_nodes in best-first fashion, integer (None by default)')
            print('min_impurity_decrease : a node will be split if this split induces a decrease of the impurity greater than or equal to this value, float (0.0 by default)')
            print('bootstrap : whether bootstrap samples are used when building trees, True(default) or False (0.0 by default)')
            print('oob_score : whether to use out-of-bag samples to estimate the generalization score, True, False(default) or callable')
            print('n_jobs : number of jobs to run in parallel (None by default(1 proc), -1 : all processors)')
            print('random_state : positive integer(seed) or None(default)')
            print('verbose : controls the verbosity when fitting and predictin, integer (0 by default)')
            print('warm_start : whether to reuse previous solution to fit initialization, True or False(default)')
            print("class_weight : class balance, 'balanced', 'balanced_subsample, or a dictionary, for exemple with 2 classes : {0:1.2, 1:3.5} (None by default)")
            print("ccp_alpha : complexity parameter used for Minimal Cost-Complexity Pruning, positive float (0.0 by default)")
            print("max_samples : number of samples to draw from X to train each base estimator for bootstrap, if float, it is a fraction of the number of samples (None by default)")
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
            criterion_var = kwargs.get('criterion')
            max_depth_var = kwargs.get('max_depth')
            min_samples_split_var = kwargs.get('min_samples_split')
            min_samples_leaf_var = kwargs.get('min_samples_leaf')
            min_weight_fraction_leaf_var = kwargs.get('min_weight_fraction_leaf')
            max_features_var = kwargs.get('max_features')
            max_leaf_nodes_var = kwargs.get('max_leaf_nodes')
            min_impurity_decrease_var = kwargs.get('min_impurity_decrease')
            bootstrap_var = kwargs.get('bootstrap')
            oob_score_var = kwargs.get('oob_score')
            n_jobs_var = kwargs.get('n_jobs')
            random_state_var = kwargs.get('random_state')
            verbose_var = kwargs.get('verbose')
            warm_start_var = kwargs.get('warm_start')
            class_weight_var = kwargs.get('class_weight')
            ccp_alpha_var = kwargs.get('ccp_alpha')
            max_samples_var = kwargs.get('max_samples')



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
                        'stats', 'save_folder', 'output_file', 'rules_file', 'n_estimators', 'criterion', 'max_depth', 'min_samples_split',
                        'min_samples_leaf', 'min_weight_fraction_leaf', 'max_features', 'max_leaf_nodes', 'min_impurity_decrease', 'bootstrap',
                        'oob_score', 'n_jobs', 'random_state', 'verbose', 'warm_start', 'class_weight', 'ccp_alpha', 'max_samples']

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
                raise ValueError('Error, parameter n_estimators is not a strictly positive integer')

            if criterion_var is None:
                criterion_var = "gini"
            elif criterion_var not in {"gini", "entropy", "log_loss"}:
                raise ValueError('Error, parameter criterion is not "gini", "entropy" or "log_loss"')

            if max_depth_var is not None and (not check_int(max_depth_var)or not check_strictly_positive(max_depth_var)):
                raise ValueError('Error, parameter max_depth is specified and is not a strictly positive integer')

            if min_samples_split_var is None:
                min_samples_split_var = 2
            elif ((check_int(min_samples_split_var) and  min_samples_split_var < 2) or (isinstance(min_samples_split_var, float) and ((min_samples_split_var <= 0) or min_samples_split_var > 1.0)) or not check_positive(min_samples_split_var)):
                raise ValueError('Error, parameter min_samples_split is not an integer bigger than 1 or a float in ]0,1.0]. For 1, put 1.0.')

            if min_samples_leaf_var is None:
                min_samples_leaf_var = 1
            elif ((isinstance(min_samples_leaf_var, float) and ((min_samples_leaf_var <= 0) or min_samples_leaf_var >= 1)) or not check_strictly_positive(min_samples_leaf_var)):
                raise ValueError('Error, parameter min_samples_leaf is not a strictly positive integer or a float in ]0,1[')

            if min_weight_fraction_leaf_var is None:
                min_weight_fraction_leaf_var = 0.0
            elif not check_positive(min_weight_fraction_leaf_var) or min_weight_fraction_leaf_var < 0 or min_weight_fraction_leaf_var > 0.5:
                raise ValueError('Error, parameter min_weight_fraction_leaf is not a float in [0,0.5]')

            if max_features_var is None:
                max_features_var = "sqrt"
            elif max_features_var == "None":
                max_features_var = None
            elif max_features_var not in {"sqrt", "log2", "None"} and not check_strictly_positive(max_features_var) or (isinstance(max_features_var, float) and max_features_var > 1):
                raise ValueError('Error, parameter max_features is not "sqrt", "log2", "None", a float in ]0,1] or a strictly positive integer')

            if max_leaf_nodes_var is not None and not check_int(max_leaf_nodes_var) or (check_int(max_leaf_nodes_var) and max_leaf_nodes_var < 2):
                    raise ValueError('Error, parameter max_leaf_nodes is not an integer bigger than 1')

            if min_impurity_decrease_var is None:
                min_impurity_decrease_var = 0.0
            elif not check_positive(min_impurity_decrease_var):
                raise ValueError('Error, parameter min_impurity_decrease is not a positive float')

            if bootstrap_var is None:
                bootstrap_var = True
            elif not check_bool(bootstrap_var):
                raise ValueError('Error, parameter bootstrap is not a boolean')

            if oob_score_var is None:
                oob_score_var = False
            elif not check_bool(oob_score_var):
                 raise ValueError('Error, parameter oob_score is not a boolean')

            if n_jobs_var is not None and not check_int(n_jobs_var):
                 raise ValueError('Error, parameter n_jobs is not an integer')

            if random_state_var is not None and (not check_int(random_state_var) or not check_positive(random_state_var)):
                raise ValueError('Error, parameter random_state is not a positive integer')

            if verbose_var is None:
                verbose_var = 0
            elif not ((check_int(verbose_var) and check_positive(verbose_var)) or check_bool(verbose_var)):
                raise ValueError('Error, parameter verbose is not a positive integer or a boolean')

            if warm_start_var is None:
                warm_start_var = False
            elif not check_bool(warm_start_var):
                raise ValueError('Error, parameter warm_start is not boolean')

            if class_weight_var is not None and not isinstance(class_weight_var, dict) and class_weight_var not in {"balanced", "balanced_subsample"}:
                raise ValueError('Error, parameter class_weight is not "balanced", "balanced_subsample", a dictionary or None')

            if ccp_alpha_var is None:
                ccp_alpha_var = 0.0
            elif not check_positive(ccp_alpha_var):
                raise ValueError('Error, parameter ccp_alpha is not a positive float')

            if  max_samples_var is not None and ((isinstance(max_samples_var, float) and ((max_samples_var <= 0) or max_samples_var > 1)) or not check_strictly_positive(max_samples_var)):
                raise ValueError('Error, parameter max_samples is not a strictly positive integer or a float in ]0,1]')

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
            model = RandomForestClassifier(n_estimators=n_estimators_var, criterion=criterion_var, max_depth=max_depth_var, # Create RF Classifier
                                            min_samples_split=min_samples_split_var, min_samples_leaf=min_samples_leaf_var, min_weight_fraction_leaf=min_weight_fraction_leaf_var,
                                            max_features=max_features_var, max_leaf_nodes=max_leaf_nodes_var, min_impurity_decrease=min_impurity_decrease_var, bootstrap=bootstrap_var,
                                            oob_score=oob_score_var, n_jobs=n_jobs_var, random_state=random_state_var, verbose=verbose_var, warm_start=warm_start_var, class_weight=class_weight_var,
                                            ccp_alpha=ccp_alpha_var, max_samples=max_samples_var)

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
