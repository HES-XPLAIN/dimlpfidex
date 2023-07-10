import time
import sys
from sklearn.neural_network import MLPClassifier
from sklearn import metrics
import warnings
from sklearn.exceptions import ConvergenceWarning

from .trnFun import check_int, check_strictly_positive, check_positive, check_bool, get_data, output_pred_proba, compute_first_hidden_layer, output_stats, check_parameters

warnings.filterwarnings("ignore", category=ConvergenceWarning)

def mlpTrn(*args, **kwargs):
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
            print("weights : output weights file name without extension (weights by default)")
            print("stats : output file name without extension with train and test accuracy")
            print("output_file : file where you redirect console result")
            print("nb_stairs : number of stairs in staircase activation function (50 by default)")
            print("hiknot : high side of the interval (5 by default)")
            print("K : Parameter to improve dynamics (1 by default)")
            print("----------------------------")
            print("MLP parameters (optional)")
            print("hidden_layer_sizes : Size of each hidden layers. Array of shape (n_layers-2) ((100,) by default)")
            print("activation : activation function, identity, logistic, tanh or relu(default)")
            print("solver : solver for weight optimization, lbfgs, sgd or adam (default)")
            print("alpha : strength of the L2 regularization term, positive float (0.0001 by default)")
            print("batch_size : size of minibatches for stochastic optimizers for adam and sgd, auto(default) or positive integer")
            print("learning_rate : learning rate schedule for weight updates for sgd solver, constant(default), invscaling or adaptive")
            print("learning_rate_init : initial learning rate for adam and sgd, positive float (0.001 by default)")
            print("power_t : exponent for inverse scaling learning rate for sgd, positive float (0.5 by default)")
            print("max_iter : maximum number of iterations, positive integer (200 by default)")
            print("shuffle : whether to shuffle samples in each iteration for sgd and adam, True(default) or False")
            print("random_state : positive integer(seed) or None(default) for sgd and adam")
            print("tol : tolerance for optimization (0.0001 by default)")
            print("verbose : enable verbose output, True or False(default)")
            print("warm_start : whether to reuse previous solution to fit initialization, True or False(default)")
            print("momentum : Momentum for gradient descent update for sgd, between 0 and 1 (0.9 by default)")
            print("nesterovs_momentum : whether to use Nesterov’s momentum for sgd and momentum > 0, True(default) or False")
            print("early_stopping : whether to use early stopping to terminate training when validation score is not improving for sgd and adam, True or False(default)")
            print("validation_fraction : proportion of training data to set aside as validation set for early stopping, between 0 and 1 excluded (0.1 by default)")
            print("beta_1 : exponential decay rate for estimates of first moment vector in adam, between 0 and 1 excluded (0.9 by default)")
            print("beta_2 : exponential decay rate for estimates of second moment vector in adam, between 0 and 1 excluded (0.999 by default)")
            print("epsilon : value for numerical stability in adam, positive float (1e-8 by default)")
            print("n_iter_no_change : maximum number of epochs to not meet tol improvement for sgd and adam, integer >= 1 (10 by default)")
            print("max_fun : maximum number of loss function calls for lbfgs, integer >= 1 (15000 by default)")
            print("----------------------------")
            print("----------------------------")
            print("Here is an example, keep same parameter names :")
            print('mlpTrn(train_data="datanormTrain",train_class="dataclass2Train", test_data="datanormTest",test_class="dataclass2Test", weights = "mlp/weights", stats = "mlp/stats.txt", train_pred = "mlp/predTrain", test_pred = "mlp/predTest", save_folder = "dimlp/datafiles")')
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
            weights_file = kwargs.get('weights')
            stats_file = kwargs.get('stats')

            K = kwargs.get('K')
            quant = kwargs.get('nb_stairs')
            hiknot = kwargs.get('hiknot')

            hidden_layer_sizes_var = kwargs.get('hidden_layer_sizes')
            activation_var = kwargs.get('activation')
            solver_var = kwargs.get('solver')
            alpha_var = kwargs.get('alpha')
            batch_size_var = kwargs.get('batch_size')
            learning_rate_var = kwargs.get('learning_rate')
            learning_rate_init_var = kwargs.get('learning_rate_init')
            power_t_var = kwargs.get('power_t')
            max_iter_var = kwargs.get('max_iter')
            shuffle_var = kwargs.get('shuffle')
            random_state_var = kwargs.get('random_state')
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

            valid_args = ['train_data', 'train_class', 'test_data', 'test_class', 'train_pred', 'test_pred', 'weights',
                        'stats', 'K', 'nb_stairs', 'hiknot', 'save_folder', 'output_file', 'hidden_layer_sizes', 'activation', 'solver', 'alpha', 'batch_size', 'learning_rate',
                        'learning_rate_init', 'power_t', 'max_iter', 'shuffle', 'random_state', 'tol', 'verbose', 'warm_start', 'momentum', 'nesterovs_momentum', 'early_stopping',
                        'validation_fraction', 'beta_1', 'beta_2', 'epsilon', 'n_iter_no_change', 'max_fun']

            # Check if wrong parameters are given
            for arg_key in kwargs.keys():
                if arg_key not in valid_args:
                    raise ValueError(f"Invalid argument : {arg_key}")

            save_folder, train_data_file, train_class_file, test_data_file, test_class_file, train_pred_file, test_pred_file, weights_file, stats_file, K, quant, hiknot = check_parameters(save_folder, train_data_file, train_class_file, test_data_file, test_class_file, train_pred_file, test_pred_file, weights_file, stats_file, K, quant, hiknot)

            if hidden_layer_sizes_var is None:
                hidden_layer_sizes_var = (100,)
            elif not isinstance(hidden_layer_sizes_var, tuple) and not isinstance(hidden_layer_sizes_var, list) and not check_strictly_positive(hidden_layer_sizes_var):
                raise ValueError('Error, parameter hidden_layer_sizes is not an array or a strictly positive number')

            if activation_var is None:
                activation_var = "relu"
            else:
                activations = {"identity", "logistic", "tanh", "relu"}
                if (activation_var not in activations):
                    raise ValueError('Error, parameter activation is not identity, logistic, tanh or relu')

            if solver_var is None:
                solver_var = "adam"
            else:
                solvers = {"lbfgs", "sgd", "adam"}
                if (solver_var not in solvers):
                    raise ValueError('Error, parameter solver is not lbfgs, sgd or adam')

            if alpha_var is None:
                alpha_var = 0.0001
            elif not check_positive(alpha_var):
                raise ValueError('Error, parameter alpha is not a positive number')

            if batch_size_var is None:
                batch_size_var = "auto"
            elif batch_size_var != "auto" and (not check_strictly_positive(batch_size_var) or not check_int(batch_size_var)):
                raise ValueError('Error, parameter batch_size is not auto or a strictly positive integer')

            if learning_rate_var is None:
                learning_rate_var = "constant"
            else:
                learning_rates = {"constant", "invscaling", "adaptive"}
                if (learning_rate_var not in learning_rates):
                    raise ValueError('Error, parameter learning_rate is not constant, invscaling or adaptive')

            if learning_rate_init_var is None:
                learning_rate_init_var = 0.001
            elif not check_strictly_positive(learning_rate_init_var):
                raise ValueError('Error, parameter learning_rate_init is not a strictly positive number')

            if power_t_var is None:
                power_t_var = 0.5
            elif not check_positive(power_t_var):
                raise ValueError('Error, parameter power_t is not a positive number')

            if max_iter_var is None:
                max_iter_var = 200
            elif not check_strictly_positive(max_iter_var) or not check_int(max_iter_var) :
                raise ValueError('Error, parameter max_iter is not a strictly positive integer')

            if shuffle_var is None:
                shuffle_var = True
            elif not check_bool(shuffle_var) :
                raise ValueError('Error, parameter shuffle is not a boolean')

            if random_state_var is not None and (not check_int(random_state_var) or not check_positive(random_state_var)):
                raise ValueError('Error, parameter random_state is specified and not a positive integer')

            if tol_var is None:
                tol_var = 0.0001
            elif not check_strictly_positive(tol_var):
                raise ValueError('Error, parameter tol is not a strictly positive number')

            if verbose_var is None:
                verbose_var = False
            elif not check_bool(verbose_var):
                raise ValueError('Error, parameter verbose is not boolean')

            if warm_start_var is None:
                warm_start_var = False
            elif not check_bool(warm_start_var):
                raise ValueError('Error, parameter warm_start is not boolean')

            if momentum_var is None:
                momentum_var = 0.9
            elif not check_positive(momentum_var) or momentum_var > 1:
                raise ValueError('Error, parameter momentum is not a number between 0 and 1')

            if nesterovs_momentum_var is None:
                nesterovs_momentum_var = True
            elif not check_bool(nesterovs_momentum_var):
                raise ValueError('Error, parameter nesterovs_momentum is not boolean')

            if early_stopping_var is None:
                early_stopping_var = False
            elif not check_bool(early_stopping_var):
                raise ValueError('Error, parameter early_stopping is not boolean')

            if validation_fraction_var is None:
                validation_fraction_var = 0.1
            elif not check_positive(validation_fraction_var) or validation_fraction_var >= 1:
                raise ValueError('Error, parameter validation_fraction is not a number between 0 and 1 excluded')

            if beta_1_var is None:
                beta_1_var = 0.9
            elif not check_positive(beta_1_var) or beta_1_var >= 1:
                raise ValueError('Error, parameter beta_1 is not a number between 0 and 1 excluded')

            if beta_2_var is None:
                beta_2_var = 0.999
            elif not check_positive(beta_2_var) or beta_2_var >= 1:
                raise ValueError('Error, parameter beta_2 is not a number between 0 and 1 excluded')

            if epsilon_var is None:
                epsilon_var = 0.00000001
            elif not check_strictly_positive(epsilon_var):
                raise ValueError('Error, parameter epsilon is not a strictly positive number')

            if n_iter_no_change_var is None:
                n_iter_no_change_var = 10
            elif not check_int(n_iter_no_change_var) or n_iter_no_change_var < 1:
                raise ValueError('Error, parameter n_iter_no_change is not an integer bigger than 0')

            if max_fun_var is None:
                max_fun_var = 15000
            elif not check_int(max_fun_var) or max_fun_var < 1:
                raise ValueError('Error, parameter max_fun is not an integer bigger than 0')

            if (save_folder is not None):
                train_data_file = save_folder + "/" + train_data_file
                train_class_file = save_folder + "/" + train_class_file
                test_data_file = save_folder + "/" + test_data_file
                test_class_file = save_folder + "/" + test_class_file
                train_pred_file = save_folder + "/" + train_pred_file
                test_pred_file = save_folder + "/" + test_pred_file
                weights_file = save_folder + "/" + weights_file
                if (stats_file is not None):
                    stats_file = save_folder + "/" + stats_file

            # Get data
            train_data = get_data(train_data_file)
            train_class = get_data(train_class_file)
            train_class = [cl.index(max(cl)) for cl in train_class]
            test_data = get_data(test_data_file)
            test_class = get_data(test_class_file)
            test_class = [cl.index(max(cl)) for cl in test_class]

            # Get weights and biais from first hidden layer as well as data transformed in first hidden layer

            train_data_h1 = compute_first_hidden_layer(train_data, K, quant, hiknot, weights_file)
            test_data_h1 = compute_first_hidden_layer(test_data, K, quant, hiknot, weights_file)

            # Train mlp
            model = MLPClassifier(hidden_layer_sizes = hidden_layer_sizes_var, activation = activation_var, solver = solver_var, alpha = alpha_var, batch_size = batch_size_var,
                                  learning_rate = learning_rate_var, learning_rate_init = learning_rate_init_var, power_t = power_t_var, max_iter = max_iter_var, shuffle = shuffle_var,
                                  random_state = random_state_var, tol = tol_var, verbose = verbose_var, warm_start = warm_start_var, momentum = momentum_var, nesterovs_momentum = nesterovs_momentum_var,
                                  early_stopping = early_stopping_var, validation_fraction = validation_fraction_var, beta_1 = beta_1_var, beta_2 = beta_2_var, epsilon = epsilon_var,
                                  n_iter_no_change = n_iter_no_change_var, max_fun = max_fun_var)

            model.fit(train_data_h1, train_class)    # Train the model using the training sets

            # Compute predictions
            train_pred_proba = model.predict_proba(train_data_h1)  # Predict the response for train dataset
            test_pred_proba = model.predict_proba(test_data_h1)    # Predict the response for test dataset
            train_pred = model.predict(train_data_h1)
            test_pred = model.predict(test_data_h1)

            # Output predictions
            output_pred_proba(train_pred_proba, train_pred_file)
            output_pred_proba(test_pred_proba, test_pred_file)

            # Calcul de l'exactitude (accuracy) du modèle
            acc_train = metrics.accuracy_score(train_class, train_pred) * 100
            acc_test = metrics.accuracy_score(test_class, test_pred) * 100
            print("Training accuracy : {}%.".format(acc_train))
            print("Testing accuracy : {}%.".format(acc_test))

            # Output accuracy statistics
            output_stats(stats_file, acc_train, acc_test)

            end_time = time.time()
            full_time = end_time - start_time
            full_time = "{:.6f}".format(full_time).rstrip(".0")

            print(f"\nFull execution time = {full_time} sec")

            # Redirect output to terminal
            if output_file != None:
                sys.stdout = sys.__stdout__

            return 0

    except ValueError as error:
        print(error)
        return -1
