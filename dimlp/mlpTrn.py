import numpy as np
import os
from .stairObj import StairObj
from sklearn.neural_network import MLPClassifier
from sklearn import metrics
import sys
import time
import warnings
from sklearn.exceptions import ConvergenceWarning

warnings.filterwarnings("ignore", category=ConvergenceWarning)

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

def compute_first_hidden_layer(input_data, k, nb_stairs, hiknot, weights_file):
    mu = np.mean(input_data, axis=0) # mean over variables
    sigma = np.std(input_data, axis=0)
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

    output_data = [[stair.funct(d) for d in row] for row in h]

    return output_data

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

            # Check parameters

            valid_args = ['train_data', 'train_class', 'test_data', 'test_class', 'train_pred', 'test_pred', 'weights',
                        'stats', 'K', 'nb_stairs', 'hiknot', 'save_folder', 'output_file', 'hidden_layer_sizes', 'activation', 'solver', 'alpha', 'batch_size', 'learning_rate',
                        'learning_rate_init', 'power_t', 'max_iter', 'shuffle', 'random_state', 'tol', 'verbose', 'warm_start', 'momentum']

            # Check if wrong parameters are given
            for arg_key in kwargs.keys():
                if arg_key not in valid_args:
                    raise ValueError(f"Invalid argument : {arg_key}")

            if (save_folder is not None and (not isinstance(save_folder, str))):
                raise ValueError('Error : parameter save_folder has to be a name contained in quotation marks "".')

            if train_data_file is None :
                raise ValueError('Error : train data file missing, add it with option train_data="your_train_data_file"')
            elif not isinstance(train_data_file, str):
                raise ValueError('Error : parameter train_data_file has to be a name contained in quotation marks "".')

            if train_class_file is None :
                raise ValueError('Error : train class file missing, add it with option train_class="your_train_class_file"')
            elif not isinstance(train_class_file, str):
                raise ValueError('Error : parameter train_class_file has to be a name contained in quotation marks "".')

            if test_data_file is None :
                raise ValueError('Error : test data file missing, add it with option test_data="your_test_data_file"')
            elif not isinstance(test_data_file, str):
                raise ValueError('Error : parameter test_data_file has to be a name contained in quotation marks "".')

            if test_class_file is None :
                raise ValueError('Error : test class file missing, add it with option test_class="your_test_class_file"')
            elif not isinstance(test_class_file, str):
                raise ValueError('Error : parameter test_class_file has to be a name contained in quotation marks "".')

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

            if weights_file is None:
                weights_file = "weights"
            elif not isinstance(weights_file, str):
                    raise ValueError('Error : parameter weights has to be a name contained in quotation marks "".')
            weights_file += ".wts"

            if stats_file is not None and not isinstance(stats_file, str):
                raise ValueError('Error : parameter stats_file has to be a name contained in quotation marks "".')

            if K is None:
                K = 1
            elif not check_strictly_positive(K):
                raise ValueError('Error, parameter K is not a strictly positive number')

            if quant is None:
                quant = 50
            elif not check_strictly_positive(quant):
                raise ValueError('Error, parameter quant is not a strictly positive number')

            if hiknot is None:
                hiknot = 5
            elif not check_int(hiknot):
                raise ValueError('Error, parameter hiknot is not a number')

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




            # Get weights and biais from first hidden layer as well as data transformed in first hidden layer

            train_data_h1 = compute_first_hidden_layer(train_data, K, quant, hiknot, weights_file)
            test_data_h1 = compute_first_hidden_layer(test_data, K, quant, hiknot, weights_file)

            # Train mlp
            """model = MLPClassifier(hidden_layer_sizes=(100,), activation='relu', solver='adam', alpha=0.0001,
                      batch_size='auto', learning_rate='constant', learning_rate_init=0.001,
                      power_t=0.5, max_iter=200, shuffle=True, random_state=None, tol=0.0001,
                      verbose=False, warm_start=False, momentum=0.9, nesterovs_momentum=True,
                      early_stopping=False, validation_fraction=0.1, beta_1=0.9, beta_2=0.999,
                      epsilon=1e-08, n_iter_no_change=10, max_fun=15000)"""
            model = MLPClassifier(hidden_layer_sizes = hidden_layer_sizes_var, activation = activation_var, solver = solver_var, alpha = alpha_var, batch_size = batch_size_var,
                                  learning_rate = learning_rate_var, learning_rate_init = learning_rate_init_var, power_t = power_t_var, max_iter = max_iter_var, shuffle = shuffle_var,
                                  random_state = random_state_var, tol = tol_var, verbose = verbose_var, warm_start = warm_start_var, momentum = momentum_var)

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
