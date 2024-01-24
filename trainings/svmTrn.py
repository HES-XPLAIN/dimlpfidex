import time
import sys
from sklearn import svm
from sklearn import metrics
import numpy as np
import matplotlib.pyplot as plt

from .trnFun import get_data, get_data_class, check_int, check_strictly_positive, check_positive, check_bool, output_pred, compute_first_hidden_layer, output_stats, check_parameters_common, check_parameters_dimlp_layer, compute_roc, validate_string_param

def svmTrn(*args, **kwargs):
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
            print("train_pred_file : output train prediction file name without extension(predTrain by default)")
            print("test_pred_file : output test prediction file name without extension(predTest by default)")
            print("weights_file : output weights file name without extension (weights by default)")
            print("stats_file : output file name with train and test accuracy (stats.txt by default)")
            print("console_file : file where you redirect console result")
            print("output_roc : output ROC curve without extension(roc_curve.png by default)")
            print("nb_quant_levels : number of stairs in staircase activation function (50 by default)")
            print("K : Parameter to improve dynamics (1 by default)")
            print("positive_class_index : index of positive class (0 for first one, None by default, if None, no roc calculation)")
            print("return_roc : whether to return ROC statistics (False by default)")
            print("with_decision_threshold : Boolean, whether to compute decision threshold (only if roc curve is computed, False by default)")
            print("test_class_threshold_file : new test class file with the new threshold (test_class_threshold.txt by default)")
            print("train_class_threshold_file : new train class file with the new threshold (train_class_threshold.txt by default)")

            print("----------------------------")
            print("SVM parameters (optional)")
            print("C : regularization, (1.0 by default)")
            print("kernel : linear, poly, rbf(default) or sigmoid")
            print("degree : polynomial degree (3 by default)")
            print("gamma : scale(default), auto or non negative float")
            print("coef0 : term in kernel function, float (0 by default)")
            print("shrinking : heuristic, True(default) or False")
            print("tol : tolerance for stopping criterion (0.001 by default)")
            print("cache_size : kernel cache size (200 MB by default)")
            print("class_weight : class balance, 'balanced' or a dictionary, for exemple with 2 classes : {0:1.2, 1:3.5} (None by default)")
            print("verbose : enable verbose output, True or False(default)")
            print("max_iterations : maximal number of iterations (-1 for no limit (default))")
            print("decision_function_shape : decision function shape, ovo(one-vs-one) or ovr(one-vs-rest, default)")
            print("break_ties : break tie decision for ovr with more than 2 classes, True or False(default)")
            print("----------------------------")
            print("----------------------------")
            print("Here is an example, keep same parameter names :")
            print('svmTrn(train_data_file="datanormTrain",train_class_file="dataclass2Train", test_data_file="datanormTest",test_class_file="dataclass2Test", weights_file = "svm/weights", stats_file = "svm/stats.txt", train_pred_file = "svm/predTrain", test_pred_file = "svm/predTest", nb_attributes=16, nb_classes=2, root_folder = "dimlp/datafiles")')
            print("---------------------------------------------------------------------")
        else:

            start_time = time.time()

            # Get parameters
            root_folder = kwargs.get('root_folder')
            train_data_file = kwargs.get('train_data_file')
            train_class_file = kwargs.get('train_class_file')
            test_data_file = kwargs.get('test_data_file')
            test_class_file = kwargs.get('test_class_file')
            nb_attributes = kwargs.get('nb_attributes')
            nb_classes = kwargs.get('nb_classes')
            console_file = kwargs.get('console_file')
            positive_class_index = kwargs.get('positive_class_index')

            train_pred_file = kwargs.get('train_pred_file')
            test_pred_file = kwargs.get('test_pred_file')
            weights_file = kwargs.get('weights_file')
            stats_file = kwargs.get('stats_file')
            output_roc = kwargs.get('output_roc')

            K = kwargs.get('K')
            quant = kwargs.get('nb_quant_levels')
            hiknot = 5
            c_var = kwargs.get('C')
            kernel_var = kwargs.get('kernel')
            degree_var = kwargs.get('degree')
            gamma_var = kwargs.get('gamma')
            coef0_var = kwargs.get('coef0')
            shrinking_var = kwargs.get('shrinking')
            tol_var = kwargs.get('tol')
            cache_size_var = kwargs.get('cache_size')
            class_weight_var = kwargs.get('class_weight')
            verbose_var = kwargs.get('verbose')
            max_iterations_var = kwargs.get('max_iterations')
            decision_function_shape_var = kwargs.get('decision_function_shape')
            break_ties_var = kwargs.get('break_ties')
            return_roc = kwargs.get('return_roc')
            with_decision_threshold = kwargs.get('with_decision_threshold')
            test_class_threshold_file = kwargs.get('test_class_threshold_file')
            train_class_threshold_file = kwargs.get('train_class_threshold_file')

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

            valid_args = ['train_data_file', 'train_class_file', 'test_data_file', 'test_class_file', 'positive_class_index', 'train_pred_file', 'test_pred_file', 'nb_attributes', 'nb_classes', 'weights_file',
                        'stats_file', 'output_roc', 'K', 'nb_quant_levels', 'C', 'kernel', 'degree', 'gamma', 'coef0', 'shrinking',
                        'tol', 'cache_size', 'class_weight', 'verbose', 'max_iterations', 'decision_function_shape', 'break_ties', 'root_folder', 'console_file', 'return_roc',
                        'with_decision_threshold', 'test_class_threshold_file', 'train_class_threshold_file']

            # Check if wrong parameters are given
            for arg_key in kwargs.keys():
                if arg_key not in valid_args:
                    raise ValueError(f"Invalid argument : {arg_key}.")

            root_folder, train_data_file, test_data_file, train_pred_file, test_pred_file, stats_file, nb_attributes, nb_classes  = check_parameters_common(root_folder, train_data_file, test_data_file, train_pred_file, test_pred_file, stats_file, nb_attributes, nb_classes)
            weights_file, K, quant = check_parameters_dimlp_layer(weights_file, K, quant)

            output_roc = validate_string_param(output_roc, "output_roc", default="roc_curve")
            output_roc += ".png"

            if c_var is None:
                c_var = 1.0
            elif not check_strictly_positive(c_var):
                raise ValueError('Error, parameter C is not a strictly positive number.')

            if kernel_var is None:
                kernel_var = "rbf"
            else:
                kernels = {"linear", "poly", "rbf", "sigmoid"}
                if (kernel_var not in kernels):
                    raise ValueError('Error, parameter kernel is not linear, poly, rbf or sigmoid.')

            if degree_var is None:
                degree_var = 3
            elif not check_positive(degree_var) or not check_int(degree_var):
                raise ValueError('Error, parameter degree is not positive integer.')

            if gamma_var is None:
                gamma_var = "scale"
            else:
                gammas = {"scale", "auto"}
                if (gamma_var not in gammas and not check_positive(gamma_var)):
                    raise ValueError('Error, parameter gamma is not scale, auto or positive float.')

            if coef0_var is None:
                coef0_var = 0.0
            if not isinstance(coef0_var, (float,int)):
                raise ValueError('Error, parameter coef0 is not a float.')

            if shrinking_var is None:
                shrinking_var = True
            elif not check_bool(shrinking_var):
                raise ValueError('Error, parameter shrinking is not boolean.')

            if tol_var is None:
                tol_var = 0.001
            elif not check_strictly_positive(tol_var):
                raise ValueError('Error, parameter tol is not a strictly positive float.')

            if cache_size_var is None:
                cache_size_var = 0.001
            elif not check_strictly_positive(cache_size_var):
                raise ValueError('Error, parameter cache_size is not a positive number.')

            if class_weight_var is not None and class_weight_var != "balanced" and not isinstance(class_weight_var, dict):
                raise ValueError('Error, parameter class_weight is not "balanced", a dictionary or None.')

            if verbose_var is None:
                verbose_var = False
            elif not check_bool(verbose_var):
                raise ValueError('Error, parameter verbose is not boolean.')

            if max_iterations_var is None:
                max_iterations_var = -1
            elif (not check_strictly_positive(max_iterations_var) or not check_int(max_iterations_var)) and max_iterations_var != -1:
                raise ValueError('Error, parameter max_iterations is not a strictly positive number or -1.')

            if decision_function_shape_var is None:
                decision_function_shape_var = "ovr"
            else:
                decision_function_shapes = {"ovo", "ovr"}
                if decision_function_shape_var not in decision_function_shapes:
                    raise ValueError('Error, parameter decision_function_shape is not ovo or ovr.')

            if break_ties_var is None:
                break_ties_var = False
            elif not check_bool(break_ties_var):
                raise ValueError('Error, parameter break_ties is not boolean.')

            if return_roc is None:
                return_roc = False
            elif not check_bool(return_roc):
                raise ValueError('Error, parameter return_roc is not boolean.')

            if with_decision_threshold is None:
                with_decision_threshold = False
            elif not check_bool(with_decision_threshold):
                raise ValueError('Error, parameter with_decision_threshold is not boolean.')

            if (root_folder is not None):
                train_data_file = root_folder + "/" + train_data_file
                test_data_file = root_folder + "/" + test_data_file
                train_pred_file = root_folder + "/" + train_pred_file
                test_pred_file = root_folder + "/" + test_pred_file
                weights_file = root_folder + "/" + weights_file
                output_roc = root_folder + "/" + output_roc
                if (stats_file is not None):
                    stats_file = root_folder + "/" + stats_file

            # Get data
            train_data, train_class = get_data(train_data_file, nb_attributes, nb_classes)
            if len(train_class) == 0:
                train_class_file_init = validate_string_param(train_class_file, "train_class_file")
                if (root_folder is not None):
                    train_class_file = root_folder + "/" + train_class_file_init
                train_class = get_data_class(train_class_file, nb_classes)
            if len(train_data) != len(train_class):
                raise ValueError('Error, there is not the same amount of train data and train class.')

            test_data, test_class = get_data(test_data_file, nb_attributes, nb_classes)
            if len(test_class) == 0:
                test_class_file_init = validate_string_param(test_class_file, "test_class_file")
                if (root_folder is not None):
                    test_class_file = root_folder + "/" + test_class_file_init
                test_class = get_data_class(test_class_file, nb_classes)
            if len(test_data) != len(test_class):
                raise ValueError('Error, there is not the same amount of test data and test class.')

            with_roc = True
            if positive_class_index is None:
                with_roc = False
            elif not isinstance(positive_class_index, int) or positive_class_index < 0 or positive_class_index >= nb_classes:
                raise ValueError(f'Error : parameter positive_class_index has to be a positive integer smaller than {nb_classes}.')


            # Get weights and biais from first hidden layer as well as data transformed in first hidden layer

            train_data_h1, mu, sigma = compute_first_hidden_layer("train", train_data, K, quant, hiknot, weights_file)
            test_data_h1 = compute_first_hidden_layer("test", test_data, K, quant, hiknot, mu=mu, sigma=sigma)

            # Train svm
            model = svm.SVC(C = c_var, kernel = kernel_var, degree = degree_var, gamma = gamma_var, coef0 = coef0_var, shrinking = shrinking_var,
                            tol = tol_var, cache_size = cache_size_var, class_weight = class_weight_var, verbose = verbose_var, max_iter = max_iterations_var,
                            decision_function_shape = decision_function_shape_var, break_ties = break_ties_var, probability=True) # Create svm Classifier

            model.fit(train_data_h1, train_class)   # Train the model using the training sets
            train_pred = model.predict(train_data_h1)    # Predict the response for train dataset
            test_pred = model.predict(test_data_h1)    # Predict the response for test dataset

            # Output predictions
            output_pred(train_pred, train_pred_file, nb_classes)
            output_pred(test_pred, test_pred_file, nb_classes)

            # Compute model Accuracy
            acc_train = metrics.accuracy_score(train_class, train_pred) * 100
            acc_test = metrics.accuracy_score(test_class, test_pred) * 100
            acc_train = "{:.6f}".format(acc_train).rstrip("0").rstrip(".")
            acc_test = "{:.6f}".format(acc_test).rstrip("0").rstrip(".")
            print("Training accuracy : {}%.".format(acc_train))
            print("Testing accuracy : {}%.".format(acc_test))

            # Output accuracy statistics
            output_stats(stats_file, acc_train, acc_test)

            print("Weights : ", model.class_weight_)
            if with_roc:
                test_class_roc = [1 if cl == positive_class_index else 0 for cl in test_class]
                test_pred_proba = model.predict_proba(test_data_h1)
                test_pred_roc = test_pred_proba[:, positive_class_index]
                fpr, tpr, auc_score = compute_roc("svm", output_roc, test_class_roc, test_pred_roc)

                # Save AUC in stats file
                with open(stats_file, 'a') as file:
                    file.write(f"\nAUC score on testing set : {auc_score}")

            end_time = time.time()
            full_time = end_time - start_time
            full_time = "{:.6f}".format(full_time).rstrip("0").rstrip(".")

            print(f"\nFull execution time = {full_time} sec")


            if with_roc and with_decision_threshold:
                train_pred_proba =  model.predict_proba(train_data_h1)
                from trainings.decisionThreshold import decisionThreshold
                decisionThreshold(with_roc_computation = False, test_class_file = test_class_file_init, test_pred_file = test_pred_proba.tolist(), train_pred_file = train_pred_proba.tolist(),
                                  train_class_threshold_file = train_class_threshold_file, test_class_threshold_file = test_class_threshold_file, positive_class_index = positive_class_index,
                                  nb_classes = nb_classes ,fpr = fpr.tolist(), tpr = tpr.tolist(), auc_score = auc_score, root_folder = root_folder)


            # Redirect output to terminal
            if console_file != None:
                sys.stdout = sys.__stdout__

            if with_roc and return_roc:
                # Interpolation to get 1000 points (necessary for crossValidation)
                fpr_interp = np.linspace(0, 1, 1000)
                tpr_interp = np.interp(fpr_interp, fpr, tpr)
                tpr_interp[0] = 0
                return [fpr_interp, tpr_interp, auc_score]

            return 0

    except ValueError as error:
        # Redirect output to terminal
        if console_file != None:
            sys.stdout = sys.__stdout__
        print(error)
        return -1


# Exemple : svmTrn(train_data_file="datanormTrain",train_class_file="dataclass2Train", test_data_file="datanormTest",test_class_file="dataclass2Test", nb_attributes=16, nb_classes=2, weights_file = "weights", stats_file = "stats.txt", root_folder = "dimlp/datafiles")
