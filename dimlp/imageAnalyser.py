import time
import re
from PIL import Image
import numpy as np

from dimlpfidex import fidex, fidexGlo
from .trnFun import get_data

def output_data(datas, file, type=""):
    try:
        with open(file, "w") as myFile:
            for val in datas:
                if type=="itg":
                    myFile.write(str(int(val)) + " ")
                else:
                    myFile.write(str(val) + " ")
            myFile.close()

    except (FileNotFoundError):
        raise ValueError(f"Error : File {file} not found.")
    except (IOError):
        raise ValueError(f"Error : Couldn't open file {file}.")

def imageAnalyser():
    try:
        start_time = time.time()

        id_samples = range(0,10)
        show_images = False
        image_folder_from_base = "dimlp/datafiles/Mnist"
        test_data_file = image_folder_from_base + "/mnistTestData.txt"
        test_class_file = image_folder_from_base + "/mnistTestClass.txt"
        test_pred_file = image_folder_from_base + "/withValid/predTest.out"
        global_rules = "withValid/globalRulesWithTestStats.rls"

        train_data_file = "mnistTrainData.txt"
        train_class_file = "mnistTrainClass.txt"
        train_pred_file = "withValid/predTrain.out"
        weights_file = "withValid/weights.wts"
        image_save_folder = image_folder_from_base + "/imagesTest"

        test_data = get_data(test_data_file)
        test_class = get_data(test_class_file)
        test_pred = get_data(test_pred_file)
        no_rule_found_sentence = "We couldn't find any global explanation for this sample."
        pattern = r'X(\d+)\s*([<>]=?)\s*([\d.]+)' # Regular expression pattern to match antecedants
        explanation_file = image_folder_from_base + "/explanation.txt"
        test_sample_data_file = image_folder_from_base + "/testSampleData.txt"
        test_sample_pred_file = image_folder_from_base + "/testSamplePred.txt"
        test_sample_class_file = image_folder_from_base + "/testSampleClass.txt"
        img_fidex_file = image_folder_from_base + "/imgFidexrule.txt"

        nb_fidex = 0 # Number of times it needs to use Fidex

        for id_sample in id_samples:

            print("\n---- Sample ", id_sample, " ----")

            test_sample_data = test_data[id_sample] if 0 <= id_sample < len(test_data) else None
            test_sample_class = test_class[id_sample] if 0 <= id_sample < len(test_class) else None
            test_sample_pred = test_pred[id_sample] if 0 <= id_sample < len(test_pred) else None

            output_data(test_sample_data, test_sample_data_file, "itg")
            output_data(test_sample_class, test_sample_class_file, "itg")
            output_data(test_sample_pred, test_sample_pred_file)

            fidexglo_command = "fidexGlo -S testSampleData.txt -p testSamplePred.txt -R " + global_rules + " -O explanation.txt -F " + image_folder_from_base# + " -r imgExplanationResult.txt"
            res_fid_glo = fidexGlo.fidexGlo(fidexglo_command)
            if res_fid_glo == -1:
                return -1

            has_global_rule = True
            rule = ""
            with open(explanation_file, "r") as my_file:
                for line in my_file:
                    if line.strip() == no_rule_found_sentence:
                        rule = False
                        has_global_rule = False
                        break
                    elif "->" in line:
                        rule = line.strip()
                        break

                my_file.close()

            if has_global_rule:
                print("Computation of result for rule :")
                print(rule)
            else:
                nb_fidex += 1
                print("No rule global rule found. We launch Fidex.")
                fidex_command = "fidex -T " + train_data_file + " -P " + train_pred_file + " -C " + train_class_file
                fidex_command += " -S testSampleData.txt -c testSampleClass.txt -p testSamplePred.txt -W "
                fidex_command += weights_file + " -O imgFidexrule.txt -s imgFidexStats.txt -Q 50 -I 5 -i 100 -v 2 -d 0.5 -h 0.5 -R " +  image_folder_from_base
                res_fid = fidex.fidex(fidex_command)
                if res_fid == -1:
                    return -1

                with open(img_fidex_file, "r") as my_file:
                    for line in my_file:
                        if "->" in line:
                            rule = line.strip()
                            break
                    my_file.close()

                print("Computation of result for this rule")

            # Get rule antecedants
            antecedants = []

            # Find all matches in the input string
            matches = re.findall(pattern, rule)

            # List to store antecedants
            antecedants = []

            # Process each match and store in antecedants
            for match in matches:
                attribute, inequality, value = match
                antecedant = {
                    "attribute": int(attribute),
                    "inequality": inequality,
                    "value": float(value)
                }
                antecedants.append(antecedant)

            with open (test_sample_data_file, "r") as my_file:
                line = my_file.readline().strip()
                baseimage = line.split(" ")
                my_file.close()

            colorimage = [[v,v,v] for v in baseimage]

            for antecedant in antecedants:
                if antecedant["inequality"] == "<":
                    colorimage[antecedant["attribute"]]=[255,0,0]
                else:
                    colorimage[antecedant["attribute"]]=[0,255,0]

            colorimage_array = np.array(colorimage).reshape(28, 28, 3)
            colorimage = Image.fromarray(colorimage_array.astype('uint8'))

            image_path = image_save_folder + '/img_'+ str(id_sample) + '_out.png'
            colorimage.save(image_path)
            if show_images:
                colorimage.show()
        if nb_fidex == 0:
            fidex_mean = 0
        else:
            fidex_mean = nb_fidex/len(id_samples)*100

        print(f"\nFidex is used for {fidex_mean}% of images.")

        end_time = time.time()
        full_time = end_time - start_time
        full_time = "{:.6f}".format(full_time).rstrip("0").rstrip(".")

        print(f"\nFull execution time = {full_time} sec")
        return 0


    except ValueError as error:
        print(error)
        return -1


imageAnalyser()
