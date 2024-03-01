import time
import sys
from .trnFun import get_data, get_data_class, output_data, output_stats, check_parameters_common, check_int, check_strictly_positive, check_positive, check_bool, trees_to_rules, check_parameters_decision_trees, validate_string_param
from sklearn.ensemble import RandomForestClassifier
from sklearn import metrics
import numpy as np

def randForestsTrn(*args, **kwargs):
    try:

        if not kwargs:
            print("---------------------------------------------------------------------")
            print("Please specify arguments using named parameters.")
            print("Warning! The files are localised with respect to root folder dimlpfidex.")
            print("----------------------------")
            print("Obligatory parameters :")
            print("train_data_file : train data file")
            print("train_class_file : train class file, not mendatory if classes are specified in train_data_file")
            print("test_data_file : test data file")
            print("test_class_file : test class file, not mendatory if classes are specified in test_data_file")
            print("nb_attributes : number of attributes")
            print("nb_classes : number of classes")
            print("----------------------------")
            print("Optional parameters :")
            print("root_folder : Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder.")
            print("train_pred_outfile : output train prediction file name (predTrain.out by default)")
            print("test_pred_outfile : output test prediction file name (predTest.out by default)")
            print("stats_file : output file name with train and test accuracy (stats.txt by default)")
            print("console_file : file where you redirect console result")
            print("rules_outfile : output random forests rules file (RF_rules.rls by default)")
            print("----------------------------")
            print("Random Forests parameters (optional)")
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
            print('seed : positive integer(seed) or None(default)')
            print('verbose : controls the verbosity when fitting and predicting, integer (0 by default)')
            print('warm_start : whether to reuse the solution of the previous call to fit and add more estimators to the ensemble, True or False(default)')
            print("class_weight : class balance, 'balanced', 'balanced_subsample, or a dictionary, for exemple with 2 classes : {0:1.2, 1:3.5} (None by default)")
            print("ccp_alpha : complexity parameter used for Minimal Cost-Complexity Pruning, positive float (0.0 by default)")
            print("max_samples : number of samples to draw to train each base estimator for bootstrap, if float, it is a fraction of the number of samples (None by default)")
            print("----------------------------")
            print("----------------------------")
            print("Here is an example, keep same parameter names :")
            print('randForestsTrn(train_data_file="datanormTrain",train_class_file="dataclass2Train", test_data_file="datanormTest",test_class_file="dataclass2Test", stats_file = "rf/stats.txt", train_pred_outfile = "rf/predTrain", test_pred_outfile = "rf/predTest", nb_attributes=16, nb_classes=2, rules_outfile = "rf/RF_rules", root_folder = "dimlp/datafiles")')
            print("---------------------------------------------------------------------")
        else:

            start_time = time.time()

            # Get parameters
            root_folder = kwargs.get('root_folder')
            train_data_file = kwargs.get('train_data_file')
            train_class_file = kwargs.get('train_class_file')
            test_data_file = kwargs.get('test_data_file')
            test_class_file = kwargs.get('test_class_file')
            console_file = kwargs.get('console_file')
            nb_attributes = kwargs.get('nb_attributes')
            nb_classes = kwargs.get('nb_classes')

            train_pred_outfile = kwargs.get('train_pred_outfile')
            test_pred_outfile = kwargs.get('test_pred_outfile')
            stats_file = kwargs.get('stats_file')
            rules_outfile = kwargs.get('rules_outfile')

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
            seed_var = kwargs.get('seed')
            verbose_var = kwargs.get('verbose')
            warm_start_var = kwargs.get('warm_start')
            class_weight_var = kwargs.get('class_weight')
            ccp_alpha_var = kwargs.get('ccp_alpha')
            max_samples_var = kwargs.get('max_samples')



            # Redirect output in file
            if console_file != None:
                try:
                    if (root_folder is not None):
                        console_file = root_folder + "/" + console_file
                    sys.stdout = open(console_file, 'w+')
                except (FileNotFoundError):
                    raise ValueError(f"Error : File {console_file} not found.")
                except (IOError):
                    raise ValueError(f"Error : Couldn't open file {console_file}.")

            # Check parameters

            valid_args = ['train_data_file', 'train_class_file', 'test_data_file', 'test_class_file', 'train_pred_outfile', 'test_pred_outfile', 'nb_attributes', 'nb_classes',
                        'stats_file', 'root_folder', 'console_file', 'rules_outfile', 'n_estimators', 'criterion', 'max_depth', 'min_samples_split',
                        'min_samples_leaf', 'min_weight_fraction_leaf', 'max_features', 'max_leaf_nodes', 'min_impurity_decrease', 'bootstrap',
                        'oob_score', 'n_jobs', 'seed', 'verbose', 'warm_start', 'class_weight', 'ccp_alpha', 'max_samples']

            # Check if wrong parameters are given
            for arg_key in kwargs.keys():
                if arg_key not in valid_args:
                    raise ValueError(f"Invalid argument : {arg_key}.")

            root_folder, train_data_file, test_data_file, train_pred_outfile, test_pred_outfile, stats_file, nb_attributes, nb_classes  = check_parameters_common(root_folder, train_data_file, test_data_file, train_pred_outfile, test_pred_outfile, stats_file, nb_attributes, nb_classes)

            n_estimators_var, min_samples_split_var, min_samples_leaf_var, min_weight_fraction_leaf_var, min_impurity_decrease_var, seed_var, max_features_var, verbose_var, max_leaf_nodes_var, warm_start_var, ccp_alpha_var = check_parameters_decision_trees(n_estimators_var, min_samples_split_var, min_samples_leaf_var, min_weight_fraction_leaf_var, min_impurity_decrease_var, seed_var, max_features_var, verbose_var, max_leaf_nodes_var, warm_start_var, ccp_alpha_var)

            rules_outfile = validate_string_param(rules_outfile, "rules_outfile", default="RF_rules.rls")

            if criterion_var is None:
                criterion_var = "gini"
            elif criterion_var not in {"gini", "entropy", "log_loss"}:
                raise ValueError('Error, parameter criterion is not "gini", "entropy" or "log_loss".')

            if max_depth_var is not None and (not check_int(max_depth_var)or not check_strictly_positive(max_depth_var)):
                raise ValueError('Error, parameter max_depth is specified and is not a strictly positive integer.')

            if bootstrap_var is None:
                bootstrap_var = True
            elif not check_bool(bootstrap_var):
                raise ValueError('Error, parameter bootstrap is not a boolean.')

            if oob_score_var is None:
                oob_score_var = False
            elif not check_bool(oob_score_var):
                 raise ValueError('Error, parameter oob_score is not a boolean.')

            if n_jobs_var is not None and not check_int(n_jobs_var):
                 raise ValueError('Error, parameter n_jobs is not an integer.')

            if class_weight_var is not None and not isinstance(class_weight_var, dict) and class_weight_var not in {"balanced", "balanced_subsample"}:
                raise ValueError('Error, parameter class_weight is not "balanced", "balanced_subsample", a dictionary or None.')

            if  max_samples_var is not None and ((isinstance(max_samples_var, float) and ((max_samples_var <= 0) or max_samples_var > 1)) or not check_strictly_positive(max_samples_var)):
                raise ValueError('Error, parameter max_samples is not a strictly positive integer or a float in ]0,1].')

            if (root_folder is not None):
                train_data_file = root_folder + "/" + train_data_file
                test_data_file = root_folder + "/" + test_data_file
                train_pred_outfile = root_folder + "/" + train_pred_outfile
                test_pred_outfile = root_folder + "/" + test_pred_outfile
                rules_outfile = root_folder + "/" + rules_outfile
                if (stats_file is not None):
                    stats_file = root_folder + "/" + stats_file

            # Get data
            train_data, train_class = get_data(train_data_file, nb_attributes, nb_classes)
            if len(train_class) == 0:
                train_class_file = validate_string_param(train_class_file, "train_class_file")
                if (root_folder is not None):
                    train_class_file = root_folder + "/" + train_class_file
                train_class = get_data_class(train_class_file, nb_classes)
            if len(train_data) != len(train_class):
                raise ValueError('Error, there is not the same amount of train data and train class.')
            classes = set(range(nb_classes))
            miss_train_classes = classes - set(train_class) # Check if a class is not represented during training

            test_data, test_class = get_data(test_data_file, nb_attributes, nb_classes)
            if len(test_class) == 0:
                test_class_file = validate_string_param(test_class_file, "test_class_file")
                if (root_folder is not None):
                    test_class_file = root_folder + "/" + test_class_file
                test_class = get_data_class(test_class_file, nb_classes)
            if len(test_data) != len(test_class):
                raise ValueError('Error, there is not the same amount of test data and test class.')

            # Train RF
            model = RandomForestClassifier(n_estimators=n_estimators_var, criterion=criterion_var, max_depth=max_depth_var, # Create RF Classifier
                                            min_samples_split=min_samples_split_var, min_samples_leaf=min_samples_leaf_var, min_weight_fraction_leaf=min_weight_fraction_leaf_var,
                                            max_features=max_features_var, max_leaf_nodes=max_leaf_nodes_var, min_impurity_decrease=min_impurity_decrease_var, bootstrap=bootstrap_var,
                                            oob_score=oob_score_var, n_jobs=n_jobs_var, random_state=seed_var, verbose=verbose_var, warm_start=warm_start_var, class_weight=class_weight_var,
                                            ccp_alpha=ccp_alpha_var, max_samples=max_samples_var)

            model.fit(train_data, train_class)   # Train the model using the training sets

            # Compute predictions
            train_pred_proba = model.predict_proba(train_data)  # Predict the response for train dataset
            test_pred_proba = model.predict_proba(test_data)    # Predict the response for test dataset
            train_pred = model.predict(train_data)    # Predict the response for train dataset
            test_pred = model.predict(test_data)    # Predict the response for test dataset

            # If a class is missing, we adapt predictions
            if len(miss_train_classes)!=0:
                miss_train_classes = sorted(list(miss_train_classes))
                train_pred_proba = [pred.tolist() for pred in train_pred_proba]
                test_pred_proba = [pred.tolist() for pred in test_pred_proba]
                for train_pred_list in [train_pred_proba, test_pred_proba]:
                    for pred in train_pred_list:
                        for classe in miss_train_classes:
                            pred.insert(classe, 0.0) # Prediction 0 for the missing class

            # Output predictions
            output_data(train_pred_proba, train_pred_outfile)
            output_data(test_pred_proba, test_pred_outfile)

            # Compute model Accuracy
            acc_train = metrics.accuracy_score(train_class, train_pred) * 100
            acc_test = metrics.accuracy_score(test_class, test_pred) * 100
            acc_train = "{:.6f}".format(acc_train).rstrip("0").rstrip(".")
            acc_test = "{:.6f}".format(acc_test).rstrip("0").rstrip(".")
            print("Training accuracy : {}%.".format(acc_train))
            print("Testing accuracy : {}%.".format(acc_test))

            # Output accuracy statistics
            output_stats(stats_file, acc_train, acc_test)
            trees_to_rules(model.estimators_, rules_outfile)

            end_time = time.time()
            full_time = end_time - start_time
            full_time = "{:.6f}".format(full_time).rstrip("0").rstrip(".")

            print(f"\nFull execution time = {full_time} sec")

            # Redirect output to terminal
            if console_file != None:
                sys.stdout = sys.__stdout__

            return 0

    except ValueError as error:
        # Redirect output to terminal
        if console_file != None:
            sys.stdout = sys.__stdout__
        print(error)
        return -1
