import numpy as np
from .stairObj import StairObj
from sklearn import svm
from sklearn import metrics

def check_strictly_positive(variable):
    if isinstance(variable, (float,int)) and variable > 0:
        return True
    return False

def check_strictly_positive_int(variable):
    if isinstance(variable, int) and variable > 0:
        return True
    return False

def check_int(variable):
    if isinstance(variable, int):
        return True
    return False

def check_positive_int(variable):
    if isinstance(variable, int) and variable >= 0:
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

def compute_first_hidden_layer(input_data, k, nb_stairs, hiknot):
    mu = np.mean(input_data, axis=0) # mean over variables
    sigma = np.std(input_data, axis=0)
    weights = k/sigma
    biais = -k*mu/sigma
    h = k*(input_data-mu)/sigma # With indices : hij=K*(xij-muj)/sigmaj
    stair = StairObj(nb_stairs, hiknot)

    output_data = [[stair.funct(d) for d in row] for row in h]

    return output_data, weights, biais

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
            print("----------------------------")
            print("Optional parameters :")
            print("test_class : test class file")
            print("nb_stairs : number of stairs in staircase activation function (50 by default)")
            print("hiknot : high side of the interval (5 by default)");
            print("K : Parameter to improve dynamics (1 by default)")
            print("----------------------------")
            print("SVM parameters (optional)")
            print("C : regularization, (1.0 by default)")
            print("kernel : linear, poly, rbf(default), precomputed")
            print("degree : polynomial degree (3 by default)")
            print("gamma : scale(default), auto or non negative float")
            print("shrinking : heuristic, true(default) or false")
            print("tol : tolerance for stopping criterion (0.001 by default)")
            print("cache_size : kernel cache size (200 MB by default)")
            print("max_iter : maximal number of iterations (-1 for no limit (default))")
            print("----------------------------")
            print("----------------------------")
            print("Here is an example, keep same parameter names :")
            print('svmTrn(trainData="trainDataFile.txt", testData="testDataFile.txt")')
            print("---------------------------------------------------------------------")
        else:

            # Get parameters
            train_data_file = kwargs.get('train_data')
            train_class_file = kwargs.get('train_class')
            test_data_file = kwargs.get('test_data')
            test_class_file = kwargs.get('test_class')
            K = kwargs.get('K')
            quant = kwargs.get('nb_stairs')
            hiknot = kwargs.get('hiknot')
            C_var = kwargs.get('C')
            kernel_var = kwargs.get('kernel')
            degree_var = kwargs.get('degree')
            gamma_var = kwargs.get('gamma')
            shrinking_var = kwargs.get('shrinking')
            tol_var = kwargs.get('tol')
            cache_size_var = kwargs.get('cache_size')
            max_iter_var = kwargs.get('max_iter')

            # Check parameters

            if train_data_file is None :
                print('Error : train data file missing, add it with option train_data="your_train_data_file"')
                return
            else :
                if not isinstance(train_data_file, str):
                    print('Error : train data file has to be a name contained in quotation marks "".')
                    return

            if train_class_file is None :
                print('Error : train class file missing, add it with option train_class="your_train_class_file"')
                return
            else :
                if not isinstance(train_class_file, str):
                    print('Error : train class file has to be a name contained in quotation marks "".')
                    return

            if test_data_file is None :
                print('Error : test data file missing, add it with option test_data="your_test_data_file"')
                return
            else :
                if not isinstance(test_data_file, str):
                    print('Error : test data file has to be a name contained in quotation marks "".')
                    return

            if test_class_file is not None and not isinstance(test_class_file, str):
                    print('Error : test class file has to be a name contained in quotation marks "".')
                    return

            if K is None:
                K = 1
            else:
                if (not check_strictly_positive(K)):
                    print('Error, parameter K is not a strictly positive number')
                    return

            if quant is None:
                quant = 50
            else:
                if (not check_strictly_positive(quant)):
                    print('Error, parameter quant is not a strictly positive number')
                    return

            if hiknot is None:
                hiknot = 5
            else:
                if (not check_int(hiknot)):
                    print('Error, parameter hiknot is not a number')
                    return

            if C_var is None:
                C_var = 1.0
            else:
                if (not check_strictly_positive(C_var)):
                    print('Error, parameter C is not a strictly positive number')
                    return

            if kernel_var is None:
                kernel_var = "rbf"
            else:
                kernels = {"linear", "poly", "rbf", "sigmoid", "precomputed"}
                if (kernel_var not in kernels):
                    print('Error, parameter kernel is not linear, poly, rbf, sigmoid or precomputed')
                    return

            if degree_var is None:
                degree_var = 3
            else:
                if (not check_positive_int(degree_var)):
                    print('Error, parameter degree is not positive integer')
                    return

            if gamma_var is None:
                gamma_var = "scale"
            else:
                gammas = {"scale", "auto"}
                if (gamma_var not in gammas and not check_positive(gamma_var)):
                    print('Error, parameter gamma is not scale, auto or positive float')
                    return

            if shrinking_var is None:
                shrinking_var = True
            else:
                if (not check_bool(shrinking_var)):
                    print('Error, parameter shrinking is not boolean')
                    return

            if tol_var is None:
                tol_var = 0.001
            else:
                if (not isinstance(tol_var, float)):
                    print('Error, parameter tol is not a float')
                    return

            if cache_size_var is None:
                cache_size_var = 0.001
            else:
                if (not check_strictly_positive(cache_size_var)):
                    print('Error, parameter cache_size is not a positive number')
                    return

            if max_iter_var is None:
                max_iter_var = -1
            else:
                if (not check_strictly_positive_int(max_iter_var) and max_iter_var != -1):
                    print('Error, parameter max_iter is not a strictly positive number or -1')
                    return

            # Get data
            train_data = get_data(train_data_file)
            train_class = get_data(train_class_file)
            train_class = [cl.index(max(cl)) for cl in train_class]
            test_data = get_data(test_data_file)
            if (test_class_file is not None):
                test_class = get_data(test_class_file)
                test_class = [cl.index(max(cl)) for cl in test_class]

            # Get weights and biais from first hidden layer as well as data transformed in first hidden layer

            train_data_h1, train_weights_h1, train_biais_h1 = compute_first_hidden_layer(train_data, K, quant, hiknot)
            test_data_h1, _, _ = compute_first_hidden_layer(test_data, K, quant, hiknot)

            # Train svm
            model = svm.SVC(C = C_var, kernel = kernel_var, degree = degree_var, gamma = gamma_var, shrinking = shrinking_var, tol = tol_var, cache_size = cache_size_var, max_iter = max_iter_var) # Create svm Classifier with linear Kernel
            model.fit(train_data_h1, train_class)   # Train the model using the training sets
            weights = model.coef_
            bias = model.intercept_
            train_pred = model.predict(train_data_h1)    # Predict the response for train dataset
            test_pred = model.predict(test_data_h1)    # Predict the response for test dataset

            # Compute model Accuracy
            print("Training accuracy :", metrics.accuracy_score(train_class, train_pred)*100, "%")
            print("Testing accuracy :", metrics.accuracy_score(test_class, test_pred)*100, "%")

    except ValueError as error:
        print(error)



#Paramètres :
# kernel : linear, poly, rbf, sigmoid, precomputed, default : rbf
# degree : integer (>=0), default : 3
# gamma : scale, auto or a float (>=0), default : scale
# C (regularization) : flaot > 0, default : 1.0
#
