# Not working with SVM because process is different to get Roc curve

from .trnFun import get_data_class, get_data_pred, compute_roc, check_int, check_strictly_positive, validate_string_param
import numpy as np
import matplotlib.pyplot as plt

def computeRocCurve(*args, **kwargs):
    try:
        if not kwargs:
            print("---------------------------------------------------------------------")
            print("Please specify arguments using named parameters.")
            print("Warning! The files are localised with respect to root folder dimlpfidex.")
            print("----------------------------")
            print("Obligatory parameters :")
            print("test_class_file : test class file")
            print("test_pred_file : test prediction file")
            print("nb_classes : number of classes")
            print("positive_class_index : index of positive class (0 for first one)")
            print("----------------------------")
            print("Optional parameters :")
            print("estimator : name of estimator")
            print("root_folder : Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder.")
            print("output_roc : output ROC curve (roc_curve.png by default)")
            print("stats_file : stats file to save AUC score")

        else:
            # Get parameters
            root_folder = kwargs.get('root_folder')
            test_class_file = kwargs.get('test_class_file')
            test_pred_file = kwargs.get('test_pred_file')
            nb_classes = kwargs.get('nb_classes')
            positive_class_index = kwargs.get('positive_class_index')
            estimator = kwargs.get('estimator')
            output_roc = kwargs.get('output_roc')
            stats_file = kwargs.get('stats_file')

            # Check parameters
            valid_args = ['test_class_file', 'test_pred_file', 'nb_classes', 'positive_class_index', 'estimator', 'root_folder', 'output_roc', 'stats_file']

            # Check if wrong parameters are given
            for arg_key in kwargs.keys():
                if arg_key not in valid_args:
                    raise ValueError(f"Invalid argument : {arg_key}.")

            root_folder = validate_string_param(root_folder, "root_folder", allow_none=True)

            is_test_class_list = False
            if test_class_file is None :
                raise ValueError('Error : test class file missing, add it with option test_class_file="your_test_class_file" or with a list.')
            elif not isinstance(test_class_file, str) and not isinstance(test_class_file, list):
                raise ValueError('Error : parameter test_class_file has to be a name contained in quotation marks "" or a list.')
            elif isinstance(test_class_file, list):
                is_test_class_list = True

            is_test_pred_list = False
            if test_pred_file is None:
                raise ValueError('Error : prediction data file missing, add it with option test_pred_file="your_prediction_data_file" or with a list.')
            elif not isinstance(test_pred_file, str) and not isinstance(test_pred_file, list):
                raise ValueError('Error : parameter test_pred_file has to be a name contained in quotation marks "" or a list.')
            elif isinstance(test_pred_file, list):
                is_test_pred_list = True

            if nb_classes is None:
                raise ValueError('Error : number of classes missing, add it with option nb_classes="your_number_of_classes".')
            elif not check_strictly_positive(nb_classes) or not check_int(nb_classes):
                raise ValueError('Error : parameter nb_classes has to be a strictly positive integer.')

            stats_file = validate_string_param(stats_file, "stats_file", allow_none=True)

            output_roc = validate_string_param(output_roc, "output_roc", default="roc_curve.png")

            if (root_folder is not None):
                output_roc = root_folder + "/" + output_roc
                if stats_file is not None:
                    stats_file = root_folder + "/" + stats_file

            # Get data
            if is_test_class_list:
                test_class = test_class_file
            elif root_folder is not None:
                test_class = get_data_class(root_folder + "/" + test_class_file, nb_classes)
            else:
                test_class = get_data_class(test_class_file, nb_classes)
            if is_test_pred_list:
                test_pred = test_pred_file
            elif root_folder is not None:
                test_pred = get_data_pred(root_folder + "/" + test_pred_file, nb_classes)
            else:
                test_pred = get_data_pred(test_pred_file, nb_classes)

            if len(test_class) != len(test_pred):
                raise ValueError('Error, there is not the same amount of test predictions and test class.')

            if positive_class_index is None:
                raise ValueError('Error : positive class index missing, add it with option positive_class_index="your_positive_class_index".')
            elif not isinstance(positive_class_index, int) or positive_class_index < 0 or positive_class_index >= nb_classes:
                raise ValueError(f'Error : parameter positive_class_index has to be a positive integer smaller than {nb_classes}.')
            test_class_roc = [int(cl == positive_class_index) for cl in test_class]
            test_pred = [p[positive_class_index] for p in test_pred]
            fpr, tpr, auc_score = compute_roc(estimator, output_roc, test_class_roc, test_pred)

            # Save AUC result in stats file
            if stats_file is not None:
                with open(stats_file, 'r') as file:
                    lines = file.readlines()
                    file.close()
                last_line = lines[-1]
                if last_line.startswith("AUC score on testing set"):
                    lines[-1] = f"AUC score on testing set : {auc_score}" # Replace the line
                else:
                    lines.append(f"\nAUC score on testing set : {auc_score}")# Add new line

                with open(stats_file, "w") as file:
                    file.writelines(lines)
                    file.close()

            # Interpolation to get 1000 points (necessary for crossValidation)
            fpr_interp = np.linspace(0, 1, 1000)
            tpr_interp = np.interp(fpr_interp, fpr, tpr)
            tpr_interp[0] = 0
            return [fpr_interp, tpr_interp, auc_score]




    except ValueError as error:
        print(error)
        return -1
