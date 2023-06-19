import numpy as np
from .stairObj import StairObj

def check_strictly_positive(variable):
    if isinstance(variable, (float,int)) and variable > 0:
        return True
    return False

def check_int(variable):
    if isinstance(variable, int) and variable > 0:
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
            print("hiknot : <high side of the interval (5 by default)>");
            print("K : Parameter to improve dynamics (1 by default)")
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
                if (not check_int(quant)):
                    print('Error, parameter hiknot is not a number')
                    return

            # Get data
            train_data = get_data(train_data_file)
            train_class = get_data(train_class_file)
            test_data = get_data(test_data_file)
            if (test_class_file is not None):
                test_class = get_data(test_class_file)

            # Get weights and biais from first hidden layer as well as data transformed in first hidden layer

            train_data_h1, train_weights_h1, train_biais_h1 = compute_first_hidden_layer(train_data, K, quant, hiknot)
            test_data_h1, test_weights_h1, test_biais_h1 = compute_first_hidden_layer(test_data, K, quant, hiknot)

    except ValueError as error:
        print(error)
