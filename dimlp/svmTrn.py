import numpy as np
import os
from .stairObj import StairObj
from sklearn import svm
from sklearn import metrics

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


def compute_first_hidden_layer(input_data, k, nb_stairs, hiknot, weights_file):
    mu = np.mean(input_data, axis=0) # mean over variables
    sigma = np.std(input_data, axis=0)
    weights = k/sigma
    biais = -k*mu/sigma

    # Output weights and biais
    try:
        with open(weights_file, "w") as my_file:
            for w in weights:
                 my_file.write(str(w))
                 my_file.write(" ")
            my_file.write("\n")
            for b in biais:
                 my_file.write(str(b))
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

def svmTrn(*args, **kwargs):
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
            print("nb_stairs : number of stairs in staircase activation function (50 by default)")
            print("hiknot : high side of the interval (5 by default)")
            print("K : Parameter to improve dynamics (1 by default)")
            print("----------------------------")
            print("SVM parameters (optional)")
            print("C : regularization, (1.0 by default)")
            print("kernel : linear, poly, rbf(default)")
            print("degree : polynomial degree (3 by default)")
            print("gamma : scale(default), auto or non negative float")
            print("coef0 : term in kernel function, float (0 by default)")
            print("shrinking : heuristic, true(default) or false")
            print("tol : tolerance for stopping criterion (0.001 by default)")
            print("cache_size : kernel cache size (200 MB by default)")
            print("class_weight : class balance, 'balanced' or a dictionary, for exemple with 2 classes : {0:1.2, 1:3.5} (None by default)")
            print("verbose : enable verbose output, True or False(default)")
            print("max_iter : maximal number of iterations (-1 for no limit (default))")
            print("decision_function_shape : decision function shape, ovo(one-vs-one) or ovr(one-vs-rest, default)")
            print("break_ties : break tie decision for ovr with more than 2 classes, True or False(default)")
            print("----------------------------")
            print("----------------------------")
            print("Here is an example, keep same parameter names :")
            print('svmTrn(trainData="trainDataFile.txt", testData="testDataFile.txt")')
            print("---------------------------------------------------------------------")
        else:

            # Get parameters
            save_folder = kwargs.get('save_folder')
            train_data_file = kwargs.get('train_data')
            train_class_file = kwargs.get('train_class')
            test_data_file = kwargs.get('test_data')
            test_class_file = kwargs.get('test_class')

            train_pred_file = kwargs.get('train_pred')
            test_pred_file = kwargs.get('test_pred')
            weights_file = kwargs.get('weights')
            stats_file = kwargs.get('stats')

            K = kwargs.get('K')
            quant = kwargs.get('nb_stairs')
            hiknot = kwargs.get('hiknot')
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
            max_iter_var = kwargs.get('max_iter')
            decision_function_shape_var = kwargs.get('decision_function_shape')
            break_ties_var = kwargs.get('break_ties')

            # Check parameters

            valid_args = ['train_data', 'train_class', 'test_data', 'test_class', 'train_pred', 'test_pred', 'weights',
                        'stats', 'K', 'nb_stairs', 'hiknot', 'C', 'kernel', 'degree', 'gamma', 'coef0', 'shrinking',
                        'tol', 'cache_size', 'class_weight', 'verbose', 'max_iter', 'decision_function_shape', 'break_ties', 'save_folder']

            # Check if wrong parameters are given
            for arg_key in kwargs.keys():
                if arg_key not in valid_args:
                    raise ValueError(f"Argument invalide : {arg_key}")

            if not isinstance(save_folder, str):
                raise ValueError('Error : save folder has to be a name contained in quotation marks "".')

            if train_data_file is None :
                raise ValueError('Error : train data file missing, add it with option train_data="your_train_data_file"')
            elif not isinstance(train_data_file, str):
                raise ValueError('Error : train data file has to be a name contained in quotation marks "".')

            if train_class_file is None :
                raise ValueError('Error : train class file missing, add it with option train_class="your_train_class_file"')
            elif not isinstance(train_class_file, str):
                raise ValueError('Error : train class file has to be a name contained in quotation marks "".')

            if test_data_file is None :
                raise ValueError('Error : test data file missing, add it with option test_data="your_test_data_file"')
            elif not isinstance(test_data_file, str):
                raise ValueError('Error : test data file has to be a name contained in quotation marks "".')

            if test_class_file is None :
                raise ValueError('Error : test class file missing, add it with option test_class="your_test_class_file"')
            elif not isinstance(test_class_file, str):
                raise ValueError('Error : test class file has to be a name contained in quotation marks "".')

            if train_pred_file is None:
                train_pred_file = "predTrain"
            elif not isinstance(train_pred_file, str):
                raise ValueError('Error : train prediction file has to be a name contained in quotation marks "".')
            train_pred_file += ".out"


            if test_pred_file is None:
                test_pred_file = "predTest"
            elif not isinstance(test_pred_file, str):
                raise ValueError('Error : test prediction file has to be a name contained in quotation marks "".')
            test_pred_file += ".out"

            if weights_file is None:
                weights_file = "weights"
            elif not isinstance(weights_file, str):
                    raise ValueError('Error : weights file has to be a name contained in quotation marks "".')
            weights_file += ".wts"

            if stats_file is not None and not isinstance(stats_file, str):
                raise ValueError('Error : stats file has to be a name contained in quotation marks "".')

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

            if c_var is None:
                c_var = 1.0
            elif not check_strictly_positive(c_var):
                raise ValueError('Error, parameter C is not a strictly positive number')

            if kernel_var is None:
                kernel_var = "rbf"
            else:
                kernels = {"linear", "poly", "rbf", "sigmoid"}
                if (kernel_var not in kernels):
                    raise ValueError('Error, parameter kernel is not linear, poly, rbf or sigmoid')

            if degree_var is None:
                degree_var = 3
            elif not check_positive(degree_var) or not check_int(degree_var):
                raise ValueError('Error, parameter degree is not positive integer')

            if gamma_var is None:
                gamma_var = "scale"
            else:
                gammas = {"scale", "auto"}
                if (gamma_var not in gammas and not check_positive(gamma_var)):
                    raise ValueError('Error, parameter gamma is not scale, auto or positive float')

            if coef0_var is None:
                coef0_var = 0.0
            if not isinstance(coef0_var, (float,int)):
                raise ValueError('Error, parameter coef0 is not a float')

            if shrinking_var is None:
                shrinking_var = True
            elif not check_bool(shrinking_var):
                raise ValueError('Error, parameter shrinking is not boolean')

            if tol_var is None:
                tol_var = 0.001
            elif not check_strictly_positive(tol_var):
                raise ValueError('Error, parameter tol is not a strictly positive float')

            if cache_size_var is None:
                cache_size_var = 0.001
            elif not check_strictly_positive(cache_size_var):
                raise ValueError('Error, parameter cache_size is not a positive number')

            if class_weight_var is not None and class_weight_var != "balanced" and not isinstance(class_weight_var, dict):
                raise ValueError('Error, parameter class_weight is not "balanced", a dictionary or None')

            if verbose_var is None:
                verbose_var = False
            elif not check_bool(verbose_var):
                raise ValueError('Error, parameter verbose is not boolean')

            if max_iter_var is None:
                max_iter_var = -1
            elif (not check_strictly_positive(max_iter_var) or not check_int(max_iter_var)) and max_iter_var != -1:
                raise ValueError('Error, parameter max_iter is not a strictly positive number or -1')

            if decision_function_shape_var is None:
                decision_function_shape_var = "ovr"
            else:
                decision_function_shapes = {"ovo", "ovr"}
                if decision_function_shape_var not in decision_function_shapes:
                    raise ValueError('Error, parameter decision_function_shape is not ovo or ovr')

            if break_ties_var is None:
                break_ties_var = False
            elif not check_bool(break_ties_var):
                raise ValueError('Error, parameter break_ties is not boolean')


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

            # Train svm
            model = svm.SVC(C = c_var, kernel = kernel_var, degree = degree_var, gamma = gamma_var, coef0 = coef0_var, shrinking = shrinking_var, tol = tol_var, cache_size = cache_size_var, class_weight = class_weight_var, verbose = verbose_var, max_iter = max_iter_var, decision_function_shape = decision_function_shape_var, break_ties = break_ties_var) # Create svm Classifier with linear Kernel
            model.fit(train_data_h1, train_class)   # Train the model using the training sets
            train_pred = model.predict(train_data_h1)    # Predict the response for train dataset
            test_pred = model.predict(test_data_h1)    # Predict the response for test dataset

            # Output predictions
            nb_classes = len(model.classes_)
            output_pred(train_pred, train_pred_file, nb_classes)
            output_pred(test_pred, test_pred_file, nb_classes)

            # Compute model Accuracy
            acc_train = metrics.accuracy_score(train_class, train_pred)*100
            acc_test = metrics.accuracy_score(test_class, test_pred)*100
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


    except ValueError as error:
        print(error)


# Exemple : svmTrn(train_data="datanormTrain",train_class="dataclass2Train", test_data="datanormTest",test_class="dataclass2Test", weights = "weights", stats = "stats.txt", save_folder = "dimlp/datafiles")