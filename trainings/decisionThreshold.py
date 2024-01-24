# It will work with SVM only if the roc curve results are given, because the process is different to get Roc curve and we can't compute it here for SVM.

from .trnFun import get_data_class, get_data_pred, check_bool, check_positive, output_data, check_int, check_strictly_positive, validate_string_param
import numpy as np
import matplotlib.pyplot as plt

def decisionThreshold(*args, **kwargs):
    try:
        if not kwargs:
            print("---------------------------------------------------------------------")
            print("Please specify arguments using named parameters.")
            print("Warning! The files are localised with respect to root folder dimlpfidex.")
            print("----------------------------")
            print("Obligatory parameters :")
            print("test_class_file : test class file")
            print("test_pred_file : test prediction file, can also be a list")
            print("train_pred_file : train prediction file, can also be a list")
            print("nb_classes : number of classes")
            print("positive_class_index : index of positive class (0 for first one)")
            print("with_roc_computation : Boolean, if we need to compute ROC curve or not")
            print("----------------------------")
            print("Optional parameters :")
            print("root_folder : Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder.")
            print("test_class_threshold_file : new test class file with the new threshold (test_class_threshold.txt by default)")
            print("train_class_threshold_file : new train class file with the new threshold (train_class_threshold.txt by default)")
            print("----------------------------")
            print("obligatory parameters if the roc curve has already been computed :")
            print("fpr : false positive rate list")
            print("tpr : true positive rate list")
            print("auc_score : AUC score")
            print("----------------------------")
            print("Obligatory parameters if the computation of roc curve is needed :")

            print("----------------------------")
            print("Optional parameters if the computation of roc curve is needed :")
            print("estimator : name of estimator")
            print("output_roc : output ROC curve without extension(roc_curve.png by default)")
            print("stats_file : stats file to save AUC score")

        else:
            # Get parameters
            root_folder = kwargs.get('root_folder')
            test_class_file = kwargs.get('test_class_file')
            test_pred_file = kwargs.get('test_pred_file')
            train_pred_file = kwargs.get('train_pred_file')
            nb_classes = kwargs.get('nb_classes')
            with_roc_computation = kwargs.get('with_roc_computation')
            train_class_threshold_file = kwargs.get('train_class_threshold_file')
            test_class_threshold_file = kwargs.get('test_class_threshold_file')
            fpr = kwargs.get('fpr')
            tpr = kwargs.get('tpr')
            auc_score = kwargs.get('auc_score')
            positive_class_index_var = kwargs.get('positive_class_index')
            estimator_var = kwargs.get('estimator')
            output_roc_file = kwargs.get('output_roc')
            stats_file = kwargs.get('stats_file')

            # Check parameters
            valid_args = ['test_class_file', 'test_pred_file', 'train_pred_file', 'nb_classes', 'with_roc_computation', 'root_folder', 'train_class_threshold_file', 'test_class_threshold_file',
                          'fpr', 'tpr', 'auc_score', 'positive_class_index', 'estimator', 'output_roc', 'stats_file']

            # Check if wrong parameters are given
            for arg_key in kwargs.keys():
                if arg_key not in valid_args:
                    raise ValueError(f"Invalid argument : {arg_key}.")

            root_folder = validate_string_param(root_folder, "root_folder", allow_none=True)

            test_class_file = validate_string_param(test_class_file, "test_class_file")

            is_test_pred_list = False
            if test_pred_file is None:
                raise ValueError('Error : test prediction data file missing, add it with option test_pred_file="your_test_prediction_data_file" or with a list.')
            elif not isinstance(test_pred_file, str) and not isinstance(test_pred_file, list):
                raise ValueError('Error : parameter test_pred has to be a name contained in quotation marks "" or a list.')
            elif isinstance(test_pred_file, list):
                is_test_pred_list = True

            is_train_pred_list = False
            if train_pred_file is None:
                raise ValueError('Error : train prediction data file missing, add it with option train_pred_file="your_train_prediction_data_file" or with a list.')
            elif not isinstance(train_pred_file, str) and not isinstance(train_pred_file, list):
                raise ValueError('Error : parameter train_pred has to be a name contained in quotation marks "" or a list.')
            elif isinstance(train_pred_file, list):
                is_train_pred_list = True

            train_class_threshold_file = validate_string_param(train_class_threshold_file, "train_class_threshold_file", default="train_class_threshold.txt")
            test_class_threshold_file = validate_string_param(test_class_threshold_file, "test_class_threshold_file", default="test_class_threshold.txt")


            if with_roc_computation is None:
                raise ValueError('Error : parameter with_roc_computation is missing, add it with with_roc_computation=True ou with_roc_computation=False.')
            elif not check_bool(with_roc_computation):
                raise ValueError('Error, parameter with_roc_computation is not a boolean.')

            # Get data
            if is_test_pred_list:
                test_pred = test_pred_file
            elif root_folder is not None:
                    test_pred = get_data_pred(root_folder + "/" + test_pred_file, nb_classes)
            else:
                test_pred = get_data_pred(test_pred_file, nb_classes)
            if is_train_pred_list:
                train_pred = train_pred_file
            elif root_folder is not None:
                train_pred = get_data_pred(root_folder + "/" + train_pred_file, nb_classes)
            else:
                train_pred = get_data_pred(train_pred_file, nb_classes)
            if root_folder is not None:
                test_class = get_data_class(root_folder + "/" + test_class_file, nb_classes)
            else:
                test_class = get_data_class(test_class_file, nb_classes)

            if len(test_class) != len(test_pred):
                raise ValueError('Error, there is not the same amount of test predictions and test class.')

            if nb_classes is None:
                raise ValueError('Error : number of classes missing, add it with option nb_classes="your_number_of_classes".')
            elif not check_strictly_positive(nb_classes) or not check_int(nb_classes):
                raise ValueError('Error : parameter nb_classes has to be a strictly positive integer.')

            if root_folder is not None:
                train_class_threshold_file = root_folder + "/" + train_class_threshold_file
                test_class_threshold_file = root_folder + "/" + test_class_threshold_file

            if positive_class_index_var is None:
                raise ValueError('Error : parameter positive_class_index is missing, add it with positive_class_index=your_positive_class_index.')
            elif not isinstance(positive_class_index_var, int) or positive_class_index_var < 0 or positive_class_index_var >= nb_classes:
                raise ValueError(f'Error : parameter positive_class_index has to be a positive integer smaller than {nb_classes}.')

            # Get ROC result

            if with_roc_computation: # If we need to compute roc curve
                from trainings.computeRocCurve import computeRocCurve
                res = computeRocCurve(test_class_file = test_class, test_pred_file = test_pred, positive_class_index = positive_class_index_var, output_roc=output_roc_file,
                                estimator = estimator_var, stats_file=stats_file, root_folder = root_folder, nb_classes = nb_classes)
                if (res == -1):
                    raise ValueError('Error during computation of ROC curve.')
                else:
                    fpr = res[0]
                    tpr = res[1]
                    auc_score = res[2]
            else: # If we don't need to compute roc curve
                if tpr is None:
                    raise ValueError('Error : parameter tpr is missing, add it with tpr=your_tpr_list.')
                elif not isinstance(tpr, list):
                    raise ValueError('Error, parameter tpr is not a list.')
                elif any(isinstance(item, list) for item in tpr):
                    raise ValueError('Error, parameter tpr is not a list of dimension 1.')
                elif any(not (isinstance(item, float) and 0 <= item <= 1) for item in tpr):
                    raise ValueError('Error, parameter tpr is not a list of floats between 0 and 1.')
                if fpr is None:
                    raise ValueError('Error : parameter fpr is missing, add it with fpr=your_fpr_list.')
                elif not isinstance(fpr, list):
                    raise ValueError('Error, parameter fpr is not a list.')
                elif any(isinstance(item, list) for item in fpr):
                    raise ValueError('Error, parameter fpr is not a list of dimension 1.')
                elif any(not (isinstance(item, float) and 0 <= item <= 1) for item in fpr):
                    raise ValueError('Error, parameter fpr is not a list of floats between 0 and 1.')
                if auc_score is None:
                    raise ValueError('Error : parameter auc_score is missing, add it with auc_score=your_auc_score.')
                elif not check_positive(auc_score):
                    raise ValueError('Error, parameter auc_score is not a positive float.')

            # Get threshold
            threshold = 0.3
            """
            # Modification of train_class and test_class with the new Threshold
            new_train_class = []
            new_test_class = []
            # We choose the positive class if it is greater than the threshold, else we choose the maximum
            for pred in train_pred:
                if pred[positive_class_index_var] > threshold:
                    new_train_class.append([0 if j != positive_class_index_var else 1 for j in range(nb_classes)])
                else:
                    new_train_class.append([0 if j != pred.index(max(pred)) else 1 for j in range(nb_classes)])

            for pred in test_pred:
                if pred[positive_class_index_var] > threshold:
                    new_test_class.append([0 if j != positive_class_index_var else 1 for j in range(nb_classes)])
                else:
                    new_test_class.append([0 if j != pred.index(max(pred)) else 1 for j in range(nb_classes)])

            #Output new class files
            output_data(new_train_class, train_class_threshold_file)
            output_data(new_test_class, test_class_threshold_file)
            """
            return threshold




    except ValueError as error:
        print(error)
        return -1
