# Works with SVM only if roc curve results are given because process is different to get Roc curve

from .trnFun import get_data, check_bool, check_positive, output_data
import numpy as np
import matplotlib.pyplot as plt

def decisionThreshold(*args, **kwargs):
    try:
        if args or not kwargs:
            print("---------------------------------------------------------------------")
            print("Please specify arguments using named parameters.")
            print("Warning! The files are localised with respect to root folder dimlpfidex.")
            print("----------------------------")
            print("Obligatory parameters :")
            print("test_class : test class file")
            print("test_pred : test prediction file, can also be a list")
            print("train_pred : test prediction file, can also be a list")
            print("positive_index : index of positive class (0 for first one)")
            print("with_roc_computation : Boolean, if we need to compute ROC curve or not")
            print("----------------------------")
            print("Optional parameters :")
            print("save_folder : Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder.")
            print("test_class_threshold : new test class file with the new threshold (test_class_threshold.txt by default)")
            print("train_class_threshold : new train class file with the new threshold (train_class_threshold.txt by default)")
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
            save_folder = kwargs.get('save_folder')
            test_class_file = kwargs.get('test_class')
            test_pred_file = kwargs.get('test_pred')
            train_pred_file = kwargs.get('train_pred')
            with_roc_computation = kwargs.get('with_roc_computation')
            train_class_threshold_file = kwargs.get('train_class_threshold')
            test_class_threshold_file = kwargs.get('test_class_threshold')
            fpr = kwargs.get('fpr')
            tpr = kwargs.get('tpr')
            auc_score = kwargs.get('auc_score')
            positive_index_var = kwargs.get('positive_index')
            estimator_var = kwargs.get('estimator')
            output_roc_file = kwargs.get('output_roc')
            stats_file = kwargs.get('stats_file')

            # Check parameters
            valid_args = ['test_class', 'test_pred', 'train_pred', 'with_roc_computation', 'save_folder', 'train_class_threshold', 'test_class_threshold',
                          'fpr', 'tpr', 'auc_score', 'positive_index', 'estimator', 'output_roc', 'stats_file']

            # Check if wrong parameters are given
            for arg_key in kwargs.keys():
                if arg_key not in valid_args:
                    raise ValueError(f"Invalid argument : {arg_key}")

            if (save_folder is not None and (not isinstance(save_folder, str))):
                raise ValueError('Error : parameter save_folder has to be a name contained in quotation marks "".')

            if test_class_file is None :
                raise ValueError('Error : test class file missing, add it with option test_class="your_test_class_file"')
            elif not isinstance(test_class_file, str):
                raise ValueError('Error : parameter test_class has to be a name contained in quotation marks "".')

            is_test_pred_list = False
            if test_pred_file is None:
                raise ValueError('Error : test prediction data file missing, add it with option test_pred="your_test_prediction_data_file" or with a list')
            elif not isinstance(test_pred_file, str) and not isinstance(test_pred_file, list):
                raise ValueError('Error : parameter test_pred has to be a name contained in quotation marks "" or a list.')
            elif isinstance(test_pred_file, list):
                is_test_pred_list = True

            is_train_pred_list = False
            if train_pred_file is None:
                raise ValueError('Error : train prediction data file missing, add it with option train_pred="your_train_prediction_data_file" or with a list')
            elif not isinstance(train_pred_file, str) and not isinstance(train_pred_file, list):
                raise ValueError('Error : parameter train_pred has to be a name contained in quotation marks "" or a list.')
            elif isinstance(train_pred_file, list):
                is_train_pred_list = True

            if train_class_threshold_file is None:
                train_class_threshold_file = "train_class_threshold.txt"
            elif not isinstance(train_class_threshold_file, str):
                raise ValueError('Error : parameter train_class_threshold_file has to be a name contained in quotation marks "".')

            if test_class_threshold_file is None:
                test_class_threshold_file = "test_class_threshold.txt"
            elif not isinstance(test_class_threshold_file, str):
                raise ValueError('Error : parameter test_class_threshold_file has to be a name contained in quotation marks "".')

            if with_roc_computation is None:
                raise ValueError('Error : parameter with_roc_computation is missing, add it with with_roc_computation=True ou with_roc_computation=False')
            elif not check_bool(with_roc_computation):
                raise ValueError('Error, parameter with_roc_computation is not a boolean')

            # Get data
            if is_test_pred_list:
                test_pred = test_pred_file
            elif save_folder is not None:
                    test_pred = get_data(save_folder + "/" + test_pred_file)
            else:
                test_pred = get_data(test_pred_file)
            if is_train_pred_list:
                train_pred = train_pred_file
            elif save_folder is not None:
                train_pred = get_data(save_folder + "/" + train_pred_file)
            else:
                get_data(train_pred_file)
            if save_folder is not None:
                test_class = get_data(save_folder + "/" + test_class_file)
            else:
                test_class = get_data(test_class_file)

            if save_folder is not None:
                train_class_threshold_file = save_folder + "/" + train_class_threshold_file
                test_class_threshold_file = save_folder + "/" + test_class_threshold_file

            nb_classes = len(test_pred[0])

            if positive_index_var is None:
                raise ValueError('Error : parameter positive_index is missing, add it with positive_index=your_positive_class_index')
            elif not isinstance(positive_index_var, int) or positive_index_var < 0 or positive_index_var >= nb_classes:
                raise ValueError(f'Error : parameter positive_index has to be a positive integer smaller than {nb_classes}.')


            # Get ROC result

            if with_roc_computation: # If we need to compute roc curve
                from trainings.computeRocCurve import computeRocCurve
                res = computeRocCurve(test_class = test_class, test_pred = test_pred, positive_index = positive_index_var, output_roc=output_roc_file,
                                estimator = estimator_var, stats_file=stats_file, save_folder = save_folder)
                if (res == -1):
                    raise ValueError('Error during computation of ROC curve')
                else:
                    fpr = res[0]
                    tpr = res[1]
                    auc_score = res[2]
            else: # If we don't need to compute roc curve
                if tpr is None:
                    raise ValueError('Error : parameter tpr is missing, add it with tpr=your_tpr_list')
                elif not isinstance(tpr, list):
                    raise ValueError('Error, parameter tpr is not a list')
                elif any(isinstance(item, list) for item in tpr):
                    raise ValueError('Error, parameter tpr is not a list of dimension 1')
                elif any(not (isinstance(item, float) and 0 <= item <= 1) for item in tpr):
                    raise ValueError('Error, parameter tpr is not a list of floats between 0 and 1')
                if fpr is None:
                    raise ValueError('Error : parameter fpr is missing, add it with fpr=your_fpr_list')
                elif not isinstance(fpr, list):
                    raise ValueError('Error, parameter fpr is not a list')
                elif any(isinstance(item, list) for item in fpr):
                    raise ValueError('Error, parameter fpr is not a list of dimension 1')
                elif any(not (isinstance(item, float) and 0 <= item <= 1) for item in fpr):
                    raise ValueError('Error, parameter fpr is not a list of floats between 0 and 1')
                if auc_score is None:
                    raise ValueError('Error : parameter auc_score is missing, add it with auc_score=your_auc_score')
                elif not check_positive(auc_score):
                    raise ValueError('Error, parameter auc_score is not a positive float')

            # Get threshold
            threshold = 0.3
            """
            # Modification of train_class and test_class with the new Threshold
            new_train_class = []
            new_test_class = []
            # We choose the positive class if it is greater than the threshold, else we choose the maximum
            for pred in train_pred:
                if pred[positive_index_var] > threshold:
                    new_train_class.append([0 if j != positive_index_var else 1 for j in range(nb_classes)])
                else:
                    new_train_class.append([0 if j != pred.index(max(pred)) else 1 for j in range(nb_classes)])

            for pred in test_pred:
                if pred[positive_index_var] > threshold:
                    new_test_class.append([0 if j != positive_index_var else 1 for j in range(nb_classes)])
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