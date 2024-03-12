import time
import sys
from sklearn.neural_network import MLPClassifier
import warnings
from sklearn.exceptions import ConvergenceWarning
from sklearn import metrics
import argparse
import json

from .trnFun import check_int, check_strictly_positive, check_positive, check_bool, get_data, get_data_class, output_data, compute_first_hidden_layer, output_stats, check_parameters_common, check_parameters_dimlp_layer, validate_string_param
from .parameters import get_common_parser, get_initial_parser, get_args, sanitizepath, CustomArgumentParser, CustomHelpFormatter, TaggableAction, int_type, float_type, bool_type, enum_type

warnings.filterwarnings("ignore", category=ConvergenceWarning)

def mlpTrn(*args, **kwargs):
    try:

        if args and isinstance(args[0], str):
            split_args = args[0].split()  # string command to list
        else:
            split_args = args

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
            print("weights_outfile : output weights file name (weights.wts by default)")
            print("stats_file : output file name with train and test accuracy (stats.txt by default)")
            print("console_file : file with console logs redirection")
            print("nb_quant_levels : number of stairs in staircase activation function (50 by default)")
            print("K : Parameter to improve dynamics (1 by default)")
            print("----------------------------")
            print("MLP parameters (optional)")
            print("hidden_layer_sizes : Size of each hidden layers. Array of shape (n_layers-2) ((100,) by default)")
            print("activation : activation function, identity, logistic, tanh or relu(default)")
            print("solver : solver for weight optimization, lbfgs, sgd or adam (default)")
            print("alpha : strength of the L2 regularization term, positive float (0.0001 by default)")
            print("batch_size : size of minibatches for stochastic optimizers for adam and sgd, auto(default) or strictly positive integer")
            print("learning_rate : learning rate schedule for weight updates for sgd solver, constant(default), invscaling or adaptive")
            print("learning_rate_init : initial learning rate for adam and sgd, strictly positive float (0.001 by default)")
            print("power_t : exponent for inverse scaling learning rate for sgd, positive float (0.5 by default)")
            print("max_iterations : maximum number of iterations, strictly positive integer (200 by default)")
            print("shuffle : whether to shuffle samples in each iteration for sgd and adam, True(default) or False")
            print("seed : positive integer(seed) or None(default) for sgd and adam")
            print("tol : tolerance for optimization (0.0001 by default)")
            print("verbose : enable verbose output, True or False(default)")
            print("warm_start : whether to reuse previous solution to fit initialization, True or False(default)")
            print("momentum : Momentum for gradient descent update for sgd, between 0 and 1 (0.9 by default)")
            print("nesterovs_momentum : whether to use Nesterov’s momentum for sgd and momentum > 0, True(default) or False")
            print("early_stopping : whether to use early stopping to terminate training when validation score is not improving for sgd and adam, True or False(default)")
            print("validation_fraction : proportion of training data to set aside as validation set for early stopping, between 0 and 1 excluded (0.1 by default)")
            print("beta_1 : exponential decay rate for estimates of first moment vector in adam, between 0 and 1 excluded (0.9 by default)")
            print("beta_2 : exponential decay rate for estimates of second moment vector in adam, between 0 and 1 excluded (0.999 by default)")
            print("epsilon : value for numerical stability in adam, strictly positive float (1e-8 by default)")
            print("n_iter_no_change : maximum number of epochs to not meet tol improvement for sgd and adam, integer >= 1 (10 by default)")
            print("max_fun : maximum number of loss function calls for lbfgs, integer >= 1 (15000 by default)")
            print("----------------------------")
            print("----------------------------")
            print("Here is an example, keep same parameter names :")
            print('mlpTrn(train_data_file="datanormTrain",train_class_file="dataclass2Train", test_data_file="datanormTest",test_class_file="dataclass2Test", weights_outfile = "mlp/weights", stats_file = "mlp/stats.txt", train_pred_outfile = "mlp/predTrain", test_pred_outfile = "mlp/predTest", nb_attributes=16, nb_classes=2, root_folder = "dimlp/datafiles")')
            print("---------------------------------------------------------------------")


        else:

            start_time = time.time()

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
            weights_outfile = kwargs.get('weights_outfile')
            stats_file = kwargs.get('stats_file')

            K = kwargs.get('K')
            quant = kwargs.get('nb_quant_levels')
            hiknot = 5

            hidden_layer_sizes_var = kwargs.get('hidden_layer_sizes')
            activation_var = kwargs.get('activation')
            solver_var = kwargs.get('solver')
            alpha_var = kwargs.get('alpha')
            batch_size_var = kwargs.get('batch_size')
            learning_rate_var = kwargs.get('learning_rate')
            learning_rate_init_var = kwargs.get('learning_rate_init')
            power_t_var = kwargs.get('power_t')
            max_iterations_var = kwargs.get('max_iterations')
            shuffle_var = kwargs.get('shuffle')
            seed_var = kwargs.get('seed')
            tol_var = kwargs.get('tol')
            verbose_var = kwargs.get('verbose')
            warm_start_var = kwargs.get('warm_start')
            momentum_var = kwargs.get('momentum')
            nesterovs_momentum_var = kwargs.get('nesterovs_momentum')
            early_stopping_var = kwargs.get('early_stopping')
            validation_fraction_var = kwargs.get('validation_fraction')
            beta_1_var = kwargs.get('beta_1')
            beta_2_var = kwargs.get('beta_2')
            epsilon_var = kwargs.get('epsilon')
            n_iter_no_change_var = kwargs.get('n_iter_no_change')
            max_fun_var = kwargs.get('max_fun')

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

            valid_args = ['train_data_file', 'train_class_file', 'test_data_file', 'test_class_file', 'train_pred_outfile', 'test_pred_outfile', 'nb_attributes', 'nb_classes', 'weights_outfile',
                        'stats_file', 'K', 'nb_quant_levels', 'root_folder', 'console_file', 'hidden_layer_sizes', 'activation', 'solver', 'alpha', 'batch_size', 'learning_rate',
                        'learning_rate_init', 'power_t', 'max_iterations', 'shuffle', 'seed', 'tol', 'verbose', 'warm_start', 'momentum', 'nesterovs_momentum', 'early_stopping',
                        'validation_fraction', 'beta_1', 'beta_2', 'epsilon', 'n_iter_no_change', 'max_fun']

            # Check if wrong parameters are given
            for arg_key in kwargs.keys():
                if arg_key not in valid_args:
                    raise ValueError(f"Invalid argument : {arg_key}.")

            root_folder, train_data_file, test_data_file, train_pred_outfile, test_pred_outfile, stats_file, nb_attributes, nb_classes = check_parameters_common(root_folder, train_data_file, test_data_file, train_pred_outfile, test_pred_outfile, stats_file, nb_attributes, nb_classes)
            weights_outfile, K, quant = check_parameters_dimlp_layer(weights_outfile, K, quant)

            if hidden_layer_sizes_var is None:
                hidden_layer_sizes_var = (100,)
            elif not isinstance(hidden_layer_sizes_var, tuple) and not isinstance(hidden_layer_sizes_var, list) and not check_strictly_positive(hidden_layer_sizes_var):
                raise ValueError('Error, parameter hidden_layer_sizes is not an array or a strictly positive number.')

            if activation_var is None:
                activation_var = "relu"
            else:
                activations = {"identity", "logistic", "tanh", "relu"}
                if (activation_var not in activations):
                    raise ValueError('Error, parameter activation is not identity, logistic, tanh or relu.')

            if solver_var is None:
                solver_var = "adam"
            else:
                solvers = {"lbfgs", "sgd", "adam"}
                if (solver_var not in solvers):
                    raise ValueError('Error, parameter solver is not lbfgs, sgd or adam.')

            if alpha_var is None:
                alpha_var = 0.0001
            elif not check_positive(alpha_var):
                raise ValueError('Error, parameter alpha is not a positive number.')

            if batch_size_var is None:
                batch_size_var = "auto"
            elif batch_size_var != "auto" and (not check_strictly_positive(batch_size_var) or not check_int(batch_size_var)):
                raise ValueError('Error, parameter batch_size is not auto or a strictly positive integer.')

            if learning_rate_var is None:
                learning_rate_var = "constant"
            else:
                learning_rates = {"constant", "invscaling", "adaptive"}
                if (learning_rate_var not in learning_rates):
                    raise ValueError('Error, parameter learning_rate is not constant, invscaling or adaptive')

            if learning_rate_init_var is None:
                learning_rate_init_var = 0.001
            elif not check_strictly_positive(learning_rate_init_var):
                raise ValueError('Error, parameter learning_rate_init is not a strictly positive number.')

            if power_t_var is None:
                power_t_var = 0.5
            elif not check_positive(power_t_var):
                raise ValueError('Error, parameter power_t is not a positive number.')

            if max_iterations_var is None:
                max_iterations_var = 200
            elif not check_strictly_positive(max_iterations_var) or not check_int(max_iterations_var) :
                raise ValueError('Error, parameter max_iterations is not a strictly positive integer.')

            if shuffle_var is None:
                shuffle_var = True
            elif not check_bool(shuffle_var) :
                raise ValueError('Error, parameter shuffle is not a boolean.')

            if seed_var is not None and (not check_int(seed_var) or not check_positive(seed_var)):
                raise ValueError('Error, parameter seed is specified and not a positive integer.')

            if tol_var is None:
                tol_var = 0.0001
            elif not check_strictly_positive(tol_var):
                raise ValueError('Error, parameter tol is not a strictly positive number.')

            if verbose_var is None:
                verbose_var = False
            elif not check_bool(verbose_var):
                raise ValueError('Error, parameter verbose is not boolean.')

            if warm_start_var is None:
                warm_start_var = False
            elif not check_bool(warm_start_var):
                raise ValueError('Error, parameter warm_start is not boolean.')

            if momentum_var is None:
                momentum_var = 0.9
            elif not check_positive(momentum_var) or momentum_var > 1:
                raise ValueError('Error, parameter momentum is not a number between 0 and 1.')

            if nesterovs_momentum_var is None:
                nesterovs_momentum_var = True
            elif not check_bool(nesterovs_momentum_var):
                raise ValueError('Error, parameter nesterovs_momentum is not boolean.')

            if early_stopping_var is None:
                early_stopping_var = False
            elif not check_bool(early_stopping_var):
                raise ValueError('Error, parameter early_stopping is not boolean.')

            if validation_fraction_var is None:
                validation_fraction_var = 0.1
            elif not check_positive(validation_fraction_var) or validation_fraction_var >= 1:
                raise ValueError('Error, parameter validation_fraction is not a number between 0 and 1 excluded.')

            if beta_1_var is None:
                beta_1_var = 0.9
            elif not check_positive(beta_1_var) or beta_1_var >= 1:
                raise ValueError('Error, parameter beta_1 is not a number between 0 and 1 excluded.')

            if beta_2_var is None:
                beta_2_var = 0.999
            elif not check_positive(beta_2_var) or beta_2_var >= 1:
                raise ValueError('Error, parameter beta_2 is not a number between 0 and 1 excluded.')

            if epsilon_var is None:
                epsilon_var = 0.00000001
            elif not check_strictly_positive(epsilon_var):
                raise ValueError('Error, parameter epsilon is not a strictly positive number.')

            if n_iter_no_change_var is None:
                n_iter_no_change_var = 10
            elif not check_int(n_iter_no_change_var) or n_iter_no_change_var < 1:
                raise ValueError('Error, parameter n_iter_no_change is not an integer bigger than 0.')

            if max_fun_var is None:
                max_fun_var = 15000
            elif not check_int(max_fun_var) or max_fun_var < 1:
                raise ValueError('Error, parameter max_fun is not an integer bigger than 0.')

            if (root_folder is not None):
                train_data_file = root_folder + "/" + train_data_file
                test_data_file = root_folder + "/" + test_data_file
                train_pred_outfile = root_folder + "/" + train_pred_outfile
                test_pred_outfile = root_folder + "/" + test_pred_outfile
                weights_outfile = root_folder + "/" + weights_outfile
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

            # Get weights and biais from first hidden layer as well as data transformed in first hidden layer

            train_data_h1, mu, sigma = compute_first_hidden_layer("train", train_data, K, quant, hiknot, weights_outfile)
            test_data_h1 = compute_first_hidden_layer("test", test_data, K, quant, hiknot, mu=mu, sigma=sigma)

            # Train mlp
            model = MLPClassifier(hidden_layer_sizes = hidden_layer_sizes_var, activation = activation_var, solver = solver_var, alpha = alpha_var, batch_size = batch_size_var,
                                  learning_rate = learning_rate_var, learning_rate_init = learning_rate_init_var, power_t = power_t_var, max_iter = max_iterations_var, shuffle = shuffle_var,
                                  random_state = seed_var, tol = tol_var, verbose = verbose_var, warm_start = warm_start_var, momentum = momentum_var, nesterovs_momentum = nesterovs_momentum_var,
                                  early_stopping = early_stopping_var, validation_fraction = validation_fraction_var, beta_1 = beta_1_var, beta_2 = beta_2_var, epsilon = epsilon_var,
                                  n_iter_no_change = n_iter_no_change_var, max_fun = max_fun_var)

            model.fit(train_data_h1, train_class)    # Train the model using the training sets

            # Compute predictions
            train_pred_proba = model.predict_proba(train_data_h1)  # Predict the response for train dataset
            test_pred_proba = model.predict_proba(test_data_h1)    # Predict the response for test dataset
            train_pred = model.predict(train_data_h1)
            test_pred = model.predict(test_data_h1)

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

            # Calcul de l'exactitude (accuracy) du modèle
            acc_train = metrics.accuracy_score(train_class, train_pred) * 100
            acc_test = metrics.accuracy_score(test_class, test_pred) * 100
            acc_train = "{:.6f}".format(acc_train).rstrip("0").rstrip(".")
            acc_test = "{:.6f}".format(acc_test).rstrip("0").rstrip(".")
            print("Training accuracy : {}%.".format(acc_train))
            print("Testing accuracy : {}%.".format(acc_test))

            # Output accuracy statistics
            output_stats(stats_file, acc_train, acc_test)

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

    """except SystemExit:
        # Specific handling of systemExit
        return -1"""
