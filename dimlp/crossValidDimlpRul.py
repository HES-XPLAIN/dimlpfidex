# Compute cross validation for dimlpRul with existing crossValidation with fidex abd fidexGlo

import os
import shutil
import sys
import platform
import time
import math

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
        raise ValueError(f"Error : FidexGlo rules {rule_file} not found.")
    except (IOError):
        raise ValueError(f"Error : Couldn't open dimlp rules file {rule_file}.")

    return stats


def get_test_acc(stats_file):

    try:
        with open(stats_file, "r") as my_file:
            lines = my_file.readlines()
            my_file.close()

        for line in lines:
            line = line.strip()
            if "ccuracy on testing set" in line:
                _, value = line.split("=")
                return float(value.strip())

    except (FileNotFoundError):
        raise ValueError(f"Error : Train stat file {stats_file} not found.")
    except (IOError):
        raise ValueError(f"Error : Couldn't open train stat file {stats_file}.")

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
            print("has_attributes : 1 if there is some attributes file")

            print("----------------------------")
            print("----------------------------")

            print("Here is an example, keep same parameter names :")
            print("Exemple with Dimlp :")
            print('crossValidDimlpRul(train_method="dimlp", nb_in=4, nb_out=3, hasAttribute=1, crossVal_folder="dimlp/datafiles/IrisDataset/CV_IRIS_DIMLP")')
            print("----------------------------")
            print("Exemple with DimlpBT :")
            print('crossValidDimlpRul(train_method="dimlpBT", nb_in=4, nb_out=3, hasAttribute=1, crossVal_folder="dimlp/datafiles/IrisDataset/CV_IRIS_DIMLPBT")')
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
            has_attributes = kwargs.get('has_attributes')
            hk = {}
            for key, value in kwargs.items():
                if key.startswith('H') and key[1:].isdigit():
                    hk[key] = value

            # Check parameters

            obligatory_args = ['train_method', 'nb_in', 'nb_out', 'crossVal_folder']
            optional_args = ['nb_stairs', 'has_attributes']

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
            crossval_stats = root + "crossValidationStats.txt"

            N = count_folders(root, "Execution", False)
            K = count_folders(root + "Execution1" + separator, "Fold", False)
            if train_method == "dimlpBT":
                nb_networks = count_folders(root + "Execution1" + separator + "Fold1" + separator, "weightsBT", True)


            # Statistics
            # One execution
            mean_nb_rules = 0.0
            mean_nb_cover = 0.0
            mean_nb_antecedants = 0.0
            mean_fidel = 0.0
            mean_rules_acc = 0.0
            mean_default_rate = 0.0
            mean_test_acc = 0.0
            mean_test_acc_when_rules_and_model_agree = 0.0

            # All executions
            mean_nb_rules_all = 0.0
            mean_nb_cover_all = 0.0
            mean_nb_antecedants_all = 0.0
            mean_fidel_all = 0.0
            mean_rules_acc_all = 0.0
            mean_default_rate_all = 0.0
            mean_test_acc_all = 0.0
            mean_test_acc_when_rules_and_model_agree_all = 0.0

            std_nb_rules_all = 0.0
            std_nb_cover_all = 0.0
            std_nb_antecedants_all = 0.0
            std_fidel_all = 0.0
            std_rules_acc_all = 0.0
            std_default_rate_all = 0.0
            std_test_acc_all = 0.0
            std_test_acc_when_rules_and_model_agree_all = 0.0

            mean_exec_values = []

            nb_no_rules = 0.0 # If there is no rule found for a Fold

            # Get dimlpRules for each execution and each fold

            for n in range(1, N + 1):
                nb_no_rules_current_exec = 0.0
                for k in range(1, K + 1 ):
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
                    if has_attributes == 1:
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

                    # Get statistics

                    stats = get_dimlprul_stats(fold_folder + separator + "dimlpRules.rls")
                    test_acc = get_test_acc(fold_folder + separator + "dimlpRulesStats.txt")

                    if not stats:
                        nb_no_rules_current_exec += 1
                        nb_no_rules += 1
                    else:
                        mean_nb_rules += stats[0]
                        mean_nb_cover += stats[3]
                        mean_nb_antecedants += stats[2]
                        mean_fidel += stats[5]
                        mean_rules_acc += stats[4]
                        mean_default_rate += stats[7]
                        mean_test_acc_when_rules_and_model_agree += stats[6]
                    mean_test_acc += test_acc

                # Compute execution Stats
                mean_current_exec_values = []
                nb_fold_with_rules = K-nb_no_rules_current_exec

                mean_current_exec_values.append(mean_nb_rules / nb_fold_with_rules)
                mean_nb_rules = 0
                mean_current_exec_values.append(mean_nb_cover / nb_fold_with_rules)
                mean_nb_cover = 0
                mean_current_exec_values.append(mean_nb_antecedants / nb_fold_with_rules)
                mean_nb_antecedants = 0
                mean_current_exec_values.append(mean_fidel / nb_fold_with_rules)
                mean_fidel = 0
                mean_current_exec_values.append(mean_rules_acc / nb_fold_with_rules)
                mean_rules_acc = 0
                mean_current_exec_values.append(mean_default_rate / nb_fold_with_rules)
                mean_default_rate = 0
                mean_current_exec_values.append(mean_test_acc / K)
                mean_test_acc = 0
                mean_current_exec_values.append(mean_test_acc_when_rules_and_model_agree / nb_fold_with_rules)
                mean_test_acc_when_rules_and_model_agree = 0
                mean_current_exec_values.append(nb_no_rules_current_exec)
                mean_exec_values.append(mean_current_exec_values)

                formatted_mean_current_exec_values = []
                for i in range(len(mean_current_exec_values)):
                    formatted_val = "{:.6f}".format(mean_current_exec_values[i]).rstrip(".0")
                    if formatted_val == "":
                        formatted_val = "0"
                    formatted_mean_current_exec_values.append(formatted_val)

                # Output stats

                try:
                    with open(crossval_stats, "r") as outputStatsFile:
                        lines = outputStatsFile.readlines()
                        outputStatsFile.close()

                    target_line = f"Results for execution {n} of {K}-Cross validation :"
                    index = -1
                    for i, line in enumerate(lines):
                        if target_line in line:
                            index = i + 2
                            break

                    if index != -1:
                        lines.insert(index, "Dimlp :\n")
                        lines.insert(index + 1, f"The mean number of rules is: {formatted_mean_current_exec_values[0]}\n")
                        if nb_no_rules_current_exec > 0:
                            if nb_no_rules_current_exec == 1:
                                lines.insert(index + 2, f"We didn't found any rule {int(nb_no_rules_current_exec)} time\n")
                            else:
                                lines.insert(index + 2, f"We didn't found any rule {int(nb_no_rules_current_exec)} times\n")
                            index += 1
                        lines.insert(index + 2, f"The mean sample covering number per rule is: {formatted_mean_current_exec_values[1]}\n")
                        lines.insert(index + 3, f"The mean number of antecedents per rule is: {formatted_mean_current_exec_values[2]}\n")
                        lines.insert(index + 4, f"The mean global rule fidelity rate is: {formatted_mean_current_exec_values[3]}\n")
                        lines.insert(index + 5, f"The mean global rule accuracy is: {formatted_mean_current_exec_values[4]}\n")
                        lines.insert(index + 6, f"The mean default rule rate (when we can't find a rule) is: {formatted_mean_current_exec_values[5]}\n")
                        lines.insert(index + 7, f"The mean model test accuracy is: {formatted_mean_current_exec_values[6]}\n")
                        lines.insert(index + 8, f"The mean model test accuracy when rules and model agree is: {formatted_mean_current_exec_values[7]}\n")
                        lines.insert(index + 9, "\n---------------------------------------------------------\n")

                    with open(crossval_stats, 'w') as outputStatsFile:
                        outputStatsFile.writelines(lines)

                except (FileNotFoundError):
                    raise ValueError(f"Error : File for stats extraction ({crossval_stats}) not found.")
                except (IOError):
                    raise ValueError(f"Error : Couldn't open stats extraction file {crossval_stats}.")

            # Compute global stats

            for exec in range(N):
                multiplier = K-mean_exec_values[exec][8] # If there is lack of some datas (sometimes we didnt found any rules), we need to take it into account
                mean_nb_rules_all += multiplier*mean_exec_values[exec][0]
                mean_nb_cover_all += multiplier*mean_exec_values[exec][1]
                mean_nb_antecedants_all += multiplier*mean_exec_values[exec][2]
                mean_fidel_all += multiplier*mean_exec_values[exec][3]
                mean_rules_acc_all += multiplier*mean_exec_values[exec][4]
                mean_default_rate_all += multiplier*mean_exec_values[exec][5]
                mean_test_acc_all += multiplier*mean_exec_values[exec][6]
                mean_test_acc_when_rules_and_model_agree_all += multiplier*mean_exec_values[exec][7]

            divider = N*K-nb_no_rules
            mean_nb_rules_all /= divider
            mean_nb_cover_all /= divider
            mean_nb_antecedants_all /= divider
            mean_fidel_all /= divider
            mean_rules_acc_all /= divider
            mean_default_rate_all /= divider
            mean_test_acc_all /= divider
            mean_test_acc_when_rules_and_model_agree_all /= divider

            for exec in range(N):
                std_nb_rules_all += pow(mean_exec_values[exec][0] - mean_nb_rules_all, 2)
                std_nb_cover_all += pow(mean_exec_values[exec][1] - mean_nb_cover_all, 2)
                std_nb_antecedants_all += pow(mean_exec_values[exec][2] - mean_nb_antecedants_all, 2)
                std_fidel_all += pow(mean_exec_values[exec][3] - mean_fidel_all, 2)
                std_rules_acc_all += pow(mean_exec_values[exec][4] - mean_rules_acc_all, 2)
                std_default_rate_all += pow(mean_exec_values[exec][5] - mean_default_rate_all, 2)
                std_test_acc_all += pow(mean_exec_values[exec][6] - mean_test_acc_all, 2)
                std_test_acc_when_rules_and_model_agree_all += pow(mean_exec_values[exec][7] - mean_test_acc_when_rules_and_model_agree_all, 2)

            std_nb_rules_all = math.sqrt(std_nb_rules_all / N)
            std_nb_cover_all = math.sqrt(std_nb_cover_all / N)
            std_nb_antecedants_all = math.sqrt(std_nb_antecedants_all / N)
            std_fidel_all = math.sqrt(std_fidel_all / N)
            std_rules_acc_all = math.sqrt(std_rules_acc_all / N)
            std_default_rate_all = math.sqrt(std_default_rate_all / N)
            std_test_acc_all = math.sqrt(std_test_acc_all / N)
            std_test_acc_when_rules_and_model_agree_all = math.sqrt(std_test_acc_when_rules_and_model_agree_all / N)

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

            formatted_mean_fidel_all = "{:.6f}".format(mean_fidel_all).rstrip(".0")
            if formatted_mean_fidel_all == "":
                formatted_mean_fidel_all = "0"

            formatted_std_fidel_all = "{:.6f}".format(std_fidel_all).rstrip(".0")
            if formatted_std_fidel_all == "":
                formatted_std_fidel_all = "0"

            formatted_mean_rules_acc_all = "{:.6f}".format(mean_rules_acc_all).rstrip(".0")
            if formatted_mean_rules_acc_all == "":
                formatted_mean_rules_acc_all = "0"

            formatted_std_rules_acc_all = "{:.6f}".format(std_rules_acc_all).rstrip(".0")
            if formatted_std_rules_acc_all == "":
                formatted_std_rules_acc_all = "0"

            formatted_mean_default_rate_all = "{:.6f}".format(mean_default_rate_all).rstrip(".0")
            if formatted_mean_default_rate_all == "":
                formatted_mean_default_rate_all = "0"

            formatted_std_default_rate_all = "{:.6f}".format(std_default_rate_all).rstrip(".0")
            if formatted_std_default_rate_all == "":
                formatted_std_default_rate_all = "0"

            formatted_mean_test_acc_all = "{:.6f}".format(mean_test_acc_all).rstrip(".0")
            if formatted_mean_test_acc_all == "":
                formatted_mean_test_acc_all = "0"

            formatted_std_test_acc_all = "{:.6f}".format(std_test_acc_all).rstrip(".0")
            if formatted_std_test_acc_all == "":
                formatted_std_test_acc_all = "0"

            formatted_mean_test_acc_when_rules_and_model_agree_all = "{:.6f}".format(mean_test_acc_when_rules_and_model_agree_all).rstrip(".0")
            if formatted_mean_test_acc_when_rules_and_model_agree_all == "":
                formatted_mean_test_acc_when_rules_and_model_agree_all = "0"

            formatted_std_test_acc_when_rules_and_model_agree_all = "{:.6f}".format(std_test_acc_when_rules_and_model_agree_all).rstrip(".0")
            if formatted_std_test_acc_when_rules_and_model_agree_all == "":
                formatted_std_test_acc_when_rules_and_model_agree_all = "0"

            # Output global stats

            try:
                with open(crossval_stats, "r") as outputStatsFile:
                    lines = outputStatsFile.readlines()
                    outputStatsFile.close()

                target_line = f"Results for {N} times {K}-Cross validation :"
                index = -1
                for i, line in enumerate(lines):
                    if target_line in line:
                        index = i + 2
                        break

                if index != -1:
                    lines.insert(index, "Dimlp :\n")
                    lines.insert(index + 1, f"The mean number of rules is: {formatted_mean_nb_rules_all}\n")
                    if nb_no_rules > 0:
                            if nb_no_rules == 1:
                                lines.insert(index + 2, f"We didn't found any rule {int(nb_no_rules)} time\n")
                            else:
                                lines.insert(index + 2, f"We didn't found any rule {int(nb_no_rules)} times\n")
                            index +=1

                    lines.insert(index + 2, f"The standard deviation of the number of rules is: {formatted_std_nb_rules_all}\n")
                    lines.insert(index + 3, f"The mean sample covering number per rule is: {formatted_mean_nb_cover_all}\n")
                    lines.insert(index + 4, f"The standard deviation of the sample covering number per rule is: {formatted_std_nb_cover_all}\n")
                    lines.insert(index + 5, f"The mean number of antecedents per rule is: {formatted_mean_nb_antecedants_all}\n")
                    lines.insert(index + 6, f"The standard deviation of the number of antecedents per rule is: {formatted_std_nb_antecedants_all}\n")
                    lines.insert(index + 7, f"The mean global rule fidelity rate is: {formatted_mean_fidel_all}\n")
                    lines.insert(index + 8, f"The standard deviation of the global rule fidelity rate is: {formatted_std_fidel_all}\n")
                    lines.insert(index + 9, f"The mean global rule accuracy is: {formatted_mean_rules_acc_all}\n")
                    lines.insert(index + 10, f"The standard deviation of the global rule accuracy is: {formatted_std_rules_acc_all}\n")
                    lines.insert(index + 11, f"The mean default rule rate (when we can't find a rule) is: {formatted_mean_default_rate_all}\n")
                    lines.insert(index + 12, f"The standard deviation of the default rule rate (when we can't find a rule) is: {formatted_std_default_rate_all}\n")
                    lines.insert(index + 13, f"The mean model test accuracy is: {formatted_mean_test_acc_all}\n")
                    lines.insert(index + 14, f"The standard deviation of the model test accuracy is: {formatted_std_test_acc_all}\n")
                    lines.insert(index + 15, f"The mean model test accuracy when rules and model agree is: {formatted_mean_test_acc_when_rules_and_model_agree_all}\n")
                    lines.insert(index + 16, f"The standard deviation of the model test accuracy when rules and model agree is: {formatted_std_test_acc_when_rules_and_model_agree_all}\n")
                    lines.insert(index + 17, "\n---------------------------------------------------------\n")

                with open(crossval_stats, 'w') as outputStatsFile:
                    outputStatsFile.writelines(lines)

            except (FileNotFoundError):
                raise ValueError(f"Error : File for stats extraction ({crossval_stats}) not found.")
            except (IOError):
                raise ValueError(f"Error : Couldn't open stats extraction file {crossval_stats}.")


        end_time = time.time()
        full_time = end_time - start_time
        full_time = "{:.6f}".format(full_time).rstrip(".0")

        try:
            with open(crossval_stats, "r") as outputStatsFile:
                lines = outputStatsFile.readlines()
                outputStatsFile.close()
            _, value = lines[-1].split("= ")
            ancien_time, _ = value.split(" sec")
            complete_full_time = "{:.6f}".format(float(full_time) + float(ancien_time)).rstrip(".0")
            lines[-1] = f"Execution time = {complete_full_time} sec"

            with open(crossval_stats, 'w') as outputStatsFile:
                outputStatsFile.writelines(lines)

        except (FileNotFoundError):
            raise ValueError(f"Error : File for stats extraction ({crossval_stats}) not found.")
        except (IOError):
            raise ValueError(f"Error : Couldn't open stats extraction file {crossval_stats}.")

        print(f"\nFull execution time = {full_time} sec")

        return 0
    except ValueError as error:
        print(error)
        return -1
