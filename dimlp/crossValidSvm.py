import os
import shutil
import sys
import platform

# import modules :

if sys.platform == 'win32':
    dir_path = os.path.abspath('dimlpfidex')
    os.add_dll_directory(dir_path)

from dimlpfidex import dimlp
from dimlpfidex import fidex
from dimlpfidex import fidexGlo
from dimlp.svmTrn import svmTrn



def create_or_clear_directory(folder_name):
    try:
        if not os.path.exists(folder_name):
            # Create folder if not exist
            os.makedirs(folder_name)
        else:
            # Empty the folder if already exists
            for filename in os.listdir(folder_name):
                file_path = os.path.join(folder_name, filename)
                if os.path.isfile(file_path):
                    os.remove(file_path)
                elif os.path.isdir(file_path):
                    # Delete recursively sub-folders
                    shutil.rmtree(file_path)
    except (OSError):
        raise ValueError(f"Error during the creation of the directory {folder_name}.")


def get_data(file_name): # Get data from file
    try:
        with open(file_name, "r") as my_file:
            data = []
            line = my_file.readline()
            while line:
                line = line.strip()  # Remove the line break at the end of the line
                if line.strip():
                    data.append(line)
                line = my_file.readline()
            my_file.close()
        return data
    except (FileNotFoundError):
        raise ValueError(f"Error : File {file_name} not found.")
    except (IOError):
        raise ValueError(f"Error : Couldn't open file {file_name}.")

def crossValidSvm(*args, **kwargs):
    try:
        if args or not kwargs:
            print("---------------------------------------------------------------------")
            print("Please specify arguments using named parameters.")
            print("Warning! The files are localised with respect to root folder dimlpfidex.")

            print("----------------------------")
            print("Obligatory parameters :")
            print("train_method : dimlp or svm")
            print("algo : fidex, fidexGlo or both")
            print("data_file : data file")
            print("class_file : class file")

            print("----------------------------")
            print("Obligatory parameters if training with dimlp :")
            print("nb_in : number of input neurons")
            print("nb_out : number of output neurons")

            print("----------------------------")
            print("Optional parameters :")
            print("save_folder : Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder.")
            print("crossVal_folder : Folder name where to save crossValidation data (CrossValidation by default)")
            print("K : K-fold cross-validation (10 by default)")
            print("N : number of times we do the cross-validation (10 by default)")
            print("fidexGlo_heuristic : 1: optimal fidexGlo(default), 2: fast fidexGlo 3: very fast fidexGlo")
            print("crossVal_stats : statistics of cross validation (crossValidationStats.txt by default)")
            print("attr_file : file of attributes")
            print("hiknot : high side of the interval (5 by default)")
            print("nb_stairs : number of stairs in staircase activation function (50 by default)")
            print("max_iter : maximum fidex and fidexGlo iteration number (100 by default)")
            print("min_cov : minimum fidex and fidexGlo covering number (2 by default)")
            print("dropout_dim : dimension dropout parameter for fidex and fidexGlo")
            print("dropout_hyp : hyperplan dropout parameter for fidex and fidexGlo")
            print("seed : 0 = random (default)")

            print("----------------------------")
            print("Optional parameters for dimlp training:")
            print("pretrained_weights : file of pretrained weights")
            print("H1 : number of neurons in the first hidden layer. If not specified this number will be equal to the number of input neurons.")
            print("Hk : number of neurons in the kth hidden layer.")
            print("eta : back-propagation learning parameter (0.1 by default)")
            print("mu : back-propagation momentum parameter (0.6 by default)")
            print("flat : back-propagation flat spot elimination parameter (0.01 by default)")
            print("err_thresh : error threshold (-1111111111.0 by default)")
            print("acc_thresh : accuracy threshold (11111111111111.0 by default)")
            print("delta_err : absolute difference error threshold (0 by default)")
            print("nb_epochs : number of train epochs (1500 by default)")
            print("show_err : number of train epochs to show error (10 by default)")

            print("----------------------------")
            print("Optional parameters for svm training:")
            print("svm_K : Parameter to improve dynamics (1 by default)")
            print("C : regularization, (1.0 by default)")
            print("kernel : linear, poly, rbf(default)")
            print("degree : polynomial degree (3 by default)")
            print("gamma : scale(default), auto or non negative float")
            print("coef0 : term in kernel function, float (0 by default)")
            print("shrinking : heuristic, True(default) or False")
            print("tol : tolerance for stopping criterion (0.001 by default)")
            print("cache_size : kernel cache size (200 MB by default)")
            print("class_weight : class balance, 'balanced' or a dictionary, for exemple with 2 classes : {0:1.2, 1:3.5} (None by default)")
            print("svm_max_iter : maximal number of iterations (-1 for no limit (default))")
            print("decision_function_shape : decision function shape, ovo(one-vs-one) or ovr(one-vs-rest, default)")
            print("break_ties : break tie decision for ovr with more than 2 classes, True or False(default)")
            print("----------------------------")
            print("----------------------------")
            print("Here is an example, keep same parameter names :")
            print('crossValidSvm(trainData="trainDataFile.txt", testData="testDataFile.txt")')
            print("---------------------------------------------------------------------")
        else:
            
            # Get parameters
            train_method = kwargs.get('train_method')
            algo = kwargs.get('algo')
            data_file = kwargs.get('data_file')
            class_file = kwargs.get('class_file')

            nb_in = kwargs.get('nb_in')
            nb_out = kwargs.get('nb_out')

            save_folder = kwargs.get('save_folder')
            crossval_folder = kwargs.get('crossVal_folder')
            k = kwargs.get('K')
            n = kwargs.get('N')
            fidexglo_heuristic = kwargs.get('fidexGlo_heuristic')
            crossval_stats = kwargs.get('crossVal_stats')
            attr_file = kwargs.get('attr_file')
            hiknot = kwargs.get('hiknot')
            nb_stairs = kwargs.get('nb_stairs')
            max_iter = kwargs.get('max_iter')
            min_cov = kwargs.get('min_cov')
            dropout_dim = kwargs.get('dropout_dim')
            dropout_hyp = kwargs.get('dropout_hyp')
            seed = kwargs.get('seed')

            pretrained_weights = kwargs.get('pretrained_weights')
            hk = {}
            for key, value in kwargs.items():
                if key.startswith('H') and key[1:].isdigit():
                    hk[key] = value
            eta = kwargs.get('eta')
            mu = kwargs.get('mu')
            flat = kwargs.get('flat')
            err_thresh = kwargs.get('err_thresh')
            acc_thresh = kwargs.get('acc_thresh')
            delta_err = kwargs.get('delta_err')
            nb_epochs = kwargs.get('nb_epochs')
            show_err = kwargs.get('show_err')

            svm_K = kwargs.get('svm_K')
            c_var = kwargs.get('C')
            kernel_var = kwargs.get('kernel')
            degree_var = kwargs.get('degree')
            gamma_var = kwargs.get('gamma')
            coef0_var = kwargs.get('coef0')
            shrinking_var = kwargs.get('shrinking')
            tol_var = kwargs.get('tol')
            cache_size_var = kwargs.get('cache_size')
            class_weight_var = kwargs.get('class_weight')
            svm_max_iter_var = kwargs.get('svm_max_iter')
            decision_function_shape_var = kwargs.get('decision_function_shape')
            break_ties_var = kwargs.get('break_ties')            

            # Check parameters

            obligatory_args = ['train_method', 'algo', 'data_file', 'class_file']
            obligatory_dimlp_args = ['nb_in', 'nb_out']

            optional_args = ['save_folder', 'crossVal_folder', 'K', 'N', 'fidexGlo_heuristic', 'crossVal_stats', 'attr_file', 'hiknot', 'nb_stairs',
                        'max_iter', 'min_cov', 'dropout_dim', 'dropout_hyp', 'seed']
            
            optional_dimlp_args = ['pretrained_weights', 'eta', 'mu', 'flat', 'err_thresh',
                        'acc_thresh', 'delta_err', 'nb_epochs', 'show_err']
            
            optional_svm_args = ['svm_K', 'C', 'kernel', 'degree', 'gamma', 'coef0', 'shrinking',
                        'tol', 'cache_size', 'class_weight', 'svm_max_iter', 'decision_function_shape', 'break_ties']

            # Check if wrong parameters are given

            train_methods = {"dimlp", "svm"}
            if train_method is None:
                raise ValueError('Error : train method is missing, add it with option train_method="dimlp" or "svm"')
            elif (train_method not in train_methods):
                raise ValueError('Error, parameter train_method is not dimlp or svm')

            for arg_key in kwargs.keys():
                if (train_method == "dimlp"):
                    if (arg_key not in optional_args and arg_key not in optional_dimlp_args and not(arg_key.startswith('H') and arg_key[1:].isdigit()) and arg_key not in obligatory_args and arg_key not in obligatory_dimlp_args):
                        raise ValueError(f"Invalid argument with dimlp training : {arg_key}")
                else:
                    if (arg_key not in optional_args and arg_key and arg_key not in optional_svm_args and arg_key not in obligatory_args):
                        raise ValueError(f"Invalid argument with dimlp training : {arg_key}")

            
            algos = {"fidex", "fidexGlo", "both"}
            if algo is None:
                raise ValueError('Error : algorithm(s) is missing, add it with option algo="fidex" or "fidexGlo" or "both"')
            elif (algo not in algos):
                raise ValueError('Error, parameter algo is not fidex, fidexGlo or both')
            
            if data_file is None:
                raise ValueError('Error : data file is missing, add it with option data_file="your_data_file_name"')
            elif not isinstance(data_file, str):
                raise ValueError('Error : parameter data_file has to be a name contained in quotation marks "".')
            
            if class_file is None:
                raise ValueError('Error : class file is missing, add it with option class_file="your_class_file_name"')
            elif not isinstance(class_file, str):
                raise ValueError('Error : parameter class_file has to be a name contained in quotation marks "".')

            if (save_folder is not None and (not isinstance(save_folder, str))):
                raise ValueError('Error, parameter save_folder has to be a name contained in quotation marks "".')

            if crossval_folder is None:
                crossval_folder = "CrossValidation"
            elif not isinstance(crossval_folder, str):
                raise ValueError('Error, parameter crossval_folder has to be a name contained in quotation marks "".')

            if k is None:
                k = 10
            elif (not isinstance(k, int) or k<3):
                raise ValueError('Error, parameter K is not an integer greater than 2')

            if n is None:
                n = 10
            elif (not isinstance(n, int) or n<1):
                raise ValueError('Error, parameter n is not an stryctly positive integer')

            if fidexglo_heuristic is None:
                fidexglo_heuristic = 1
            elif (fidexglo_heuristic not in {1,2,3}):
                raise ValueError('Error, parameter fidexglo_heuristic is not 1, 2 or 3')

            if crossval_stats is None:
                crossval_stats = "crossValidationStats.txt"
            elif (not isinstance(crossval_stats, str)):
                raise ValueError('Error, parameter crossval_stats has to be a name contained in quotation marks "".')

            if (attr_file is not None and (not isinstance(attr_file, str))):
                raise ValueError('Error, parameter attr_file has to be a name contained in quotation marks "".')

            if hiknot is None:
                hiknot = 5
            if nb_stairs is None:
                nb_stairs = 50
            if max_iter is None:
                max_iter = 100
            if min_cov is None:
                min_cov = 2

            if seed is None:
                seed = 0
            elif (not isinstance(seed, int) or seed<0):
                raise ValueError('Error, parameter seed is not an positive integer')

            if train_method == "dimlp":

                if nb_in is None:
                    raise ValueError('Error : the number of input neurons is missing, add it with option nb_in=your_number')
                elif (not isinstance(nb_in, int) or nb_in<=0):
                    raise ValueError('Error, parameter nb_in is not an strictly positive integer')
                
                if nb_out is None:
                    raise ValueError('Error : the number of output neurons is missing, add it with option nb_out=your_number')
                elif (not isinstance(nb_out, int) or nb_out<=0):
                    raise ValueError('Error, parameter nb_out is not an strictly positive integer')

                if (pretrained_weights is not None and (not isinstance(pretrained_weights, str))):
                    raise ValueError('Error, parameter pretrained_weights has to be a name contained in quotation marks "".')

                if eta is None:
                    eta = 0.1
                if mu is None:
                    mu = 0.6
                if flat is None:
                    flat = 0.01
                if err_thresh is None:
                    err_thresh = -1111111111.0
                if acc_thresh is None:
                    acc_thresh = 11111111111111.0
                if delta_err is None:
                    delta_err = 0
                if nb_epochs is None:
                    nb_epochs = 1500
                if show_err is None:
                    show_err = 10

            isFidex = False
            isFidexGlo = False
            if (algo == "fidex" or algo == "both"):
                isFidex = True

            if (algo == "fidexGlo" or algo == "both"):
                isFidexGlo = True

            #create paths with root foler
            system = platform.system()
            if system == "Linux" or system == "Darwin":
                separator = "/"
            elif system == "Windows":
                separator = "\\"

            if save_folder is not None:
                root = save_folder + separator
            else:
                root = ""

            data_file = root + data_file
            class_file = root + class_file
            crossval_folder = root + crossval_folder
            crossval_stats = crossval_folder + separator + crossval_stats
            
            # Get datas in a list
            datas = get_data(data_file)
            nb_samples = len(datas)
            if k > nb_samples:
                raise ValueError(f'The number of divisions K of the dataset must be less or equal to the number of train samples({nb_samples}).')

            classes = get_data(class_file)

            if len(classes) != nb_samples:
                raise ValueError('The number of samples in data file and class file need to be the same.')


            # Create folder if doesn't exist
            create_or_clear_directory(crossval_folder)

        #svmTrn(train_data="datanormTrain",train_class="dataclass2Train", test_data="datanormTest",test_class="dataclass2Test", weights = "svm/weights", stats = "svm/stats.txt", train_pred = "svm/predTrain", test_pred = "svm/predTest", save_folder = "dimlp/datafiles")
    
    except ValueError as error:
        print(error)