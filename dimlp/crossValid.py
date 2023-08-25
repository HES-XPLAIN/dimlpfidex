import os
import shutil
import sys
import platform
import random
import math
import time

# import modules :

if sys.platform == 'win32':
    dir_path = os.path.abspath('dimlpfidex')
    os.add_dll_directory(dir_path)

from dimlpfidex import dimlp
from dimlpfidex import fidex
from dimlpfidex import fidexGlo
from dimlp.svmTrn import svmTrn
from dimlp.mlpTrn import mlpTrn
from dimlp.randForestsTrn import randForestsTrn
from dimlp.gradBoostTrn import gradBoostTrn


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

def get_dimlprul_stats(rule_file):

    try:
        with open(rule_file, "r") as my_file:
            lines = my_file.readlines()
            my_file.close()

        stats = []
        in_section = False
        collect = False
        for line in lines:
            line = line.strip()
            if line.startswith("--- Number of rules ="):
                in_section = True
                collect = True
            elif line.startswith("--- Default rule activations rate"):
                in_section = False
            if collect and "=" in line:
                _, value = line.split("=")
                stats.append(float(value.strip()))
            if not in_section:
                collect = False

    except (FileNotFoundError):
        raise ValueError(f"Error : dimlp rules file {rule_file} not found.")
    except (IOError):
        raise ValueError(f"Error : Couldn't open  dimlp rules file {rule_file}.")

    return stats


def get_test_acc(stats_file, train_method):

    try:
        with open(stats_file, "r") as my_file:
            lines = my_file.readlines()
            my_file.close()

        for line in lines:
            line = line.strip()
            if train_method == "dimlp":
                if "Accuracy on testing set" in line:
                    _, value = line.split("=")
                    return float(value.strip())
            else:
                if "Global accuracy on testing set" in line:
                    _, value = line.split("=")
                    return float(value.strip())

    except (FileNotFoundError):
        raise ValueError(f"Error : Train stat file {stats_file} not found.")
    except (IOError):
        raise ValueError(f"Error : Couldn't open train stat file {stats_file}.")



def crossValid(*args, **kwargs):
    try:
        if args or not kwargs:
            print("---------------------------------------------------------------------")
            print("Please specify arguments using named parameters.")
            print("Warning! The files are localised with respect to root folder dimlpfidex.")

            print("----------------------------")
            print("Obligatory parameters :")
            print("train_method : dimlp, dimlpBT, svm, mlp, randForest or gradBoost")
            print("algo : fidex, fidexGlo or both")
            print("data_file : data file")
            print("class_file : class file")

            print("----------------------------")
            print("Obligatory parameters if training with dimlp :")
            print("nb_in : number of input neurons")
            print("nb_out : number of output neurons")
            print("dimlpRul : 1(with dimlpRul, default) or 0")

            print("----------------------------")
            print("Optional parameters :")
            print("save_folder : Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder.")
            print("crossVal_folder : Folder name where to save crossValidation data (CrossValidation by default)")
            print("K : K-fold cross-validation (10 by default)")
            print("N : number of times we do the cross-validation (10 by default)")
            print("fidexGlo_heuristic : 1: optimal fidexGlo(default), 2: fast fidexGlo 3: very fast fidexGlo")
            print("crossVal_stats : statistics of cross validation (crossValidationStats.txt by default)")
            print("attr_file : file of attributes")
            print("max_iter : maximum fidex and fidexGlo iteration number (100 by default)")
            print("min_cov : minimum fidex and fidexGlo covering number (2 by default)")
            print("dropout_dim : dimension dropout parameter for fidex and fidexGlo")
            print("dropout_hyp : hyperplan dropout parameter for fidex and fidexGlo")
            print("seed : 0 = random (default)")

            print("----------------------------")
            print("Optional parameters if not training with decision trees :")
            print("hiknot : high side of the interval (5 by default)")
            print("nb_stairs : number of stairs in staircase activation function (50 by default)")

            print("----------------------------")
            print("Optional parameters for dimlp and dimlpBT training:")
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
            print("Optional parameters for dimlpBT training:")
            print("nb_dimlp_nets : number of dimlp networks, integer >= 2 (25 by default)")
            print("nb_ex_in_one : number of examples for one single network, positive integer, 0 = number of train examples (default)")

            print("----------------------------")
            print("Optional parameters for svm training:")
            print("svm_K : Parameter to improve dynamics (1 by default)")
            print("C : regularization, (1.0 by default)")
            print("kernel : linear, poly, rbf(default) or sigmoid")
            print("degree : polynomial degree (3 by default)")
            print("gamma : scale(default), auto or non negative float")
            print("coef0 : term in kernel function, float (0 by default)")
            print("shrinking : heuristic, True(default) or False")
            print("svm_tol : tolerance for stopping criterion (0.001 by default)")
            print("cache_size : kernel cache size (200 MB by default)")
            print("svm_class_weight : class balance, 'balanced' or a dictionary, for exemple with 2 classes : {0:1.2, 1:3.5} (None by default)")
            print("svm_max_iter : maximal number of iterations (-1 for no limit (default))")
            print("decision_function_shape : decision function shape, ovo(one-vs-one) or ovr(one-vs-rest, default)")
            print("break_ties : break tie decision for ovr with more than 2 classes, True or False(default)")

            print("----------------------------")
            print("Optional parameters for mlp training:")
            print("mlp_K : Parameter to improve dynamics (1 by default)")
            print("hidden_layer_sizes : Size of each hidden layers. Array of shape (n_layers-2) ((100,) by default)")
            print("activation : activation function, identity, logistic, tanh or relu(default)")
            print("solver : solver for weight optimization, lbfgs, sgd or adam (default)")
            print("alpha : strength of the L2 regularization term, positive float (0.0001 by default)")
            print("batch_size : size of minibatches for stochastic optimizers for adam and sgd, auto(default) or positive integer")
            print("mlp_learning_rate : learning rate schedule for weight updates for sgd solver, constant(default), invscaling or adaptive")
            print("learning_rate_init : initial learning rate for adam and sgd, positive float (0.001 by default)")
            print("power_t : exponent for inverse scaling learning rate for sgd, positive float (0.5 by default)")
            print("mlp_max_iter : maximum number of iterations, positive integer (200 by default)")
            print("shuffle : whether to shuffle samples in each iteration for sgd and adam, True(default) or False")
            print("mlp_tol : tolerance for optimization (0.0001 by default)")
            print("mlp_warm_start : whether to reuse previous solution to fit initialization, True or False(default)")
            print("momentum : Momentum for gradient descent update for sgd, between 0 and 1 (0.9 by default)")
            print("nesterovs_momentum : whether to use Nesterov’s momentum for sgd and momentum > 0, True(default) or False")
            print("early_stopping : whether to use early stopping to terminate training when validation score is not improving for sgd and adam, True or False(default)")
            print("mlp_validation_fraction : proportion of training data to set aside as validation set for early stopping, between 0 and 1 excluded (0.1 by default)")
            print("beta_1 : exponential decay rate for estimates of first moment vector in adam, between 0 and 1 excluded (0.9 by default)")
            print("beta_2 : exponential decay rate for estimates of second moment vector in adam, between 0 and 1 excluded (0.999 by default)")
            print("epsilon : value for numerical stability in adam, positive float (1e-8 by default)")
            print("mlp_n_iter_no_change : maximum number of epochs to not meet tol improvement for sgd and adam, integer >= 1 (10 by default)")
            print("max_fun : maximum number of loss function calls for lbfgs, integer >= 1 (15000 by default)")

            print("----------------------------")
            print("Optional parameters for decision trees (random forests and gradient boosting) training:")
            print("n_estimators : Number of generated trees in the forest(100 by default)")
            print("min_samples_split : minimum number of samples required to split an internal node, if float, it is a fraction of the number of samples (2 by default)")
            print("min_samples_leaf : minimum number of samples required to be at a leaf node, if float, it is a fraction of the number of samples (1 by default)")
            print("min_weight_fraction_leaf : minimum weighted fraction of the sum total of input samples weights required to be at a leaf node (0.0 by default)")
            print('max_features : number of features to consider when looking for the best split, sqrt(default), log2, None(specify "None"), integer or float')
            print('min_impurity_decrease : a node will be split if this split induces a decrease of the impurity greater than or equal to this value, float (0.0 by default)')
            print('max_leaf_nodes : grow trees with max_leaf_nodes in best-first fashion, integer (None by default)')
            print('dt_warm_start : whether to reuse the solution of the previous call to fit and add more estimators to the ensemble, True or False(default)')
            print("ccp_alpha : complexity parameter used for Minimal Cost-Complexity Pruning, positive float (0.0 by default)")

            print("----------------------------")
            print("Optional parameters for random forests training:")
            print("rf_criterion : function to measure split quality, gini(default), entropy or log_loss,")
            print("rf_max_depth : max depth of the tree, integer (None by default)")
            print('bootstrap : whether bootstrap samples are used when building trees, True(default) or False (0.0 by default)')
            print('oob_score : whether to use out-of-bag samples to estimate the generalization score, True, False(default) or callable')
            print('n_jobs : number of jobs to run in parallel (None by default(1 proc), -1 : all processors)')
            print("rf_class_weight : class balance, 'balanced', 'balanced_subsample, or a dictionary, for exemple with 2 classes : {0:1.2, 1:3.5} (None by default)")
            print("max_samples : number of samples to draw to train each base estimator for bootstrap, if float, it is a fraction of the number of samples (None by default)")

            print("----------------------------")
            print("Optional parameters for gradient boosting training:")
            print("loss : loss function to be optimized, log_loss(default) or exponential")
            print("gb_learning_rate : shrinks the contribution of each tree, positive float (0.1 by default)")
            print("subsample : fraction of samples to be used for fitting the individual base learners, float in ]0,1] (1.0 by default)")
            print("gb_criterion : function to measure split quality, friedman_mse(default) or squared_error,")
            print('gb_max_depth : maximum depth of the individual regression estimators, strictly positive integer or "None" (3 by default)')
            print('init : estimator object used to compute the initial predictions, "zero"(None by default)')
            print('gb_validation_fraction : proportion of training data to set aside as validation set for early stopping, float (0.1 by default)')
            print("gb_n_iter_no_change : decide if early stopping will be used to terminate training when validation score is not improving, stopping if validation doesn't improve during this number of iterations (None by default)")
            print("gb_tol : tolerance for the early stopping (0.0001 by default)")

            print("----------------------------")
            print("----------------------------")

            print("Here is an example, keep same parameter names :")
            print("Exemple with Dimlp :")
            print('crossValid(train_method="dimlp", algo="both", data_file="datanorm", class_file="dataclass2", dimlpRul=1, nb_in=16, nb_out=2, H2=5, save_folder="dimlp/datafiles", crossVal_folder="CrossValidationDIMLP")')
            print("----------------------------")
            print("Exemple with DimlpBT :")
            print('crossValid(train_method="dimlpBT", algo="both", data_file="datanorm", class_file="dataclass2", dimlpRul=1, nb_in=16, nb_out=2, H2=5, save_folder="dimlp/datafiles", crossVal_folder="CrossValidationDIMLPBT")')
            print("----------------------------")
            print("Exemple with SVM :")
            print('crossValid(train_method="svm", algo="both", data_file="datanorm", class_file="dataclass2", save_folder="dimlp/datafiles", crossVal_folder="CrossValidationSVM")')
            print("----------------------------")
            print("Exemple with MLP :")
            print('crossValid(train_method="mlp", algo="both", data_file="datanorm", class_file="dataclass2", save_folder="dimlp/datafiles", crossVal_folder="CrossValidationMLP")')
            print("----------------------------")
            print("Exemple with Random forests :")
            print('crossValid(train_method="randForest", algo="both", data_file="datanorm", class_file="dataclass2", save_folder="dimlp/datafiles", crossVal_folder="CrossValidationRF")')
            print("---------------------------------------------------------------------")
            print("Exemple with Gradient boosting :")
            print('crossValid(train_method="gradBoost", algo="both", data_file="datanorm", class_file="dataclass2", save_folder="dimlp/datafiles", crossVal_folder="CrossValidationGB")')
            print("---------------------------------------------------------------------")

            return 0

        else:

            start_time = time.time()

            # Get parameters
            train_method = kwargs.get('train_method')
            algo = kwargs.get('algo')
            data_file = kwargs.get('data_file')
            class_file = kwargs.get('class_file')

            nb_in = kwargs.get('nb_in')
            nb_out = kwargs.get('nb_out')
            dimlprul = kwargs.get('dimlpRul')

            save_folder = kwargs.get('save_folder')
            crossval_folder = kwargs.get('crossVal_folder')
            k = kwargs.get('K')
            n = kwargs.get('N')
            fidexglo_heuristic = kwargs.get('fidexGlo_heuristic')
            crossval_stats = kwargs.get('crossVal_stats')
            attr_file = kwargs.get('attr_file')
            max_iter = kwargs.get('max_iter')
            min_cov = kwargs.get('min_cov')
            dropout_dim = kwargs.get('dropout_dim')
            dropout_hyp = kwargs.get('dropout_hyp')
            seed = kwargs.get('seed')

            hiknot = kwargs.get('hiknot')
            nb_stairs = kwargs.get('nb_stairs')

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

            nb_dimlp_nets = kwargs.get('nb_dimlp_nets')
            nb_ex_in_one = kwargs.get('nb_ex_in_one')

            svm_k = kwargs.get('svm_K')
            c_var = kwargs.get('C')
            kernel_var = kwargs.get('kernel')
            degree_var = kwargs.get('degree')
            gamma_var = kwargs.get('gamma')
            coef0_var = kwargs.get('coef0')
            shrinking_var = kwargs.get('shrinking')
            svm_tol_var = kwargs.get('svm_tol')
            cache_size_var = kwargs.get('cache_size')
            svm_class_weight_var = kwargs.get('svm_class_weight')
            svm_max_iter_var = kwargs.get('svm_max_iter')
            decision_function_shape_var = kwargs.get('decision_function_shape')
            break_ties_var = kwargs.get('break_ties')

            mlp_k = kwargs.get('mlp_K')
            hidden_layer_sizes_var = kwargs.get('hidden_layer_sizes')
            activation_var = kwargs.get('activation')
            solver_var = kwargs.get('solver')
            alpha_var = kwargs.get('alpha')
            batch_size_var = kwargs.get('batch_size')
            mlp_learning_rate_var = kwargs.get('mlp_learning_rate')
            learning_rate_init_var = kwargs.get('learning_rate_init')
            power_t_var = kwargs.get('power_t')
            mlp_max_iter_var = kwargs.get('mlp_max_iter')
            shuffle_var = kwargs.get('shuffle')
            mlp_tol_var = kwargs.get('mlp_tol')
            mlp_warm_start_var = kwargs.get('mlp_warm_start')
            momentum_var = kwargs.get('momentum')
            nesterovs_momentum_var = kwargs.get('nesterovs_momentum')
            early_stopping_var = kwargs.get('early_stopping')
            mlp_validation_fraction_var = kwargs.get('mlp_validation_fraction')
            beta_1_var = kwargs.get('beta_1')
            beta_2_var = kwargs.get('beta_2')
            epsilon_var = kwargs.get('epsilon')
            mlp_n_iter_no_change_var = kwargs.get('mlp_n_iter_no_change')
            max_fun_var = kwargs.get('max_fun')

            n_estimators_var = kwargs.get('n_estimators')
            min_samples_split_var = kwargs.get('min_samples_split')
            min_samples_leaf_var = kwargs.get('min_samples_leaf')
            min_weight_fraction_leaf_var = kwargs.get('min_weight_fraction_leaf')
            max_features_var = kwargs.get('max_features')
            min_impurity_decrease_var = kwargs.get('min_impurity_decrease')
            max_leaf_nodes_var = kwargs.get('max_leaf_nodes')
            dt_warm_start_var = kwargs.get('dt_warm_start')
            ccp_alpha_var = kwargs.get('ccp_alpha')

            rf_criterion_var = kwargs.get('rf_criterion')
            rf_max_depth_var = kwargs.get('rf_max_depth')
            bootstrap_var = kwargs.get('bootstrap')
            oob_score_var = kwargs.get('oob_score')
            n_jobs_var = kwargs.get('n_jobs')
            rf_class_weight_var = kwargs.get('rf_class_weight')
            max_samples_var = kwargs.get('max_samples')

            loss_var = kwargs.get('loss')
            gb_learning_rate_var = kwargs.get('gb_learning_rate')
            subsample_var = kwargs.get('subsample')
            gb_criterion_var = kwargs.get('gb_criterion')
            gb_max_depth_var = kwargs.get('gb_max_depth')
            init_var = kwargs.get('init')
            gb_validation_fraction_var = kwargs.get('gb_validation_fraction')
            gb_n_iter_no_change_var = kwargs.get('gb_n_iter_no_change')
            gb_tol_var = kwargs.get('gb_tol')

            # Check parameters

            obligatory_args = ['train_method', 'algo', 'data_file', 'class_file']
            obligatory_dimlp_args = ['nb_in', 'nb_out', 'dimlpRul']

            optional_args = ['save_folder', 'crossVal_folder', 'K', 'N', 'fidexGlo_heuristic', 'crossVal_stats', 'attr_file',
                        'max_iter', 'min_cov', 'dropout_dim', 'dropout_hyp', 'seed']

            optional_non_dt_args = ['hiknot', 'nb_stairs']

            optional_dimlp_args = ['pretrained_weights', 'eta', 'mu', 'flat', 'err_thresh',
                        'acc_thresh', 'delta_err', 'nb_epochs', 'show_err']

            optional_dimlpBT_args = ['nb_dimlp_nets', 'nb_ex_in_one']

            optional_svm_args = ['svm_K', 'C', 'kernel', 'degree', 'gamma', 'coef0', 'shrinking',
                        'svm_tol', 'cache_size', 'svm_class_weight', 'svm_max_iter', 'decision_function_shape', 'break_ties']

            optional_mlp_args = ['mlp_K', 'hidden_layer_sizes', 'activation', 'solver', 'alpha',
                        'batch_size', 'mlp_learning_rate', 'learning_rate_init', 'power_t', 'mlp_max_iter',
                        'shuffle', 'mlp_tol', 'mlp_warm_start', 'momentum', 'nesterovs_momentum',
                        'early_stopping', 'mlp_validation_fraction', 'beta_1', 'beta_2', 'epsilon', 'mlp_n_iter_no_change', 'max_fun']

            optional_dt_args = ['n_estimators', 'min_samples_split', 'min_samples_leaf', 'min_weight_fraction_leaf',
                                            'max_features', 'min_impurity_decrease', 'max_leaf_nodes', 'dt_warm_start', 'ccp_alpha']

            optional_rf_args = ['rf_criterion', 'rf_max_depth', 'bootstrap', 'oob_score', 'n_jobs', 'rf_class_weight', 'max_samples']

            optional_gb_args = ['loss', 'gb_learning_rate', 'subsample', 'gb_criterion', 'gb_max_depth', 'init'
                                , 'gb_validation_fraction', 'gb_n_iter_no_change', 'gb_tol']

            # Check if wrong parameters are given

            train_methods = {"dimlp", "dimlpBT", "svm", "mlp", "randForest", "gradBoost"}
            if train_method is None:
                raise ValueError('Error : train method is missing, add it with option train_method="dimlp", "dimlpBT", "svm", "mlp", randForest or gradBoost')
            elif (train_method not in train_methods):
                raise ValueError('Error, parameter train_method is not "dimlp", "dimlpBT", "svm", "mlp", "randForest" or "gradBoost"')

            for arg_key in kwargs.keys():
                if (train_method == "dimlp"):
                    if (arg_key not in optional_args and arg_key not in optional_non_dt_args and arg_key not in optional_dimlp_args and not(arg_key.startswith('H') and arg_key[1:].isdigit()) and arg_key not in obligatory_args and arg_key not in obligatory_dimlp_args):
                        raise ValueError(f"Invalid argument with dimlp training : {arg_key}")
                elif (train_method == "dimlpBT"):
                    if (arg_key not in optional_args and arg_key not in optional_non_dt_args and arg_key  not in optional_dimlp_args and not(arg_key.startswith('H') and arg_key[1:].isdigit()) and arg_key not in obligatory_args and arg_key not in obligatory_dimlp_args and arg_key not in optional_dimlpBT_args):
                        raise ValueError(f"Invalid argument with dimlpBT training : {arg_key}")
                elif (train_method == "svm"):
                    if (arg_key not in optional_args and arg_key not in optional_non_dt_args and arg_key not in optional_svm_args and arg_key not in obligatory_args):
                        raise ValueError(f"Invalid argument with svm training : {arg_key}")
                elif (train_method == "mlp"):
                    if (arg_key not in optional_args and arg_key not in optional_non_dt_args and arg_key not in optional_mlp_args and arg_key not in obligatory_args):
                        raise ValueError(f"Invalid argument with mlp training : {arg_key}")
                elif (train_method == "randForest"):
                    if (arg_key not in optional_args and arg_key not in optional_dt_args and arg_key not in optional_rf_args and arg_key not in obligatory_args):
                        raise ValueError(f"Invalid argument with random forest training : {arg_key}")
                else:
                    if (arg_key not in optional_args and arg_key not in optional_dt_args and arg_key not in optional_gb_args and arg_key not in obligatory_args):
                        raise ValueError(f"Invalid argument with gradient boosting training : {arg_key}")


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

            if train_method not in {"randForest", "gradBoost"}:
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

            if train_method == "dimlpBT":
                if nb_dimlp_nets is None:
                    nb_dimlp_nets = 25
                elif not isinstance(nb_dimlp_nets, int) or nb_dimlp_nets < 2:
                    raise ValueError('Error, parameter nb_dimlp_nets is not an integer greater than 1')

                if nb_ex_in_one is None :
                    nb_ex_in_one = 0
                elif not isinstance(nb_ex_in_one, int) or nb_ex_in_one < 0:
                    raise ValueError('Error, parameter nb_ex_in_one is not a positive integer')

            if train_method in {"dimlp", "dimlpBT"}:

                if dimlprul is None:
                    raise ValueError('Error : dimlpRul is missing, add it with option dimlpRul=1 or 0')
                elif (dimlprul not in {0,1}):
                    raise ValueError('Error, parameter dimlpRul is not 1 or 0')

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

            elif train_method == "svm":
                if svm_k is None:
                    svm_k = 1
                if c_var is None:
                    c_var = 1.0
                if kernel_var is None:
                    kernel_var = "rbf"
                if degree_var is None:
                    degree_var = 3
                if gamma_var is None:
                    gamma_var = "scale"
                if coef0_var is None:
                    coef0_var = 0
                if shrinking_var is None:
                    shrinking_var = True
                if svm_tol_var is None:
                    svm_tol_var = 0.001
                if cache_size_var is None:
                    cache_size_var = 200
                if svm_max_iter_var is None:
                    svm_max_iter_var = -1
                if decision_function_shape_var is None:
                    decision_function_shape_var = "ovr"
                if break_ties_var is None:
                    break_ties_var = False

            elif train_method == "mlp":
                if mlp_k is None:
                    mlp_k = 1
                if hidden_layer_sizes_var is None:
                    hidden_layer_sizes_var = (100,)
                if activation_var is None:
                    activation_var = "relu"
                if solver_var is None:
                    solver_var = "adam"
                if alpha_var is None:
                    alpha_var = 0.0001
                if batch_size_var is None:
                    batch_size_var = "auto"
                if mlp_learning_rate_var is None:
                    mlp_learning_rate_var = "constant"
                if learning_rate_init_var is None:
                    learning_rate_init_var = 0.001
                if power_t_var is None:
                    power_t_var = 0.5
                if mlp_max_iter_var is None:
                    mlp_max_iter_var = 200
                if shuffle_var is None:
                    shuffle_var = True
                if mlp_tol_var is None:
                    mlp_tol_var = 0.0001
                if mlp_warm_start_var is None:
                    mlp_warm_start_var = False
                if momentum_var is None:
                    momentum_var = 0.9
                if nesterovs_momentum_var is None:
                    nesterovs_momentum_var = True
                if early_stopping_var is None:
                    early_stopping_var = False
                if mlp_validation_fraction_var is None:
                    mlp_validation_fraction_var = 0.1
                if beta_1_var is None:
                    beta_1_var = 0.9
                if beta_2_var is None:
                    beta_2_var = 0.999
                if epsilon_var is None:
                    epsilon_var = 0.00000001
                if mlp_n_iter_no_change_var is None:
                    mlp_n_iter_no_change_var = 10
                if max_fun_var is None:
                    max_fun_var = 15000

            else:
                if n_estimators_var is None:
                    n_estimators_var = 100

                if min_samples_split_var is None:
                    min_samples_split_var = 2

                if min_samples_leaf_var is None:
                    min_samples_leaf_var = 1

                if min_weight_fraction_leaf_var is None:
                    min_weight_fraction_leaf_var = 0.0

                if min_impurity_decrease_var is None:
                    min_impurity_decrease_var = 0.0

                if max_features_var is None:
                    max_features_var = "sqrt"

                if dt_warm_start_var is None:
                    dt_warm_start_var = False

                if ccp_alpha_var is None:
                    ccp_alpha_var = 0.0

                if train_method == "randForest":
                    if rf_criterion_var is None:
                        rf_criterion_var = "gini"

                    if bootstrap_var is None:
                        bootstrap_var = True

                    if oob_score_var is None:
                        oob_score_var = False

                else:
                    if loss_var is None:
                        loss_var = "log_loss"

                    if gb_learning_rate_var is None:
                        gb_learning_rate_var = 0.1

                    if subsample_var is None:
                        subsample_var = 1.0

                    if gb_criterion_var is None:
                        gb_criterion_var = "friedman_mse"

                    if gb_max_depth_var is None:
                        gb_max_depth_var = 3

                    if gb_validation_fraction_var is None:
                        gb_validation_fraction_var = 0.1

                    if gb_tol_var is None:
                        gb_tol_var = 0.0001

            is_fidex = False
            is_fidexglo = False
            is_dimlprul = False
            if (algo == "fidex" or algo == "both"):
                is_fidex = True

            if (algo == "fidexGlo" or algo == "both"):
                is_fidexglo = True

            if train_method in {"dimlp", "dimlpBT"} and dimlprul == 1:
                is_dimlprul = True

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
            crossval_folder_temp = crossval_folder
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

            # Initialize random seeds
            if seed != 0: # Not random
                random.seed(seed)
                seeds = random.sample(range(10*n), n)


            # Create temp files for train, test and validation
            temp_train_file= crossval_folder + separator + "tempTrain.txt"
            temp_test_file = crossval_folder + separator + "tempTest.txt"
            temp_valid_file = crossval_folder + separator + "tempValid.txt"
            temp_train_tar_file = crossval_folder + separator + "tempTarTrain.txt"
            temp_test_tar_file = crossval_folder + separator + "tempTarTest.txt"
            temp_valid_tar_file = crossval_folder + separator + "tempTarValid.txt"

            if is_fidex:
                # statistics for Fidex
                # One execution
                mean_cov_size_fid = 0.0
                mean_nb_ant_fid = 0.0
                mean_fidel_fid = 0.0
                mean_rules_acc_fid = 0.0
                mean_confid_fid = 0.0

                # All executions
                mean_cov_size_fid_all = 0.0
                mean_nb_ant_fid_all = 0.0
                mean_fidel_fid_all = 0.0
                mean_rules_acc_fid_all = 0.0
                mean_confid_fid_all = 0.0

                std_cov_size_fid_all = 0.0
                std_nb_ant_fid_all = 0.0
                std_fidel_fid_all = 0.0
                std_rules_acc_fid_all = 0.0
                std_confid_fid_all = 0.0

                mean_exec_values_fidex = [] # each mean value in an entire fold for each fold for fidex

            if is_fidexglo:
                # Statistics for FidexGlo
                # One execution
                mean_nb_rules = 0.0
                mean_nb_cover = 0.0
                mean_nb_antecedants = 0.0
                mean_fidel_glo = 0.0
                mean_rules_acc_glo = 0.0
                mean_expl_glo = 0.0
                mean_default_rate = 0.0
                mean_nb_fidel_activations = 0.0
                mean_wrong_activations = 0.0
                mean_test_acc_glo = 0.0
                mean_test_acc_when_rules_and_model_agree = 0.0
                mean_test_acc_when_activated_rules_and_model_agree = 0.0

                # All executions
                mean_nb_rules_all = 0.0
                mean_nb_cover_all = 0.0
                mean_nb_antecedants_all = 0.0
                mean_fidel_glo_all = 0.0
                mean_rules_acc_glo_all = 0.0
                mean_expl_glo_all = 0.0
                mean_default_rate_all = 0.0
                mean_nb_fidel_activations_all = 0.0
                mean_wrong_activations_all = 0.0
                mean_test_acc_glo_all = 0.0
                mean_test_acc_when_rules_and_model_agree_all = 0.0
                mean_test_acc_when_activated_rules_and_model_agree_all = 0.0

                std_nb_rules_all = 0.0
                std_nb_cover_all = 0.0
                std_nb_antecedants_all = 0.0
                std_fidel_glo_all = 0.0
                std_rules_acc_glo_all = 0.0
                std_expl_glo_all = 0.0
                std_default_rate_all = 0.0
                std_nb_fidel_activations_all = 0.0
                std_wrong_activations_all = 0.0
                std_test_acc_glo_all = 0.0
                std_test_acc_when_rules_and_model_agree_all = 0.0
                std_test_acc_when_activated_rules_and_model_agree_all = 0.0

                mean_exec_values_fidexglo = [] # each mean value in an entire fold for each fold for fidexGlo

            if is_dimlprul:
                # Statistics for Dimlp Rules
                # One execution
                mean_nb_rules_dimlp = 0.0
                mean_nb_cover_dimlp = 0.0
                mean_nb_antecedants_dimlp = 0.0
                mean_fidel_dimlp = 0.0
                mean_rules_acc_dimlp = 0.0
                mean_default_rate_dimlp = 0.0
                mean_test_acc_dimlp = 0.0
                mean_test_acc_when_rules_and_model_agree_dimlp = 0.0

                # All executions
                mean_nb_rules_dimlp_all = 0.0
                mean_nb_cover_dimlp_all = 0.0
                mean_nb_antecedants_dimlp_all = 0.0
                mean_fidel_dimlp_all = 0.0
                mean_rules_acc_dimlp_all = 0.0
                mean_default_rate_dimlp_all = 0.0
                mean_test_acc_dimlp_all = 0.0
                mean_test_acc_when_rules_and_model_agree_dimlp_all = 0.0

                std_nb_rules_dimlp_all = 0.0
                std_nb_cover_dimlp_all = 0.0
                std_nb_antecedants_dimlp_all = 0.0
                std_fidel_dimlp_all = 0.0
                std_rules_acc_dimlp_all = 0.0
                std_default_rate_dimlp_all = 0.0
                std_test_acc_dimlp_all = 0.0
                std_test_acc_when_rules_and_model_agree_dimlp_all = 0.0

                mean_exec_values_dimlp = []

                nb_no_rules_dimlp = 0.0 # If there is no rule found for a Fold


            # Write parameters on stats file

            try:
                with open(crossval_stats, "w") as outputStatsFile:

                    outputStatsFile.write(f"Parameters for {n} times {k}-Cross validation :\n")
                    outputStatsFile.write(f"Training with {train_method}\n")
                    outputStatsFile.write("---------------------------------------------------------\n")
                    if train_method in {"dimlp", "dimlpBT"}:
                        outputStatsFile.write(f"Training with {nb_in} input neurons and {nb_out} output neurons\n")
                        for key, value in hk.items():
                            outputStatsFile.write(f"Layer {key} has {value} neurons\n")
                    if train_method not in {"randForest", "gradBoost"}:
                        outputStatsFile.write(f"The number of stairs in staircase activation function is {nb_stairs} and the interval in which hyperplans are contained is [-{hiknot},{hiknot}]\n")
                    if train_method == "dimlpBT":
                        outputStatsFile.write(f"The number of dimlp networks is {nb_dimlp_nets}\n")
                        if nb_ex_in_one == 0:
                            outputStatsFile.write("The number of examples for one single network is the number of train examples\n")
                        else:
                            outputStatsFile.write(f"The number of examples for one single network is {nb_ex_in_one} (but not more than the number of train examples)\n")
                    if train_method in {"dimlp", "dimlpBT"}:
                        outputStatsFile.write(f"The back-propagation learning parameter (Eta) is {eta}\n")
                        outputStatsFile.write(f"The back-propagation momentum parameter (Mu) is {mu}\n")
                        outputStatsFile.write(f"The back-propagation flat spot elimination parameter (Flat) is {flat}\n")
                        outputStatsFile.write(f"The error threshold is {err_thresh}\n")
                        outputStatsFile.write(f"The accuracy threshold is {acc_thresh}\n")
                        outputStatsFile.write(f"The absolute difference error threshold is {delta_err}\n")
                        outputStatsFile.write(f"The number of train epochs is {nb_epochs}\n")
                        outputStatsFile.write(f"The number of train epochs to show error is {show_err}\n")
                    elif train_method == "svm":
                        outputStatsFile.write(f"The K parameter to improve dynamics is {svm_k}\n")
                        outputStatsFile.write(f"The regularization parameter C is {c_var}\n")
                        outputStatsFile.write(f"The kernel is {kernel_var}\n")
                        if kernel_var == "poly":
                            outputStatsFile.write(f"The polynomial degree is {degree_var}\n")
                        if kernel_var in {"rbf", "poly", "sigmoid"}:
                            outputStatsFile.write(f"The kernel coefficient gamma is {gamma_var}\n")
                        if kernel_var in {"poly", "sigmoid"}:
                            outputStatsFile.write(f"The coef0 term is {coef0_var}\n")
                        if shrinking_var:
                            outputStatsFile.write("Using of the shrinking heuristic\n")
                        else:
                            outputStatsFile.write("No using of the shrinking heuristic\n")
                        outputStatsFile.write(f"The tolerance for stopping criterion is {svm_tol_var}\n")
                        if svm_class_weight_var == None:
                            outputStatsFile.write("Class weights are unchanged\n")
                        else:
                            outputStatsFile.write(f"Class weights are {svm_class_weight_var}\n")
                        if svm_max_iter_var == -1:
                            outputStatsFile.write("There is no limit in the number of iterations\n")
                        else:
                            outputStatsFile.write(f"The maximal number of iterations is {svm_max_iter_var}\n")
                        if decision_function_shape_var == "ovr":
                            outputStatsFile.write("The decision function shape is one-vs-rest\n")
                            if break_ties_var:
                                outputStatsFile.write("Using break tie decision\n")
                        else:
                            outputStatsFile.write("The decision function shape is one-vs-one\n")
                    elif train_method == "mlp":
                        outputStatsFile.write(f"The K parameter to improve dynamics is {mlp_k}\n")
                        outputStatsFile.write(f"The size of the hidden layers is {hidden_layer_sizes_var}\n")
                        outputStatsFile.write(f"The activation function is {activation_var}\n")
                        outputStatsFile.write(f"The solver is {solver_var}\n")
                        outputStatsFile.write(f"The alpha parameter is {alpha_var}\n")
                        if solver_var in {"adam", "sgd"}:
                            outputStatsFile.write(f"The batch size is {alpha_var}\n")
                        if solver_var =="sgd":
                            outputStatsFile.write(f"The learning rate is {mlp_learning_rate_var}\n")
                        if solver_var in {"adam", "sgd"}:
                            outputStatsFile.write(f"The initial learning rate is {learning_rate_init_var}\n")
                        if solver_var =="sgd":
                            outputStatsFile.write(f"The power_t exponent for inverse scaling learning rate is {power_t_var}\n")
                        outputStatsFile.write(f"The maximum number of iterations is {mlp_max_iter_var}\n")
                        if solver_var in {"adam", "sgd"} and shuffle_var == True:
                            outputStatsFile.write("Samples are shuffled in each iteration")
                        outputStatsFile.write(f"The tolerance for optimization is {mlp_tol_var}\n")
                        if mlp_warm_start_var == True:
                            outputStatsFile.write("Previous solution is reused to fit initialization")
                        if solver_var =="sgd":
                            outputStatsFile.write(f"The momentum for gradient descent update is {momentum_var}\n")
                            if nesterovs_momentum_var == True:
                                outputStatsFile.write("Using Nesterov’s momentum")
                        if solver_var in {"adam", "sgd"} and early_stopping_var == True:
                            outputStatsFile.write("Using early stopping")
                            outputStatsFile.write(f"The proportion of training data to set aside as validation set is {mlp_validation_fraction_var}\n")
                        if solver_var =="adam":
                            outputStatsFile.write(f"Exponential decay rate for estimates of first moment vector is {beta_1_var}\n")
                            outputStatsFile.write(f"For the second moment it's {beta_2_var}\n")
                            outputStatsFile.write(f"The value for numerical stability is {epsilon_var}\n")
                        if solver_var in {"adam", "sgd"}:
                            outputStatsFile.write(f"The maximum number of epochs to not meet tol improvement is {mlp_n_iter_no_change_var}\n")
                        if solver_var =="lbfgs":
                            outputStatsFile.write(f"The maximum number of loss function calls is {max_fun_var}\n")
                    else:
                        outputStatsFile.write(f"The number of generated trees in the forest is {n_estimators_var}\n")
                        if min_samples_split_var <= 1:
                            outputStatsFile.write(f"The minimum number of samples required to split an internal node is {min_samples_split_var*100}% of the number of samples\n")
                        else:
                            outputStatsFile.write(f"The minimum number of samples required to split an internal node is {min_samples_split_var}\n")
                        if min_samples_leaf_var < 1:
                            outputStatsFile.write(f"The minimum number of samples required to be at a leaf node is {min_samples_leaf_var*100}% of the number of samples\n")
                        else:
                            outputStatsFile.write(f"The minimum number of samples required to be at a leaf node is {min_samples_leaf_var}\n")
                        outputStatsFile.write(f"The minimum weighted fraction of the sum total of input samples weights required to be at a leaf node is {min_weight_fraction_leaf_var}\n")
                        if max_features_var == "None":
                            outputStatsFile.write("All features are considered  when looking for the best split\n")
                        elif max_features_var == "sqrt":
                            outputStatsFile.write("The number of features to consider when looking for the best split is computed as the square root of the number of features\n")
                        elif max_features_var == "log2":
                            outputStatsFile.write("The number of features to consider when looking for the best split is computed as log2 of the number of features\n")
                        elif max_features_var < 1:
                            outputStatsFile.write(f"The number of features to consider when looking for the best split is {max_features_var*100}% of the number of features\n")
                        else:
                            outputStatsFile.write(f"The number of features to consider when looking for the best split is {max_features_var}\n")
                        outputStatsFile.write(f"A node will be split if this split induces a decrease of the impurity greater than or equal to {min_impurity_decrease_var}\n")
                        if max_leaf_nodes_var is None:
                            outputStatsFile.write("Trees grow with unlimited number of leaf nodes in best-first fashion\n")
                        else:
                            outputStatsFile.write(f"Trees grow with {max_leaf_nodes_var} leaf nodes in best-first fashion\n")
                        if dt_warm_start_var == True:
                            outputStatsFile.write("The solution of the previous call is reused to fit and add more estimators to the ensemble\n")
                        else:
                            outputStatsFile.write("The solution of the previous call is not reused to fit and to add more estimators to the ensemble\n")
                        outputStatsFile.write(f"The complexity parameter used for Minimal Cost-Complexity Pruning is {ccp_alpha_var}\n")
                        if train_method == "randForest":
                            outputStatsFile.write(f"The function used to measure split quality is {rf_criterion_var}\n")
                            if rf_max_depth_var is None:
                                outputStatsFile.write("The depth of the tree is unlimited\n")
                            else:
                                outputStatsFile.write(f"The maximum depth of the tree is {rf_max_depth_var}\n")
                            if bootstrap_var == True:
                                outputStatsFile.write("Bootstrap samples are used when building trees\n")
                                if oob_score_var == True:
                                    outputStatsFile.write("Using out-of-bag samples to estimate the generalization score\n")
                                if max_samples_var is None:
                                    outputStatsFile.write("Every sample is used to train each base estimator for bootstrap\n")
                                elif max_samples_var < 1:
                                    outputStatsFile.write(f"The number of samples to draw to train each base estimator for bootstrap is {max_samples_var*100}% of the number of samples\n")
                                else:
                                    outputStatsFile.write(f"The number of samples to draw to train each base estimator for bootstrap is {max_samples_var}\n")
                            if n_jobs_var is None:
                                outputStatsFile.write("The jobs are not runned in parallel\n")
                            elif n_jobs_var == -1:
                                outputStatsFile.write("The jobs are runned using all processors\n")
                            else:
                                outputStatsFile.write(f"The number of jobs to run in parallel is {n_jobs_var}\n")
                            if rf_class_weight_var is None:
                                outputStatsFile.write("Class weights are unchanged\n")
                            elif rf_class_weight_var == "balanced_subsample":
                                outputStatsFile.write("Class weights are balanced based on the bootstrap sample\n")
                            else:
                                outputStatsFile.write(f"Class weights are {rf_class_weight_var}\n")
                        else:
                            outputStatsFile.write(f"The loss function to be optimized is {loss_var}\n")
                            outputStatsFile.write(f"The learning rate shrinking the contribution of each tree is {gb_learning_rate_var}\n")
                            outputStatsFile.write(f"The fraction of samples to be used for fitting the individual base learners is {subsample_var}\n")
                            outputStatsFile.write(f"The function used to measure split quality is {gb_criterion_var}\n")
                            if gb_max_depth_var == "None":
                                outputStatsFile.write("The depth of the tree is unlimited\n")
                            else:
                                outputStatsFile.write(f"The maximum depth of the tree is {gb_max_depth_var}\n")
                            if init_var =="zero":
                                outputStatsFile.write("Special zero estimator is used\n")
                            if gb_n_iter_no_change_var is None:
                                outputStatsFile.write("Early stopping is disabled\n")
                            else:
                                outputStatsFile.write(f"Stopping if validation score doesn't improve during {gb_n_iter_no_change_var} iterations\n")
                                outputStatsFile.write(f"The proportion of training data to set aside as validation set for early stopping is {gb_validation_fraction_var} \n")
                                outputStatsFile.write(f"The tolerance for the early stopping is {gb_tol_var} \n")
                    outputStatsFile.write(f"The max fidex and fidexGlo iteration number is {max_iter}\n")
                    outputStatsFile.write(f"The minimum fidex and fidexGlo covering number is {min_cov}\n")
                    if is_fidexglo:
                        outputStatsFile.write(f"The fidexGlo heuristic is {fidexglo_heuristic}\n")
                    if dropout_hyp:
                        outputStatsFile.write(f"The hyperplan dropout parameter for fidex and fidexGlo is {dropout_hyp}\n")
                    else:
                        outputStatsFile.write("There is no hyperplan dropout\n")
                    if dropout_dim:
                        outputStatsFile.write(f"The dimension dropout parameter for fidex and fidexGlo is {dropout_dim}\n")
                    else:
                        outputStatsFile.write("There is no dimension dropout\n")

                    outputStatsFile.write("---------------------------------------------------------\n\n")

                    outputStatsFile.close()
            except (FileNotFoundError):
                raise ValueError(f"Error : File for stats extraction ({crossval_stats}) not found.")
            except (IOError):
                raise ValueError(f"Error : Couldn't open stats extraction file {crossval_stats}.")

            has_confidence = True # Check if we have confidence statistics

            # Loop on N executions of cross-validation
            for ni in range(n):
                print(f"n={ni+1}")

                #  Create folder for this execution
                if system == "Linux" or system == "Darwin":
                    folder_name = crossval_folder + "/Execution" + str(ni+1) + "/"
                elif system == "Windows":
                    folder_name = crossval_folder + "\\Execution" + str(ni+1)
                try:
                    os.makedirs(folder_name)
                except (IOError):
                    raise ValueError(f"Error : Couldn't create execution folder {folder_name}.")


                # Randomly split data in K sub-parts
                data_split = []
                tar_data_split = []

                indexes = list(range(nb_samples))
                if seed != 0:  # Not random
                    random.seed(seeds[ni])
                    random.shuffle(indexes)
                else:  # Random
                    random.shuffle(indexes)

                range_values = [i*(nb_samples/k) for i in range(k + 1)]
                # Split data evenly in K groups
                for q in range(len(range_values) - 1):
                    start = int(range_values[q])
                    end = int(range_values[q + 1])
                    temp_vect = [datas[indexes[ind]] for ind in range(start, end)]
                    temp_vect_tar = [classes[indexes[ind]] for ind in range(start, end)]
                    data_split.append(temp_vect)
                    tar_data_split.append(temp_vect_tar)

                nb_no_rules_current_exec_dimlp = 0.0 # If there is no rule found for a Fold in this execution

                for ki in range(k): # K-fold, we shift each time groups by 1.
                    print("----")
                    print(f"k={ki+1}")

                    #  Create folder for this fold
                    folder_path = crossval_folder + separator + "Execution" + str(ni+1) + separator + "Fold" + str(ki+1)
                    if system == "Linux" or system == "Darwin":
                        folder_fold_name = folder_path + separator
                    elif system == "Windows":
                        folder_fold_name = folder_path
                    try:
                        os.makedirs(folder_fold_name)
                    except (IOError):
                        raise ValueError(f"Error : Couldn't create fold folder {folder_fold_name}.")

                    # Get group index for test, validation and train
                    train_idx = []
                    validation_idx = ki
                    test_idx = (ki + 1) % k
                    for m in range(2, k):
                        train_idx.append((ki + m) % k)
                    if train_method in {"dimlpBT", "svm", "mlp", "randForest", "gradBoost"}: # There is no validation data to be provided to tune hyperparameters
                        train_idx.append(validation_idx)

                    # Creation of train, test and validation files (temp files)
                    try:
                        with open(temp_train_file, "w") as trn_file:
                            for id in train_idx:
                                for line_trn in data_split[id]:
                                    trn_file.write(f"{line_trn}\n")
                        trn_file.close()

                    except (IOError):
                        raise ValueError(f"Error : Couldn't open file {temp_train_file}.")

                    try:
                        with open(temp_test_file, "w") as tst_file:
                            for line_tst in data_split[test_idx]:
                                tst_file.write(f"{line_tst}\n")
                        tst_file.close()

                    except (IOError):
                        raise ValueError(f"Error : Couldn't open file {temp_test_file}.")
                    if train_method == "dimlp":
                        try:
                            with open(temp_valid_file, "w") as val_file:
                                for line_val in data_split[validation_idx]:
                                    val_file.write(f"{line_val}\n")
                            val_file.close()

                        except (IOError):
                            raise ValueError(f"Error : Couldn't open file {temp_valid_file}.")

                    try:
                        with open(temp_train_tar_file, "w") as trn_val_file:
                            for id in train_idx:
                                for line_trn_tar in tar_data_split[id]:
                                    trn_val_file.write(f"{line_trn_tar}\n")
                        trn_val_file.close()

                    except (IOError):
                        raise ValueError(f"Error : Couldn't open file {temp_train_tar_file}.")

                    try:
                        with open(temp_test_tar_file, "w") as tst_tar_file:
                            for line_tst_tar in tar_data_split[test_idx]:
                                tst_tar_file.write(f"{line_tst_tar}\n")
                        tst_tar_file.close()

                    except (IOError):
                        raise ValueError(f"Error : Couldn't open file {temp_test_tar_file}.")
                    if train_method == "dimlp":
                        try:
                            with open(temp_valid_tar_file, "w") as val_tar_file:
                                for line_val_tar in tar_data_split[validation_idx]:
                                    val_tar_file.write(f"{line_val_tar}\n")
                            val_tar_file.close()

                        except (IOError):
                            raise ValueError(f"Error : Couldn't open file {temp_valid_tar_file}.")

                    # Get train, test and validation files in folder

                    try:
                        shutil.copy2(temp_train_file, folder_path + separator + "train.txt")
                    except IOError:
                        print("File tempTrain.txt coundn't be copied.")

                    try:
                        shutil.copy2(temp_test_file, folder_path + separator + "test.txt")
                    except IOError:
                        print("File tempTest.txt coundn't be copied.")

                    if train_method == "dimlp":
                        try:
                            shutil.copy2(temp_valid_file, folder_path + separator + "valid.txt")
                        except IOError:
                            print("File tempValid.txt coundn't be copied.")

                    try:
                        shutil.copy2(temp_train_tar_file, folder_path + separator + "trainTarget.txt")
                    except IOError:
                        print("File tempTarTrain.txt coundn't be copied.")

                    try:
                        shutil.copy2(temp_test_tar_file, folder_path + separator + "testTarget.txt")
                    except IOError:
                        print("File tempTarTest.txt coundn't be copied.")

                    if train_method == "dimlp":
                        try:
                            shutil.copy2(temp_valid_tar_file, folder_path + separator + "validTarget.txt")
                        except IOError:
                            print("File tempTarValid.txt coundn't be copied.")


                    # Training with dimlp
                    folder_path_from_root = str(crossval_folder_temp) + separator + "Execution" + str(ni + 1) + separator + "Fold" + str(ki + 1)
                    if train_method in {"dimlp", "dimlpBT"}:
                        if train_method == "dimlp":
                            dimlp_command = "dimlpTrn"
                        else:
                            dimlp_command = "dimlpBT"
                            dimlp_command += " -N " + str(nb_dimlp_nets)
                            dimlp_command += " -n " + str(nb_ex_in_one)

                        dimlp_command += " -l " + str(eta)
                        dimlp_command += " -m " + str(mu)
                        dimlp_command += " -f " + str(flat)
                        dimlp_command += " -e " + str(err_thresh)
                        dimlp_command += " -a " + str(acc_thresh)
                        dimlp_command += " -d " + str(delta_err)
                        dimlp_command += " -s " + str(show_err)
                        dimlp_command += " -i " + str(nb_epochs)
                        dimlp_command += " -I " + str(nb_in)
                        dimlp_command += " -O " + str(nb_out)
                        if save_folder is not None:
                            dimlp_command += " -S " + save_folder
                        if attr_file is not None:
                            dimlp_command += " -A " + attr_file
                        if pretrained_weights is not None:
                            dimlp_command += " -W " + pretrained_weights
                        dimlp_command += " -z " + str(seed)
                        dimlp_command += " -q " + str(nb_stairs)
                        for key, value in hk.items():
                            dimlp_command += " -" + key + " " + str(value)

                        dimlp_command += " -L " + folder_path_from_root + separator + "train.txt "
                        dimlp_command += "-T " + folder_path_from_root + separator + "test.txt "
                        dimlp_command += "-1 " + folder_path_from_root + separator + "trainTarget.txt "
                        dimlp_command += "-2 " + folder_path_from_root + separator + "testTarget.txt "

                        dimlp_command += "-p " + folder_path_from_root + separator + "train.out "   # Output train pred file
                        dimlp_command += "-t " + folder_path_from_root + separator + "test.out "    # Output test pred file
                        dimlp_command += "-o " + folder_path_from_root + separator + "stats.txt "    # Output stats file

                        if train_method == "dimlp":
                            dimlp_command += "-V " + folder_path_from_root + separator + "valid.txt "
                            dimlp_command += "-3 " + folder_path_from_root + separator + "validTarget.txt "
                            dimlp_command += "-v " + folder_path_from_root + separator + "valid.out "   # Output validation pred file
                            dimlp_command += "-w " + folder_path_from_root + separator + "weights.wts " # Output weight file
                        else:
                            dimlp_command += "-w " + folder_path_from_root + separator + "weightsBT " # Output weight generic filename

                        if is_dimlprul:
                            dimlp_command += "-R -F " + folder_path_from_root + separator + "dimlpRules.rls "

                        dimlp_command += "-r " + str(crossval_folder_temp) + separator + "consoleTemp.txt" # To not show console result

                        if train_method == "dimlp":
                            print("Enter in DimlpTrn function")
                            res = dimlp.dimlpTrn(dimlp_command)
                        else:
                            print("Enter in DimlpBT function")
                            res = dimlp.dimlpBT(dimlp_command)
                        if (res == -1):
                            return -1 # If there is an error in the Trn

                    # Training with svm
                    elif train_method == "svm":
                        print("Enter in svmTrn function")
                        res = svmTrn(train_data=folder_path_from_root + separator + "train.txt",train_class=folder_path_from_root + separator + "trainTarget.txt",
                                     test_data=folder_path_from_root + separator + "test.txt",test_class=folder_path_from_root + separator + "testTarget.txt",
                                     weights = folder_path_from_root + separator + "weights", stats = folder_path_from_root + separator + "stats.txt",
                                     output_file = crossval_folder_temp + separator + "consoleTemp.txt", train_pred = folder_path_from_root + separator + "train",
                                     test_pred = folder_path_from_root + separator + "test", save_folder = save_folder, nb_stairs = nb_stairs, hiknot = hiknot,
                                     K = svm_k, C = c_var, kernel = kernel_var, degree = degree_var, gamma = gamma_var, coef0 = coef0_var, shrinking = shrinking_var,
                                     tol = svm_tol_var, cache_size = cache_size_var, class_weight = svm_class_weight_var, max_iter = svm_max_iter_var,
                                     decision_function_shape = decision_function_shape_var, break_ties = break_ties_var)

                        if (res == -1):
                            return -1 # If there is an error in the Trn

                    # Training with mlp
                    elif train_method == "mlp":
                        print("Enter in mlpTrn function")
                        random_state_var = seed
                        if seed == 0:
                            random_state_var = None
                        res = mlpTrn(train_data=folder_path_from_root + separator + "train.txt",train_class=folder_path_from_root + separator + "trainTarget.txt",
                                     test_data=folder_path_from_root + separator + "test.txt",test_class=folder_path_from_root + separator + "testTarget.txt",
                                     weights = folder_path_from_root + separator + "weights", stats = folder_path_from_root + separator + "stats.txt",
                                     output_file = crossval_folder_temp + separator + "consoleTemp.txt", train_pred = folder_path_from_root + separator + "train",
                                     test_pred = folder_path_from_root + separator + "test", save_folder = save_folder, nb_stairs = nb_stairs, hiknot = hiknot,
                                     K = mlp_k, hidden_layer_sizes = hidden_layer_sizes_var, activation = activation_var, solver = solver_var, alpha = alpha_var,
                                     batch_size = batch_size_var, learning_rate = mlp_learning_rate_var, learning_rate_init = learning_rate_init_var, power_t = power_t_var,
                                     max_iter = mlp_max_iter_var, shuffle = shuffle_var, tol = mlp_tol_var, warm_start = mlp_warm_start_var, momentum = momentum_var,
                                     nesterovs_momentum = nesterovs_momentum_var, early_stopping = early_stopping_var, validation_fraction = mlp_validation_fraction_var,
                                     beta_1 = beta_1_var, beta_2 = beta_2_var, epsilon = epsilon_var, n_iter_no_change = mlp_n_iter_no_change_var, max_fun = max_fun_var,
                                     random_state = random_state_var)

                        if (res == -1):
                            return -1 # If there is an error in the Trn

                    # Training with random forests
                    elif train_method == "randForest":
                        print("Enter in randForestsTrn function")
                        random_state_var = seed
                        if seed == 0:
                            random_state_var = None
                        res = randForestsTrn(train_data=folder_path_from_root + separator + "train.txt",train_class=folder_path_from_root + separator + "trainTarget.txt",
                                     test_data=folder_path_from_root + separator + "test.txt",test_class=folder_path_from_root + separator + "testTarget.txt",
                                     stats = folder_path_from_root + separator + "stats.txt", output_file = crossval_folder_temp + separator + "consoleTemp.txt",
                                     train_pred = folder_path_from_root + separator + "train", test_pred = folder_path_from_root + separator + "test",
                                     rules_file = folder_path_from_root + separator + "treesRules", save_folder = save_folder, n_estimators = n_estimators_var,
                                     min_samples_split = min_samples_split_var, min_samples_leaf = min_samples_leaf_var, min_weight_fraction_leaf = min_weight_fraction_leaf_var,
                                     max_features = max_features_var, min_impurity_decrease = min_impurity_decrease_var, max_leaf_nodes = max_leaf_nodes_var,
                                     warm_start = dt_warm_start_var, ccp_alpha = ccp_alpha_var, criterion = rf_criterion_var, max_depth = rf_max_depth_var,
                                     bootstrap = bootstrap_var, oob_score = oob_score_var, n_jobs = n_jobs_var, class_weight = rf_class_weight_var,
                                     max_samples = max_samples_var, random_state = random_state_var)

                        if (res == -1):
                            return -1 # If there is an error in the Trn

                    # Training with gradient boosting
                    else:
                        print("Enter in gradBoostTrn function")
                        random_state_var = seed
                        if seed == 0:
                            random_state_var = None
                        res = gradBoostTrn(train_data=folder_path_from_root + separator + "train.txt",train_class=folder_path_from_root + separator + "trainTarget.txt",
                                     test_data=folder_path_from_root + separator + "test.txt",test_class=folder_path_from_root + separator + "testTarget.txt",
                                     stats = folder_path_from_root + separator + "stats.txt", output_file = crossval_folder_temp + separator + "consoleTemp.txt",
                                     train_pred = folder_path_from_root + separator + "train", test_pred = folder_path_from_root + separator + "test",
                                     rules_file = folder_path_from_root + separator + "treesRules", save_folder = save_folder, n_estimators = n_estimators_var,
                                     min_samples_split = min_samples_split_var, min_samples_leaf = min_samples_leaf_var, min_weight_fraction_leaf = min_weight_fraction_leaf_var,
                                     max_features = max_features_var, min_impurity_decrease = min_impurity_decrease_var, max_leaf_nodes = max_leaf_nodes_var,
                                     warm_start = dt_warm_start_var, ccp_alpha = ccp_alpha_var, loss = loss_var, learning_rate = gb_learning_rate_var,
                                     subsample = subsample_var, criterion = gb_criterion_var, max_depth = gb_max_depth_var, init = init_var,
                                     validation_fraction = gb_validation_fraction_var, n_iter_no_change = gb_n_iter_no_change_var, tol = gb_tol_var,
                                     random_state = random_state_var)

                        if (res == -1):
                            return -1 # If there is an error in the Trn

                    if is_dimlprul:
                        folder_path_from_real_root = str(crossval_folder) + separator + "Execution" + str(ni + 1) + separator + "Fold" + str(ki + 1)
                        # Get statistics from dimlpRul
                        stats = get_dimlprul_stats(folder_path_from_real_root + separator + "dimlpRules.rls")
                        test_acc = get_test_acc(folder_path_from_real_root + separator + "stats.txt", train_method)

                        if not stats:
                            nb_no_rules_current_exec_dimlp += 1
                            nb_no_rules_dimlp += 1
                        else:
                            mean_nb_rules_dimlp += stats[0]
                            mean_nb_cover_dimlp += stats[3]
                            mean_nb_antecedants_dimlp += stats[2]
                            mean_fidel_dimlp += stats[5]
                            mean_rules_acc_dimlp += stats[4]
                            mean_default_rate_dimlp += stats[7]
                            mean_test_acc_when_rules_and_model_agree_dimlp += stats[6]
                        mean_test_acc_dimlp += test_acc


                    if is_fidex:
                        # Compute fidex stats in folder
                        fidex_command = "fidex"
                        if train_method in {"dimlp", "dimlpBT", "svm", "mlp"}:
                            fidex_command +=  " -I " + str(hiknot)
                            fidex_command +=  " -Q " + str(nb_stairs)
                        if save_folder is not None:
                            fidex_command +=  " -R " + save_folder
                        if attr_file is not None:
                            fidex_command +=  " -A " + attr_file
                        fidex_command +=  " -i " + str(max_iter)
                        fidex_command +=  " -v " + str(min_cov)
                        if dropout_dim != None:
                            fidex_command +=  " -d " + str(dropout_dim)
                        if dropout_hyp != None:
                            fidex_command +=  " -h " + str(dropout_hyp)
                        fidex_command +=  " -z " + str(seed)

                        if train_method in {"dimlp", "svm", "mlp"}:
                            fidex_command +=  " -W " + folder_path_from_root + separator + "weights.wts"
                        elif train_method == "dimlpBT":
                            fidex_command +=  " -W " + folder_path_from_root + separator + "weightsBT"
                            fidex_command +=  " -N " + str(nb_dimlp_nets)
                        else:
                            fidex_command += " -f " + folder_path_from_root + separator + "treesRules.rls"

                        fidex_command += " -T " + folder_path_from_root + separator + "train.txt"
                        fidex_command += " -P " + folder_path_from_root + separator + "train.out"
                        fidex_command += " -C " + folder_path_from_root + separator + "trainTarget.txt"
                        fidex_command += " -S " + folder_path_from_root + separator + "test.txt"
                        fidex_command += " -p " + folder_path_from_root + separator + "test.out"
                        fidex_command += " -c " + folder_path_from_root + separator + "testTarget.txt"
                        fidex_command += " -O " + folder_path_from_root + separator + "fidexRule.txt"
                        fidex_command += " -s " + folder_path_from_root + separator + "fidexStats.txt"
                        fidex_command += " -r " + folder_path_from_root + separator + "fidexResult.txt"

                        print("Enter in fidex function")
                        res_fid = fidex.fidex(fidex_command)
                        if res_fid == -1:
                            return -1 # If there is an error in fidex

                        # Get statistics from fidex
                        stats_file = folder_path + separator + "fidexStats.txt"

                        try:
                            with open(stats_file, "r") as my_file:
                                line = my_file.readline()
                                line = my_file.readline()
                                line = my_file.readline()
                                stat_vals = []
                                while line:
                                    line = line.strip()  # Remove the line break at the end of the line
                                    if line != "":
                                        elements = line.split()
                                        stat_vals.append(float(elements[-1]))
                                    line = my_file.readline()
                                my_file.close()
                        except (FileNotFoundError):
                            raise ValueError(f"Error : Fidex stat file {stats_file} not found.")
                        except (IOError):
                            raise ValueError(f"Error : Couldn't open fidex stat file {stats_file}.")

                        mean_cov_size_fid += stat_vals[0]
                        mean_nb_ant_fid += stat_vals[1]
                        mean_fidel_fid += stat_vals[2]
                        mean_rules_acc_fid += stat_vals[3]
                        if len(stat_vals) == 5:
                            mean_confid_fid += stat_vals[4]
                        else:
                            has_confidence = False

                    if is_fidexglo:
                        # Compute fidexGlo rules in folder
                        fidexglo_rules_command = "fidexGloRules"
                        if train_method in {"dimlp", "dimlpBT", "svm", "mlp"}:
                            fidexglo_rules_command +=  " -I " + str(hiknot)
                            fidexglo_rules_command +=  " -Q " + str(nb_stairs)
                        if save_folder is not None:
                            fidexglo_rules_command +=  " -S " + save_folder
                        if attr_file is not None:
                            fidexglo_rules_command +=  " -A " + attr_file
                        fidexglo_rules_command +=  " -i " + str(max_iter)
                        fidexglo_rules_command +=  " -v " + str(min_cov)
                        if dropout_dim != None:
                            fidexglo_rules_command +=  " -d " + str(dropout_dim)
                        if dropout_hyp != None:
                            fidexglo_rules_command +=  " -h " + str(dropout_hyp)
                        fidexglo_rules_command +=  " -z " + str(seed)
                        if train_method in {"dimlp", "svm", "mlp"}:
                            fidexglo_rules_command +=  " -W " + folder_path_from_root + separator + "weights.wts"
                        elif train_method == "dimlpBT":
                            fidexglo_rules_command +=  " -W " + folder_path_from_root + separator + "weightsBT"
                            fidexglo_rules_command +=  " -N " + str(nb_dimlp_nets)
                        else:
                            fidexglo_rules_command += " -f " + folder_path_from_root + separator + "treesRules.rls"

                        fidexglo_rules_command += " -T " + folder_path_from_root + separator + "train.txt"
                        fidexglo_rules_command += " -P " + folder_path_from_root + separator + "train.out"
                        fidexglo_rules_command += " -C " + folder_path_from_root + separator + "trainTarget.txt"
                        fidexglo_rules_command += " -O " + folder_path_from_root + separator + "fidexGloRules.txt"
                        fidexglo_rules_command += " -r " + folder_path_from_root + separator + "fidexGloResult.txt"
                        fidexglo_rules_command += " -M " + str(fidexglo_heuristic)

                        print("Enter in fidexGloRules function")
                        res_fid_glo_rules = fidexGlo.fidexGloRules(fidexglo_rules_command)
                        if res_fid_glo_rules == -1:
                            return -1 # If there is an error in fidexGloRules

                        # Compute fidexGlo statistics in folder
                        fidexglo_stats_command = "fidexGloStats"
                        if save_folder is not None:
                            fidexglo_stats_command +=  " -S " + save_folder
                        if attr_file is not None:
                            fidexglo_stats_command +=  " -A " + attr_file
                        fidexglo_stats_command += " -T " + folder_path_from_root + separator + "test.txt"
                        fidexglo_stats_command += " -P " + folder_path_from_root + separator + "test.out"
                        fidexglo_stats_command += " -C " + folder_path_from_root + separator + "testTarget.txt"
                        fidexglo_stats_command += " -R " + folder_path_from_root + separator + "fidexGloRules.txt"
                        fidexglo_stats_command += " -O " + folder_path_from_root + separator + "fidexGloStats.txt"
                        fidexglo_stats_command += " -r " + folder_path_from_root + separator + "fidexGloStatsResult.txt"

                        print("Enter in fidexGloStats function")
                        res_fid_glo_stats = fidexGlo.fidexGloStats(fidexglo_stats_command)
                        if res_fid_glo_stats == -1:
                            return -1 # If there is an error in fidexGloStats

                        # Get statistics from fidexGlo
                        stats_glo_file = folder_path + separator + "fidexGloStats.txt"

                        try:
                            with open(stats_glo_file, "r") as my_file:
                                line_stats_glo = my_file.readline()
                                line_stats_glo = my_file.readline()
                                line_stats_glo = line_stats_glo.strip()
                                if line_stats_glo != "":
                                    values = line_stats_glo.split()
                                    if "rules" in values:
                                        mean_nb_rules += float(values[values.index("rules") + 2].rstrip(','))
                                    else:
                                        raise ValueError("Error in second line of fidexGlo stat file, there is not the number of rules")
                                    if "sample" in values:
                                        mean_nb_cover += float(values[values.index("sample") + 6].rstrip(','))
                                    else:
                                        raise ValueError("Error in second line of fidexGlo stat file, there is not the mean sample covering number per rule")
                                    if "antecedents" in values:
                                        mean_nb_antecedants += float(values[values.index("antecedents") + 4])
                                    else:
                                        raise ValueError("Error in second line of fidexGlo stat file, there is not the mean number of antecedents per rule")

                                else:
                                    raise ValueError("Error in second line of fidexGlo stat file.")

                                line_stats_glo = my_file.readline()
                                line_stats_glo = my_file.readline()
                                line_stats_glo = my_file.readline()
                                line_stats_glo = my_file.readline()
                                stat_glo_vals = []
                                while line_stats_glo:
                                    line_stats_glo = line_stats_glo.strip()  # Remove the line break at the end of the line
                                    if line_stats_glo != "":
                                        elements = line_stats_glo.split()
                                        stat_glo_vals.append(float(elements[-1]))
                                    line_stats_glo = my_file.readline()

                                my_file.close()
                        except (FileNotFoundError):
                            raise ValueError(f"Error : FidexGlo stat file {stats_glo_file} not found.")
                        except (IOError):
                            raise ValueError(f"Error : Couldn't open fidexGlo stat file {stats_glo_file}.")

                        mean_fidel_glo += stat_glo_vals[0]
                        mean_rules_acc_glo += stat_glo_vals[1]
                        mean_expl_glo += stat_glo_vals[2]
                        mean_default_rate += stat_glo_vals[3]
                        mean_nb_fidel_activations += stat_glo_vals[4]
                        mean_wrong_activations += stat_glo_vals[5]
                        mean_test_acc_glo += stat_glo_vals[6]
                        mean_test_acc_when_rules_and_model_agree += stat_glo_vals[7]
                        mean_test_acc_when_activated_rules_and_model_agree += stat_glo_vals[8]
                # Compute execution Stats

                mean_current_exec_values_dimlp = []
                if is_dimlprul: # For DimlpRul
                    nb_fold_with_rules_dimlp = k-nb_no_rules_current_exec_dimlp

                    mean_current_exec_values_dimlp.append(mean_nb_rules_dimlp / nb_fold_with_rules_dimlp)
                    mean_nb_rules_dimlp = 0
                    mean_current_exec_values_dimlp.append(mean_nb_cover_dimlp / nb_fold_with_rules_dimlp)
                    mean_nb_cover_dimlp = 0
                    mean_current_exec_values_dimlp.append(mean_nb_antecedants_dimlp / nb_fold_with_rules_dimlp)
                    mean_nb_antecedants_dimlp = 0
                    mean_current_exec_values_dimlp.append(mean_fidel_dimlp / nb_fold_with_rules_dimlp)
                    mean_fidel_dimlp = 0
                    mean_current_exec_values_dimlp.append(mean_rules_acc_dimlp / nb_fold_with_rules_dimlp)
                    mean_rules_acc_dimlp = 0
                    mean_current_exec_values_dimlp.append(mean_default_rate_dimlp / nb_fold_with_rules_dimlp)
                    mean_default_rate_dimlp = 0
                    mean_current_exec_values_dimlp.append(mean_test_acc_dimlp / k)
                    mean_test_acc_dimlp = 0
                    mean_current_exec_values_dimlp.append(mean_test_acc_when_rules_and_model_agree_dimlp / nb_fold_with_rules_dimlp)
                    mean_test_acc_when_rules_and_model_agree_dimlp = 0
                    mean_current_exec_values_dimlp.append(nb_no_rules_current_exec_dimlp)
                    mean_exec_values_dimlp.append(mean_current_exec_values_dimlp)

                mean_current_exec_values_fidex = []
                if is_fidex:  # For Fidex
                    mean_current_exec_values_fidex.append(mean_cov_size_fid / k)
                    mean_cov_size_fid = 0
                    mean_current_exec_values_fidex.append(mean_nb_ant_fid / k)
                    mean_nb_ant_fid = 0
                    mean_current_exec_values_fidex.append(mean_fidel_fid / k)
                    mean_fidel_fid = 0
                    mean_current_exec_values_fidex.append(mean_rules_acc_fid / k)
                    mean_rules_acc_fid = 0
                    mean_current_exec_values_fidex.append(mean_confid_fid / k)
                    mean_confid_fid = 0
                    mean_exec_values_fidex.append(mean_current_exec_values_fidex)

                mean_current_exec_values_fidexglo = []
                if is_fidexglo:  # For FidexGlo
                    mean_current_exec_values_fidexglo.append(mean_nb_rules / k)
                    mean_nb_rules = 0
                    mean_current_exec_values_fidexglo.append(mean_nb_cover / k)
                    mean_nb_cover = 0
                    mean_current_exec_values_fidexglo.append(mean_nb_antecedants / k)
                    mean_nb_antecedants = 0
                    mean_current_exec_values_fidexglo.append(mean_fidel_glo / k)
                    mean_fidel_glo = 0
                    mean_current_exec_values_fidexglo.append(mean_rules_acc_glo / k)
                    mean_rules_acc_glo = 0
                    mean_current_exec_values_fidexglo.append(mean_expl_glo / k)
                    mean_expl_glo = 0
                    mean_current_exec_values_fidexglo.append(mean_default_rate / k)
                    mean_default_rate = 0
                    mean_current_exec_values_fidexglo.append(mean_nb_fidel_activations / k)
                    mean_nb_fidel_activations = 0
                    mean_current_exec_values_fidexglo.append(mean_wrong_activations / k)
                    mean_wrong_activations = 0
                    mean_current_exec_values_fidexglo.append(mean_test_acc_glo / k)
                    mean_test_acc_glo = 0
                    mean_current_exec_values_fidexglo.append(mean_test_acc_when_rules_and_model_agree / k)
                    mean_test_acc_when_rules_and_model_agree = 0
                    mean_current_exec_values_fidexglo.append(mean_test_acc_when_activated_rules_and_model_agree / k)
                    mean_test_acc_when_activated_rules_and_model_agree = 0
                    mean_exec_values_fidexglo.append(mean_current_exec_values_fidexglo)

                # Output and show stats
                try:
                    with open(crossval_stats, "a") as outputStatsFile:
                        outputStatsFile.write(f"Results for execution {ni + 1} of {k}-Cross validation :\n\n")
                        print("\n---------------------------------------------------------")
                        print("---------------------------------------------------------\n")
                        print(f"Results for execution {ni + 1} of {k}-Cross validation :\n")
                        if is_dimlprul:
                            formatted_mean_current_exec_values_dimlp = []
                            for i in range(len(mean_current_exec_values_dimlp)):
                                formatted_val = "{:.6f}".format(mean_current_exec_values_dimlp[i]).rstrip(".0")
                                if formatted_val == "":
                                    formatted_val = "0"
                                formatted_mean_current_exec_values_dimlp.append(formatted_val)
                            outputStatsFile.write("Dimlp :\n")
                            outputStatsFile.write(f"The mean number of rules is: {formatted_mean_current_exec_values_dimlp[0]}\n")
                            if nb_no_rules_current_exec_dimlp > 0:
                                if nb_no_rules_current_exec_dimlp == 1:
                                    outputStatsFile.write(f"We didn't found any rule {int(nb_no_rules_current_exec_dimlp)} time\n")
                                else:
                                    outputStatsFile.write(f"We didn't found any rule {int(nb_no_rules_current_exec_dimlp)} times\n")
                            outputStatsFile.write(f"The mean sample covering number per rule is: {formatted_mean_current_exec_values_dimlp[1]}\n")
                            outputStatsFile.write(f"The mean number of antecedents per rule is: {formatted_mean_current_exec_values_dimlp[2]}\n")
                            outputStatsFile.write(f"The mean global rule fidelity rate is: {formatted_mean_current_exec_values_dimlp[3]}\n")
                            outputStatsFile.write(f"The mean global rule accuracy is: {formatted_mean_current_exec_values_dimlp[4]}\n")
                            outputStatsFile.write(f"The mean default rule rate (when we can't find a rule) is: {formatted_mean_current_exec_values_dimlp[5]}\n")
                            outputStatsFile.write(f"The mean model test accuracy is: {formatted_mean_current_exec_values_dimlp[6]}\n")
                            outputStatsFile.write(f"The mean model test accuracy when rules and model agree is: {formatted_mean_current_exec_values_dimlp[7]}\n")
                            print("Dimlp :")
                            print(f"The mean number of rules is: {formatted_mean_current_exec_values_dimlp[0]}")
                            if nb_no_rules_current_exec_dimlp > 0:
                                if nb_no_rules_current_exec_dimlp == 1:
                                    print(f"We didn't found any rule {int(nb_no_rules_current_exec_dimlp)} time")
                                else:
                                    print(f"We didn't found any rule {int(nb_no_rules_current_exec_dimlp)} times")
                            print(f"The mean sample covering number per rule is: {formatted_mean_current_exec_values_dimlp[1]}")
                            print(f"The mean number of antecedents per rule is: {formatted_mean_current_exec_values_dimlp[2]}")
                            print(f"The mean global rule fidelity rate is: {formatted_mean_current_exec_values_dimlp[3]}")
                            print(f"The mean global rule accuracy is: {formatted_mean_current_exec_values_dimlp[4]}")
                            print(f"The mean default rule rate (when we can't find a rule) is: {formatted_mean_current_exec_values_dimlp[5]}")
                            print(f"The mean model test accuracy is: {formatted_mean_current_exec_values_dimlp[6]}")
                            print(f"The mean model test accuracy when rules and model agree is: {formatted_mean_current_exec_values_dimlp[7]}")
                            print("\n---------------------------------------------------------")
                            print("---------------------------------------------------------")
                            outputStatsFile.write("\n---------------------------------------------------------\n")
                            print("")
                        if is_fidex:
                            formatted_mean_current_exec_values_fidex = []
                            for i in range(len(mean_current_exec_values_fidex)):
                                formatted_val = "{:.6f}".format(mean_current_exec_values_fidex[i]).rstrip(".0")
                                if formatted_val == "":
                                    formatted_val = "0"
                                formatted_mean_current_exec_values_fidex.append(formatted_val)
                            outputStatsFile.write("Fidex :\n")
                            outputStatsFile.write(f"The mean covering size per rule is: {formatted_mean_current_exec_values_fidex[0]}\n")
                            outputStatsFile.write(f"The mean number of antecedents per rule is: {formatted_mean_current_exec_values_fidex[1]}\n")
                            outputStatsFile.write(f"The mean rule fidelity rate is: {formatted_mean_current_exec_values_fidex[2]}\n")
                            outputStatsFile.write(f"The mean rule accuracy is: {formatted_mean_current_exec_values_fidex[3]}\n")
                            if has_confidence:
                                outputStatsFile.write(f"The mean rule confidence is: {formatted_mean_current_exec_values_fidex[4]}\n")
                            print("Fidex :")
                            print(f"The mean covering size per rule is: {formatted_mean_current_exec_values_fidex[0]}")
                            print(f"The mean number of antecedents per rule is: {formatted_mean_current_exec_values_fidex[1]}")
                            print(f"The mean rule fidelity rate is: {formatted_mean_current_exec_values_fidex[2]}")
                            print(f"The mean rule accuracy is: {formatted_mean_current_exec_values_fidex[3]}")
                            if has_confidence:
                                print(f"The mean rule confidence is: {formatted_mean_current_exec_values_fidex[4]}")
                            print("\n---------------------------------------------------------")
                            print("---------------------------------------------------------")
                        if is_fidex and is_fidexglo:
                            outputStatsFile.write("\n---------------------------------------------------------\n")
                            print("")
                        if is_fidexglo:
                            formatted_mean_current_exec_values_fidexglo = []
                            for i in range(len(mean_current_exec_values_fidexglo)):
                                formatted_val = "{:.6f}".format(mean_current_exec_values_fidexglo[i]).rstrip(".0")
                                if formatted_val == "":
                                    formatted_val = "0"
                                formatted_mean_current_exec_values_fidexglo.append(formatted_val)
                            outputStatsFile.write("FidexGlo :\n")
                            outputStatsFile.write(f"The mean number of rules is: {formatted_mean_current_exec_values_fidexglo[0]}\n")
                            outputStatsFile.write(f"The mean sample covering number per rule is: {formatted_mean_current_exec_values_fidexglo[1]}\n")
                            outputStatsFile.write(f"The mean number of antecedents per rule is: {formatted_mean_current_exec_values_fidexglo[2]}\n")
                            outputStatsFile.write(f"The mean global rule fidelity rate is: {formatted_mean_current_exec_values_fidexglo[3]}\n")
                            outputStatsFile.write(f"The mean global rule accuracy is: {formatted_mean_current_exec_values_fidexglo[4]}\n")
                            outputStatsFile.write(f"The mean explainability rate (when we can find a rule) is: {formatted_mean_current_exec_values_fidexglo[5]}\n")
                            outputStatsFile.write(f"The mean default rule rate (when we can't find a rule) is: {formatted_mean_current_exec_values_fidexglo[6]}\n")
                            outputStatsFile.write(f"The mean number of correct (fidel) activated rules per sample is: {formatted_mean_current_exec_values_fidexglo[7]}\n")
                            outputStatsFile.write(f"The mean number of wrong (not fidel) activated rules per sample is: {formatted_mean_current_exec_values_fidexglo[8]}\n")
                            outputStatsFile.write(f"The mean model test accuracy is: {formatted_mean_current_exec_values_fidexglo[9]}\n")
                            outputStatsFile.write(f"The mean model test accuracy when rules and model agree is: {formatted_mean_current_exec_values_fidexglo[10]}\n")
                            outputStatsFile.write(f"The mean model test accuracy when activated rules and model agree is: {formatted_mean_current_exec_values_fidexglo[11]}\n")
                            print("FidexGlo :")
                            print(f"The mean number of rules is: {formatted_mean_current_exec_values_fidexglo[0]}")
                            print(f"The mean sample covering number per rule is: {formatted_mean_current_exec_values_fidexglo[1]}")
                            print(f"The mean number of antecedents per rule is: {formatted_mean_current_exec_values_fidexglo[2]}")
                            print(f"The mean global rule fidelity rate is: {formatted_mean_current_exec_values_fidexglo[3]}")
                            print(f"The mean global rule accuracy is: {formatted_mean_current_exec_values_fidexglo[4]}")
                            print(f"The mean explainability rate (when we can find a rule) is: {formatted_mean_current_exec_values_fidexglo[5]}")
                            print(f"The mean default rule rate (when we can't find a rule) is: {formatted_mean_current_exec_values_fidexglo[6]}")
                            print(f"The mean number of correct (fidel) activated rules per sample is: {formatted_mean_current_exec_values_fidexglo[7]}")
                            print(f"The mean number of wrong (not fidel) activated rules per sample is: {formatted_mean_current_exec_values_fidexglo[8]}")
                            print(f"The mean model test accuracy is: {formatted_mean_current_exec_values_fidexglo[9]}")
                            print(f"The mean model test accuracy when rules and model agree is: {formatted_mean_current_exec_values_fidexglo[10]}")
                            print(f"The mean model test accuracy when activated rules and model agree is: {formatted_mean_current_exec_values_fidexglo[11]}")
                            print("\n---------------------------------------------------------")
                            print("---------------------------------------------------------")
                        outputStatsFile.write("\n---------------------------------------------------------\n")
                        outputStatsFile.write("---------------------------------------------------------\n\n")

                        outputStatsFile.close()
                except (FileNotFoundError):
                    raise ValueError(f"Error : File for stats extraction ({crossval_stats}) not found.")
                except (IOError):
                    raise ValueError(f"Error : Couldn't open stats extraction file {crossval_stats}.")

            # Compute stats on all executions

            if is_dimlprul: # For dimlpRul
                for exec in range(n):
                    multiplier = k-mean_exec_values_dimlp[exec][8] # If there is lack of some datas (sometimes we didnt found any rules), we need to take it into account
                    mean_nb_rules_dimlp_all += multiplier*mean_exec_values_dimlp[exec][0]
                    mean_nb_cover_dimlp_all += multiplier*mean_exec_values_dimlp[exec][1]
                    mean_nb_antecedants_dimlp_all += multiplier*mean_exec_values_dimlp[exec][2]
                    mean_fidel_dimlp_all += multiplier*mean_exec_values_dimlp[exec][3]
                    mean_rules_acc_dimlp_all += multiplier*mean_exec_values_dimlp[exec][4]
                    mean_default_rate_dimlp_all += multiplier*mean_exec_values_dimlp[exec][5]
                    mean_test_acc_dimlp_all += multiplier*mean_exec_values_dimlp[exec][6]
                    mean_test_acc_when_rules_and_model_agree_dimlp_all += multiplier*mean_exec_values_dimlp[exec][7]

                divider = n*k-nb_no_rules_dimlp
                mean_nb_rules_dimlp_all /= divider
                mean_nb_cover_dimlp_all /= divider
                mean_nb_antecedants_dimlp_all /= divider
                mean_fidel_dimlp_all /= divider
                mean_rules_acc_dimlp_all /= divider
                mean_default_rate_dimlp_all /= divider
                mean_test_acc_dimlp_all /= divider
                mean_test_acc_when_rules_and_model_agree_dimlp_all /= divider

                for exec in range(n):
                    std_nb_rules_dimlp_all += pow(mean_exec_values_dimlp[exec][0] - mean_nb_rules_dimlp_all, 2)
                    std_nb_cover_dimlp_all += pow(mean_exec_values_dimlp[exec][1] - mean_nb_cover_dimlp_all, 2)
                    std_nb_antecedants_dimlp_all += pow(mean_exec_values_dimlp[exec][2] - mean_nb_antecedants_dimlp_all, 2)
                    std_fidel_dimlp_all += pow(mean_exec_values_dimlp[exec][3] - mean_fidel_dimlp_all, 2)
                    std_rules_acc_dimlp_all += pow(mean_exec_values_dimlp[exec][4] - mean_rules_acc_dimlp_all, 2)
                    std_default_rate_dimlp_all += pow(mean_exec_values_dimlp[exec][5] - mean_default_rate_dimlp_all, 2)
                    std_test_acc_dimlp_all += pow(mean_exec_values_dimlp[exec][6] - mean_test_acc_dimlp_all, 2)
                    std_test_acc_when_rules_and_model_agree_dimlp_all += pow(mean_exec_values_dimlp[exec][7] - mean_test_acc_when_rules_and_model_agree_dimlp_all, 2)

                std_nb_rules_dimlp_all = math.sqrt(std_nb_rules_dimlp_all / n)
                std_nb_cover_dimlp_all = math.sqrt(std_nb_cover_dimlp_all / n)
                std_nb_antecedants_dimlp_all = math.sqrt(std_nb_antecedants_dimlp_all / n)
                std_fidel_dimlp_all = math.sqrt(std_fidel_dimlp_all / n)
                std_rules_acc_dimlp_all = math.sqrt(std_rules_acc_dimlp_all / n)
                std_default_rate_dimlp_all = math.sqrt(std_default_rate_dimlp_all / n)
                std_test_acc_dimlp_all = math.sqrt(std_test_acc_dimlp_all / n)
                std_test_acc_when_rules_and_model_agree_dimlp_all = math.sqrt(std_test_acc_when_rules_and_model_agree_dimlp_all / n)

            if is_fidex: # For Fidex
                for exec in range(n):
                    mean_cov_size_fid_all += mean_exec_values_fidex[exec][0]
                    mean_nb_ant_fid_all += mean_exec_values_fidex[exec][1]
                    mean_fidel_fid_all += mean_exec_values_fidex[exec][2]
                    mean_rules_acc_fid_all += mean_exec_values_fidex[exec][3]
                    mean_confid_fid_all += mean_exec_values_fidex[exec][4]

                mean_cov_size_fid_all /= n
                mean_nb_ant_fid_all /= n
                mean_fidel_fid_all /= n
                mean_rules_acc_fid_all /= n
                mean_confid_fid_all /= n

                for exec in range(n):
                    std_cov_size_fid_all += pow(mean_exec_values_fidex[exec][0] - mean_cov_size_fid_all, 2)
                    std_nb_ant_fid_all += pow(mean_exec_values_fidex[exec][1] - mean_nb_ant_fid_all, 2)
                    std_fidel_fid_all += pow(mean_exec_values_fidex[exec][2] - mean_fidel_fid_all, 2)
                    std_rules_acc_fid_all += pow(mean_exec_values_fidex[exec][3] - mean_rules_acc_fid_all, 2)
                    std_confid_fid_all += pow(mean_exec_values_fidex[exec][4] - mean_confid_fid_all, 2)

                std_cov_size_fid_all = math.sqrt(std_cov_size_fid_all / n)
                std_nb_ant_fid_all = math.sqrt(std_nb_ant_fid_all / n)
                std_fidel_fid_all = math.sqrt(std_fidel_fid_all / n)
                std_rules_acc_fid_all = math.sqrt(std_rules_acc_fid_all / n)
                std_confid_fid_all = math.sqrt(std_confid_fid_all / n)

            if is_fidexglo: # For FidexGlo
                for exec in range(n):
                    mean_nb_rules_all += mean_exec_values_fidexglo[exec][0]
                    mean_nb_cover_all += mean_exec_values_fidexglo[exec][1]
                    mean_nb_antecedants_all += mean_exec_values_fidexglo[exec][2]
                    mean_fidel_glo_all += mean_exec_values_fidexglo[exec][3]
                    mean_rules_acc_glo_all += mean_exec_values_fidexglo[exec][4]
                    mean_expl_glo_all += mean_exec_values_fidexglo[exec][5]
                    mean_default_rate_all += mean_exec_values_fidexglo[exec][6]
                    mean_nb_fidel_activations_all += mean_exec_values_fidexglo[exec][7]
                    mean_wrong_activations_all += mean_exec_values_fidexglo[exec][8]
                    mean_test_acc_glo_all += mean_exec_values_fidexglo[exec][9]
                    mean_test_acc_when_rules_and_model_agree_all += mean_exec_values_fidexglo[exec][10]
                    mean_test_acc_when_activated_rules_and_model_agree_all += mean_exec_values_fidexglo[exec][11]

                mean_nb_rules_all /= n
                mean_nb_cover_all /= n
                mean_nb_antecedants_all /= n
                mean_fidel_glo_all /= n
                mean_rules_acc_glo_all /= n
                mean_expl_glo_all /= n
                mean_default_rate_all /= n
                mean_nb_fidel_activations_all /= n
                mean_wrong_activations_all /= n
                mean_test_acc_glo_all /= n
                mean_test_acc_when_rules_and_model_agree_all /= n
                mean_test_acc_when_activated_rules_and_model_agree_all /= n

                for exec in range(n):
                    std_nb_rules_all += pow(mean_exec_values_fidexglo[exec][0] - mean_nb_rules_all, 2)
                    std_nb_cover_all += pow(mean_exec_values_fidexglo[exec][1] - mean_nb_cover_all, 2)
                    std_nb_antecedants_all += pow(mean_exec_values_fidexglo[exec][2] - mean_nb_antecedants_all, 2)
                    std_fidel_glo_all += pow(mean_exec_values_fidexglo[exec][3] - mean_fidel_glo_all, 2)
                    std_rules_acc_glo_all += pow(mean_exec_values_fidexglo[exec][4] - mean_rules_acc_glo_all, 2)
                    std_expl_glo_all += pow(mean_exec_values_fidexglo[exec][5] - mean_expl_glo_all, 2)
                    std_default_rate_all += pow(mean_exec_values_fidexglo[exec][6] - mean_default_rate_all, 2)
                    std_nb_fidel_activations_all += pow(mean_exec_values_fidexglo[exec][7] - mean_nb_fidel_activations_all, 2)
                    std_wrong_activations_all += pow(mean_exec_values_fidexglo[exec][8] - mean_wrong_activations_all, 2)
                    std_test_acc_glo_all += pow(mean_exec_values_fidexglo[exec][9] - mean_test_acc_glo_all, 2)
                    std_test_acc_when_rules_and_model_agree_all += pow(mean_exec_values_fidexglo[exec][10] - mean_test_acc_when_rules_and_model_agree_all, 2)
                    std_test_acc_when_activated_rules_and_model_agree_all += pow(mean_exec_values_fidexglo[exec][11] - mean_test_acc_when_activated_rules_and_model_agree_all, 2)

                std_nb_rules_all = math.sqrt(std_nb_rules_all / n)
                std_nb_cover_all = math.sqrt(std_nb_cover_all / n)
                std_nb_antecedants_all = math.sqrt(std_nb_antecedants_all / n)
                std_fidel_glo_all = math.sqrt(std_fidel_glo_all / n)
                std_rules_acc_glo_all = math.sqrt(std_rules_acc_glo_all / n)
                std_expl_glo_all = math.sqrt(std_expl_glo_all / n)
                std_default_rate_all = math.sqrt(std_default_rate_all / n)
                std_nb_fidel_activations_all = math.sqrt(std_nb_fidel_activations_all / n)
                std_wrong_activations_all = math.sqrt(std_wrong_activations_all / n)
                std_test_acc_glo_all = math.sqrt(std_test_acc_glo_all / n)
                std_test_acc_when_rules_and_model_agree_all = math.sqrt(std_test_acc_when_rules_and_model_agree_all / n)
                std_test_acc_when_activated_rules_and_model_agree_all = math.sqrt(std_test_acc_when_activated_rules_and_model_agree_all / n)

            # Show and save results
            try:
                with open(crossval_stats, "a") as outputStatsFile:
                    outputStatsFile.write(f"Results for {n} times {k}-Cross validation :\n\n")
                    print("\n---------------------------------------------------------")
                    print("---------------------------------------------------------\n")
                    print(f"Results for {n} times {k}-Cross validation :\n")
                    if is_dimlprul:

                        formatted_mean_nb_rules_dimlp_all = "{:.6f}".format(mean_nb_rules_dimlp_all).rstrip(".0")
                        if formatted_mean_nb_rules_dimlp_all == "":
                            formatted_mean_nb_rules_dimlp_all = "0"

                        formatted_std_nb_rules_dimlp_all = "{:.6f}".format(std_nb_rules_dimlp_all).rstrip(".0")
                        if formatted_std_nb_rules_dimlp_all == "":
                            formatted_std_nb_rules_dimlp_all = "0"

                        formatted_mean_nb_cover_dimlp_all = "{:.6f}".format(mean_nb_cover_dimlp_all).rstrip(".0")
                        if formatted_mean_nb_cover_dimlp_all == "":
                            formatted_mean_nb_cover_dimlp_all = "0"

                        formatted_std_nb_cover_dimlp_all = "{:.6f}".format(std_nb_cover_dimlp_all).rstrip(".0")
                        if formatted_std_nb_cover_dimlp_all == "":
                            formatted_std_nb_cover_dimlp_all = "0"

                        formatted_mean_nb_antecedants_dimlp_all = "{:.6f}".format(mean_nb_antecedants_dimlp_all).rstrip(".0")
                        if formatted_mean_nb_antecedants_dimlp_all == "":
                            formatted_mean_nb_antecedants_dimlp_all = "0"

                        formatted_std_nb_antecedants_dimlp_all = "{:.6f}".format(std_nb_antecedants_dimlp_all).rstrip(".0")
                        if formatted_std_nb_antecedants_dimlp_all == "":
                            formatted_std_nb_antecedants_dimlp_all = "0"

                        formatted_mean_fidel_dimlp_all = "{:.6f}".format(mean_fidel_dimlp_all).rstrip(".0")
                        if formatted_mean_fidel_dimlp_all == "":
                            formatted_mean_fidel_dimlp_all = "0"

                        formatted_std_fidel_dimlp_all = "{:.6f}".format(std_fidel_dimlp_all).rstrip(".0")
                        if formatted_std_fidel_dimlp_all == "":
                            formatted_std_fidel_dimlp_all = "0"

                        formatted_mean_rules_acc_dimlp_all = "{:.6f}".format(mean_rules_acc_dimlp_all).rstrip(".0")
                        if formatted_mean_rules_acc_dimlp_all == "":
                            formatted_mean_rules_acc_dimlp_all = "0"

                        formatted_std_rules_acc_dimlp_all = "{:.6f}".format(std_rules_acc_dimlp_all).rstrip(".0")
                        if formatted_std_rules_acc_dimlp_all == "":
                            formatted_std_rules_acc_dimlp_all = "0"

                        formatted_mean_default_rate_dimlp_all = "{:.6f}".format(mean_default_rate_dimlp_all).rstrip(".0")
                        if formatted_mean_default_rate_dimlp_all == "":
                            formatted_mean_default_rate_dimlp_all = "0"

                        formatted_std_default_rate_dimlp_all = "{:.6f}".format(std_default_rate_dimlp_all).rstrip(".0")
                        if formatted_std_default_rate_dimlp_all == "":
                            formatted_std_default_rate_dimlp_all = "0"

                        formatted_mean_test_acc_dimlp_all = "{:.6f}".format(mean_test_acc_dimlp_all).rstrip(".0")
                        if formatted_mean_test_acc_dimlp_all == "":
                            formatted_mean_test_acc_dimlp_all = "0"

                        formatted_std_test_acc_dimlp_all = "{:.6f}".format(std_test_acc_dimlp_all).rstrip(".0")
                        if formatted_std_test_acc_dimlp_all == "":
                            formatted_std_test_acc_dimlp_all = "0"

                        formatted_mean_test_acc_when_rules_and_model_agree_dimlp_all = "{:.6f}".format(mean_test_acc_when_rules_and_model_agree_dimlp_all).rstrip(".0")
                        if formatted_mean_test_acc_when_rules_and_model_agree_dimlp_all == "":
                            formatted_mean_test_acc_when_rules_and_model_agree_dimlp_all = "0"

                        formatted_std_test_acc_when_rules_and_model_agree_dimlp_all = "{:.6f}".format(std_test_acc_when_rules_and_model_agree_dimlp_all).rstrip(".0")
                        if formatted_std_test_acc_when_rules_and_model_agree_dimlp_all == "":
                            formatted_std_test_acc_when_rules_and_model_agree_dimlp_all = "0"

                        outputStatsFile.write("Dimlp :\n")
                        outputStatsFile.write(f"The mean number of rules is: {formatted_mean_nb_rules_dimlp_all}\n")
                        if nb_no_rules_dimlp > 0:
                                if nb_no_rules_dimlp == 1:
                                    outputStatsFile.write(f"We didn't found any rule {int(nb_no_rules_dimlp)} time\n")
                                else:
                                    outputStatsFile.write(f"We didn't found any rule {int(nb_no_rules_dimlp)} times\n")

                        outputStatsFile.write(f"The standard deviation of the number of rules is: {formatted_std_nb_rules_dimlp_all}\n")
                        outputStatsFile.write(f"The mean sample covering number per rule is: {formatted_mean_nb_cover_dimlp_all}\n")
                        outputStatsFile.write(f"The standard deviation of the sample covering number per rule is: {formatted_std_nb_cover_dimlp_all}\n")
                        outputStatsFile.write(f"The mean number of antecedents per rule is: {formatted_mean_nb_antecedants_dimlp_all}\n")
                        outputStatsFile.write(f"The standard deviation of the number of antecedents per rule is: {formatted_std_nb_antecedants_dimlp_all}\n")
                        outputStatsFile.write(f"The mean global rule fidelity rate is: {formatted_mean_fidel_dimlp_all}\n")
                        outputStatsFile.write(f"The standard deviation of the global rule fidelity rate is: {formatted_std_fidel_dimlp_all}\n")
                        outputStatsFile.write(f"The mean global rule accuracy is: {formatted_mean_rules_acc_dimlp_all}\n")
                        outputStatsFile.write(f"The standard deviation of the global rule accuracy is: {formatted_std_rules_acc_dimlp_all}\n")
                        outputStatsFile.write(f"The mean default rule rate (when we can't find a rule) is: {formatted_mean_default_rate_dimlp_all}\n")
                        outputStatsFile.write(f"The standard deviation of the default rule rate (when we can't find a rule) is: {formatted_std_default_rate_dimlp_all}\n")
                        outputStatsFile.write(f"The mean model test accuracy is: {formatted_mean_test_acc_dimlp_all}\n")
                        outputStatsFile.write(f"The standard deviation of the model test accuracy is: {formatted_std_test_acc_dimlp_all}\n")
                        outputStatsFile.write(f"The mean model test accuracy when rules and model agree is: {formatted_mean_test_acc_when_rules_and_model_agree_dimlp_all}\n")
                        outputStatsFile.write(f"The standard deviation of the model test accuracy when rules and model agree is: {formatted_std_test_acc_when_rules_and_model_agree_dimlp_all}\n")
                        print("Dimlp :")
                        print(f"The mean number of rules is: {formatted_mean_nb_rules_dimlp_all}")
                        if nb_no_rules_dimlp > 0:
                                if nb_no_rules_dimlp == 1:
                                    print(f"We didn't found any rule {int(nb_no_rules_dimlp)} time")
                                else:
                                    print(f"We didn't found any rule {int(nb_no_rules_dimlp)} times")

                        print(f"The standard deviation of the number of rules is: {formatted_std_nb_rules_dimlp_all}")
                        print(f"The mean sample covering number per rule is: {formatted_mean_nb_cover_dimlp_all}")
                        print(f"The standard deviation of the sample covering number per rule is: {formatted_std_nb_cover_dimlp_all}")
                        print(f"The mean number of antecedents per rule is: {formatted_mean_nb_antecedants_dimlp_all}")
                        print(f"The standard deviation of the number of antecedents per rule is: {formatted_std_nb_antecedants_dimlp_all}")
                        print(f"The mean global rule fidelity rate is: {formatted_mean_fidel_dimlp_all}")
                        print(f"The standard deviation of the global rule fidelity rate is: {formatted_std_fidel_dimlp_all}")
                        print(f"The mean global rule accuracy is: {formatted_mean_rules_acc_dimlp_all}")
                        print(f"The standard deviation of the global rule accuracy is: {formatted_std_rules_acc_dimlp_all}")
                        print(f"The mean default rule rate (when we can't find a rule) is: {formatted_mean_default_rate_dimlp_all}")
                        print(f"The standard deviation of the default rule rate (when we can't find a rule) is: {formatted_std_default_rate_dimlp_all}")
                        print(f"The mean model test accuracy is: {formatted_mean_test_acc_dimlp_all}")
                        print(f"The standard deviation of the model test accuracy is: {formatted_std_test_acc_dimlp_all}")
                        print(f"The mean model test accuracy when rules and model agree is: {formatted_mean_test_acc_when_rules_and_model_agree_dimlp_all}")
                        print(f"The standard deviation of the model test accuracy when rules and model agree is: {formatted_std_test_acc_when_rules_and_model_agree_dimlp_all}")
                        print("\n---------------------------------------------------------")
                        print("---------------------------------------------------------")
                        print("")
                        outputStatsFile.write("\n---------------------------------------------------------\n\n")

                    if is_fidex:

                        formatted_mean_cov_size_fid_all = "{:.6f}".format(mean_cov_size_fid_all).rstrip(".0")
                        if formatted_mean_cov_size_fid_all == "":
                            formatted_mean_cov_size_fid_all = "0"

                        formatted_std_cov_size_fid_all = "{:.6f}".format(std_cov_size_fid_all).rstrip(".0")
                        if formatted_std_cov_size_fid_all == "":
                            formatted_std_cov_size_fid_all = "0"

                        formatted_mean_nb_ant_fid_all = "{:.6f}".format(mean_nb_ant_fid_all).rstrip(".0")
                        if formatted_mean_nb_ant_fid_all == "":
                            formatted_mean_nb_ant_fid_all = "0"

                        formatted_std_nb_ant_fid_all = "{:.6f}".format(std_nb_ant_fid_all).rstrip(".0")
                        if formatted_std_nb_ant_fid_all == "":
                            formatted_std_nb_ant_fid_all = "0"

                        formatted_mean_fidel_fid_all = "{:.6f}".format(mean_fidel_fid_all).rstrip(".0")
                        if formatted_mean_fidel_fid_all == "":
                            formatted_mean_fidel_fid_all = "0"

                        formatted_std_fidel_fid_all = "{:.6f}".format(std_fidel_fid_all).rstrip(".0")
                        if formatted_std_fidel_fid_all == "":
                            formatted_std_fidel_fid_all = "0"

                        formatted_mean_rules_acc_fid_all = "{:.6f}".format(mean_rules_acc_fid_all).rstrip(".0")
                        if formatted_mean_rules_acc_fid_all == "":
                            formatted_mean_rules_acc_fid_all = "0"

                        formatted_std_rules_acc_fid_all = "{:.6f}".format(std_rules_acc_fid_all).rstrip(".0")
                        if formatted_std_rules_acc_fid_all == "":
                            formatted_std_rules_acc_fid_all = "0"

                        formatted_mean_confid_fid_all = "{:.6f}".format(mean_confid_fid_all).rstrip(".0")
                        if formatted_mean_confid_fid_all == "":
                            formatted_mean_confid_fid_all = "0"

                        formatted_std_confid_fid_all = "{:.6f}".format(std_confid_fid_all).rstrip(".0")
                        if formatted_std_confid_fid_all == "":
                            formatted_std_confid_fid_all = "0"

                        outputStatsFile.write("Fidex :\n")
                        outputStatsFile.write(f"The mean covering size per rule is : {formatted_mean_cov_size_fid_all}\n")
                        outputStatsFile.write(f"The standard deviation of the covering size per rule is : {formatted_std_cov_size_fid_all}\n")
                        outputStatsFile.write(f"The mean number of antecedents per rule is : {formatted_mean_nb_ant_fid_all}\n")
                        outputStatsFile.write(f"The standard deviation of the number of antecedents per rule is : {formatted_std_nb_ant_fid_all}\n")
                        outputStatsFile.write(f"The mean rule fidelity rate is : {formatted_mean_fidel_fid_all}\n")
                        outputStatsFile.write(f"The standard deviation of the rule fidelity rate is : {formatted_std_fidel_fid_all}\n")
                        outputStatsFile.write(f"The mean rule accuracy is : {formatted_mean_rules_acc_fid_all}\n")
                        outputStatsFile.write(f"The standard deviation of the rule accuracy is : {formatted_std_rules_acc_fid_all}\n")
                        if has_confidence:
                            outputStatsFile.write(f"The mean rule confidence is : {formatted_mean_confid_fid_all}\n")
                            outputStatsFile.write(f"The standard deviation of the rule confidence is : {formatted_std_confid_fid_all}\n")
                        print("Fidex :")
                        print(f"The mean covering size per rule is : {formatted_mean_cov_size_fid_all}")
                        print(f"The standard deviation of the covering size per rule is : {formatted_std_cov_size_fid_all}")
                        print(f"The mean number of antecedents per rule is : {formatted_mean_nb_ant_fid_all}")
                        print(f"The standard deviation of the number of antecedents per rule is : {formatted_std_nb_ant_fid_all}")
                        print(f"The mean rule fidelity rate is : {formatted_mean_fidel_fid_all}")
                        print(f"The standard deviation of the rule fidelity rate is : {formatted_std_fidel_fid_all}")
                        print(f"The mean rule accuracy is : {formatted_mean_rules_acc_fid_all}")
                        print(f"The standard deviation of the rule accuracy is : {formatted_std_rules_acc_fid_all}")
                        if has_confidence:
                            print(f"The mean rule confidence is : {formatted_mean_confid_fid_all}")
                            print(f"The standard deviation of the rule confidence is : {formatted_std_confid_fid_all}")
                        print("\n---------------------------------------------------------")
                        print("---------------------------------------------------------")

                    if is_fidex and is_fidexglo:
                        print("")
                        outputStatsFile.write("\n---------------------------------------------------------\n\n")

                    if is_fidexglo:

                        formatted_mean_nb_rules_all = "{:.6f}".format(mean_nb_rules_all).rstrip(".0")
                        if formatted_mean_nb_rules_all == "":
                            formatted_mean_nb_rules_all = "0"

                        formatted_std_nb_rules_all = "{:.6f}".format(std_nb_rules_all).rstrip(".0")
                        if formatted_std_nb_rules_all == "":
                            formatted_std_nb_rules_all = "0"

                        formatted_mean_nb_cover_all = "{:.6f}".format(mean_nb_cover_all).rstrip(".0")
                        if formatted_mean_nb_cover_all == "":
                            formatted_mean_nb_cover_all = "0"

                        formatted_std_nb_cover_all = "{:.6f}".format(std_nb_cover_all).rstrip(".0")
                        if formatted_std_nb_cover_all == "":
                            formatted_std_nb_cover_all = "0"

                        formatted_mean_nb_antecedants_all = "{:.6f}".format(mean_nb_antecedants_all).rstrip(".0")
                        if formatted_mean_nb_antecedants_all == "":
                            formatted_mean_nb_antecedants_all = "0"

                        formatted_std_nb_antecedants_all = "{:.6f}".format(std_nb_antecedants_all).rstrip(".0")
                        if formatted_std_nb_antecedants_all == "":
                            formatted_std_nb_antecedants_all = "0"

                        formatted_mean_fidel_glo_all = "{:.6f}".format(mean_fidel_glo_all).rstrip(".0")
                        if formatted_mean_fidel_glo_all == "":
                            formatted_mean_fidel_glo_all = "0"

                        formatted_std_fidel_glo_all = "{:.6f}".format(std_fidel_glo_all).rstrip(".0")
                        if formatted_std_fidel_glo_all == "":
                            formatted_std_fidel_glo_all = "0"

                        formatted_mean_rules_acc_glo_all = "{:.6f}".format(mean_rules_acc_glo_all).rstrip(".0")
                        if formatted_mean_rules_acc_glo_all == "":
                            formatted_mean_rules_acc_glo_all = "0"

                        formatted_std_rules_acc_glo_all = "{:.6f}".format(std_rules_acc_glo_all).rstrip(".0")
                        if formatted_std_rules_acc_glo_all == "":
                            formatted_std_rules_acc_glo_all = "0"

                        formatted_mean_expl_glo_all = "{:.6f}".format(mean_expl_glo_all).rstrip(".0")
                        if formatted_mean_expl_glo_all == "":
                            formatted_mean_expl_glo_all = "0"

                        formatted_std_expl_glo_all = "{:.6f}".format(std_expl_glo_all).rstrip(".0")
                        if formatted_std_expl_glo_all == "":
                            formatted_std_expl_glo_all = "0"

                        formatted_mean_default_rate_all = "{:.6f}".format(mean_default_rate_all).rstrip(".0")
                        if formatted_mean_default_rate_all == "":
                            formatted_mean_default_rate_all = "0"

                        formatted_std_default_rate_all = "{:.6f}".format(std_default_rate_all).rstrip(".0")
                        if formatted_std_default_rate_all == "":
                            formatted_std_default_rate_all = "0"

                        formatted_mean_nb_fidel_activations_all = "{:.6f}".format(mean_nb_fidel_activations_all).rstrip(".0")
                        if formatted_mean_nb_fidel_activations_all == "":
                            formatted_mean_nb_fidel_activations_all = "0"

                        formatted_std_nb_fidel_activations_all = "{:.6f}".format(std_nb_fidel_activations_all).rstrip(".0")
                        if formatted_std_nb_fidel_activations_all == "":
                            formatted_std_nb_fidel_activations_all = "0"

                        formatted_mean_wrong_activations_all = "{:.6f}".format(mean_wrong_activations_all).rstrip(".0")
                        if formatted_mean_wrong_activations_all == "":
                            formatted_mean_wrong_activations_all = "0"

                        formatted_std_wrong_activations_all = "{:.6f}".format(std_wrong_activations_all).rstrip(".0")
                        if formatted_std_wrong_activations_all == "":
                            formatted_std_wrong_activations_all = "0"

                        formatted_mean_test_acc_glo_all = "{:.6f}".format(mean_test_acc_glo_all).rstrip(".0")
                        if formatted_mean_test_acc_glo_all == "":
                            formatted_mean_test_acc_glo_all = "0"

                        formatted_std_test_acc_glo_all = "{:.6f}".format(std_test_acc_glo_all).rstrip(".0")
                        if formatted_std_test_acc_glo_all == "":
                            formatted_std_test_acc_glo_all = "0"

                        formatted_mean_test_acc_when_rules_and_model_agree_all = "{:.6f}".format(mean_test_acc_when_rules_and_model_agree_all).rstrip(".0")
                        if formatted_mean_test_acc_when_rules_and_model_agree_all == "":
                            formatted_mean_test_acc_when_rules_and_model_agree_all = "0"

                        formatted_std_test_acc_when_rules_and_model_agree_all = "{:.6f}".format(std_test_acc_when_rules_and_model_agree_all).rstrip(".0")
                        if formatted_std_test_acc_when_rules_and_model_agree_all == "":
                            formatted_std_test_acc_when_rules_and_model_agree_all = "0"

                        formatted_mean_test_acc_when_activated_rules_and_model_agree_all = "{:.6f}".format(mean_test_acc_when_activated_rules_and_model_agree_all).rstrip(".0")
                        if formatted_mean_test_acc_when_activated_rules_and_model_agree_all == "":
                            formatted_mean_test_acc_when_activated_rules_and_model_agree_all = "0"

                        formatted_std_test_acc_when_activated_rules_and_model_agree_all = "{:.6f}".format(std_test_acc_when_activated_rules_and_model_agree_all).rstrip(".0")
                        if formatted_std_test_acc_when_activated_rules_and_model_agree_all == "":
                            formatted_std_test_acc_when_activated_rules_and_model_agree_all = "0"

                        outputStatsFile.write("FidexGlo :\n")
                        outputStatsFile.write(f"The mean number of rules is : {formatted_mean_nb_rules_all}\n")
                        outputStatsFile.write(f"The standard deviation of the number of rules is : {formatted_std_nb_rules_all}\n")
                        outputStatsFile.write(f"The mean sample covering number per rule is : {formatted_mean_nb_cover_all}\n")
                        outputStatsFile.write(f"The standard deviation of the sample covering number per rule is : {formatted_std_nb_cover_all}\n")
                        outputStatsFile.write(f"The mean number of antecedents per rule is : {formatted_mean_nb_antecedants_all}\n")
                        outputStatsFile.write(f"The standard deviation of the number of antecedents per rule is : {formatted_std_nb_antecedants_all}\n")
                        outputStatsFile.write(f"The mean global rule fidelity rate is : {formatted_mean_fidel_glo_all}\n")
                        outputStatsFile.write(f"The standard deviation of the global rule fidelity rate is : {formatted_std_fidel_glo_all}\n")
                        outputStatsFile.write(f"The mean global rule accuracy is : {formatted_mean_rules_acc_glo_all}\n")
                        outputStatsFile.write(f"The standard deviation of the global rule accuracy is : {formatted_std_rules_acc_glo_all}\n")
                        outputStatsFile.write(f"The mean explainability rate (when we can find a rule) is : {formatted_mean_expl_glo_all}\n")
                        outputStatsFile.write(f"The standard deviation of the explainability rate (when we can find a rule) is : {formatted_std_expl_glo_all}\n")
                        outputStatsFile.write(f"The mean default rule rate (when we can't find a rule) is : {formatted_mean_default_rate_all}\n")
                        outputStatsFile.write(f"The standard deviation of the default rule rate (when we can't find a rule) is : {formatted_std_default_rate_all}\n")
                        outputStatsFile.write(f"The mean number of correct(fidel) activated rules per sample is : {formatted_mean_nb_fidel_activations_all}\n")
                        outputStatsFile.write(f"The standard deviation of the number of correct(fidel) activated rules per sample is : {formatted_std_nb_fidel_activations_all}\n")
                        outputStatsFile.write(f"The mean number of wrong(not fidel) activated rules per sample is : {formatted_mean_wrong_activations_all}\n")
                        outputStatsFile.write(f"The standard deviation of the number of wrong(not fidel) activated rules per sample is : {formatted_std_wrong_activations_all}\n")
                        outputStatsFile.write(f"The mean model test accuracy is : {formatted_mean_test_acc_glo_all}\n")
                        outputStatsFile.write(f"The standard deviation of the model test accuracy is : {formatted_std_test_acc_glo_all}\n")
                        outputStatsFile.write(f"The mean model test accuracy when rules and model agree is : {formatted_mean_test_acc_when_rules_and_model_agree_all}\n")
                        outputStatsFile.write(f"The standard deviation of the model test accuracy when rules and model agree is : {formatted_std_test_acc_when_rules_and_model_agree_all}\n")
                        outputStatsFile.write(f"The mean model test accuracy when activated rules and model agree is : {formatted_mean_test_acc_when_activated_rules_and_model_agree_all}\n")
                        outputStatsFile.write(f"The standard deviation of the model test accuracy when activated rules and model agree is : {formatted_std_test_acc_when_activated_rules_and_model_agree_all}\n")
                        print("FidexGlo :")
                        print(f"The mean number of rules is : {formatted_mean_nb_rules_all}")
                        print(f"The standard deviation of the number of rules is : {formatted_std_nb_rules_all}")
                        print(f"The mean sample covering number per rule is : {formatted_mean_nb_cover_all}")
                        print(f"The standard deviation of the sample covering number per rule is : {formatted_std_nb_cover_all}")
                        print(f"The mean number of antecedents per rule is : {formatted_mean_nb_antecedants_all}")
                        print(f"The standard deviation of the number of antecedents per rule is : {formatted_std_nb_antecedants_all}")
                        print(f"The mean global rule fidelity rate is : {formatted_mean_fidel_glo_all}")
                        print(f"The standard deviation of the global rule fidelity rate is : {formatted_std_fidel_glo_all}")
                        print(f"The mean global rule accuracy is : {formatted_mean_rules_acc_glo_all}")
                        print(f"The standard deviation of the global rule accuracy is : {formatted_std_rules_acc_glo_all}")
                        print(f"The mean explainability rate (when we can find a rule) is : {formatted_mean_expl_glo_all}")
                        print(f"The standard deviation of the explainability rate (when we can find a rule) is : {formatted_std_expl_glo_all}")
                        print(f"The mean default rule rate (when we can't find a rule) is : {formatted_mean_default_rate_all}")
                        print(f"The standard deviation of the default rule rate (when we can't find a rule) is : {formatted_std_default_rate_all}")
                        print(f"The mean number of correct(fidel) activated rules per sample is : {formatted_mean_nb_fidel_activations_all}")
                        print(f"The standard deviation of the number of correct(fidel) activated rules per sample is : {formatted_std_nb_fidel_activations_all}")
                        print(f"The mean number of wrong(not fidel) activated rules per sample is : {formatted_mean_wrong_activations_all}")
                        print(f"The standard deviation of the number of wrong(not fidel) activated rules per sample is : {formatted_std_wrong_activations_all}")
                        print(f"The mean model test accuracy is : {formatted_mean_test_acc_glo_all}")
                        print(f"The standard deviation of the model test accuracy is : {formatted_std_test_acc_glo_all}")
                        print(f"The mean model test accuracy when rules and model agree is : {formatted_mean_test_acc_when_rules_and_model_agree_all}")
                        print(f"The standard deviation of the model test accuracy when rules and model agree is : {formatted_std_test_acc_when_rules_and_model_agree_all}")
                        print(f"The mean model test accuracy when activated rules and model agree is : {formatted_mean_test_acc_when_activated_rules_and_model_agree_all}")
                        print(f"The standard deviation of the model test accuracy when activated rules and model agree is : {formatted_std_test_acc_when_activated_rules_and_model_agree_all}")
                        print("\n---------------------------------------------------------")
                        print("---------------------------------------------------------")



                    outputStatsFile.close()
            except (FileNotFoundError):
                raise ValueError(f"Error : File for stats extraction ({crossval_stats}) not found.")
            except (IOError):
                raise ValueError(f"Error : Couldn't open stats extraction file {crossval_stats}.")


            # Delete temporary files
            try:
                console_file = crossval_folder + separator + "consoleTemp.txt"
                os.remove(console_file)
            except FileNotFoundError:
                print(f"Error : File '{console_file}' not found.")
            except Exception:
                print(f"Error during delete of file {console_file}")

            try:
                train_file = crossval_folder + separator + "tempTrain.txt"
                os.remove(train_file)
            except FileNotFoundError:
                print(f"Error : File '{train_file}' not found.")
            except Exception:
                print(f"Error during delete of file {train_file}")

            try:
                test_file = crossval_folder + separator + "tempTest.txt"
                os.remove(test_file)
            except FileNotFoundError:
                print(f"Error : File '{test_file}' not found.")
            except Exception:
                print(f"Error during delete of file {test_file}")

            try:
                tar_train_file = crossval_folder + separator + "tempTarTrain.txt"
                os.remove(tar_train_file)
            except FileNotFoundError:
                print(f"Error : File '{tar_train_file}' not found.")
            except Exception:
                print(f"Error during delete of file {tar_train_file}")

            try:
                tar_test_file = crossval_folder + separator + "tempTarTest.txt"
                os.remove(tar_test_file)
            except FileNotFoundError:
                print(f"Error : File '{tar_test_file}' not found.")
            except Exception:
                print(f"Error during delete of file {tar_test_file}")

            if train_method == "dimlp":
                try:
                    valid_file = crossval_folder + separator + "tempValid.txt"
                    os.remove(valid_file)
                except FileNotFoundError:
                    print(f"Error : File '{valid_file}' not found.")
                except Exception:
                    print(f"Error during delete of file {valid_file}")

                try:
                    tar_valid_file = crossval_folder + separator + "tempTarValid.txt"
                    os.remove(tar_valid_file)
                except FileNotFoundError:
                    print(f"Error : File '{tar_valid_file}' not found.")
                except Exception:
                    print(f"Error during delete of file {tar_valid_file}")

        end_time = time.time()
        full_time = end_time - start_time
        full_time = "{:.6f}".format(full_time).rstrip(".0")

        print(f"\nFull execution time = {full_time} sec")

        try:
            with open(crossval_stats, "a") as outputStatsFile:
                outputStatsFile.write("\n")
                outputStatsFile.write(f"\nExecution time = {full_time} sec")
                outputStatsFile.close()
        except (FileNotFoundError):
            raise ValueError(f"Error : File for stats extraction ({crossval_stats}) not found.")
        except (IOError):
            raise ValueError(f"Error : Couldn't open stats extraction file {crossval_stats}.")

        return 0
    except ValueError as error:
        print(error)
        return -1


# Exemple Dimlp : crossValid(train_method="dimlp", algo="both", data_file="datanorm", class_file="dataclass2", nb_in=16, nb_out=2, H2=5, save_folder="dimlp/datafiles", crossVal_folder="CrossValidationDIMLP", K=3, N=2, seed=33)
# Exemple DimlpBT : crossValid(train_method="dimlpBT", algo="both", data_file="datanorm", class_file="dataclass2", nb_in=16, nb_out=2, H2=5, save_folder="dimlp/datafiles", crossVal_folder="CrossValidationDIMLPBT", K=3, N=2, seed=33)
# Exemple SVM : crossValid(train_method="svm", algo="both", data_file="datanorm", class_file="dataclass2", save_folder="dimlp/datafiles", crossVal_folder="CrossValidationSVM", K=3, N=2, seed=33)
# Exemple MLP : crossValid(train_method="mlp", algo="both", data_file="datanorm", class_file="dataclass2", save_folder="dimlp/datafiles", crossVal_folder="CrossValidationMLP", K=3, N=2, seed=33)
