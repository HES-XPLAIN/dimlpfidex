import os
import shutil
import sys
import platform
import random
import math
import time
import numpy as np
from sklearn.metrics import RocCurveDisplay
import matplotlib.pyplot as plt

# import modules :

if sys.platform == 'win32':
    dir_path = os.path.abspath('dimlpfidex')
    os.add_dll_directory(dir_path)

from dimlpfidex import dimlp
from dimlpfidex import fidex
from trainings.svmTrn import svmTrn
from trainings.mlpTrn import mlpTrn
from trainings.randForestsTrn import randForestsTrn
from trainings.gradBoostTrn import gradBoostTrn
from trainings.computeRocCurve import computeRocCurve
from trainings.trnFun import delete_file, get_data, get_data_class

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

def formatting(number):
    if number == "N/A":
        return number
    else:
        formatted_number = "{:.6f}".format(number).rstrip(".0")
        if formatted_number == "":
            formatted_number = "0"
        return formatted_number

def crossValid(*args, **kwargs):
    try:
        if not kwargs:
            print("---------------------------------------------------------------------")
            print("Please specify arguments using named parameters.")
            print("Warning! The files are located with respect to the root folder dimlpfidex.")

            print("----------------------------")
            print("Required parameters :")
            print("train_method : dimlp, dimlpBT, svm, mlp, randForest or gradBoost")
            print("algo : fidex, fidexGlo or both")
            print("data_file : data file")
            print("class_file : class file, not mandatory if classes are specified in train file")
            print("nb_attributes : number of input neurons")
            print("nb_classes : number of output neurons")

            print("----------------------------")
            print("Required parameters if training with dimlp and dimlpBT :")
            print("dimlpRul : 1(with dimlpRul) or 0")

            print("----------------------------")
            print("Optional parameters :")
            print("root_folder : Path to the folder, based on main default folder dimlpfidex, containing all used files and where generated files will be saved. If a file name is specified with another option, its path will be relative to this root folder.")
            print("crossVal_folder : Folder name where to save crossValidation data (CrossValidation by default)")
            print("K : K-fold cross-validation (10 by default)")
            print("N : number of times we do the cross-validation (10 by default)")
            print("fidexGlo_heuristic : 1: optimal fidexGlo(default), 2: fast fidexGlo 3: very fast fidexGlo")
            print("crossVal_stats : statistics of cross validation (crossValidationStats.txt by default)")
            print("attributes_file : file of attributes")
            print("max_iterations : maximum fidex and fidexGlo iteration number (100 by default)")
            print("min_covering : minimum fidex and fidexGlo covering number (2 by default)")
            print("covering_strategy <Whether to use this strategy : if no rule is found with min_covering, find best rule with best covering using dichotomic search. Decreases min_fidelity if needed (True by default)>")
            print("min_fidelity <minimal rule fidelity accepted when generating a rule [0,1] (1 by default)>")
            print("lowest_min_fidelity <minimal min_fidelity to which we agree to go down during covering_strategy (0.75 by default)>")
            print("dropout_dim : dimension dropout parameter for fidex and fidexGlo")
            print("dropout_hyp : hyperplane dropout parameter for fidex and fidexGlo")
            print("seed : 0 = random (default)")
            print("positive_class_index <index of positive class sample to compute true/false positive/negative rates and ROC curve (None by default, put 0 for first class)")
            print("decision_threshold <decision threshold for predictions, need to specify the index of positive class if you want to use it (None by default)>")
            print("normalization_file <file containing the mean and std of some attributes. Used to denormalize the rules if specified>")
            print("mus <list of float in the form [1.1,3.5] without spaces(!) corresponding to mean or median of each attribute index to denormalize in the rules>")
            print("sigmas <list of float in the form [4.5,12] without spaces(!) corresponding to standard deviation of each attribute index to denormalize in the rules>")
            print("normalization_indices <list of integers in the form [0,3,7] without spaces(!) corresponding to attribute indices to denormalize in the rules (first column is index 0, all indices by default, only used when no normalization_file is given)>")
            print("nb_threads <number of threads used for computing the fidexGloRules algorithm (default=1, this means by default its a sequential execution)>")

            print("----------------------------")
            print("Optional parameters if not training with decision trees :")
            print("nb_quant_levels : number of stairs in staircase activation function (50 by default)")

            print("----------------------------")
            print("Optional parameters for dimlp and dimlpBT training:")
            print("pretrained_weights : file of pretrained weights")
            print("first_hidden_layer <int k*nb_attributes, k in [1,inf[> Number of neurons in the first hidden layer (default: nb_attributes)")
            print("hidden_layers <list<int [1,inf[>> Number of neurons in each hidden layer, from the second layer through to the last.")
            print("learning_rate : back-propagation learning parameter (0.1 by default)")
            print("momentum : back-propagation momentum parameter (0.6 by default)")
            print("flat : back-propagation flat spot elimination parameter (0.01 by default)")
            print("error_thresh : error threshold (None by default)")
            print("acc_thresh : accuracy threshold (None by default)")
            print("abs_error_thresh : absolute difference error threshold (0 by default)")
            print("nb_epochs : number of train epochs (1500 by default)")
            print("nb_epochs_error : number of train epochs to show error (10 by default)")

            print("----------------------------")
            print("Optional parameters for dimlpBT training:")
            print("nb_dimlp_nets : number of dimlp networks, integer >= 2 (25 by default)")
            print("nb_ex_per_net : number of examples for one single network, positive integer, 0 = number of train examples (default)")

            print("----------------------------")
            print("Optional parameters for svm training:")
            print("svm_K : Parameter to improve dynamics by normalizing input data (1 by default)")
            print("C : regularization, (1.0 by default)")
            print("kernel : linear, poly, rbf(default) or sigmoid")
            print("degree : polynomial degree (3 by default)")
            print("gamma : scale(default), auto or non negative float")
            print("coef0 : term in kernel function, float (0 by default)")
            print("shrinking : heuristic, True(default) or False")
            print("svm_tol : tolerance for stopping criterion (0.001 by default)")
            print("cache_size : kernel cache size (200 MB by default)")
            print("svm_class_weight : class balance, 'balanced' or a dictionary, for exemple with 2 classes : {0:1.2, 1:3.5} (None by default)")
            print("svm_max_iterations : maximal number of iterations (-1 for no limit (default))")
            print("decision_function_shape : decision function shape, ovo(one-vs-one) or ovr(one-vs-rest, default)")
            print("break_ties : break tie decision for ovr with more than 2 classes, True or False(default)")

            print("----------------------------")
            print("Optional parameters for mlp training:")
            print("mlp_K : Parameter to improve dynamics by normalizing input data (1 by default)")
            print("hidden_layer_sizes : Size of each hidden layers. Array of shape (n_layers-2) ((100,) by default)")
            print("activation : activation function, identity, logistic, tanh or relu(default)")
            print("solver : solver for weight optimization, lbfgs, sgd or adam (default)")
            print("alpha : strength of the L2 regularization term, positive float (0.0001 by default)")
            print("batch_size : size of minibatches for stochastic optimizers for adam and sgd, auto(default) or positive integer")
            print("mlp_learning_rate : learning rate schedule for weight updates for sgd solver, constant(default), invscaling or adaptive")
            print("learning_rate_init : initial learning rate for adam and sgd, positive float (0.001 by default)")
            print("power_t : exponent for inverse scaling learning rate for sgd, positive float (0.5 by default)")
            print("mlp_max_iterations : maximum number of iterations, positive integer (200 by default)")
            print("shuffle : whether to shuffle samples in each iteration for sgd and adam, True(default) or False")
            print("mlp_tol : tolerance for optimization (0.0001 by default)")
            print("mlp_warm_start : whether to reuse previous solution to fit initialization, True or False(default)")
            print("mlp_momentum : Momentum for gradient descent update for sgd, between 0 and 1 (0.9 by default)")
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
            print('crossValid(train_method="dimlp", algo="both", data_file="datanorm", class_file="dataclass2", positive_class_index=1, dimlpRul=1, nb_attributes=16, nb_classes=2, hidden_layers=[5], root_folder="dimlp/datafiles", crossVal_folder="CrossValidationDIMLP")')
            print("----------------------------")
            print("Exemple with dimlpBT :")
            print('crossValid(train_method="dimlpBT", algo="both", data_file="datanorm", class_file="dataclass2", positive_class_index=1, dimlpRul=1, nb_attributes=16, nb_classes=2, hidden_layers=[5], root_folder="dimlp/datafiles", crossVal_folder="CrossValidationDIMLPBT")')
            print("----------------------------")
            print("Exemple with SVM :")
            print('crossValid(train_method="svm", algo="both", data_file="datanorm", class_file="dataclass2", positive_class_index=1, nb_attributes=16, nb_classes=2, root_folder="dimlp/datafiles", crossVal_folder="CrossValidationSVM")')
            print("----------------------------")
            print("Exemple with MLP :")
            print('crossValid(train_method="mlp", algo="both", data_file="datanorm", class_file="dataclass2", positive_class_index=1, nb_attributes=16, nb_classes=2, root_folder="dimlp/datafiles", crossVal_folder="CrossValidationMLP")')
            print("----------------------------")
            print("Exemple with Random forests :")
            print('crossValid(train_method="randForest", algo="both", data_file="datanorm", class_file="dataclass2", positive_class_index=1, nb_attributes=16, nb_classes=2, root_folder="dimlp/datafiles", crossVal_folder="CrossValidationRF")')
            print("---------------------------------------------------------------------")
            print("Exemple with Gradient boosting :")
            print('crossValid(train_method="gradBoost", algo="both", data_file="datanorm", class_file="dataclass2", positive_class_index=1, nb_attributes=16, nb_classes=2, root_folder="dimlp/datafiles", crossVal_folder="CrossValidationGB")')
            print("---------------------------------------------------------------------")

            return 0

        else:

            start_time = time.time()

            # Get parameters
            train_method = kwargs.get('train_method')
            algo = kwargs.get('algo')
            data_file = kwargs.get('data_file')
            class_file = kwargs.get('class_file')

            nb_attributes = kwargs.get('nb_attributes')
            nb_classes = kwargs.get('nb_classes')
            dimlprul = kwargs.get('dimlpRul')

            root_folder = kwargs.get('root_folder')
            crossval_folder = kwargs.get('crossVal_folder')
            k = kwargs.get('K')
            n = kwargs.get('N')
            fidexglo_heuristic = kwargs.get('fidexGlo_heuristic')
            crossval_stats = kwargs.get('crossVal_stats')
            attributes_file = kwargs.get('attributes_file')
            max_iterations = kwargs.get('max_iterations')
            min_covering = kwargs.get('min_covering')
            covering_strategy = kwargs.get('covering_strategy')
            min_fidelity = kwargs.get('min_fidelity')
            lowest_min_fidelity = kwargs.get('lowest_min_fidelity')
            dropout_dim = kwargs.get('dropout_dim')
            dropout_hyp = kwargs.get('dropout_hyp')
            positive_class_index = kwargs.get('positive_class_index')
            decision_threshold = kwargs.get('decision_threshold')
            seed = kwargs.get('seed')
            normalization_file = kwargs.get('normalization_file')
            mus = kwargs.get('mus')
            sigmas = kwargs.get('sigmas')
            normalization_indices = kwargs.get('normalization_indices')
            nb_threads = kwargs.get('nb_threads')

            hiknot = 5
            nb_quant_levels = kwargs.get('nb_quant_levels')

            pretrained_weights = kwargs.get('pretrained_weights')
            first_hidden_layer = kwargs.get('first_hidden_layer')
            hidden_layers = kwargs.get('hidden_layers')
            learning_rate = kwargs.get('learning_rate')
            momentum = kwargs.get('momentum')
            flat = kwargs.get('flat')
            error_thresh = kwargs.get('error_thresh')
            acc_thresh = kwargs.get('acc_thresh')
            abs_error_thresh = kwargs.get('abs_error_thresh')
            nb_epochs = kwargs.get('nb_epochs')
            nb_epochs_error = kwargs.get('nb_epochs_error')

            nb_dimlp_nets = kwargs.get('nb_dimlp_nets')
            nb_ex_per_net = kwargs.get('nb_ex_per_net')

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
            svm_max_iterations_var = kwargs.get('svm_max_iterations')
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
            mlp_max_iterations_var = kwargs.get('mlp_max_iterations')
            shuffle_var = kwargs.get('shuffle')
            mlp_tol_var = kwargs.get('mlp_tol')
            mlp_warm_start_var = kwargs.get('mlp_warm_start')
            mlp_momentum_var = kwargs.get('mlp_momentum')
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

            obligatory_args = ['train_method', 'algo', 'data_file','nb_attributes', 'nb_classes']
            obligatory_dimlp_args = ['dimlpRul']

            optional_args = ['class_file', 'root_folder', 'crossVal_folder', 'K', 'N', 'fidexGlo_heuristic', 'crossVal_stats', 'attributes_file',
                        'max_iterations', 'min_covering', 'covering_strategy', 'min_fidelity', 'lowest_min_fidelity', 'dropout_dim', 'dropout_hyp',
                        'seed', 'positive_class_index', 'decision_threshold', 'normalization_file', 'mus', 'sigmas', 'normalization_indices', 'nb_threads']

            optional_non_dt_args = ['nb_quant_levels']

            optional_dimlp_args = ['pretrained_weights', 'learning_rate', 'momentum', 'flat', 'error_thresh',
                        'acc_thresh', 'abs_error_thresh', 'nb_epochs', 'nb_epochs_error', 'hidden_layers']

            optional_dimlpBT_args = ['nb_dimlp_nets', 'nb_ex_per_net']

            optional_svm_args = ['svm_K', 'C', 'kernel', 'degree', 'gamma', 'coef0', 'shrinking',
                        'svm_tol', 'cache_size', 'svm_class_weight', 'svm_max_iterations', 'decision_function_shape', 'break_ties']

            optional_mlp_args = ['mlp_K', 'hidden_layer_sizes', 'activation', 'solver', 'alpha',
                        'batch_size', 'mlp_learning_rate', 'learning_rate_init', 'power_t', 'mlp_max_iterations',
                        'shuffle', 'mlp_tol', 'mlp_warm_start', 'mlp_momentum', 'nesterovs_momentum',
                        'early_stopping', 'mlp_validation_fraction', 'beta_1', 'beta_2', 'epsilon', 'mlp_n_iter_no_change', 'max_fun']

            optional_dt_args = ['n_estimators', 'min_samples_split', 'min_samples_leaf', 'min_weight_fraction_leaf',
                                            'max_features', 'min_impurity_decrease', 'max_leaf_nodes', 'dt_warm_start', 'ccp_alpha']

            optional_rf_args = ['rf_criterion', 'rf_max_depth', 'bootstrap', 'oob_score', 'n_jobs', 'rf_class_weight', 'max_samples']

            optional_gb_args = ['loss', 'gb_learning_rate', 'subsample', 'gb_criterion', 'gb_max_depth', 'init'
                                , 'gb_validation_fraction', 'gb_n_iter_no_change', 'gb_tol']

            # Check if wrong parameters are given

            train_methods = {"dimlp", "dimlpBT", "svm", "mlp", "randForest", "gradBoost"}
            if train_method is None:
                raise ValueError('Error : train method is missing, add it with option train_method="dimlp", "dimlpBT", "svm", "mlp", randForest or gradBoost.')
            elif (train_method not in train_methods):
                raise ValueError('Error, parameter train_method is not "dimlp", "dimlpBT", "svm", "mlp", "randForest" or "gradBoost".')

            for arg_key in kwargs.keys():
                if (train_method == "dimlp"):
                    if (arg_key not in optional_args and arg_key not in optional_non_dt_args and arg_key not in optional_dimlp_args and arg_key not in obligatory_args and arg_key not in obligatory_dimlp_args):
                        raise ValueError(f"Invalid argument with dimlp training : {arg_key}.")
                elif (train_method == "dimlpBT"):
                    if (arg_key not in optional_args and arg_key not in optional_non_dt_args and arg_key  not in optional_dimlp_args and arg_key not in obligatory_args and arg_key not in obligatory_dimlp_args and arg_key not in optional_dimlpBT_args):
                        raise ValueError(f"Invalid argument with dimlpBT training : {arg_key}.")
                elif (train_method == "svm"):
                    if (arg_key not in optional_args and arg_key not in optional_non_dt_args and arg_key not in optional_svm_args and arg_key not in obligatory_args):
                        raise ValueError(f"Invalid argument with svm training : {arg_key}.")
                elif (train_method == "mlp"):
                    if (arg_key not in optional_args and arg_key not in optional_non_dt_args and arg_key not in optional_mlp_args and arg_key not in obligatory_args):
                        raise ValueError(f"Invalid argument with mlp training : {arg_key}.")
                elif (train_method == "randForest"):
                    if (arg_key not in optional_args and arg_key not in optional_dt_args and arg_key not in optional_rf_args and arg_key not in obligatory_args):
                        raise ValueError(f"Invalid argument with random forest training : {arg_key}.")
                else:
                    if (arg_key not in optional_args and arg_key not in optional_dt_args and arg_key not in optional_gb_args and arg_key not in obligatory_args):
                        raise ValueError(f"Invalid argument with gradient boosting training : {arg_key}.")


            algos = {"fidex", "fidexGlo", "both"}
            if algo is None:
                raise ValueError('Error : algorithm(s) is missing, add it with option algo="fidex" or "fidexGlo" or "both".')
            elif (algo not in algos):
                raise ValueError('Error, parameter algo is not fidex, fidexGlo or both.')

            if data_file is None:
                raise ValueError('Error : data file is missing, add it with option data_file="your_data_file_name".')
            elif not isinstance(data_file, str):
                raise ValueError('Error : parameter data_file has to be a name contained in quotation marks "".')

            if nb_attributes is None:
                raise ValueError('Error : the number of input neurons is missing, add it with option nb_attributes=your_number.')
            elif (not isinstance(nb_attributes, int) or nb_attributes<=0):
                raise ValueError('Error, parameter nb_attributes is not an strictly positive integer.')

            if nb_classes is None:
                raise ValueError('Error : the number of output neurons is missing, add it with option nb_classes=your_number.')
            elif (not isinstance(nb_classes, int) or nb_classes<=0):
                raise ValueError('Error, parameter nb_classes is not an strictly positive integer.')

            # Check optional parameters

            if (root_folder is not None and (not isinstance(root_folder, str))):
                raise ValueError('Error, parameter root_folder has to be a name contained in quotation marks "".')

            if crossval_folder is None:
                crossval_folder = "CrossValidation"
            elif not isinstance(crossval_folder, str):
                raise ValueError('Error, parameter crossval_folder has to be a name contained in quotation marks "".')

            if k is None:
                k = 10
            elif (not isinstance(k, int) or k<3):
                raise ValueError('Error, parameter K is not an integer greater than 2.')

            if n is None:
                n = 10
            elif (not isinstance(n, int) or n<1):
                raise ValueError('Error, parameter n is not an stryctly positive integer.')

            if fidexglo_heuristic is None:
                fidexglo_heuristic = 1
            elif (fidexglo_heuristic not in {1,2,3}):
                raise ValueError('Error, parameter fidexglo_heuristic is not 1, 2 or 3.')

            if crossval_stats is None:
                crossval_stats = "crossValidationStats.txt"
            elif (not isinstance(crossval_stats, str)):
                raise ValueError('Error, parameter crossval_stats has to be a name contained in quotation marks "".')

            if (attributes_file is not None and (not isinstance(attributes_file, str))):
                raise ValueError('Error, parameter attributes_file has to be a name contained in quotation marks "".')

            with_roc = True
            if positive_class_index is None:
                with_roc = False

            if train_method not in {"randForest", "gradBoost"}:
                if nb_quant_levels is None:
                    nb_quant_levels = 50

            if max_iterations is None:
                max_iterations = 100
            if min_covering is None:
                min_covering = 2

            if min_fidelity is None:
                min_fidelity = 1.0
            if lowest_min_fidelity is None:
                lowest_min_fidelity = 0.75
            if covering_strategy is None:
                covering_strategy = True

            if seed is None:
                seed = 0
            elif (not isinstance(seed, int) or seed<0):
                raise ValueError('Error, parameter seed is not an positive integer.')

            # Check dimlpBT parameters

            if train_method == "dimlpBT":
                if nb_dimlp_nets is None:
                    nb_dimlp_nets = 25
                elif not isinstance(nb_dimlp_nets, int) or nb_dimlp_nets < 2:
                    raise ValueError('Error, parameter nb_dimlp_nets is not an integer greater than 1.')

                if nb_ex_per_net is None :
                    nb_ex_per_net = 0
                elif not isinstance(nb_ex_per_net, int) or nb_ex_per_net < 0:
                    raise ValueError('Error, parameter nb_ex_per_net is not a positive integer.')

            # Check dimlp and dimlpBT parameters

            if train_method in {"dimlp", "dimlpBT"}:

                if dimlprul is None:
                    raise ValueError('Error : dimlpRul is missing, add it with option dimlpRul=1 or 0.')
                elif (dimlprul not in {0,1}):
                    raise ValueError('Error, parameter dimlpRul is not 1 or 0.')

                if (pretrained_weights is not None and (not isinstance(pretrained_weights, str))):
                    raise ValueError('Error, parameter pretrained_weights has to be a name contained in quotation marks "".')

                if learning_rate is None:
                    learning_rate = 0.1
                if momentum is None:
                    momentum = 0.6
                if flat is None:
                    flat = 0.01
                if error_thresh is None:
                    error_thresh = -1111111111.0
                if acc_thresh is None:
                    acc_thresh = 11111111111111.0
                if abs_error_thresh is None:
                    abs_error_thresh = 0
                if nb_epochs is None:
                    nb_epochs = 1500
                if nb_epochs_error is None:
                    nb_epochs_error = 10

            # Check decision trees parameters

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

                # Check Random forests parameters

                if train_method == "randForest":
                    if rf_criterion_var is None:
                        rf_criterion_var = "gini"

                    if bootstrap_var is None:
                        bootstrap_var = True

                    if oob_score_var is None:
                        oob_score_var = False

                # Check Gradient boosting parameters
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

            if root_folder is not None:
                root = root_folder + separator
            else:
                root = ""

            data_file = root + data_file
            crossval_folder_temp = crossval_folder
            crossval_folder = root + crossval_folder
            crossval_stats = crossval_folder + separator + crossval_stats

            # Get datas in a list
            datas, classes = get_data(data_file, nb_attributes, nb_classes)
            if len(classes) == 0:
                if class_file is None:
                    raise ValueError('Error: class_file missing, add it with option class_file="your_class_file".')
                elif not isinstance(class_file, str):
                    raise ValueError('Error: parameter class_file has to be a name contained in quotation marks "".')
                class_file = root + class_file
                classes = get_data_class(class_file, nb_classes)

            nb_samples = len(datas)
            if k > nb_samples:
                raise ValueError(f'The number of divisions K of the dataset must be less or equal to the number of train samples({nb_samples}).')

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


            # Statistics initialization

            mean_fprs = [] #false positive rates for ROC curve
            mean_tprs = [] #true positive rates for ROC curve
            mean_aucs = []

            if is_fidex:
                # statistics for Fidex
                mean_cov_size_fid = 0.0
                mean_nb_ant_fid = 0.0
                mean_fidel_fid = 0.0
                mean_rules_acc_fid = 0.0
                mean_confid_fid = 0.0

                mean_exec_values_fidex = [] # each mean value in an entire fold for each fold for fidex

            if is_fidexglo:
                # Statistics for FidexGlo
                mean_nb_rules = 0.0
                mean_nb_cover = 0.0
                mean_nb_antecedents = 0.0
                mean_fidel_glo = 0.0
                mean_rules_acc_glo = 0.0
                mean_expl_glo = 0.0
                mean_default_rate = 0.0
                mean_nb_fidel_activations = 0.0
                mean_wrong_activations = 0.0
                mean_test_acc_glo = 0.0
                mean_test_acc_when_rules_and_model_agree = 0.0
                mean_test_acc_when_activated_rules_and_model_agree = 0.0

                if with_roc:
                    mean_nb_true_positive = 0
                    mean_nb_false_positive = 0
                    mean_nb_true_negative = 0
                    mean_nb_false_negative = 0
                    mean_false_positive_rate = 0.0
                    mean_false_negative_rate = 0.0
                    mean_precision = 0.0
                    mean_recall = 0.0
                    nb_no_false_positive_rate = 0.0
                    nb_no_false_negative_rate = 0.0
                    nb_no_precision = 0.0
                    nb_no_recall = 0.0

                    mean_nb_true_positive_rule = 0
                    mean_nb_false_positive_rule = 0
                    mean_nb_true_negative_rule = 0
                    mean_nb_false_negative_rule = 0
                    mean_false_positive_rate_rule = 0.0
                    mean_false_negative_rate_rule = 0.0
                    mean_precision_rule = 0.0
                    mean_recall_rule = 0.0
                    nb_no_false_positive_rate_rule = 0.0
                    nb_no_false_negative_rate_rule = 0.0
                    nb_no_precision_rule = 0.0
                    nb_no_recall_rule = 0.0

                mean_exec_values_fidexglo = [] # each mean value in an entire fold for each fold for fidexGlo

            if is_dimlprul:
                # Statistics for Dimlp Rules
                # One execution
                mean_nb_rules_dimlp = 0.0
                mean_nb_cover_dimlp = 0.0
                mean_nb_antecedents_dimlp = 0.0
                mean_fidel_dimlp = 0.0
                mean_rules_acc_dimlp = 0.0
                mean_default_rate_dimlp = 0.0
                mean_test_acc_dimlp = 0.0
                mean_test_acc_when_rules_and_model_agree_dimlp = 0.0

                # All executions
                mean_nb_rules_dimlp_all = 0.0
                mean_nb_cover_dimlp_all = 0.0
                mean_nb_antecedents_dimlp_all = 0.0
                mean_fidel_dimlp_all = 0.0
                mean_rules_acc_dimlp_all = 0.0
                mean_default_rate_dimlp_all = 0.0
                mean_test_acc_dimlp_all = 0.0
                mean_test_acc_when_rules_and_model_agree_dimlp_all = 0.0

                mean_exec_values_dimlp = []

                nb_no_rules_dimlp = 0.0 # If there is no rule found for a Fold


            # Write parameters on stats file

            try:
                with open(crossval_stats, "w") as outputStatsFile:

                    outputStatsFile.write(f"Parameters for {n} times {k}-Cross validation :\n")
                    outputStatsFile.write(f"Training with {train_method}\n")
                    outputStatsFile.write("---------------------------------------------------------\n")
                    if train_method not in {"dimlp", "dimlpBT"}:
                        outputStatsFile.write(f"There are {nb_attributes} attributes and {nb_classes} classes\n")
                    if train_method in {"dimlp", "dimlpBT"}:
                        outputStatsFile.write(f"Training with {nb_attributes} input neurons and {nb_classes} output neurons\n")
                        if first_hidden_layer is not None:
                            outputStatsFile.write(f"Layer 1 has {first_hidden_layer} neurons\n")
                        if hidden_layers is not None:
                            for key,value in enumerate(hidden_layers):
                                outputStatsFile.write(f"Layer {key+2} has {value} neurons\n")
                    if train_method not in {"randForest", "gradBoost"}:
                        outputStatsFile.write(f"The number of stairs in staircase activation function is {nb_quant_levels} and the interval in which hyperplans are contained is [-{hiknot},{hiknot}]\n")
                    if train_method == "dimlpBT":
                        outputStatsFile.write(f"The number of dimlp networks is {nb_dimlp_nets}\n")
                        if nb_ex_per_net == 0:
                            outputStatsFile.write("The number of examples for one single network is the number of train examples\n")
                        else:
                            outputStatsFile.write(f"The number of examples for one single network is {nb_ex_per_net} (but not more than the number of train examples)\n")
                    if train_method in {"dimlp", "dimlpBT"}:
                        outputStatsFile.write(f"The back-propagation learning parameter (Eta) is {learning_rate}\n")
                        outputStatsFile.write(f"The back-propagation momentum parameter (Mu) is {momentum}\n")
                        outputStatsFile.write(f"The back-propagation flat spot elimination parameter (Flat) is {flat}\n")
                        outputStatsFile.write(f"The error threshold is {error_thresh}\n")
                        outputStatsFile.write(f"The accuracy threshold is {acc_thresh}\n")
                        outputStatsFile.write(f"The absolute difference error threshold is {abs_error_thresh}\n")
                        outputStatsFile.write(f"The number of train epochs is {nb_epochs}\n")
                        outputStatsFile.write(f"The number of train epochs to show error is {nb_epochs_error}\n")
                    elif train_method == "svm":
                        outputStatsFile.write(f"The K parameter to improve dynamics by normalizing input data is {svm_k}\n")
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
                        if svm_max_iterations_var == -1:
                            outputStatsFile.write("There is no limit in the number of iterations\n")
                        else:
                            outputStatsFile.write(f"The maximal number of iterations is {svm_max_iterations_var}\n")
                        if decision_function_shape_var == "ovr":
                            outputStatsFile.write("The decision function shape is one-vs-rest\n")
                            if break_ties_var:
                                outputStatsFile.write("Using break tie decision\n")
                        else:
                            outputStatsFile.write("The decision function shape is one-vs-one\n")
                    elif train_method == "mlp":
                        outputStatsFile.write(f"The K parameter to improve dynamics by normalizing input data is {mlp_k}\n")
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
                        outputStatsFile.write(f"The maximum number of iterations is {mlp_max_iterations_var}\n")
                        if solver_var in {"adam", "sgd"} and shuffle_var == True:
                            outputStatsFile.write("Samples are shuffled in each iteration")
                        outputStatsFile.write(f"The tolerance for optimization is {mlp_tol_var}\n")
                        if mlp_warm_start_var == True:
                            outputStatsFile.write("Previous solution is reused to fit initialization")
                        if solver_var =="sgd":
                            outputStatsFile.write(f"The momentum for gradient descent update is {mlp_momentum_var}\n")
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
                    outputStatsFile.write(f"The max fidex and fidexGlo iteration number is {max_iterations}\n")
                    outputStatsFile.write(f"The minimum fidex and fidexGlo covering number is {min_covering}\n")
                    outputStatsFile.write(f"The minimum fidex and fidexGlo accepted fidelity is {min_fidelity}\n")
                    if covering_strategy:
                        outputStatsFile.write("The covering strategy is set\n")
                        outputStatsFile.write(f"The minimum fidex and fidexGlo accepted fidelity during covering strategy is {lowest_min_fidelity}\n")
                    if is_fidexglo:
                        outputStatsFile.write(f"The fidexGlo heuristic is {fidexglo_heuristic}\n")
                    if dropout_hyp:
                        outputStatsFile.write(f"The hyperplane dropout parameter for fidex and fidexGlo is {dropout_hyp}\n")
                    else:
                        outputStatsFile.write("There is no hyperplane dropout\n")
                    if dropout_dim:
                        outputStatsFile.write(f"The dimension dropout parameter for fidex and fidexGlo is {dropout_dim}\n")
                    else:
                        outputStatsFile.write("There is no dimension dropout\n")
                    if decision_threshold is not None :
                        outputStatsFile.write(f"We use a decision threshold of {decision_threshold} with positive class of index {positive_class_index}\n")
                    if normalization_file is not None or normalization_indices is not None:
                        outputStatsFile.write("We denormalize the rules\n")

                    outputStatsFile.write("---------------------------------------------------------\n\n")

                    outputStatsFile.close()
            except (FileNotFoundError):
                raise ValueError(f"Error : File for stats extraction ({crossval_stats}) not found.")
            except (IOError):
                raise ValueError(f"Error : Couldn't open stats extraction file {crossval_stats}.")

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

                fprs = [] #false positive rates for ROC curve
                tprs = [] #true positive rates for ROC curve
                aucs = []

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
                                for sample_trn in data_split[id]:
                                    for attr in sample_trn:
                                        trn_file.write(f"{str(attr)} ")
                                    trn_file.write("\n")
                        trn_file.close()

                    except (IOError):
                        raise ValueError(f"Error : Couldn't open file {temp_train_file}.")

                    try:
                        with open(temp_test_file, "w") as tst_file:
                            for sample_tst in data_split[test_idx]:
                                for attr in sample_tst:
                                    tst_file.write(f"{str(attr)} ")
                                tst_file.write("\n")
                        tst_file.close()

                    except (IOError):
                        raise ValueError(f"Error : Couldn't open file {temp_test_file}.")
                    if train_method == "dimlp":
                        try:
                            with open(temp_valid_file, "w") as val_file:
                                for sample_val in data_split[validation_idx]:
                                    for attr in sample_val:
                                        val_file.write(f"{str(attr)} ")
                                    val_file.write("\n")
                            val_file.close()

                        except (IOError):
                            raise ValueError(f"Error : Couldn't open file {temp_valid_file}.")

                    try:
                        with open(temp_train_tar_file, "w") as trn_val_file:
                            for id in train_idx:
                                for line_trn_tar in tar_data_split[id]:
                                    trn_val_file.write(f"{int(line_trn_tar)} \n")
                        trn_val_file.close()

                    except (IOError):
                        raise ValueError(f"Error : Couldn't open file {temp_train_tar_file}.")

                    try:
                        with open(temp_test_tar_file, "w") as tst_tar_file:
                            for line_tst_tar in tar_data_split[test_idx]:
                                tst_tar_file.write(f"{int(line_tst_tar)} \n")
                        tst_tar_file.close()

                    except (IOError):
                        raise ValueError(f"Error : Couldn't open file {temp_test_tar_file}.")
                    if train_method == "dimlp":
                        try:
                            with open(temp_valid_tar_file, "w") as val_tar_file:
                                for line_val_tar in tar_data_split[validation_idx]:
                                    val_tar_file.write(f"{int(line_val_tar)} \n")
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
                            dimlp_command += " --nb_dimlp_nets " + str(nb_dimlp_nets)
                            dimlp_command += " --nb_ex_per_net " + str(nb_ex_per_net)

                        dimlp_command += " --learning_rate " + str(learning_rate)
                        dimlp_command += " --momentum " + str(momentum)
                        dimlp_command += " --flat " + str(flat)
                        dimlp_command += " --error_thresh " + str(error_thresh)
                        dimlp_command += " --acc_thresh " + str(acc_thresh)
                        dimlp_command += " --abs_error_thresh " + str(abs_error_thresh)
                        dimlp_command += " --nb_epochs_error " + str(nb_epochs_error)
                        dimlp_command += " --nb_epochs " + str(nb_epochs)
                        dimlp_command += " --nb_attributes " + str(nb_attributes)
                        dimlp_command += " --nb_classes " + str(nb_classes)
                        if root_folder is not None:
                            dimlp_command += " --root_folder " + root_folder
                        if attributes_file is not None:
                            dimlp_command += " --attributes_file " + attributes_file
                        if pretrained_weights is not None:
                            dimlp_command += " --weights_file " + pretrained_weights
                        dimlp_command += " --seed " + str(seed)
                        dimlp_command += " --nb_quant_levels " + str(nb_quant_levels)
                        if first_hidden_layer is not None:
                            dimlp_command += " --first_hidden_layer " + str(first_hidden_layer)
                        if hidden_layers is not None:
                            dimlp_command += " --hidden_layers [" + ", ".join(str(value) for value in hidden_layers) + "]"

                        dimlp_command += " --train_data_file " + folder_path_from_root + separator + "train.txt "
                        dimlp_command += "--test_data_file " + folder_path_from_root + separator + "test.txt "
                        dimlp_command += "--train_class_file " + folder_path_from_root + separator + "trainTarget.txt "
                        dimlp_command += "--test_class_file " + folder_path_from_root + separator + "testTarget.txt "

                        dimlp_command += "--train_pred_outfile " + folder_path_from_root + separator + "train.out "   # Output train pred file
                        dimlp_command += "--test_pred_outfile " + folder_path_from_root + separator + "test.out "    # Output test pred file
                        dimlp_command += "--stats_file " + folder_path_from_root + separator + "stats.txt "    # Output stats file
                        dimlp_command += "--hidden_layers_outfile " + folder_path_from_root + separator + "hidden_layers.out "    # Output hidden layers file

                        if train_method == "dimlp":
                            dimlp_command += "--valid_data_file " + folder_path_from_root + separator + "valid.txt "
                            dimlp_command += "--valid_class_file " + folder_path_from_root + separator + "validTarget.txt "
                            dimlp_command += "--valid_pred_outfile " + folder_path_from_root + separator + "valid.out "   # Output validation pred file
                            dimlp_command += "--weights_outfile " + folder_path_from_root + separator + "weights.wts " # Output weight file
                        else:
                            dimlp_command += "--weights_outfile " + folder_path_from_root + separator + "weights.wts " # Output weight filename

                        if is_dimlprul:
                            dimlp_command += "--with_rule_extraction true --global_rules_outfile " + folder_path_from_root + separator + "dimlpRules.rls "

                        if normalization_file is not None:
                            dimlp_command += "--normalization_file " + normalization_file + " "
                        if mus is not None:
                            dimlp_command += "--mus " + mus + " "
                        if sigmas is not None:
                            dimlp_command += "--sigmas " + sigmas + " "
                        if normalization_indices is not None:
                            dimlp_command += "--normalization_indices " + normalization_indices + " "

                        dimlp_command += "--console_file " + str(crossval_folder_temp) + separator + "consoleTemp.txt" # To not show console result

                        if train_method == "dimlp":
                            print("Enter in dimlpTrn function")
                            res = dimlp.dimlpTrn(dimlp_command)
                        else:
                            print("Enter in dimlpBT function")
                            res = dimlp.dimlpBT(dimlp_command)
                        if (res == -1):
                            raise ValueError('Error during training with Dimlp or dimlpBT.')

                    # Training with svm
                    elif train_method == "svm":
                        print("Enter in svmTrn function")
                        return_roc_var = False
                        if with_roc:
                            return_roc_var = True
                        res = svmTrn(f"--train_data_file {folder_path_from_root + separator + 'train.txt'} --train_class_file {folder_path_from_root + separator + 'trainTarget.txt'} "
                                     f"--test_data_file {folder_path_from_root + separator + 'test.txt'} --test_class_file {folder_path_from_root + separator + 'testTarget.txt'} "
                                     f"--weights_outfile {folder_path_from_root + separator + 'weights.wts'} --stats_file {folder_path_from_root + separator + 'stats.txt'} --nb_attributes {nb_attributes} "
                                     f"--nb_classes {nb_classes} --console_file {crossval_folder_temp + separator + 'consoleTemp.txt'} --train_pred_outfile {folder_path_from_root + separator + 'train.out'} "
                                     f"--test_pred_outfile {folder_path_from_root + separator + 'test.out'} --positive_class_index {positive_class_index} "
                                     f"--output_roc {folder_path_from_root + separator + 'ROC_curve'} --root_folder {root_folder} --nb_quant_levels {nb_quant_levels} "
                                     f"--K {svm_k} --C {c_var} --kernel {kernel_var} --degree {degree_var} --gamma {gamma_var} --coef0 {coef0_var} --shrinking {shrinking_var} "
                                     f"--tol {svm_tol_var} --cache_size {cache_size_var} --class_weight {svm_class_weight_var} --max_iterations {svm_max_iterations_var} "
                                     f"--decision_function_shape {decision_function_shape_var} --break_ties {break_ties_var} --return_roc {return_roc_var}")

                        if (res == -1):
                            raise ValueError('Error during training with SVM.')
                        elif with_roc:
                            fprs.append(res[0])
                            tprs.append(res[1])
                            aucs.append(res[2])

                    # Training with mlp
                    elif train_method == "mlp":
                        print("Enter in mlpTrn function")
                        seed_var = seed
                        if seed == 0:
                            seed_var = None
                        res = mlpTrn(f"--train_data_file {folder_path_from_root + separator + 'train.txt'} --train_class_file {folder_path_from_root + separator + 'trainTarget.txt'} "
                                     f"--test_data_file {folder_path_from_root + separator + 'test.txt'} --test_class_file {folder_path_from_root + separator + 'testTarget.txt'} "
                                     f"--weights_outfile {folder_path_from_root + separator + 'weights.wts'} --stats_file {folder_path_from_root + separator + 'stats.txt'} --nb_attributes {nb_attributes} "
                                     f"--nb_classes {nb_classes} --console_file {crossval_folder_temp + separator + 'consoleTemp.txt'} --train_pred_outfile {folder_path_from_root + separator + 'train.out'} "
                                     f"--test_pred_outfile {folder_path_from_root + separator + 'test.out'} --root_folder {root_folder} --nb_quant_levels {nb_quant_levels} "
                                     f"--K {mlp_k} --hidden_layer_sizes {hidden_layer_sizes_var} --activation {activation_var} --solver {solver_var} --alpha {alpha_var} "
                                     f"--batch_size {batch_size_var} --learning_rate {mlp_learning_rate_var} --learning_rate_init {learning_rate_init_var} --power_t {power_t_var} "
                                     f"--max_iterations {mlp_max_iterations_var} --shuffle {shuffle_var} --tol {mlp_tol_var} --warm_start {mlp_warm_start_var} --momentum {mlp_momentum_var} "
                                     f"--nesterovs_momentum {nesterovs_momentum_var} --early_stopping {early_stopping_var} --validation_fraction {mlp_validation_fraction_var} "
                                     f"--beta_1 {beta_1_var} --beta_2 {beta_2_var} --epsilon {epsilon_var} --n_iter_no_change {mlp_n_iter_no_change_var} --max_fun {max_fun_var} "
                                     f"--seed {seed_var}")

                        if (res == -1):
                            raise ValueError('Error during training with MLP.')

                    # Training with random forests
                    elif train_method == "randForest":
                        print("Enter in randForestsTrn function")
                        seed_var = seed
                        if seed == 0:
                            seed_var = None
                        res = randForestsTrn(f"--train_data_file {folder_path_from_root + separator + 'train.txt'} --train_class_file {folder_path_from_root + separator + 'trainTarget.txt'} "
                                     f"--test_data_file {folder_path_from_root + separator + 'test.txt'} --test_class_file {folder_path_from_root + separator + 'testTarget.txt'} "
                                     f"--stats_file {folder_path_from_root + separator + 'stats.txt'} --console_file {crossval_folder_temp + separator + 'consoleTemp.txt'} "
                                     f"--train_pred_outfile {folder_path_from_root + separator + 'train.out'} --test_pred_outfile {folder_path_from_root + separator + 'test.out'} --nb_attributes {nb_attributes} "
                                     f"--nb_classes {nb_classes} --rules_outfile {folder_path_from_root + separator + 'treesRules.rls'} --root_folder {root_folder} --n_estimators {n_estimators_var} "
                                     f"--min_samples_split {min_samples_split_var} --min_samples_leaf {min_samples_leaf_var} --min_weight_fraction_leaf {min_weight_fraction_leaf_var} "
                                     f"--max_features {max_features_var} --min_impurity_decrease {min_impurity_decrease_var} --max_leaf_nodes {max_leaf_nodes_var} "
                                     f"--warm_start {dt_warm_start_var} --ccp_alpha {ccp_alpha_var} --criterion {rf_criterion_var} --max_depth {rf_max_depth_var} "
                                     f"--bootstrap {bootstrap_var} --oob_score {oob_score_var} --n_jobs {n_jobs_var} --class_weight {rf_class_weight_var} "
                                     f"--max_samples {max_samples_var} --seed {seed_var}")

                        if (res == -1):
                            raise ValueError('Error during training with Random Forests.')

                    # Training with gradient boosting
                    else:
                        print("Enter in gradBoostTrn function")
                        seed_var = seed
                        if seed == 0:
                            seed_var = None
                        res = gradBoostTrn(f"--train_data_file {folder_path_from_root + separator + 'train.txt'} --train_class_file {folder_path_from_root + separator + 'trainTarget.txt'} "
                                     f"--test_data_file {folder_path_from_root + separator + 'test.txt'} --test_class_file {folder_path_from_root + separator + 'testTarget.txt'} "
                                     f"--stats_file {folder_path_from_root + separator + 'stats.txt'} --console_file {crossval_folder_temp + separator + 'consoleTemp.txt'} "
                                     f"--train_pred_outfile {folder_path_from_root + separator + 'train.out'} --test_pred_outfile {folder_path_from_root + separator + 'test.out'} --nb_attributes {nb_attributes} "
                                     f"--nb_classes {nb_classes} --rules_outfile {folder_path_from_root + separator + 'treesRules.rls'} --root_folder {root_folder} --n_estimators {n_estimators_var} "
                                     f"--min_samples_split {min_samples_split_var} --min_samples_leaf {min_samples_leaf_var} --min_weight_fraction_leaf {min_weight_fraction_leaf_var} "
                                     f"--max_features {max_features_var} --min_impurity_decrease {min_impurity_decrease_var} --max_leaf_nodes {max_leaf_nodes_var} "
                                     f"--warm_start {dt_warm_start_var} --ccp_alpha {ccp_alpha_var} --loss {loss_var} --learning_rate {gb_learning_rate_var} "
                                     f"--subsample {subsample_var} --criterion {gb_criterion_var} --max_depth {gb_max_depth_var} --init {init_var} "
                                     f"--validation_fraction {gb_validation_fraction_var} --n_iter_no_change {gb_n_iter_no_change_var} --tol {gb_tol_var} "
                                     f"--seed {seed_var}")

                        if (res == -1):
                            raise ValueError('Error during training with Gradient Boosting.')

                    if train_method != "svm" and with_roc:
                        res = computeRocCurve(f"--test_class_file {folder_path_from_root + separator + 'testTarget.txt'} --test_pred_file {folder_path_from_root + separator + 'test.out'} "
                                              f"--positive_class_index {positive_class_index} --nb_classes {nb_classes} --output_roc {folder_path_from_root + separator + 'ROC_curve'} "
                                              f"--stats_file {folder_path_from_root + separator + 'stats.txt'} --root_folder {root_folder} --show_params False")
                        if (res == -1):
                            raise ValueError('Error during computation of ROC curve.')
                        else:
                            fprs.append(res[0])
                            tprs.append(res[1])
                            aucs.append(res[2])

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
                            mean_nb_antecedents_dimlp += stats[2]
                            mean_fidel_dimlp += stats[5]
                            mean_rules_acc_dimlp += stats[4]
                            mean_default_rate_dimlp += stats[7]
                            mean_test_acc_when_rules_and_model_agree_dimlp += stats[6]
                        mean_test_acc_dimlp += test_acc


                    if is_fidex:
                        # Compute fidex stats in folder
                        fidex_command = "fidex"
                        if train_method in {"dimlp", "dimlpBT", "svm", "mlp"}:
                            fidex_command +=  " --nb_quant_levels " + str(nb_quant_levels)
                        if root_folder is not None:
                            fidex_command +=  " --root_folder " + root_folder
                        fidex_command += " --nb_attributes " + str(nb_attributes)
                        fidex_command += " --nb_classes " + str(nb_classes)
                        if attributes_file is not None:
                            fidex_command +=  " --attributes_file " + attributes_file
                        fidex_command +=  " --max_iterations " + str(max_iterations)
                        fidex_command +=  " --min_covering " + str(min_covering)
                        fidex_command +=  " --min_fidelity " + str(min_fidelity)
                        fidex_command +=  " --lowest_min_fidelity " + str(lowest_min_fidelity)
                        fidex_command +=  " --covering_strategy " + str(covering_strategy)
                        if dropout_dim != None:
                            fidex_command +=  " --dropout_dim " + str(dropout_dim)
                        if dropout_hyp != None:
                            fidex_command +=  " --dropout_hyp " + str(dropout_hyp)
                        fidex_command +=  " --seed " + str(seed)

                        if train_method in {"dimlp", "dimlpBT", "svm", "mlp"}:
                            fidex_command +=  " --weights_file " + folder_path_from_root + separator + "weights.wts"
                        else:
                            fidex_command += " --rules_file " + folder_path_from_root + separator + "treesRules.rls"

                        fidex_command += " --train_data_file " + folder_path_from_root + separator + "train.txt"
                        fidex_command += " --train_pred_file " + folder_path_from_root + separator + "train.out"
                        fidex_command += " --train_class_file " + folder_path_from_root + separator + "trainTarget.txt"
                        fidex_command += " --test_data_file " + folder_path_from_root + separator + "test.txt"
                        fidex_command += " --test_pred_file " + folder_path_from_root + separator + "test.out"
                        fidex_command += " --test_class_file " + folder_path_from_root + separator + "testTarget.txt"
                        fidex_command += " --rules_outfile " + folder_path_from_root + separator + "fidexRule.txt"
                        fidex_command += " --stats_file " + folder_path_from_root + separator + "fidexStats.txt"
                        fidex_command += " --console_file " + folder_path_from_root + separator + "fidexResult.txt"
                        if with_roc:
                            fidex_command += " --positive_class_index " + str(positive_class_index)
                        if decision_threshold is not None:
                            fidex_command += " --decision_threshold " + str(decision_threshold)

                        if normalization_file is not None:
                            fidex_command += " --normalization_file " + normalization_file
                        if mus is not None:
                            fidex_command += " --mus " + mus
                        if sigmas is not None:
                            fidex_command += " --sigmas " + sigmas
                        if normalization_indices is not None:
                            fidex_command += " --normalization_indices " + normalization_indices

                        print("Enter in fidex function")
                        res_fid = fidex.fidex(fidex_command)
                        if res_fid == -1:
                            raise ValueError('Error during execution of Fidex.')

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
                        mean_confid_fid += stat_vals[4]

                    if is_fidexglo:
                        # Compute fidexGlo rules in folder
                        fidexglo_rules_command = "fidexGloRules"
                        if train_method in {"dimlp", "dimlpBT", "svm", "mlp"}:
                            fidexglo_rules_command +=  " --nb_quant_levels " + str(nb_quant_levels)
                        if root_folder is not None:
                            fidexglo_rules_command +=  " --root_folder " + root_folder
                        fidexglo_rules_command += " --nb_attributes " + str(nb_attributes)
                        fidexglo_rules_command += " --nb_classes " + str(nb_classes)
                        if attributes_file is not None:
                            fidexglo_rules_command +=  " --attributes_file " + attributes_file
                        fidexglo_rules_command +=  " --max_iterations " + str(max_iterations)
                        fidexglo_rules_command +=  " --min_covering " + str(min_covering)
                        fidexglo_rules_command +=  " --min_fidelity " + str(min_fidelity)
                        fidexglo_rules_command +=  " --lowest_min_fidelity " + str(lowest_min_fidelity)
                        fidexglo_rules_command +=  " --covering_strategy " + str(covering_strategy)
                        if dropout_dim != None:
                            fidexglo_rules_command +=  " --dropout_dim " + str(dropout_dim)
                        if dropout_hyp != None:
                            fidexglo_rules_command +=  " --dropout_hyp " + str(dropout_hyp)
                        fidexglo_rules_command +=  " --seed " + str(seed)
                        if train_method in {"dimlp", "dimlpBT", "svm", "mlp"}:
                            fidexglo_rules_command +=  " --weights_file " + folder_path_from_root + separator + "weights.wts"
                        else:
                            fidexglo_rules_command += " --rules_file " + folder_path_from_root + separator + "treesRules.rls"

                        fidexglo_rules_command += " --train_data_file " + folder_path_from_root + separator + "train.txt"
                        fidexglo_rules_command += " --train_pred_file " + folder_path_from_root + separator + "train.out"
                        fidexglo_rules_command += " --train_class_file " + folder_path_from_root + separator + "trainTarget.txt"
                        fidexglo_rules_command += " --global_rules_outfile " + folder_path_from_root + separator + "fidexGloRules.txt"
                        fidexglo_rules_command += " --console_file " + folder_path_from_root + separator + "fidexGloResult.txt"
                        fidexglo_rules_command += " --heuristic " + str(fidexglo_heuristic)
                        if with_roc:
                            fidexglo_rules_command += " --positive_class_index " + str(positive_class_index)
                        if decision_threshold is not None:
                            fidexglo_rules_command += " --decision_threshold " + str(decision_threshold)

                        if normalization_file is not None:
                            fidexglo_rules_command += " --normalization_file " + normalization_file
                        if mus is not None:
                            fidexglo_rules_command += " --mus " + mus
                        if sigmas is not None:
                            fidexglo_rules_command += " --sigmas " + sigmas
                        if normalization_indices is not None:
                            fidexglo_rules_command += " --normalization_indices " + normalization_indices
                        if nb_threads is not None:
                            fidexglo_rules_command += " --nb_threads " + str(nb_threads)
                        print("Enter in fidexGloRules function")
                        res_fid_glo_rules = fidex.fidexGloRules(fidexglo_rules_command)
                        if res_fid_glo_rules == -1:
                            raise ValueError('Error during execution of FidexGloRules.')

                        # Compute fidexGlo statistics in folder
                        fidexglo_stats_command = "fidexGloStats"
                        if root_folder is not None:
                            fidexglo_stats_command +=  " --root_folder " + root_folder
                        fidexglo_stats_command += " --nb_attributes " + str(nb_attributes)
                        fidexglo_stats_command += " --nb_classes " + str(nb_classes)
                        if attributes_file is not None:
                            fidexglo_stats_command +=  " --attributes_file " + attributes_file
                        fidexglo_stats_command += " --test_data_file " + folder_path_from_root + separator + "test.txt"
                        fidexglo_stats_command += " --test_pred_file " + folder_path_from_root + separator + "test.out"
                        fidexglo_stats_command += " --test_class_file " + folder_path_from_root + separator + "testTarget.txt"
                        fidexglo_stats_command += " --global_rules_file " + folder_path_from_root + separator + "fidexGloRules.txt"
                        fidexglo_stats_command += " --stats_file " + folder_path_from_root + separator + "fidexGloStats.txt"
                        fidexglo_stats_command += " --console_file " + folder_path_from_root + separator + "fidexGloStatsResult.txt"
                        fidexglo_stats_command += " --global_rules_outfile " + folder_path_from_root + separator + "fidexGloRules.txt"
                        if decision_threshold is not None:
                            fidexglo_stats_command += " --decision_threshold " + str(decision_threshold)
                        if with_roc:
                            fidexglo_stats_command += " --positive_class_index " + str(positive_class_index)

                        print("Enter in fidexGloStats function")
                        res_fid_glo_stats = fidex.fidexGloStats(fidexglo_stats_command)
                        if res_fid_glo_stats == -1:
                            raise ValueError('Error during execution of FidexGloStats.')
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
                                        raise ValueError("Error in second line of fidexGlo stat file, there is not the number of rules.")
                                    if "sample" in values:
                                        mean_nb_cover += float(values[values.index("sample") + 6].rstrip(','))
                                    else:
                                        raise ValueError("Error in second line of fidexGlo stat file, there is not the mean sample covering number per rule.")
                                    if "antecedents" in values:
                                        mean_nb_antecedents += float(values[values.index("antecedents") + 4])
                                    else:
                                        raise ValueError("Error in second line of fidexGlo stat file, there is not the mean number of antecedents per rule.")

                                else:
                                    raise ValueError("Error in second line of fidexGlo stat file.")
                                line_stats_glo = my_file.readline()
                                line_stats_glo = my_file.readline()
                                line_stats_glo = my_file.readline()
                                line_stats_glo = my_file.readline()
                                stat_glo_vals = []
                                while line_stats_glo:
                                    if line_stats_glo.startswith("With positive") or line_stats_glo.startswith("Computation with"):
                                        line_stats_glo = my_file.readline()
                                    line_stats_glo = line_stats_glo.strip()  # Remove the line break at the end of the line
                                    if line_stats_glo != "":
                                        elements = line_stats_glo.split()
                                        if elements[-1] == "N/A":
                                            stat_glo_vals.append("N/A")
                                        else:
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
                        if with_roc:
                            mean_nb_true_positive += stat_glo_vals[9]
                            mean_nb_false_positive += stat_glo_vals[10]
                            mean_nb_true_negative += stat_glo_vals[11]
                            mean_nb_false_negative += stat_glo_vals[12]
                            if stat_glo_vals[13] == "N/A":
                                nb_no_false_positive_rate += 1
                            else:
                                mean_false_positive_rate += stat_glo_vals[13]
                            if stat_glo_vals[14] == "N/A":
                                nb_no_false_negative_rate += 1
                            else:
                                mean_false_negative_rate += stat_glo_vals[14]
                            if stat_glo_vals[15] == "N/A":
                                nb_no_precision += 1
                            else:
                                mean_precision += stat_glo_vals[15]
                            if stat_glo_vals[16] == "N/A":
                                nb_no_recall += 1
                            else:
                                mean_recall += stat_glo_vals[16]

                            mean_nb_true_positive_rule += stat_glo_vals[17]
                            mean_nb_false_positive_rule += stat_glo_vals[18]
                            mean_nb_true_negative_rule += stat_glo_vals[19]
                            mean_nb_false_negative_rule += stat_glo_vals[20]
                            if stat_glo_vals[21] == "N/A":
                                nb_no_false_positive_rate_rule += 1
                            else:
                                mean_false_positive_rate_rule += stat_glo_vals[21]
                            if stat_glo_vals[22] == "N/A":
                                nb_no_false_negative_rate_rule += 1
                            else:
                                mean_false_negative_rate_rule += stat_glo_vals[22]
                            if stat_glo_vals[23] == "N/A":
                                nb_no_precision_rule += 1
                            else:
                                mean_precision_rule += stat_glo_vals[23]
                            if stat_glo_vals[24] == "N/A":
                                nb_no_recall_rule += 1
                            else:
                                mean_recall_rule += stat_glo_vals[24]
                # Compute execution Stats

                if with_roc:
                    mean_aucs.append(np.mean(np.array(aucs)))
                    mean_tprs.append(np.mean(np.array(tprs), axis=0))
                    mean_fprs.append(np.mean(np.array(fprs), axis=0))
                    formatted_mean_aucs = formatting(mean_aucs[-1])
                    viz = RocCurveDisplay(fpr=mean_fprs[-1],
                    tpr=mean_tprs[-1],
                    roc_auc=mean_aucs[-1]).plot(color="darkorange", plot_chance_level=True)

                    viz.figure_.savefig(str(crossval_folder) + separator + "Execution" + str(ni + 1) + separator + "ROC_curve.png")
                    plt.close(viz.figure_)

                mean_current_exec_values_dimlp = []
                if is_dimlprul: # For dimlpRul
                    nb_fold_with_rules_dimlp = k-nb_no_rules_current_exec_dimlp

                    mean_current_exec_values_dimlp.append(mean_nb_rules_dimlp / nb_fold_with_rules_dimlp)
                    mean_nb_rules_dimlp = 0
                    mean_current_exec_values_dimlp.append(mean_nb_cover_dimlp / nb_fold_with_rules_dimlp)
                    mean_nb_cover_dimlp = 0
                    mean_current_exec_values_dimlp.append(mean_nb_antecedents_dimlp / nb_fold_with_rules_dimlp)
                    mean_nb_antecedents_dimlp = 0
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
                    mean_current_exec_values_fidexglo.append(mean_nb_antecedents / k)
                    mean_nb_antecedents = 0
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
                    if with_roc:
                        mean_current_exec_values_fidexglo.append(mean_nb_true_positive / k)
                        mean_nb_true_positive = 0
                        mean_current_exec_values_fidexglo.append(mean_nb_false_positive / k)
                        mean_nb_false_positive = 0
                        mean_current_exec_values_fidexglo.append(mean_nb_true_negative / k)
                        mean_nb_true_negative = 0
                        mean_current_exec_values_fidexglo.append(mean_nb_false_negative / k)
                        mean_nb_false_negative = 0
                        if nb_no_false_positive_rate == k:
                            mean_current_exec_values_fidexglo.append("N/A")
                        else:
                            mean_current_exec_values_fidexglo.append(mean_false_positive_rate / (k-nb_no_false_positive_rate))
                        mean_false_positive_rate = 0
                        nb_no_false_positive_rate = 0
                        if nb_no_false_negative_rate == k:
                            mean_current_exec_values_fidexglo.append("N/A")
                        else:
                            mean_current_exec_values_fidexglo.append(mean_false_negative_rate / (k-nb_no_false_negative_rate))
                        mean_false_negative_rate = 0
                        nb_no_false_negative_rate = 0
                        if nb_no_precision == k:
                            mean_current_exec_values_fidexglo.append("N/A")
                        else:
                            mean_current_exec_values_fidexglo.append(mean_precision / (k-nb_no_precision))
                        mean_precision = 0
                        nb_no_precision = 0
                        if nb_no_recall == k:
                            mean_current_exec_values_fidexglo.append("N/A")
                        else:
                            mean_current_exec_values_fidexglo.append(mean_recall / (k-nb_no_recall))
                        mean_recall = 0
                        nb_no_recall = 0

                        mean_current_exec_values_fidexglo.append(mean_nb_true_positive_rule / k)
                        mean_nb_true_positive_rule = 0
                        mean_current_exec_values_fidexglo.append(mean_nb_false_positive_rule / k)
                        mean_nb_false_positive_rule = 0
                        mean_current_exec_values_fidexglo.append(mean_nb_true_negative_rule / k)
                        mean_nb_true_negative_rule = 0
                        mean_current_exec_values_fidexglo.append(mean_nb_false_negative_rule / k)
                        mean_nb_false_negative_rule = 0
                        if nb_no_false_positive_rate_rule == k:
                            mean_current_exec_values_fidexglo.append("N/A")
                        else:
                            mean_current_exec_values_fidexglo.append(mean_false_positive_rate_rule / (k-nb_no_false_positive_rate_rule))
                        mean_false_positive_rate_rule = 0
                        nb_no_false_positive_rate_rule = 0
                        if nb_no_false_negative_rate_rule == k:
                            mean_current_exec_values_fidexglo.append("N/A")
                        else:
                            mean_current_exec_values_fidexglo.append(mean_false_negative_rate_rule / (k-nb_no_false_negative_rate_rule))
                        mean_false_negative_rate_rule = 0
                        nb_no_false_negative_rate_rule = 0
                        if nb_no_precision_rule == k:
                            mean_current_exec_values_fidexglo.append("N/A")
                        else:
                            mean_current_exec_values_fidexglo.append(mean_precision_rule / (k-nb_no_precision_rule))
                        mean_precision_rule = 0
                        nb_no_precision_rule = 0
                        if nb_no_recall_rule == k:
                            mean_current_exec_values_fidexglo.append("N/A")
                        else:
                            mean_current_exec_values_fidexglo.append(mean_recall_rule / (k-nb_no_recall_rule))
                        mean_recall_rule = 0
                        nb_no_recall_rule = 0
                    mean_exec_values_fidexglo.append(mean_current_exec_values_fidexglo)

                # Output and show stats
                try:
                    with open(crossval_stats, "a") as outputStatsFile:
                        outputStatsFile.write(f"Results for execution {ni + 1} of {k}-Cross validation :\n\n")
                        print("\n---------------------------------------------------------")
                        print("---------------------------------------------------------\n")
                        print(f"Results for execution {ni + 1} of {k}-Cross validation :\n")
                        if with_roc:
                            outputStatsFile.write(f"The mean AUC score is: {formatted_mean_aucs}\n\n")
                            print(f"The mean AUC score is: {formatted_mean_aucs}\n")
                        if is_dimlprul:
                            formatted_mean_current_exec_values_dimlp = []
                            for i in range(len(mean_current_exec_values_dimlp)):
                                formatted_mean_current_exec_values_dimlp.append(formatting(mean_current_exec_values_dimlp[i]))
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
                                formatted_mean_current_exec_values_fidex.append(formatting(mean_current_exec_values_fidex[i]))
                            outputStatsFile.write("Fidex :\n")
                            outputStatsFile.write(f"The mean covering size per rule is: {formatted_mean_current_exec_values_fidex[0]}\n")
                            outputStatsFile.write(f"The mean number of antecedents per rule is: {formatted_mean_current_exec_values_fidex[1]}\n")
                            outputStatsFile.write(f"The mean rule fidelity rate is: {formatted_mean_current_exec_values_fidex[2]}\n")
                            outputStatsFile.write(f"The mean rule accuracy is: {formatted_mean_current_exec_values_fidex[3]}\n")
                            outputStatsFile.write(f"The mean rule confidence is: {formatted_mean_current_exec_values_fidex[4]}\n")
                            print("Fidex :")
                            print(f"The mean covering size per rule is: {formatted_mean_current_exec_values_fidex[0]}")
                            print(f"The mean number of antecedents per rule is: {formatted_mean_current_exec_values_fidex[1]}")
                            print(f"The mean rule fidelity rate is: {formatted_mean_current_exec_values_fidex[2]}")
                            print(f"The mean rule accuracy is: {formatted_mean_current_exec_values_fidex[3]}")
                            print(f"The mean rule confidence is: {formatted_mean_current_exec_values_fidex[4]}")
                            print("\n---------------------------------------------------------")
                            print("---------------------------------------------------------")
                        if is_fidex and is_fidexglo:
                            outputStatsFile.write("\n---------------------------------------------------------\n")
                            print("")
                        if is_fidexglo:
                            formatted_mean_current_exec_values_fidexglo = []
                            for i in range(len(mean_current_exec_values_fidexglo)):
                                formatted_mean_current_exec_values_fidexglo.append(formatting(mean_current_exec_values_fidexglo[i]))
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
                            if with_roc:
                                outputStatsFile.write(f"\nWith positive class {positive_class_index} :\n\n")
                                outputStatsFile.write("Computation with model decision :\n\n")
                                outputStatsFile.write(f"The mean number of true positive test samples is : {formatted_mean_current_exec_values_fidexglo[12]}\n")
                                outputStatsFile.write(f"The mean number of false positive test samples is : {formatted_mean_current_exec_values_fidexglo[13]}\n")
                                outputStatsFile.write(f"The mean number of true negative test samples is : {formatted_mean_current_exec_values_fidexglo[14]}\n")
                                outputStatsFile.write(f"The mean number of false negative test samples is : {formatted_mean_current_exec_values_fidexglo[15]}\n")
                                outputStatsFile.write(f"The mean false positive rate is : {formatted_mean_current_exec_values_fidexglo[16]}\n")
                                outputStatsFile.write(f"The mean false negative rate is : {formatted_mean_current_exec_values_fidexglo[17]}\n")
                                outputStatsFile.write(f"The mean precision is : {formatted_mean_current_exec_values_fidexglo[18]}\n")
                                outputStatsFile.write(f"The mean recall is : {formatted_mean_current_exec_values_fidexglo[19]}\n")
                                outputStatsFile.write("\nComputation with model decision :\n\n")
                                outputStatsFile.write(f"The mean number of true positive test samples is : {formatted_mean_current_exec_values_fidexglo[20]}\n")
                                outputStatsFile.write(f"The mean number of false positive test samples is : {formatted_mean_current_exec_values_fidexglo[21]}\n")
                                outputStatsFile.write(f"The mean number of true negative test samples is : {formatted_mean_current_exec_values_fidexglo[22]}\n")
                                outputStatsFile.write(f"The mean number of false negative test samples is : {formatted_mean_current_exec_values_fidexglo[23]}\n")
                                outputStatsFile.write(f"The mean false positive rate is : {formatted_mean_current_exec_values_fidexglo[24]}\n")
                                outputStatsFile.write(f"The mean false negative rate is : {formatted_mean_current_exec_values_fidexglo[25]}\n")
                                outputStatsFile.write(f"The mean precision is : {formatted_mean_current_exec_values_fidexglo[26]}\n")
                                outputStatsFile.write(f"The mean recall is : {formatted_mean_current_exec_values_fidexglo[27]}\n")
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
                            if with_roc:
                                print(f"\nWith positive class {positive_class_index} :\n")
                                print("Computation with model decision :\n")
                                print(f"The mean number of true positive test samples is : {formatted_mean_current_exec_values_fidexglo[12]}")
                                print(f"The mean number of false positive test samples is : {formatted_mean_current_exec_values_fidexglo[13]}")
                                print(f"The mean number of true negative test samples is : {formatted_mean_current_exec_values_fidexglo[14]}")
                                print(f"The mean number of false negative test samples is : {formatted_mean_current_exec_values_fidexglo[15]}")
                                print(f"The mean false positive rate is : {formatted_mean_current_exec_values_fidexglo[16]}")
                                print(f"The mean false negative rate is : {formatted_mean_current_exec_values_fidexglo[17]}")
                                print(f"The mean precision is : {formatted_mean_current_exec_values_fidexglo[18]}")
                                print(f"The mean recall is : {formatted_mean_current_exec_values_fidexglo[19]}")
                                print("\nComputation with rules decision :\n")
                                print(f"The mean number of true positive test samples is : {formatted_mean_current_exec_values_fidexglo[20]}")
                                print(f"The mean number of false positive test samples is : {formatted_mean_current_exec_values_fidexglo[21]}")
                                print(f"The mean number of true negative test samples is : {formatted_mean_current_exec_values_fidexglo[22]}")
                                print(f"The mean number of false negative test samples is : {formatted_mean_current_exec_values_fidexglo[23]}")
                                print(f"The mean false positive rate is : {formatted_mean_current_exec_values_fidexglo[24]}")
                                print(f"The mean false negative rate is : {formatted_mean_current_exec_values_fidexglo[25]}")
                                print(f"The mean precision is : {formatted_mean_current_exec_values_fidexglo[26]}")
                                print(f"The mean recall is : {formatted_mean_current_exec_values_fidexglo[27]}")
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
            if with_roc:
                mean_all_aucs = np.mean(np.array(mean_aucs))
                std_all_aucs = np.std(np.array(mean_aucs))

                mean_all_fprs = np.mean(np.array(mean_fprs), axis=0)
                mean_all_tprs = np.mean(np.array(mean_tprs), axis=0)

                viz = RocCurveDisplay(fpr=mean_all_fprs,
                                    tpr=mean_all_tprs,
                                    roc_auc=mean_all_aucs).plot(color="darkorange", plot_chance_level=True)

                viz.figure_.savefig(crossval_folder + separator + "ROC_curve.png")
                plt.close(viz.figure_)

                formatted_mean_all_aucs = formatting(mean_all_aucs)
                formatted_std_all_aucs = formatting(std_all_aucs)


            if is_dimlprul: # For dimlpRul
                for exec in range(n):
                    multiplier = k-mean_exec_values_dimlp[exec][8] # If there is lack of some datas (sometimes we didnt found any rules), we need to take it into account
                    mean_nb_rules_dimlp_all += multiplier*mean_exec_values_dimlp[exec][0]
                    mean_nb_cover_dimlp_all += multiplier*mean_exec_values_dimlp[exec][1]
                    mean_nb_antecedents_dimlp_all += multiplier*mean_exec_values_dimlp[exec][2]
                    mean_fidel_dimlp_all += multiplier*mean_exec_values_dimlp[exec][3]
                    mean_rules_acc_dimlp_all += multiplier*mean_exec_values_dimlp[exec][4]
                    mean_default_rate_dimlp_all += multiplier*mean_exec_values_dimlp[exec][5]
                    mean_test_acc_dimlp_all += multiplier*mean_exec_values_dimlp[exec][6]
                    mean_test_acc_when_rules_and_model_agree_dimlp_all += multiplier*mean_exec_values_dimlp[exec][7]

                divider = n*k-nb_no_rules_dimlp
                mean_nb_rules_dimlp_all /= divider
                mean_nb_cover_dimlp_all /= divider
                mean_nb_antecedents_dimlp_all /= divider
                mean_fidel_dimlp_all /= divider
                mean_rules_acc_dimlp_all /= divider
                mean_default_rate_dimlp_all /= divider
                mean_test_acc_dimlp_all /= divider
                mean_test_acc_when_rules_and_model_agree_dimlp_all /= divider

                std_nb_rules_dimlp_all = np.std(np.array(mean_exec_values_dimlp)[:,0].astype(float))
                std_nb_cover_dimlp_all = np.std(np.array(mean_exec_values_dimlp)[:,1].astype(float))
                std_nb_antecedents_dimlp_all = np.std(np.array(mean_exec_values_dimlp)[:,2].astype(float))
                std_fidel_dimlp_all = np.std(np.array(mean_exec_values_dimlp)[:,3].astype(float))
                std_rules_acc_dimlp_all = np.std(np.array(mean_exec_values_dimlp)[:,4].astype(float))
                std_default_rate_dimlp_all = np.std(np.array(mean_exec_values_dimlp)[:,5].astype(float))
                std_test_acc_dimlp_all = np.std(np.array(mean_exec_values_dimlp)[:,6].astype(float))
                std_test_acc_when_rules_and_model_agree_dimlp_all = np.std(np.array(mean_exec_values_dimlp)[:,7].astype(float))

            if is_fidex: # For Fidex

                mean_cov_size_fid_all = np.mean(np.array(mean_exec_values_fidex)[:,0].astype(float))
                mean_nb_ant_fid_all = np.mean(np.array(mean_exec_values_fidex)[:,1].astype(float))
                mean_fidel_fid_all = np.mean(np.array(mean_exec_values_fidex)[:,2].astype(float))
                mean_rules_acc_fid_all = np.mean(np.array(mean_exec_values_fidex)[:,3].astype(float))
                mean_confid_fid_all = np.mean(np.array(mean_exec_values_fidex)[:,4].astype(float))

                std_cov_size_fid_all = np.std(np.array(mean_exec_values_fidex)[:,0].astype(float))
                std_nb_ant_fid_all = np.std(np.array(mean_exec_values_fidex)[:,1].astype(float))
                std_fidel_fid_all = np.std(np.array(mean_exec_values_fidex)[:,2].astype(float))
                std_rules_acc_fid_all = np.std(np.array(mean_exec_values_fidex)[:,3].astype(float))
                std_confid_fid_all = np.std(np.array(mean_exec_values_fidex)[:,4].astype(float))

            if is_fidexglo: # For FidexGlo
                mean_nb_rules_all = np.mean(np.array(mean_exec_values_fidexglo)[:,0].astype(float))
                mean_nb_cover_all = np.mean(np.array(mean_exec_values_fidexglo)[:,1].astype(float))
                mean_nb_antecedents_all = np.mean(np.array(mean_exec_values_fidexglo)[:,2].astype(float))
                mean_fidel_glo_all = np.mean(np.array(mean_exec_values_fidexglo)[:,3].astype(float))
                mean_rules_acc_glo_all = np.mean(np.array(mean_exec_values_fidexglo)[:,4].astype(float))
                mean_expl_glo_all = np.mean(np.array(mean_exec_values_fidexglo)[:,5].astype(float))
                mean_default_rate_all = np.mean(np.array(mean_exec_values_fidexglo)[:,6].astype(float))
                mean_nb_fidel_activations_all = np.mean(np.array(mean_exec_values_fidexglo)[:,7].astype(float))
                mean_wrong_activations_all = np.mean(np.array(mean_exec_values_fidexglo)[:,8].astype(float))
                mean_test_acc_glo_all = np.mean(np.array(mean_exec_values_fidexglo)[:,9].astype(float))
                mean_test_acc_when_rules_and_model_agree_all = np.mean(np.array(mean_exec_values_fidexglo)[:,10].astype(float))
                mean_test_acc_when_activated_rules_and_model_agree_all = np.mean(np.array(mean_exec_values_fidexglo)[:,11].astype(float))

                if with_roc:
                    mean_nb_true_positive_all = np.mean(np.array(mean_exec_values_fidexglo)[:,12].astype(float))
                    mean_nb_false_positive_all = np.mean(np.array(mean_exec_values_fidexglo)[:,13].astype(float))
                    mean_nb_true_negative_all = np.mean(np.array(mean_exec_values_fidexglo)[:,14].astype(float))
                    mean_nb_false_negative_all = np.mean(np.array(mean_exec_values_fidexglo)[:,15].astype(float))
                    false_positive_rate_exec_temp = [v for v in np.array(mean_exec_values_fidexglo)[:,16] if v != "N/A"]
                    false_negative_rate_exec_temp = [v for v in np.array(mean_exec_values_fidexglo)[:,17] if v != "N/A"]
                    precision_exec_temp = [v for v in np.array(mean_exec_values_fidexglo)[:,18] if v != "N/A"]
                    recall_exec_temp = [v for v in np.array(mean_exec_values_fidexglo)[:,19] if v != "N/A"]
                    if len(false_positive_rate_exec_temp) == 0:
                           mean_false_positive_rate_all = "N/A"
                           std_false_positive_rate_all = "N/A"
                    else:
                        mean_false_positive_rate_all = np.mean(np.array(false_positive_rate_exec_temp).astype(float))
                        std_false_positive_rate_all = np.std(np.array(false_positive_rate_exec_temp).astype(float))
                    if len(false_negative_rate_exec_temp) == 0:
                           mean_false_negative_rate_all = "N/A"
                           std_false_negative_rate_all = "N/A"
                    else:
                        mean_false_negative_rate_all = np.mean(np.array(false_negative_rate_exec_temp).astype(float))
                        std_false_negative_rate_all = np.mean(np.array(false_negative_rate_exec_temp).astype(float))
                    if len(precision_exec_temp) == 0:
                           mean_precision_all = "N/A"
                           std_precision_all = "N/A"
                    else:
                        mean_precision_all = np.mean(np.array(precision_exec_temp).astype(float))
                        std_precision_all = np.mean(np.array(precision_exec_temp).astype(float))
                    if len(recall_exec_temp) == 0:
                           mean_recall_all = "N/A"
                           std_recall_all = "N/A"
                    else:
                        mean_recall_all = np.mean(np.array(recall_exec_temp).astype(float))
                        std_recall_all = np.mean(np.array(recall_exec_temp).astype(float))

                    mean_nb_true_positive_rule_all = np.mean(np.array(mean_exec_values_fidexglo)[:,20].astype(float))
                    mean_nb_false_positive_rule_all = np.mean(np.array(mean_exec_values_fidexglo)[:,21].astype(float))
                    mean_nb_true_negative_rule_all = np.mean(np.array(mean_exec_values_fidexglo)[:,22].astype(float))
                    mean_nb_false_negative_rule_all = np.mean(np.array(mean_exec_values_fidexglo)[:,23].astype(float))
                    false_positive_rate_exec_rule_temp = [v for v in np.array(mean_exec_values_fidexglo)[:,24] if v != "N/A"]
                    false_negative_rate_exec_rule_temp = [v for v in np.array(mean_exec_values_fidexglo)[:,25] if v != "N/A"]
                    precision_exec_rule_temp = [v for v in np.array(mean_exec_values_fidexglo)[:,26] if v != "N/A"]
                    recall_exec_rule_temp = [v for v in np.array(mean_exec_values_fidexglo)[:,27] if v != "N/A"]
                    if len(false_positive_rate_exec_rule_temp) == 0:
                           mean_false_positive_rate_rule_all = "N/A"
                           std_false_positive_rate_rule_all = "N/A"
                    else:
                        mean_false_positive_rate_rule_all = np.mean(np.array(false_positive_rate_exec_rule_temp).astype(float))
                        std_false_positive_rate_rule_all = np.std(np.array(false_positive_rate_exec_rule_temp).astype(float))
                    if len(false_negative_rate_exec_rule_temp) == 0:
                           mean_false_negative_rate_rule_all = "N/A"
                           std_false_negative_rate_rule_all = "N/A"
                    else:
                        mean_false_negative_rate_rule_all = np.mean(np.array(false_negative_rate_exec_rule_temp).astype(float))
                        std_false_negative_rate_rule_all = np.mean(np.array(false_negative_rate_exec_rule_temp).astype(float))
                    if len(precision_exec_rule_temp) == 0:
                           mean_precision_rule_all = "N/A"
                           std_precision_rule_all = "N/A"
                    else:
                        mean_precision_rule_all = np.mean(np.array(precision_exec_rule_temp).astype(float))
                        std_precision_rule_all = np.mean(np.array(precision_exec_rule_temp).astype(float))
                    if len(recall_exec_rule_temp) == 0:
                           mean_recall_rule_all = "N/A"
                           std_recall_rule_all = "N/A"
                    else:
                        mean_recall_rule_all = np.mean(np.array(recall_exec_rule_temp).astype(float))
                        std_recall_rule_all = np.mean(np.array(recall_exec_rule_temp).astype(float))

                std_nb_rules_all = np.std(np.array(mean_exec_values_fidexglo)[:,0].astype(float))
                std_nb_cover_all = np.std(np.array(mean_exec_values_fidexglo)[:,1].astype(float))
                std_nb_antecedents_all = np.std(np.array(mean_exec_values_fidexglo)[:,2].astype(float))
                std_fidel_glo_all = np.std(np.array(mean_exec_values_fidexglo)[:,3].astype(float))
                std_rules_acc_glo_all = np.std(np.array(mean_exec_values_fidexglo)[:,4].astype(float))
                std_expl_glo_all = np.std(np.array(mean_exec_values_fidexglo)[:,5].astype(float))
                std_default_rate_all = np.std(np.array(mean_exec_values_fidexglo)[:,6].astype(float))
                std_nb_fidel_activations_all = np.std(np.array(mean_exec_values_fidexglo)[:,7].astype(float))
                std_wrong_activations_all = np.std(np.array(mean_exec_values_fidexglo)[:,8].astype(float))
                std_test_acc_glo_all = np.std(np.array(mean_exec_values_fidexglo)[:,9].astype(float))
                std_test_acc_when_rules_and_model_agree_all = np.std(np.array(mean_exec_values_fidexglo)[:,10].astype(float))
                std_test_acc_when_activated_rules_and_model_agree_all = np.std(np.array(mean_exec_values_fidexglo)[:,11].astype(float))
                if with_roc:
                    std_nb_true_positive_all = np.std(np.array(mean_exec_values_fidexglo)[:,12].astype(float))
                    std_nb_false_positive_all = np.std(np.array(mean_exec_values_fidexglo)[:,13].astype(float))
                    std_nb_true_negative_all = np.std(np.array(mean_exec_values_fidexglo)[:,14].astype(float))
                    std_nb_false_negative_all = np.std(np.array(mean_exec_values_fidexglo)[:,15].astype(float))

                    std_nb_true_positive_rule_all = np.std(np.array(mean_exec_values_fidexglo)[:,20].astype(float))
                    std_nb_false_positive_rule_all = np.std(np.array(mean_exec_values_fidexglo)[:,21].astype(float))
                    std_nb_true_negative_rule_all = np.std(np.array(mean_exec_values_fidexglo)[:,22].astype(float))
                    std_nb_false_negative_rule_all = np.std(np.array(mean_exec_values_fidexglo)[:,23].astype(float))

            # Show and save results
            try:
                with open(crossval_stats, "a") as outputStatsFile:
                    outputStatsFile.write(f"Results for {n} times {k}-Cross validation :\n\n")
                    print("\n---------------------------------------------------------")
                    print("---------------------------------------------------------\n")
                    print(f"Results for {n} times {k}-Cross validation :\n")
                    if with_roc:
                        outputStatsFile.write(f"The mean AUC score is: {formatted_mean_all_aucs}\n")
                        outputStatsFile.write(f"The standard deviation of the AUC score is: {formatted_std_all_aucs}\n\n")
                        print(f"The mean AUC score is: {formatted_mean_all_aucs}")
                        print(f"The standard deviation of the AUC score is: {formatted_std_all_aucs}\n")

                    if is_dimlprul:

                        formatted_mean_nb_rules_dimlp_all = formatting(mean_nb_rules_dimlp_all)
                        formatted_std_nb_rules_dimlp_all = formatting(std_nb_rules_dimlp_all)
                        formatted_mean_nb_cover_dimlp_all = formatting(mean_nb_cover_dimlp_all)
                        formatted_std_nb_cover_dimlp_all = formatting(std_nb_cover_dimlp_all)
                        formatted_mean_nb_antecedents_dimlp_all = formatting(mean_nb_antecedents_dimlp_all)
                        formatted_std_nb_antecedents_dimlp_all = formatting(std_nb_antecedents_dimlp_all)
                        formatted_mean_fidel_dimlp_all = formatting(mean_fidel_dimlp_all)
                        formatted_std_fidel_dimlp_all = formatting(std_fidel_dimlp_all)
                        formatted_mean_rules_acc_dimlp_all = formatting(mean_rules_acc_dimlp_all)
                        formatted_std_rules_acc_dimlp_all = formatting(std_rules_acc_dimlp_all)
                        formatted_mean_default_rate_dimlp_all = formatting(mean_default_rate_dimlp_all)
                        formatted_std_default_rate_dimlp_all = formatting(std_default_rate_dimlp_all)
                        formatted_mean_test_acc_dimlp_all = formatting(mean_test_acc_dimlp_all)
                        formatted_std_test_acc_dimlp_all = formatting(std_test_acc_dimlp_all)
                        formatted_mean_test_acc_when_rules_and_model_agree_dimlp_all = formatting(mean_test_acc_when_rules_and_model_agree_dimlp_all)
                        formatted_std_test_acc_when_rules_and_model_agree_dimlp_all = formatting(std_test_acc_when_rules_and_model_agree_dimlp_all)
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
                        outputStatsFile.write(f"The mean number of antecedents per rule is: {formatted_mean_nb_antecedents_dimlp_all}\n")
                        outputStatsFile.write(f"The standard deviation of the number of antecedents per rule is: {formatted_std_nb_antecedents_dimlp_all}\n")
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
                        print(f"The mean number of antecedents per rule is: {formatted_mean_nb_antecedents_dimlp_all}")
                        print(f"The standard deviation of the number of antecedents per rule is: {formatted_std_nb_antecedents_dimlp_all}")
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

                        formatted_mean_cov_size_fid_all = formatting(mean_cov_size_fid_all)
                        formatted_std_cov_size_fid_all = formatting(std_cov_size_fid_all)
                        formatted_mean_nb_ant_fid_all = formatting(mean_nb_ant_fid_all)
                        formatted_std_nb_ant_fid_all = formatting(std_nb_ant_fid_all)
                        formatted_mean_fidel_fid_all = formatting(mean_fidel_fid_all)
                        formatted_std_fidel_fid_all = formatting(std_fidel_fid_all)
                        formatted_mean_rules_acc_fid_all = formatting(mean_rules_acc_fid_all)
                        formatted_std_rules_acc_fid_all = formatting(std_rules_acc_fid_all)
                        formatted_mean_confid_fid_all = formatting(mean_confid_fid_all)
                        formatted_std_confid_fid_all = formatting(std_confid_fid_all)

                        outputStatsFile.write("Fidex :\n")
                        outputStatsFile.write(f"The mean covering size per rule is : {formatted_mean_cov_size_fid_all}\n")
                        outputStatsFile.write(f"The standard deviation of the covering size per rule is : {formatted_std_cov_size_fid_all}\n")
                        outputStatsFile.write(f"The mean number of antecedents per rule is : {formatted_mean_nb_ant_fid_all}\n")
                        outputStatsFile.write(f"The standard deviation of the number of antecedents per rule is : {formatted_std_nb_ant_fid_all}\n")
                        outputStatsFile.write(f"The mean rule fidelity rate is : {formatted_mean_fidel_fid_all}\n")
                        outputStatsFile.write(f"The standard deviation of the rule fidelity rate is : {formatted_std_fidel_fid_all}\n")
                        outputStatsFile.write(f"The mean rule accuracy is : {formatted_mean_rules_acc_fid_all}\n")
                        outputStatsFile.write(f"The standard deviation of the rule accuracy is : {formatted_std_rules_acc_fid_all}\n")
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
                        print(f"The mean rule confidence is : {formatted_mean_confid_fid_all}")
                        print(f"The standard deviation of the rule confidence is : {formatted_std_confid_fid_all}")
                        print("\n---------------------------------------------------------")
                        print("---------------------------------------------------------")

                    if is_fidex and is_fidexglo:
                        print("")
                        outputStatsFile.write("\n---------------------------------------------------------\n\n")

                    if is_fidexglo:

                        formatted_mean_nb_rules_all = formatting(mean_nb_rules_all)
                        formatted_std_nb_rules_all = formatting(std_nb_rules_all)
                        formatted_mean_nb_cover_all = formatting(mean_nb_cover_all)
                        formatted_std_nb_cover_all = formatting(std_nb_cover_all)
                        formatted_mean_nb_antecedents_all = formatting(mean_nb_antecedents_all)
                        formatted_std_nb_antecedents_all = formatting(std_nb_antecedents_all)
                        formatted_mean_fidel_glo_all = formatting(mean_fidel_glo_all)
                        formatted_std_fidel_glo_all = formatting(std_fidel_glo_all)
                        formatted_mean_rules_acc_glo_all = formatting(mean_rules_acc_glo_all)
                        formatted_std_rules_acc_glo_all = formatting(std_rules_acc_glo_all)
                        formatted_mean_expl_glo_all = formatting(mean_expl_glo_all)
                        formatted_std_expl_glo_all = formatting(std_expl_glo_all)
                        formatted_mean_default_rate_all = formatting(mean_default_rate_all)
                        formatted_std_default_rate_all = formatting(std_default_rate_all)
                        formatted_mean_nb_fidel_activations_all = formatting(mean_nb_fidel_activations_all)
                        formatted_std_nb_fidel_activations_all = formatting(std_nb_fidel_activations_all)
                        formatted_mean_wrong_activations_all = formatting(mean_wrong_activations_all)
                        formatted_std_wrong_activations_all = formatting(std_wrong_activations_all)
                        formatted_mean_test_acc_glo_all = formatting(mean_test_acc_glo_all)
                        formatted_std_test_acc_glo_all = formatting(std_test_acc_glo_all)
                        formatted_mean_test_acc_when_rules_and_model_agree_all = formatting(mean_test_acc_when_rules_and_model_agree_all)
                        formatted_std_test_acc_when_rules_and_model_agree_all = formatting(std_test_acc_when_rules_and_model_agree_all)
                        formatted_mean_test_acc_when_activated_rules_and_model_agree_all = formatting(mean_test_acc_when_activated_rules_and_model_agree_all)
                        formatted_std_test_acc_when_activated_rules_and_model_agree_all = formatting(std_test_acc_when_activated_rules_and_model_agree_all)

                        if with_roc:

                            formatted_mean_nb_true_positive_all = formatting(mean_nb_true_positive_all)
                            formatted_std_nb_true_positive_all = formatting(std_nb_true_positive_all)
                            formatted_mean_nb_false_positive_all = formatting(mean_nb_false_positive_all)
                            formatted_std_nb_false_positive_all = formatting(std_nb_false_positive_all)
                            formatted_mean_nb_true_negative_all = formatting(mean_nb_true_negative_all)
                            formatted_std_nb_true_negative_all = formatting(std_nb_true_negative_all)
                            formatted_mean_nb_false_negative_all = formatting(mean_nb_false_negative_all)
                            formatted_std_nb_false_negative_all = formatting(std_nb_false_negative_all)
                            formatted_mean_false_positive_rate_all = formatting(mean_false_positive_rate_all)
                            formatted_std_false_positive_rate_all = formatting(std_false_positive_rate_all)
                            formatted_mean_false_negative_rate_all = formatting(mean_false_negative_rate_all)
                            formatted_std_false_negative_rate_all = formatting(std_false_negative_rate_all)
                            formatted_mean_precision_all = formatting(mean_precision_all)
                            formatted_std_precision_all = formatting(std_precision_all)
                            formatted_mean_recall_all = formatting(mean_recall_all)
                            formatted_std_recall_all = formatting(std_recall_all)
                            formatted_mean_nb_true_positive_rule_all = formatting(mean_nb_true_positive_rule_all)
                            formatted_std_nb_true_positive_rule_all = formatting(std_nb_true_positive_rule_all)
                            formatted_mean_nb_false_positive_rule_all = formatting(mean_nb_false_positive_rule_all)
                            formatted_std_nb_false_positive_rule_all = formatting(std_nb_false_positive_rule_all)
                            formatted_mean_nb_true_negative_rule_all = formatting(mean_nb_true_negative_rule_all)
                            formatted_std_nb_true_negative_rule_all = formatting(std_nb_true_negative_rule_all)
                            formatted_mean_nb_false_negative_rule_all = formatting(mean_nb_false_negative_rule_all)
                            formatted_std_nb_false_negative_rule_all = formatting(std_nb_false_negative_rule_all)
                            formatted_mean_false_positive_rate_rule_all = formatting(mean_false_positive_rate_rule_all)
                            formatted_std_false_positive_rate_rule_all = formatting(std_false_positive_rate_rule_all)
                            formatted_mean_false_negative_rate_rule_all = formatting(mean_false_negative_rate_rule_all)
                            formatted_std_false_negative_rate_rule_all = formatting(std_false_negative_rate_rule_all)
                            formatted_mean_precision_rule_all = formatting(mean_precision_rule_all)
                            formatted_std_precision_rule_all = formatting(std_precision_rule_all)
                            formatted_mean_recall_rule_all = formatting(mean_recall_rule_all)
                            formatted_std_recall_rule_all = formatting(std_recall_rule_all)

                        outputStatsFile.write("FidexGlo :\n")
                        outputStatsFile.write(f"The mean number of rules is : {formatted_mean_nb_rules_all}\n")
                        outputStatsFile.write(f"The standard deviation of the number of rules is : {formatted_std_nb_rules_all}\n")
                        outputStatsFile.write(f"The mean sample covering number per rule is : {formatted_mean_nb_cover_all}\n")
                        outputStatsFile.write(f"The standard deviation of the sample covering number per rule is : {formatted_std_nb_cover_all}\n")
                        outputStatsFile.write(f"The mean number of antecedents per rule is : {formatted_mean_nb_antecedents_all}\n")
                        outputStatsFile.write(f"The standard deviation of the number of antecedents per rule is : {formatted_std_nb_antecedents_all}\n")
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
                        if with_roc:
                            outputStatsFile.write(f"\nWith positive class {positive_class_index} :\n\n")
                            outputStatsFile.write("Computation with model decision :\n\n")
                            outputStatsFile.write(f"The mean number of true positive test samples is : {formatted_mean_nb_true_positive_all}\n")
                            outputStatsFile.write(f"The standard deviation number of true positive test samples is : {formatted_std_nb_true_positive_all}\n")
                            outputStatsFile.write(f"The mean number of false positive test samples is : {formatted_mean_nb_false_positive_all}\n")
                            outputStatsFile.write(f"The standard deviation number of false positive test samples is : {formatted_std_nb_false_positive_all}\n")
                            outputStatsFile.write(f"The mean number of true negative test samples is : {formatted_mean_nb_true_negative_all}\n")
                            outputStatsFile.write(f"The standard deviation number of true negative test samples is : {formatted_std_nb_true_negative_all}\n")
                            outputStatsFile.write(f"The mean number of false negative test samples is : {formatted_mean_nb_false_negative_all}\n")
                            outputStatsFile.write(f"The standard deviation number of false negative test samples is : {formatted_std_nb_false_negative_all}\n")
                            outputStatsFile.write(f"The mean false positive rate is : {formatted_mean_false_positive_rate_all}\n")
                            outputStatsFile.write(f"The standard deviation false positive rate is : {formatted_std_false_positive_rate_all}\n")
                            outputStatsFile.write(f"The mean false negative rate is : {formatted_mean_false_negative_rate_all}\n")
                            outputStatsFile.write(f"The standard deviation false negative rate is : {formatted_std_false_negative_rate_all}\n")
                            outputStatsFile.write(f"The mean precision is : {formatted_mean_precision_all}\n")
                            outputStatsFile.write(f"The standard deviation precision is : {formatted_std_precision_all}\n")
                            outputStatsFile.write(f"The mean recall is : {formatted_mean_recall_all}\n")
                            outputStatsFile.write(f"The standard deviation recall is : {formatted_std_recall_all}\n")
                            outputStatsFile.write("\nComputation with rules decision :\n\n")
                            outputStatsFile.write(f"The mean number of true positive test samples is : {formatted_mean_nb_true_positive_rule_all}\n")
                            outputStatsFile.write(f"The standard deviation number of true positive test samples is : {formatted_std_nb_true_positive_rule_all}\n")
                            outputStatsFile.write(f"The mean number of false positive test samples is : {formatted_mean_nb_false_positive_rule_all}\n")
                            outputStatsFile.write(f"The standard deviation number of false positive test samples is : {formatted_std_nb_false_positive_rule_all}\n")
                            outputStatsFile.write(f"The mean number of true negative test samples is : {formatted_mean_nb_true_negative_rule_all}\n")
                            outputStatsFile.write(f"The standard deviation number of true negative test samples is : {formatted_std_nb_true_negative_rule_all}\n")
                            outputStatsFile.write(f"The mean number of false negative test samples is : {formatted_mean_nb_false_negative_rule_all}\n")
                            outputStatsFile.write(f"The standard deviation number of false negative test samples is : {formatted_std_nb_false_negative_rule_all}\n")
                            outputStatsFile.write(f"The mean false positive rate is : {formatted_mean_false_positive_rate_rule_all}\n")
                            outputStatsFile.write(f"The standard deviation false positive rate is : {formatted_std_false_positive_rate_rule_all}\n")
                            outputStatsFile.write(f"The mean false negative rate is : {formatted_mean_false_negative_rate_rule_all}\n")
                            outputStatsFile.write(f"The standard deviation false negative rate is : {formatted_std_false_negative_rate_rule_all}\n")
                            outputStatsFile.write(f"The mean precision is : {formatted_mean_precision_rule_all}\n")
                            outputStatsFile.write(f"The standard deviation precision is : {formatted_std_precision_rule_all}\n")
                            outputStatsFile.write(f"The mean recall is : {formatted_mean_recall_rule_all}\n")
                            outputStatsFile.write(f"The standard deviation recall is : {formatted_std_recall_rule_all}\n")
                        print("FidexGlo :")
                        print(f"The mean number of rules is : {formatted_mean_nb_rules_all}")
                        print(f"The standard deviation of the number of rules is : {formatted_std_nb_rules_all}")
                        print(f"The mean sample covering number per rule is : {formatted_mean_nb_cover_all}")
                        print(f"The standard deviation of the sample covering number per rule is : {formatted_std_nb_cover_all}")
                        print(f"The mean number of antecedents per rule is : {formatted_mean_nb_antecedents_all}")
                        print(f"The standard deviation of the number of antecedents per rule is : {formatted_std_nb_antecedents_all}")
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
                        if with_roc:
                            print(f"\nWith positive class {positive_class_index} :\n")
                            print("Computation with model decision :\n")
                            print(f"The mean number of true positive test samples is : {formatted_mean_nb_true_positive_all}")
                            print(f"The standard deviation number of true positive test samples is : {formatted_std_nb_true_positive_all}")
                            print(f"The mean number of false positive test samples is : {formatted_mean_nb_false_positive_all}")
                            print(f"The standard deviation number of false positive test samples is : {formatted_std_nb_false_positive_all}")
                            print(f"The mean number of true negative test samples is : {formatted_mean_nb_true_negative_all}")
                            print(f"The standard deviation number of true negative test samples is : {formatted_std_nb_true_negative_all}")
                            print(f"The mean number of false negative test samples is : {formatted_mean_nb_false_negative_all}")
                            print(f"The standard deviation number of false negative test samples is : {formatted_std_nb_false_negative_all}")
                            print(f"The mean false positive rate is : {formatted_mean_false_positive_rate_all}")
                            print(f"The standard deviation false positive rate is : {formatted_std_false_positive_rate_all}")
                            print(f"The mean false negative rate is : {formatted_mean_false_negative_rate_all}")
                            print(f"The standard deviation false negative rate is : {formatted_std_false_negative_rate_all}")
                            print(f"The mean precision is : {formatted_mean_precision_all}")
                            print(f"The standard deviation precision is : {formatted_std_precision_all}")
                            print(f"The mean recall is : {formatted_mean_recall_all}")
                            print(f"The standard deviation recall is : {formatted_std_recall_all}")
                            print("\nComputation with rules decision :\n")
                            print(f"The mean number of true positive test samples is : {formatted_mean_nb_true_positive_rule_all}")
                            print(f"The standard deviation number of true positive test samples is : {formatted_std_nb_true_positive_rule_all}")
                            print(f"The mean number of false positive test samples is : {formatted_mean_nb_false_positive_rule_all}")
                            print(f"The standard deviation number of false positive test samples is : {formatted_std_nb_false_positive_rule_all}")
                            print(f"The mean number of true negative test samples is : {formatted_mean_nb_true_negative_rule_all}")
                            print(f"The standard deviation number of true negative test samples is : {formatted_std_nb_true_negative_rule_all}")
                            print(f"The mean number of false negative test samples is : {formatted_mean_nb_false_negative_rule_all}")
                            print(f"The standard deviation number of false negative test samples is : {formatted_std_nb_false_negative_rule_all}")
                            print(f"The mean false positive rate is : {formatted_mean_false_positive_rate_rule_all}")
                            print(f"The standard deviation false positive rate is : {formatted_std_false_positive_rate_rule_all}")
                            print(f"The mean false negative rate is : {formatted_mean_false_negative_rate_rule_all}")
                            print(f"The standard deviation false negative rate is : {formatted_std_false_negative_rate_rule_all}")
                            print(f"The mean precision is : {formatted_mean_precision_rule_all}")
                            print(f"The standard deviation precision is : {formatted_std_precision_rule_all}")
                            print(f"The mean recall is : {formatted_mean_recall_rule_all}")
                            print(f"The standard deviation recall is : {formatted_std_recall_rule_all}")
                        print("\n---------------------------------------------------------")
                        print("---------------------------------------------------------")



                    outputStatsFile.close()
            except (FileNotFoundError):
                raise ValueError(f"Error : File for stats extraction ({crossval_stats}) not found.")
            except (IOError):
                raise ValueError(f"Error : Couldn't open stats extraction file {crossval_stats}.")


            # Delete temporary files
            delete_file(crossval_folder + separator + "consoleTemp.txt")
            delete_file(crossval_folder + separator + "tempTrain.txt")
            delete_file(crossval_folder + separator + "tempTest.txt")
            delete_file(crossval_folder + separator + "tempTarTrain.txt")
            delete_file(crossval_folder + separator + "tempTarTest.txt")
            if train_method == "dimlp":
                delete_file(crossval_folder + separator + "tempValid.txt")
                delete_file(crossval_folder + separator + "tempTarValid.txt")

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

if __name__ == "__main__":
    cmdline_args = " ".join(sys.argv[1:])
    mlpTrn(cmdline_args)
