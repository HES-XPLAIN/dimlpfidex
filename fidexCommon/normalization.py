import time
import sys
from sklearn import svm
from sklearn import metrics
import numpy as np
import matplotlib.pyplot as plt
import os
import math
import re

from trainings.trnFun import get_data, validate_string_param, check_int, check_strictly_positive, check_bool, get_attribute_file


def gaussian_normalization(datas, attribute_indices, normalized_file, fill_missing_values, normalization_stats=None, attributes=None, missing_value=None, with_median=False, mus=None, sigmas=None):

    """
    Perform Gaussian normalization on specified attributes of the given data.

    This function normalizes the data based on the Gaussian distribution, where each selected attribute is
    adjusted to have a mean (or median) of zero and a standard deviation of one. It handles missing values,
    writes the normalization statistics to a file if provided, and saves the normalized data to a specified file.

    :param datas: The dataset to be normalized.
    :type datas: list of lists
    :param attribute_indices: Indices of the attributes to be normalized.
    :type attribute_indices: list
    :param normalized_file: File path to save the normalized data.
    :type normalized_file: str
    :param fill_missing_values: Flag to fill missing values with the mean/median.
    :type fill_missing_values: bool
    :param normalization_stats: Optional file path to save the normalization statistics.
    :type normalization_stats: str, optional
    :param attributes: Optional list of attribute names corresponding to indices.
    :type attributes: list, optional
    :param missing_value: Representation of missing values in the data.
    :type missing_value: str or None
    :param with_median: Flag to use median for normalization instead of mean.
    :type with_median: bool
    :param mus: Predefined list of mean (or median) values for each attribute, defaults to None.
    :type mus: list of float, optional
    :param sigmas: Predefined list of standard deviation values for each attribute, defaults to None.
    :type sigmas: list of float, optional
    :raises ValueError: If mus, sigmas, and attribute_indices are not of the same length or other existing conditions.
    :return: Tuple containing the mean (or median) and standard deviation used for normalization.
    :rtype: (float, float)
    """

    if mus and len(mus) != len(attribute_indices):
            raise ValueError("Error: The length of 'mus' and 'attribute_indices' must be the same.")

    if sigmas and len(sigmas) != len(attribute_indices):
            raise ValueError("Error: The length of 'sigmas' and 'attribute_indices' must be the same.")

    has_mus = True
    if not mus:
        has_mus = False
        mus = []
    has_sigmas = True
    if not sigmas:
        has_sigmas = False
        sigmas = []

    # Erase everything in the normalization file in case it already exists
    if normalization_stats is not None:
        try:
            with open(normalization_stats, 'w') as myfile:
                pass
        except FileNotFoundError:
                raise ValueError(f"Error : File {normalization_stats} not found.")
        except IOError:
            raise ValueError(f"Error : Couldn't open file {normalization_stats}.")

    for idx, indice in enumerate(attribute_indices):
        name_id = str(attributes[indice]) if attributes else str(indice)
        attribut = []
        for data in datas:
            if data[indice] != missing_value:
                try:
                    value = float(data[indice])
                    if math.isnan(value):
                        raise ValueError
                    attribut.append(value)
                except ValueError:
                    raise ValueError("Error during gaussian normalization : Found a Non-numeric value found which is not the missing value.")
        if len(attribut) != 0:
            attribut = np.array(attribut)
            current_mu = mus[idx] if has_mus else None
            current_sigma = sigmas[idx] if has_sigmas else None
            if current_sigma is None:
                current_sigma = np.std(attribut)
                sigmas.append(current_sigma)
            if current_mu is None:
                if with_median:
                    current_mu = np.median(attribut)
                else:
                    current_mu = np.mean(attribut)
                mus.append(current_mu)

            if normalization_stats is not None:
                try:
                    with open(normalization_stats, 'a') as myfile:

                        if with_median:
                            myfile.write(name_id + " : original median: " + str(current_mu) + ", original std: " + str(current_sigma) + '\n')
                        else:
                            myfile.write(name_id + " : original mean: " + str(current_mu) + ", original std: " + str(current_sigma) + '\n')
                except (FileNotFoundError):
                    raise ValueError(f"Error : File {normalization_stats} not found.")
                except (IOError):
                    raise ValueError(f"Error : Couldn't open file {normalization_stats}.")

            if fill_missing_values:
                for data in datas:
                    data[indice] = np.where(data[indice] == missing_value, str(current_mu), data[indice])
                    data[indice] = str(data[indice])
            for i in range(len(datas)):
                if datas[i][indice] != missing_value:
                    if current_sigma == 0:
                        datas[i][indice] = str(0)
                    else:
                        datas[i][indice] = str(round((float(datas[i][indice]) - current_mu) / current_sigma, 6))
        else:
            raise ValueError(f"Error : no value for attribute {name_id}. Impossible to normalize.")

    try:
        with open(normalized_file, 'w') as norm_file:
            for data in datas:
                norm_file.write(' '.join(data) + '\n')
    except IOError:
        raise ValueError(f"Error: Could not write to file {normalized_file}.")

    return mus, sigmas


def parse_normalization_stats(file_name, nb_attributes, attributes=None):
    """
    Parse a file containing normalization statistics previously generated the using the normalization function.

    :param file_name: Name of the file containing the normalization statistics.
                      The expected format for each line in the file is either:
                      "[attribute_name] : original (mean|median): [value], original std: [value]"
                      or
                      "[attribute_index] : original (mean|median): [value], original std: [value]"
                      where [attribute_name] is a string from the 'attributes' list (if provided),
                      [attribute_index] is an integer (if 'attributes' is not provided),
                      and [value] is a floating-point number.
    :param nb_attributes: Number of attributes
    :param attributes: List of attribute names (optional)
    :raises ValueError: If the file is not found, cannot be opened, is not in the correct format,
                        or if there is an inconsistency in using mean or median across different lines.
    :return: Tuple of (indices_list, with_median, mean_median_values, std_values)
    """

    with_median = None

    # Check if each line is on good format, either with attribute names or with attribute indices
    patterns = []

    if attributes is not None:
        if len(attributes) != nb_attributes:
            raise ValueError(f"Error during parsing of {file_name} : the number of attributes is not equal to the length of attributes list.")

        attr_pattern = "|".join(map(re.escape, attributes))
        attr_pattern = re.compile(fr'({attr_pattern}) : original (mean|median): (\d+(\.\d+)?), original std: (\d+(\.\d+)?)') # Regular expression pattern to match for each line
        patterns.append(attr_pattern)
    index_pattern = "|".join(map(str, range(nb_attributes)))
    index_pattern = re.compile(fr'({index_pattern}) : original (mean|median): (\d+(\.\d+)?), original std: (\d+(\.\d+)?)') # Regular expression pattern to match for each line
    patterns.append(index_pattern)

    for pattern in patterns: # We test if it works with attribute names, and if not, with indices
        indices_list = []
        mus = []
        sigmas = []
        pattern_error = False

        try:
            with open(file_name, 'r') as file:
                for line in file:
                    if line.strip(): # Check if line is not empty
                        # Check if line is on good format.
                        match = re.match(pattern, line)
                        if not match:
                            pattern_error = True
                            break

                        if pattern == index_pattern:
                            indices_list.append(int(match.group(1)))
                        else:
                            indices_list.append(attributes.index(match.group(1)))

                        if with_median is None:
                            with_median = match.group(2) == "median"
                        else:
                            if match.group(2) != ("median" if with_median else "mean"):
                                raise ValueError(f"Error in {file_name} : There is sometimes mean and sometimes median.")

                        mus.append(float(match.group(3)))
                        sigmas.append(float(match.group(5)))
        except FileNotFoundError:
            raise ValueError(f"Error: File {file_name} not found.")
        except IOError:
            raise ValueError(f"Error: Couldn't open file {file_name}.")
        if not pattern_error:
            break

    if pattern_error:
        if attributes is None:
            raise ValueError(f"Error in {file_name} : File not in the correct format, maybe you forgot to add the attribute file")
        else:
            raise ValueError(f"Error in {file_name} : File not in the correct format.")

    if len(indices_list) != len(set(indices_list)):
                    raise ValueError(f"Error in {file_name} : The same attribute is specified twice.")

    return indices_list, with_median, mus, sigmas


def get_and_check_string_data_from_list(datas, name):
    """
    This function checks whether the 'datas' parameter is either a list of strings or a single string.
    The function raises a ValueError if the conditions are not met.

    :param datas: The data to check. It can be either a list of file names (strings) or a single file name (string).
    :type datas: list or str
    :raises ValueError: If 'datas' is a list but contains non-string elements.
    :raises ValueError: If 'datas' is neither a list nor a string.
    :return: The original 'datas' if it's a list, or a list containing 'datas' if it's a single string.
    :rtype: list
    """
    if isinstance(datas, list):
        if not datas:
            raise ValueError(f"Error : parameter {name} is an empty list")
        for data in datas:
            if not isinstance(data, str):
                raise ValueError(f'Error: in parameter {name} : Each file name has to be contained in quotation marks "".')
        return datas
    elif not isinstance(datas, str):
        raise ValueError(f'Error: parameter {name} has to be a list or a name contained in quotation marks "".')
    else:
        return [datas]

def normalization(*args, **kwargs):

    """
    Note : On accepte les fichiers avec des classes, on se base sur nbAttributes
    Ce qu'on peut faire : normaliser des fichiers de donneés ou dénormaliser des fichiers de règles
    On normalise selon les colonnes indiquées
    Génère un fichier avec les moyennes et std des colonnes normalisées ainsi que le nouveau fichier de données normalisé
    Pour normaliser, soit on se base sur le premier fichier donné, soit sur un fichier de normalisation obtenu aurapavant, soit en donnant explicitement une moyenne et un std
    On peut choisir de remplir les valeurs manquantes ou non. Il est important de toujours préciser le symbole représentant les valeurs manquantes, ou NaN sinon
    On peut normaliser avec la moyenne ou la médiane
    Pour la dénormalisation...
    """

    try:

        if args or not kwargs:
            print("---------------------------------------------------------------------")
            print("Please specify arguments using named parameters.")
            print("Warning! The files are localised with respect to root folder dimlpfidex.")
            print("----------------------------")
            print("Obligatory parameters :")
            print("nb_attributes : number of attributes")
            print('missing_values : string representing a missing value in your data, put "NaN" (or any string not in your data) if you do not have any missing value (only when working on data files)')
            print("----------------------------")
            print("Optional parameters :")
            print("nb_classes : mendatory if classes are also present in data file")
            print("save_folder : Folder based on main folder dimlpfidex(default folder) where generated files will be saved. If a file name is specified with another option, his path will be configured with respect to this root folder.")
            print("normalization_stats : file containing the mean and std of some attributes. Used to normalize or denormalize if specified.")
            print("mus : list of float corresponding to mean or median of each attribute index of interest.")
            print("sigmas : list of float corresponding to standard deviation of each attribute index of interest.")
            print("datas : every data files to normalize contained in a list if there is more than one file. Normalize every file with respect to the first one if normalization_stats is not specified.")
            print("rule_files : every rule files to denormalize contained in a list if there is more than one file. Can only be denormalized if a normalization_stats file or mus, sigmas and attribute_indices are given.")
            print("Either 'datas' or 'rule_files' must be specified.")
            print("attribute_file : file containing attribute names (mendatory if rules or normalization stats are written with attribute names)")
            print("attribute_indices : indices of the attributes(columns) to normalize a data file, contained in a list (first column is index 0, all indices by default, only used when no normalization_stats is given).")
            print("output_normalization_stats : name of the output file containing the mean and std of some attributes when normalization_stats is not specified (normalization_stats.txt by default).")
            print("output_datas : name of the normalized files contained in a list if there is more than one file. Need to specify everyone of them if one is specified (<original_name>_normalized<original_extension> by default)")
            print("output_rules : name of the normalized rule files contained in a list if there is more than one file. Need to specify everyone of them if one is specified (<original_name>_normalized<original_extension> by default)")
            print("with_median : boolean, whether we use median instead of mean to compute normalitzation (False by default, only when working on data files)")
            print("fill_missing_values : boolean, whether we fill missing values with mean or median (True by default, only when working on data files)")
            print("----------------------------")
            print("Here is an example with data files, keep same parameter names :")
            print('normalization(datas = ["datanormTrain", "datanormTest"], attribute_indices = [0,2,4], nb_attributes=16, missing_values="NaN", save_folder = "dimlp/datafiles")')
            print("Another example with rule files :")
            print('normalization(normalization_stats = "normalization_stats.txt", rule_files = "globalRulesDatanorm", nb_attributes=16, save_folder = "dimlp/datafiles")')
            print("---------------------------------------------------------------------")
        else:

            start_time = time.time()

            # Get parameters
            nb_attributes = kwargs.get('nb_attributes')
            nb_classes = kwargs.get('nb_classes')
            missing_values = kwargs.get('missing_values')
            save_folder = kwargs.get('save_folder')
            normalization_stats = kwargs.get('normalization_stats')
            mus = kwargs.get('mus')
            sigmas = kwargs.get('sigmas')
            datas = kwargs.get('datas')
            rule_files = kwargs.get('rule_files')
            attribute_file = kwargs.get('attribute_file')
            attribute_indices = kwargs.get('attribute_indices')
            output_normalization_stats = kwargs.get('output_normalization_stats')
            output_datas = kwargs.get('output_datas')
            output_rules = kwargs.get('output_rules')
            with_median = kwargs.get('with_median')
            fill_missing_values = kwargs.get('fill_missing_values')

            # Check parameters

            valid_args = ['nb_attributes', 'nb_classes', 'missing_values', 'save_folder', 'normalization_stats', 'mus', 'sigmas', 'datas', 'rule_files', 'attribute_file', 'attribute_indices', 'output_normalization_stats',
                          'output_datas', 'output_rules', 'with_median', 'fill_missing_values']

            # Check if wrong parameters are given
            for arg_key in kwargs.keys():
                if arg_key not in valid_args:
                    raise ValueError(f"Invalid argument : {arg_key}.")

            if nb_attributes is None:
                raise ValueError('Error : number of attributes missing, add it with option nb_attributes="your_number_of_attributes".')
            elif not check_strictly_positive(nb_attributes) or not check_int(nb_attributes):
                raise ValueError('Error : parameter nb_attributes has to be a strictly positive integer.')

            if (nb_classes is not None and (not check_strictly_positive(nb_classes) or not check_int(nb_attributes))):
                raise ValueError('Error : parameter nb_classes has to be a strictly positive integer.')

            save_folder = validate_string_param(save_folder, "save_folder", allow_none=True)
            normalization_stats = validate_string_param(normalization_stats, "normalization_stats", allow_none=True)

            if mus is not None :
                if normalization_stats is not None:
                    raise ValueError("Error : mus are given but normalization_stats is also given.")
                if sigmas is None:
                    raise ValueError("Error : mus are given but sigmas are not given.")
                if not isinstance(mus, list) or not all(isinstance(mu, (float,int)) for mu in mus):
                    raise ValueError("Error : parameter mus has to be a list of float.")
                if not mus:
                    raise ValueError("Error : parameter mus is an empty list")
            if sigmas is not None :
                if normalization_stats is not None:
                    raise ValueError("Error : sigmas are given but normalization_stats is also given.")
                if mus is None:
                    raise ValueError("Error : sigmas are given but mus are not given.")
                if not isinstance(sigmas, list) or not all(isinstance(sigma, (float,int)) for sigma in sigmas):
                    raise ValueError("Error : parameter sigmas has to be a list of float.")
                if not sigmas:
                    raise ValueError("Error : parameter sigmas is an empty list")

            if datas:
                datas = get_and_check_string_data_from_list(datas, "datas")
            if rule_files:
                rule_files = get_and_check_string_data_from_list(rule_files, "rule_files")

            attribute_file = validate_string_param(attribute_file, "attribute_file", allow_none=True)

            if attribute_indices is not None:
                if normalization_stats is not None:
                    raise ValueError("Error : attribute_indices are given but normalization_stats is also given.")
                if not isinstance(attribute_indices, list) or not all(check_int(index) and 0 <= index < nb_attributes for index in attribute_indices):
                    raise ValueError("Error : parameter attribute_indices has to be a list of positive integers smaller than the number of attributes.")
                if len(attribute_indices) != len(set(attribute_indices)):
                    raise ValueError("Error : The same attribute is specified twice in attribute_indices.")
                if not attribute_indices:
                    raise ValueError("Error : parameter attribute_indices is an empty list")
            else:
                if normalization_stats is None:
                    attribute_indices = list(range(nb_attributes))

            if mus is not None and sigmas is not None:
                if len(mus) != len(sigmas):
                    raise ValueError("Error : mus and sigmas have not the same amount of values.")
                if len(mus) != len(attribute_indices):
                    raise ValueError("Error : attribute_indices has not the same amount of values as mus and sigmas.")


            output_normalization_stats = validate_string_param(output_normalization_stats, "output_normalization_stats", default="normalization_stats.txt")

            if datas is not None:

                if missing_values is None:
                    raise ValueError("Error : parameter missing_values missing.")
                else:
                    missing_values = validate_string_param(missing_values, "missing_values")

                if fill_missing_values is None:
                    fill_missing_values = True
                elif not check_bool(fill_missing_values):
                        raise ValueError("Error : parameter fill_missing_values is not a boolean.")

                if with_median is None:
                    with_median = False
                elif not check_bool(with_median):
                        raise ValueError("Error : parameter with_median is not a boolean.")

                if output_datas is not None:
                    output_datas = get_and_check_string_data_from_list(output_datas, "output_datas")
                else:
                    output_datas = []
                    for data in datas:
                        base, ext = os.path.splitext(data)
                        if ext:
                            output_datas.append(f"{base}_normalized{ext}")
                        else:
                            output_datas.append(f"{data}_normalized")
                if len(output_datas) != len(datas):
                    raise ValueError("Error : the size of output_datas is not equal to the size of datas")

            if rule_files is not None:
                if normalization_stats is None and mus is None:
                    raise ValueError("Error : rule files specified but there is no normalization_stats file or mus, sigmas and attribute_indices")
                if output_rules is not None:
                    output_rules = get_and_check_string_data_from_list(output_rules, "output_rules")
                else:
                    output_rules = []
                    for rule_file in rule_files:
                        base, ext = os.path.splitext(rule_file)
                        if ext:
                            output_rules.append(f"{base}_normalized{ext}")
                        else:
                            output_rules.append(f"{rule_file}_normalized")
                if len(output_rules) != len(rule_files):
                    raise ValueError("Error : the size of output_rules is not equal to the size of rule_files")

            if (rule_files is None and datas is None):
                raise ValueError("Error : rule_files or datas must be specified.")


            if (save_folder):
                if normalization_stats is not None:
                    normalization_stats = save_folder + "/" + normalization_stats
                if output_normalization_stats is not None:
                    output_normalization_stats = save_folder + "/" + output_normalization_stats
                if datas is not None:
                    datas = [save_folder + "/" + data for data in datas]
                    output_datas = [save_folder + "/" + out_data for out_data in output_datas]
                if rule_files is not None:
                    rule_files = [save_folder + "/" + rule_file for rule_file in rule_files]
                    output_rules = [save_folder + "/" + out_rule_file for out_rule_file in output_rules]
                if attribute_file is not None:
                    attribute_file = save_folder + "/" + attribute_file


            # Get attributes
            attributes = None
            if (attribute_file is not None):
                attributes = []
                attributes, _ = get_attribute_file(attribute_file, nb_attributes, nb_classes)

            if normalization_stats is not None:
                # Get attribute_indices, mean/median, std and withMedian from normalization file
                attribute_indices, with_median, mus, sigmas = parse_normalization_stats(normalization_stats, nb_attributes, attributes)

            elif mus is None:
                #Normalize first file and get mean/median and std
                main_file = datas.pop(0)
                output_main_file = output_datas.pop(0)
                if nb_classes is not None:
                    main_datas = get_data(main_file, nb_attributes, nb_classes, keep_string=True)[0]
                else:
                    main_datas = get_data(main_file, nb_attributes, keep_string=True)[0]

                mus, sigmas = gaussian_normalization(main_datas, attribute_indices, output_main_file, fill_missing_values, output_normalization_stats, attributes, missing_values, with_median)


            if datas is not None:
                #Normalize each data file with mean/median and std obtained above
                if normalization_stats is not None:
                    output_normalization_stats = None
                for i in range(len(datas)):
                    if nb_classes is not None:
                        data = get_data(datas[i], nb_attributes, nb_classes, keep_string=True)[0]
                    else:
                        data = get_data(datas[i], nb_attributes, keep_string=True)[0]

                    gaussian_normalization(data, attribute_indices, output_datas[i], fill_missing_values, output_normalization_stats, attributes, missing_values, with_median, mus, sigmas)


            if rule_files is not None:
                #Denormalize each rule file with mean/median and std obtained above


                print("Denormalization of rules")
                """
                But : transformer le fichier rempli de règles par le même fichier en dénormalisant ce qui avait été auparavant été normalisé (les indices des colonnes sont données par attribute_indices)
                Fichiers de règles possibles :
                - Règles obtenues avec fidexGloRules. Sous la forme :
                    Rule 1: X1>=0.500000 X7<0.475000 -> class 0
                - Règles obtenues avec fidex. Sous cette forme :
                X7>=0.505 X0<0.561112 X10>=0.501666 -> class 1
                - Règles obtenues avec fidexGlo (Explanation, utilisation de getRules et de Fidex), sous les formes :
                R14: Rule 15: RS10764743>=0.934500 NODES_EXAMINED<3.500000 RS67342194<0.006500 -> NO_FOLLOW_UP_ARM_LYMPHEDEMA
                F14: Rule 15: RS10764743>=0.934500 NODES_EXAMINED<3.500000 RS67342194<0.006500 -> NO_FOLLOW_UP_ARM_LYMPHEDEMA
                CLINICAL_N_STAGE_N1>=0.500000 RS1788750<0.152000 WEIGHT>=70.200001 RS10764743<0.994500 ADJUVANT_CHEMOTHERAPY>=0.500000 RS2101893<1.997500 IMRT_TYPE_VMAT<0.500000 -> FOLLOW_UP_ARM_LYMPHEDEMA
                - Règles obtenues avec DimlpRul. Sous cette forme :
                Rule 1: (x1 > 0.665967) (x2 > 0.900465) (x6 > 0.355243) Class = 1 (173)


                Trouver une règle si elle correspond à un des patterns recherchés.
                Il y a un pattern pour les règles Dimlp et un pattern pour les règles fidex
                Pour chaque pattern il y a une version avec les attributs et une version avec les indices

                Pour un même fichier, un seul pattern doit fonctionner tout du long
                Première lecture :
                Au début les 4 patterns sont ok. Si on trouve un ligne qui correspond à un des 4 patterns, ceux qui ne correspondent pas à la ligne c'est chao. Si il n'y en a plus aucun de ok on stop
                Si il y en a plus que 1, on le choisit comme principal et on stop la première lecture
                À la fin, on prend le premier si yen a plusieurs
                Erreur si aucun pattern n'a jamais été trouvé
                Deuxième lecture :
                On écrit chaque ligne dans un nouveau fichier, et on modifie celles qui correspondent au pattern principal.

                """



            end_time = time.time()
            full_time = end_time - start_time
            full_time = "{:.6f}".format(full_time).rstrip("0").rstrip(".")

            print(f"\nFull execution time = {full_time} sec")

            return 0

    except ValueError as error:
        print(error)
        return -1
