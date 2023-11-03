# Not working with SVM because process is different to get Roc curve

from .trnFun import get_data, compute_roc
import numpy as np
import matplotlib.pyplot as plt

def computeRocCurve(*args, **kwargs):
    try:
        if args or not kwargs:
            print("---------------------------------------------------------------------")
            print("Please specify arguments using named parameters.")
            print("Warning! The files are localised with respect to root folder dimlpfidex.")
            print("----------------------------")
            print("Obligatory parameters :")
            print("test_class : test class file")
            print("test_pred : test prediction file")
            print("positive_index : index of positive class (0 for first one)")
            print("----------------------------")
            print("Optional parameters :")
            print("estimator : name of estimator")
            print("save_folder : Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder.")
            print("output_roc : output ROC curve without extension(roc_curve.png by default)")
            print("stats_file : stats file to save AUC score")

        else:
            # Get parameters
            save_folder = kwargs.get('save_folder')
            test_class_file = kwargs.get('test_class')
            test_pred_file = kwargs.get('test_pred')
            positive_index = kwargs.get('positive_index')
            estimator = kwargs.get('estimator')
            output_roc = kwargs.get('output_roc')
            stats_file = kwargs.get('stats_file')

            # Check parameters
            valid_args = ['test_class', 'test_pred', 'positive_index', 'estimator', 'save_folder', 'output_roc', 'stats_file']

            # Check if wrong parameters are given
            for arg_key in kwargs.keys():
                if arg_key not in valid_args:
                    raise ValueError(f"Invalid argument : {arg_key}")

            if (save_folder is not None and (not isinstance(save_folder, str))):
                raise ValueError('Error : parameter save_folder has to be a name contained in quotation marks "".')

            is_test_class_list = False
            if test_class_file is None :
                raise ValueError('Error : test class file missing, add it with option test_class="your_test_class_file" or with a list')
            elif not isinstance(test_class_file, str) and not isinstance(test_class_file, list):
                raise ValueError('Error : parameter test_class has to be a name contained in quotation marks "" or a list.')
            elif isinstance(test_class_file, list):
                is_test_class_list = True

            is_test_pred_list = False
            if test_pred_file is None:
                raise ValueError('Error : prediction data file missing, add it with option test_pred="your_prediction_data_file" or with a list')
            elif not isinstance(test_pred_file, str) and not isinstance(test_pred_file, list):
                raise ValueError('Error : parameter test_pred has to be a name contained in quotation marks "" or a list.')
            elif isinstance(test_pred_file, list):
                is_test_pred_list = True

            if stats_file is not None and not isinstance(stats_file, str):
                raise ValueError('Error : parameter stats_file has to be a name contained in quotation marks "".')

            if output_roc is None:
                output_roc = "roc_curve"
            elif not isinstance(output_roc, str):
                raise ValueError('Error : parameter output_roc has to be a name contained in quotation marks "".')
            output_roc += ".png"

            if (save_folder is not None):
                output_roc = save_folder + "/" + output_roc
                if stats_file is not None:
                    stats_file = save_folder + "/" + stats_file

            # Get data
            if is_test_class_list:
                test_class = test_class_file
            elif save_folder is not None:
                test_class = get_data(save_folder + "/" + test_class_file)
            else:
                test_class = get_data(test_class_file)
            if is_test_pred_list:
                test_pred = test_pred_file
            elif save_folder is not None:
                test_pred = get_data(save_folder + "/" + test_pred_file)
            else:
                test_pred = get_data(test_pred_file)

            if positive_index is None:
                raise ValueError('Error : positive class index missing, add it with option positive_index="your_positive_class_index"')
            elif not isinstance(positive_index, int) or positive_index < 0 or positive_index >= len(test_pred[0]):
                raise ValueError(f'Error : parameter positive_index has to be a positive integer smaller than {len(test_pred[0])}.')
            test_class_roc = [1 if cl.index(max(cl)) == positive_index else 0 for cl in test_class]
            test_pred = [p[positive_index] for p in test_pred]
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
