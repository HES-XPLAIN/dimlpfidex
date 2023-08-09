# Compute cross validation for dimlpRul with existing crossValidation with fidex abd fidexGlo

import os
import shutil
import sys
import platform
import time

# import module dimlp :

if sys.platform == 'win32':
    dir_path = os.path.abspath('dimlpfidex')
    os.add_dll_directory(dir_path)

from dimlpfidex import dimlp



def count_folders(folder_path, folder_name, is_file):
    sub_folders = [item for item in os.listdir(folder_path) if os.path.exists(os.path.join(folder_path, item)) and item.startswith(folder_name)]
    n = len(sub_folders)

    if n == 0:
        raise ValueError("Error : crossVal folder is not in good shape(Execution folders with Fold folders inside)")

    for i in range(1, n + 1):
        expected_folder_name = f"{folder_name}{i}"
        if is_file:
            expected_folder_name += ".wts"
        if expected_folder_name not in sub_folders:
            raise ValueError("Error : crossVal folder is not in good shape(Execution folders from 1 to N with Fold folders from 1 to K inside)")

    return n

def crossValidDimlpRul(*args, **kwargs):
    try:
        if args or not kwargs:
            print("---------------------------------------------------------------------")
            print("Please specify arguments using named parameters.")
            print("Warning! The files are localised with respect to root folder dimlpfidex.")

            print("----------------------------")
            print("Obligatory parameters :")
            print("crossVal_folder : Folder where crossValidation files are saved.")
            print("nb_in : number of input neurons")
            print("nb_out : number of output neurons")
            print("train_method : dimlp or dimlpBT")

            print("----------------------------")
            print("Optional parameters :")
            print("H1 : number of neurons in the first hidden layer. If not specified this number will be equal to the number of input neurons.")
            print("Hk : number of neurons in the kth hidden layer.")
            print("nb_stairs : number of stairs in staircase activation function (50 by default)")

            print("----------------------------")
            print("----------------------------")

            print("Here is an example, keep same parameter names :")
            print("Exemple with Dimlp :")
            print('crossValidDimlpRul(train_method="dimlp", nb_in=4, nb_out=3, crossVal_folder="dimlp/datafiles/IrisDataset/CV_IRIS_DIMLP")')
            print("----------------------------")
            print("Exemple with DimlpBT :")
            print('crossValidDimlpRul(train_method="dimlpBT", nb_in=4, nb_out=3, crossVal_folder="dimlp/datafiles/IrisDataset/CV_IRIS_DIMLPBT")')
            print("---------------------------------------------------------------------")

            return 0

        else:

            start_time = time.time()

            # Get parameters
            train_method = kwargs.get('train_method')
            nb_in = kwargs.get('nb_in')
            nb_out = kwargs.get('nb_out')
            crossval_folder = kwargs.get('crossVal_folder')
            nb_stairs = kwargs.get('nb_stairs')
            hk = {}
            for key, value in kwargs.items():
                if key.startswith('H') and key[1:].isdigit():
                    hk[key] = value

            # Check parameters

            obligatory_args = ['train_method', 'nb_in', 'nb_out', 'crossVal_folder']
            optional_args = ['nb_stairs']

            train_methods = {"dimlp", "dimlpBT"}
            if train_method is None:
                raise ValueError('Error : train method is missing, add it with option train_method="dimlp" or "dimlpBT"')
            elif (train_method not in train_methods):
                raise ValueError('Error, parameter train_method is not dimlp or dimlpBT')

            for arg_key in kwargs.keys():
                if (arg_key not in obligatory_args and arg_key not in optional_args and not(arg_key.startswith('H') and arg_key[1:].isdigit())):
                    raise ValueError(f"Invalid argument : {arg_key}")

            if crossval_folder is None:
                raise ValueError('Error : the cross validation folder has to be given with crossVal_folder="your_crossVal_folder"')
            elif not isinstance(crossval_folder, str):
                raise ValueError('Error, parameter crossval_folder has to be a name contained in quotation marks "".')

            if nb_in is None:
                raise ValueError('Error : the number of input neurons is missing, add it with option nb_in=your_number')
            elif (not isinstance(nb_in, int) or nb_in<=0):
                raise ValueError('Error, parameter nb_in is not an strictly positive integer')

            if nb_out is None:
                raise ValueError('Error : the number of output neurons is missing, add it with option nb_out=your_number')
            elif (not isinstance(nb_out, int) or nb_out<=0):
                raise ValueError('Error, parameter nb_out is not an strictly positive integer')

            if nb_stairs is None:
                nb_stairs = 50


            #create paths with root foler

            system = platform.system()
            if system == "Linux" or system == "Darwin":
                separator = "/"
            elif system == "Windows":
                separator = "\\"

            root = crossval_folder + separator

            N = count_folders(root, "Execution", False)
            K = count_folders(root + "Execution1" + separator, "Fold", False)
            if train_method == "dimlpBT":
                nb_networks = count_folders(root + "Execution1" + separator + "Fold1" + separator, "weightsBT", True)

            # Get dimlpRules for each execution and each fold

            for n in range(1, N + 1):
                for k in range(1, K+ 1 ):
                    fold_folder = f"{root}Execution{n}{separator}Fold{k}"

                    command = ""
                    if train_method == "dimlp":
                        command += "dimlpRul"
                        command += " -R dimlpRules.rls"
                        command += " -W weights.wts"
                    else:
                        command += "densCls"
                        command += " -R -F dimlpRules.rls"
                        command += " -W weightsBT"
                        command += " -N " + str(nb_networks)
                    command += " -S " + fold_folder
                    command += " -I " + str(nb_in)
                    command += " -O " + str(nb_out)
                    command += " -L train.txt"
                    command += " -1 trainTarget.txt"
                    command += " -T test.txt"
                    command += " -2 testTarget.txt"
                    command += " -r dimlpRulesResult.txt"
                    command += " -o dimlpRulesStats.txt"
                    command += " -q " + str(nb_stairs)
                    command += " -A ../../../attributes.txt"
                    for key, value in hk.items():
                            command += " -" + key + " " + str(value)


                    if train_method == "dimlp":
                        print("Enter in DimlpRul function")
                        res = dimlp.dimlpRul(command)
                    else:
                        print("Enter in DensCls function")
                        res = dimlp.densCls(command)
                    if (res == -1):
                        return -1 # If there is an error in the Rul


        end_time = time.time()
        full_time = end_time - start_time
        full_time = "{:.6f}".format(full_time).rstrip(".0")

        print(f"\nFull execution time = {full_time} sec")

        return 0
    except ValueError as error:
        print(error)
        return -1
