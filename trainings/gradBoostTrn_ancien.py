import time
import sys
from trainings.trnFun import get_data, get_data_class, output_data, output_stats, trees_to_rules, check_parameters_common, check_int, check_strictly_positive, check_positive, check_bool, check_parameters_decision_trees, validate_string_param
from sklearn.ensemble import GradientBoostingClassifier
from sklearn import metrics

def gradBoostTrn(*args, **kwargs):
    try:

        if not kwargs:
            print("---------------------------------------------------------------------")
            print("Please specify arguments using named parameters.")
            print("Warning! The files are localised with respect to root folder dimlpfidex.")
            print("The arguments can be specified in the command or in a json configuration file with --json_config_file your_config_file.json.")
            print("----------------------------")
            print("Required parameters :")
            print("train_data_file : train data file")
            print("train_class_file : train class file, not mendatory if classes are specified in train_data_file")
            print("test_data_file : test data file")
            print("test_class_file : test class file, not mendatory if classes are specified in test_data_file")
            print("nb_attributes : number of attributes")
            print("nb_classes : number of classes")
            print("----------------------------")
            print("Optional parameters :")
            print("root_folder : Folder based on main folder dimlpfidex(default folder) containg all used files and where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder.")
            print("train_pred_outfile : output train prediction file name (predTrain.out by default)")
            print("test_pred_outfile : output test prediction file name (predTest.out by default)")
            print("stats_file : output file name with train and test accuracy (stats.txt by default)")
            print("console_file : file with console logs redirection")
            print("rules_outfile : output gradient boosting rules file (GB_rules.rls by default)")
            print("----------------------------")
            print("Gradient boosting parameters (optional)")
            print("n_estimators : number of generated trees in the forest (100 by default)")
            print("loss : loss function to be optimized, log_loss(default) or exponential")
            print("learning_rate : shrinks the contribution of each tree, positive float (0.1 by default)")
            print("subsample : fraction of samples to be used for fitting the individual base learners, float in ]0,1] (1.0 by default)")
            print("criterion : function to measure split quality, friedman_mse(default) or squared_error,")
            print("min_samples_split : minimum number of samples required to split an internal node, if float, it is a fraction of the number of samples (2 by default)")
            print("min_samples_leaf : minimum number of samples required to be at a leaf node, if float, it is a fraction of the number of samples (1 by default)")
            print("min_weight_fraction_leaf : minimum weighted fraction of the sum total of input samples weights required to be at a leaf node (0.0 by default)")
            print('max_depth : maximum depth of the individual regression estimators, strictly positive integer or "None" (3 by default)')
            print('min_impurity_decrease : a node will be split if this split induces a decrease of the impurity greater than or equal to this value, float (0.0 by default)')
            print('init : estimator object used to compute the initial predictions, "zero"(None by default)')
            print('seed : positive integer(seed) or None(default)')
            print('max_features : number of features to consider when looking for the best split, sqrt(default), log2, None(specify "None"), integer or float')
            print('verbose : enable verbose output, integer (0 by default)')
            print('max_leaf_nodes : grow trees with max_leaf_nodes in best-first fashion, integer (None by default)')
            print('warm_start : whether to reuse previous solution to fit and add more estimators to the ensemble, True or False(default)')
            print('validation_fraction : proportion of training data to set aside as validation set for early stopping, float (0.1 by default)')
            print("n_iter_no_change : decide if early stopping will be used to terminate training when validation score is not improving, stopping if validation doesn't improve during this number of iterations (None by default)")
            print("tol : tolerance for the early stopping (0.0001 by default)")
            print("ccp_alpha : complexity parameter used for Minimal Cost-Complexity Pruning, positive float (0.0 by default)")
            print("----------------------------")
            print("----------------------------")
            print("Here is an example, keep same parameter names :")
            print('gradBoostTrn(train_data_file="datanormTrain",train_class_file="dataclass2Train", test_data_file="datanormTest",test_class_file="dataclass2Test", stats_file = "gb/stats.txt", train_pred_outfile = "gb/predTrain", test_pred_outfile = "gb/predTest", nb_attributes=16, nb_classes=2, rules_outfile = "gb/GB_rules", root_folder = "dimlp/datafiles")')
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
            loss_var = kwargs.get('loss')
            learning_rate_var = kwargs.get('learning_rate')
            subsample_var = kwargs.get('subsample')
            criterion_var = kwargs.get('criterion')
            min_samples_split_var = kwargs.get('min_samples_split')
            min_samples_leaf_var = kwargs.get('min_samples_leaf')
            min_weight_fraction_leaf_var = kwargs.get('min_weight_fraction_leaf')
            max_depth_var = kwargs.get('max_depth')
            min_impurity_decrease_var = kwargs.get('min_impurity_decrease')
            init_var = kwargs.get('init')
            seed_var = kwargs.get('seed')
            max_features_var = kwargs.get('max_features')
            verbose_var = kwargs.get('verbose')
            max_leaf_nodes_var = kwargs.get('max_leaf_nodes')
            warm_start_var = kwargs.get('warm_start')
            validation_fraction_var = kwargs.get('validation_fraction')
            n_iter_no_change_var = kwargs.get('n_iter_no_change')
            tol_var = kwargs.get('tol')
            ccp_alpha_var = kwargs.get('ccp_alpha')

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
            valid_args = ['train_data_file', 'train_class_file', 'test_data_file', 'test_class_file', 'train_pred_outfile', 'test_pred_outfile',
                        'nb_attributes', 'nb_classes', 'stats_file', 'root_folder', 'console_file', 'rules_outfile', 'n_estimators', 'loss', 'learning_rate',
                        'subsample', 'criterion', 'min_samples_split', 'min_samples_leaf', 'min_weight_fraction_leaf',
                        'max_depth', 'min_impurity_decrease', 'init', 'seed', 'max_features', 'verbose', 'max_leaf_nodes',
                        'warm_start', 'validation_fraction', 'n_iter_no_change', 'tol', 'ccp_alpha']

            # Check if wrong parameters are given
            for arg_key in kwargs.keys():
                if arg_key not in valid_args:
                    raise ValueError(f"Invalid argument : {arg_key}.")

            root_folder, train_data_file, test_data_file, train_pred_outfile, test_pred_outfile, stats_file, nb_attributes, nb_classes  = check_parameters_common(root_folder, train_data_file, test_data_file, train_pred_outfile, test_pred_outfile, stats_file, nb_attributes, nb_classes)

            n_estimators_var, min_samples_split_var, min_samples_leaf_var, min_weight_fraction_leaf_var, min_impurity_decrease_var, seed_var, max_features_var, verbose_var, max_leaf_nodes_var, warm_start_var, ccp_alpha_var = check_parameters_decision_trees(n_estimators_var, min_samples_split_var, min_samples_leaf_var, min_weight_fraction_leaf_var, min_impurity_decrease_var, seed_var, max_features_var, verbose_var, max_leaf_nodes_var, warm_start_var, ccp_alpha_var)

            rules_outfile = validate_string_param(rules_outfile, "rules_outfile", default="GB_rules.rls")

            if loss_var is None:
                loss_var = "log_loss"
            elif loss_var not in {"log_loss", "exponential"}:
                raise ValueError('Error : parameter loss is not "log_loss" or "exponential".')

            if learning_rate_var is None:
                learning_rate_var = 0.1
            elif not check_positive(learning_rate_var):
                raise ValueError('Error, parameter learning_rate is not a positive float.')

            if subsample_var is None:
                subsample_var = 1.0
            elif not check_strictly_positive(subsample_var) or subsample_var > 1:
                raise ValueError('Error, parameter subsample is not  a float in ]0,1].')

            if criterion_var is None:
                criterion_var = "friedman_mse"
            elif criterion_var not in {"friedman_mse", "squared_error"}:
                raise ValueError('Error, parameter criterion is not "friedman_mse" or "squared_error".')

            if max_depth_var is None:
                max_depth_var = 3
            elif max_depth_var == "None":
                max_depth_var = None
            elif not check_int(max_depth_var) or not check_strictly_positive(max_depth_var):
                raise ValueError('Error, parameter max_depth is specified and is not "None" or a strictly positive integer.')

            if init_var is not None and init_var != "zero":
                raise ValueError('Error, parameter init is specified and is not "zero".')

            if validation_fraction_var is None:
                validation_fraction_var = 0.1
            elif not check_strictly_positive(validation_fraction_var) or validation_fraction_var >= 1:
                raise ValueError('Error, parameter validation_fraction is not a float in ]0,1[.')

            if n_iter_no_change_var is not None and not (check_int(n_iter_no_change_var) and check_strictly_positive(n_iter_no_change_var)):
                raise ValueError('Error, parameter n_iter_no_change is not a strictly positive integer.')

            if tol_var is None:
                tol_var = 0.0001
            elif not check_positive(tol_var):
                raise ValueError('Error, parameter tol is not a positive float.')

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

            # Train GB
            model = GradientBoostingClassifier(n_estimators=n_estimators_var, loss=loss_var, learning_rate=learning_rate_var, # Create GB Classifier
                                               subsample=subsample_var, criterion=criterion_var, min_samples_split=min_samples_split_var,
                                               min_samples_leaf=min_samples_leaf_var, min_weight_fraction_leaf=min_weight_fraction_leaf_var,
                                               max_depth=max_depth_var, min_impurity_decrease=min_impurity_decrease_var, init=init_var,
                                               random_state=seed_var, max_features=max_features_var, verbose=verbose_var,
                                               max_leaf_nodes=max_leaf_nodes_var, warm_start=warm_start_var, validation_fraction=validation_fraction_var,
                                               n_iter_no_change=n_iter_no_change_var, tol=tol_var, ccp_alpha=ccp_alpha_var)

            model.fit(train_data, train_class)

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
            from_grad_boost = True

            trees_to_rules(model.estimators_.flatten(), rules_outfile, from_grad_boost)

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

if __name__ == "__main__":
    cmdline_args = " ".join(sys.argv[1:])
    gradBoostTrn(cmdline_args)