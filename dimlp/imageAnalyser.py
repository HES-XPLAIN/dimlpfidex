import time
import re
from PIL import Image
import numpy as np
import colorsys
import os

from dimlpfidex import fidex, fidexGlo

def get_data(file_name): # Get data from file
    try:
        with open(file_name, "r") as my_file:
            data = []
            line = my_file.readline()
            while line:
                line = line.strip()  # Remove the line break at the end of the line
                if line:
                    di = [float(elt) for elt in line.split(" ")]
                    data.append(di)
                line = my_file.readline()
            my_file.close()
        return data
    except (FileNotFoundError):
        raise ValueError(f"Error : File {file_name} not found.")
    except (IOError):
        raise ValueError(f"Error : Couldn't open file {file_name}.")

def hsl_to_rgb(data):
    rgb_data = np.apply_along_axis(hsl_to_rgb_fun, -1, data)
    return rgb_data

def hsl_to_rgb_fun(hsl):
    h, s, l = [float(x) for x in hsl]
    r, g, b = colorsys.hls_to_rgb(h, l, s)
    r, g, b = int(r*255), int(g*255), int(b*255)
    return (r, g, b)

def write_lines(filename, lines):
    with open(filename, 'w') as file:
        for line in lines:
            for val in line:
                file.write(str(val) + " ")
            file.write("\n")

def imageAnalyser(dataSet):
    try:
        start_time = time.time()

        id_samples = range(0,5)
        show_images = False
        if dataSet == "Mnist":
            image_folder_from_base = "dimlp/datafiles/Mnist"
            test_data_file = image_folder_from_base + "/mnistTestData.txt"
            test_class_file = image_folder_from_base + "/mnistTestClass.txt"
            test_pred_file = image_folder_from_base + "/predTest.out"
            global_rules = "globalRules.txt"

            train_data_file = "mnistTrainData.txt"
            train_class_file = "mnistTrainClass.txt"
            train_pred_file = "predTrain.out"

            with_file = True
            rules_file = "globalRules.txt"
            weights_file = "weights.wts"

            dropout_dim = 0.9
            dropout_hyp = 0.9
            size1d = 28
            nb_channels = 1
            with_hsl = False
            normalized = False
        elif dataSet == "Cifar10":
            with_hsl = True
            normalized = True #True by default ...jsp

            image_folder_from_base = "dimlp/datafiles/Cifar10/Cifar10HSLResnet"
            test_data_file = image_folder_from_base + "/testDataHSL.txt"
            test_class_file = image_folder_from_base + "/testClass.txt"
            test_pred_file = image_folder_from_base + "/predTest.out"
            global_rules = "globalRulesWithTestStats.txt"

            train_data_file = "trainDataHSL.txt"
            train_class_file = "trainClass.txt"
            train_pred_file = "predTrain.out"

            with_file = False #Using of rule file or of weights to launch Fidex
            rules_file = "globalRules.txt"
            weights_file = "weights.wts"

            dropout_dim = 0.9
            dropout_hyp = 0.9
            size1d = 32
            nb_channels = 3
        elif dataSet == "fer":
            normalized = True # Data between 0 and 1
            with_hsl = False

            image_folder_from_base = "dimlp/datafiles/FER"
            #image_folder_from_base = "/mnt/d/dimlpfidex/FER"
            test_data_file = image_folder_from_base + "/testData.txt"
            test_class_file = image_folder_from_base + "/testClass.txt"
            test_pred_file = image_folder_from_base + "/predTest.out"
            global_rules = "globalRulesWithTestStats.txt"

            train_data_file = "trainData.txt"
            train_class_file = "trainClass.txt"
            train_pred_file = "predTrain.out"

            with_file = False #Using of rule file or of weights to launch Fidex
            rules_file = "globalRules.txt"
            weights_file = "weights.wts"

            dropout_dim = 0.9
            dropout_hyp = 0.9
            size1d = 48
            nb_channels = 1

        image_save_folder = image_folder_from_base + "/imagesTest"

        test_data = get_data(test_data_file)
        test_class = get_data(test_class_file)
        test_pred = get_data(test_pred_file)
        pattern = r'X(\d+)\s*([<>]=?)\s*([\d.]+)' # Regular expression pattern to match antecedants

        # Create test files with desired samples
        lines_test_data = [test_data[i] for i in id_samples if i < len(test_data)]
        lines_test_class = [test_class[i] for i in id_samples if i < len(test_class)]
        lines_test_pred = [test_pred[i] for i in id_samples if i < len(test_pred)]

        test_data_samples = "test_data_samples.txt"
        test_pred_samples = "test_pred_samples.txt"
        test_class_samples = "test_class_samples.txt"

        write_lines(image_folder_from_base + "/" + test_data_samples, lines_test_data)
        write_lines(image_folder_from_base + "/" + test_pred_samples, lines_test_pred)
        write_lines(image_folder_from_base + "/" + test_class_samples, lines_test_class)

        # Lauch fidexGlo to get explanation

        fidexglo_command = "fidexGlo -S " + test_data_samples + " -p " + test_pred_samples + " -c " + test_class_samples
        fidexglo_command += " -R " + global_rules + " -O explanation.txt -F " + image_folder_from_base # + " -r imgExplanationResult.txt "
        fidexglo_command += " -w true -T " + train_data_file + " -P " + train_pred_file + " -C " + train_class_file
        if with_file:
            fidexglo_command += " -f " + rules_file
        else:
            fidexglo_command += " -W " + weights_file
        fidexglo_command += " -Q 100 -i 25"
        fidexglo_command += " -d " + str(dropout_dim) + " -h " + str(dropout_hyp)

        res_fid_glo = fidexGlo.fidexGlo(fidexglo_command)
        if res_fid_glo == -1:
            raise ValueError('Error during execution of FidexGlo')

        # Get one explanation rule for each test sample
        #import antecedant
        explanation_file = image_folder_from_base + "/explanation.txt"

        rules = []
        with open(explanation_file, "r") as my_file:
            for line in my_file:
                if line.startswith("R1: Rule "):
                    rules.append(line.strip())
                if line.startswith("Local rule :"):
                    # Search next non empty line
                    next_line = next(my_file, '').strip()
                    while not next_line:
                        next_line = next(my_file, '').strip()
                    rules.append(next_line)
            my_file.close()

        # Find all matches in the input string
        for id_sample in range(len(rules)):
            antecedants = []
            matches = re.findall(pattern, rules[id_sample])

            # Process each match and store in antecedants
            for match in matches:
                attribute, inequality, value = match
                antecedant = {
                    "attribute": int(attribute),
                    "inequality": inequality,
                    "value": float(value)
                }
                antecedants.append(antecedant)

            # Generate test sample images with colored pixels where the rule is activated
            baseimage = lines_test_data[id_sample]

            if nb_channels == 1:
                if normalized:
                   colorimage = [[int(255*float(v)),int(255*float(v)),int(255*float(v))] for v in baseimage]
                else:
                    colorimage = [[v,v,v] for v in baseimage]
            else:
                colorimage = baseimage
                if with_hsl:
                    #Image back to RGB
                    colorimage = np.array(colorimage)
                    colorimage = colorimage.reshape(size1d, size1d, nb_channels)
                    colorimage = hsl_to_rgb(colorimage)
                    colorimage = colorimage.reshape(size1d*size1d*nb_channels)
                elif normalized:
                    colorimage = [int(float(v) * 255) for v in colorimage]

            for antecedant in antecedants:
                if antecedant["inequality"] == "<":
                    if nb_channels == 1:
                        colorimage[antecedant["attribute"]]=[255,0,0]
                    else:
                        colorimage[antecedant["attribute"] - (antecedant["attribute"] % 3)]=255
                        colorimage[antecedant["attribute"] - (antecedant["attribute"] % 3)+1]=0
                        colorimage[antecedant["attribute"] - (antecedant["attribute"] % 3)+2]=0
                else:
                    if nb_channels == 1:
                        colorimage[antecedant["attribute"]]=[0,255,0]
                    else:
                        colorimage[antecedant["attribute"] - (antecedant["attribute"] % 3)]=0
                        colorimage[antecedant["attribute"] - (antecedant["attribute"] % 3)+1]=255
                        colorimage[antecedant["attribute"] - (antecedant["attribute"] % 3)+2]=0

            colorimage_array = np.array(colorimage).reshape(size1d, size1d, 3)
            colorimage = Image.fromarray(colorimage_array.astype('uint8'))

            image_path = image_save_folder + '/img_'+ str(id_sample) + '_out.png'
            colorimage.save(image_path)
            if show_images:
                colorimage.show()

        end_time = time.time()
        full_time = end_time - start_time
        full_time = "{:.6f}".format(full_time).rstrip("0").rstrip(".")

        print(f"\nFull image analysis execution time = {full_time} sec")
        return 0

    except ValueError as error:
        print(error)
        return -1

#dataSet = "Mnist"
dataSet = "Cifar10"
#dataSet = "fer"
imageAnalyser(dataSet)
