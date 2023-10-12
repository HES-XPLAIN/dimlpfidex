import time
import re
from PIL import Image
import numpy as np


from dimlpfidex import fidex, fidexGlo
from trnFun import get_data

def output_data(datas, file, type=""):
    try:
        with open(file, "w") as myFile:
            for data in datas:
                for val in data:
                    if type=="itg":
                        myFile.write(str(int(val)) + " ")
                    else:
                        myFile.write(str(val) + " ")
                myFile.write("\n")
            myFile.close()

    except (FileNotFoundError):
        raise ValueError(f"Error : File {file} not found.")
    except (IOError):
        raise ValueError(f"Error : Couldn't open file {file}.")

def image_analyser():
    try:

        start_time = time.time()

        id_samples = [0,2]
        show_images = False
        image_folder_from_base = "../dimlp/datafiles/Mnist"
        image_folder = "datafiles/Mnist"
        test_data_file = image_folder_from_base + "/mnistTestData.txt"
        test_class_file = image_folder_from_base + "/mnistTestClass.txt"
        test_pred_file = image_folder_from_base + "/withValid/predTest.out"

        train_data_file = "mnistTrainData.txt"
        train_class_file = "mnistTrainClass.txt"
        train_pred_file = "withValid/predTrain.out"
        weights_file = "withValid/predTrain.out"

        image_save_folder = image_folder + "/images"

        test_data = get_data(test_data_file)
        test_class = get_data(test_class_file)
        test_pred = get_data(test_pred_file)


        test_samples_data = [test_data[i] if 0 <= i < len(test_data) else None for i in id_samples]
        test_samples_class = [test_class[i] if 0 <= i < len(test_class) else None for i in id_samples]
        test_samples_pred = [test_pred[i] if 0 <= i < len(test_pred) else None for i in id_samples]

        output_data(test_samples_data, image_folder + "/testSampleData.txt", "itg")
        output_data(test_samples_class, image_folder + "/testSampleClass.txt", "itg")
        output_data(test_samples_pred, image_folder + "/testSamplePred.txt")




        fidexglo_command = "fidexGlo -S testSampleData.txt -p testSamplePred.txt -R withValid/globalRules.txt -O explanation.txt -F " + image_folder_from_base + " -r imgExplanationResult.txt"
        res_fid_glo = fidexGlo.fidexGlo(fidexglo_command)
        if res_fid_glo == -1:
            return -1

        no_rule_found_sentence = "We couldn't find any global explanation for this sample."
        new_sample_sentence = "Explanation for sample"
        sample_count = -1
        explanation_file = image_folder + "/explanation.txt"
        rule = False
        has_global_rule = 0

        pattern = r'X(\d+)\s*([<>]=?)\s*([\d.]+)'   # Regular expression pattern to match antecedants
        base_images = []
        with open (image_folder + "/testSampleData.txt", "r") as my_file:
            for line in my_file:
                line = line.strip()
                base_images.append(line.split(" "))
            my_file.close()

        go = False
        with open(explanation_file, "r") as my_file:
            for line in my_file:
                if new_sample_sentence in line.strip():
                    rule = False
                    has_global_rule = 0
                    sample_count += 1
                    print("\n---- Sample ", id_samples[sample_count], " ----")
                    go = True
                #if line.strip() == no_rule_found_sentence:
                if go:
                    has_global_rule = -1
                    print("No global rule found. We launch Fidex.")
                    fidex_command = "fidex -T " + train_data_file + " -P " + train_pred_file + " -C " + train_class_file
                    fidex_command += " -S testSampleData.txt -c testSampleClass.txt -p testSamplePred.txt -W "
                    fidex_command += weights_file + " -O imgFidexrule.txt -s imgFidexStats.txt -Q 50 -I 5 -i 100 -v 2 -d 0.5 -h 0.5 -R " +  image_folder_from_base
                    #res_fid = fidex.fidex(fidex_command)
                    res_fid = fidex.fidex("fidex -T mnistTrainData.txt -P withValid/predTrain.out -C mnistTrainClass.txt -S testSampleData.txt -c testSampleClass.txt -p testSamplePred.txt -W withValid/weights.wts -O imgFidexrule.txt -s imgFidexStats.txt -Q 50 -I 5 -i 100 -v 2 -d 0.5 -h 0.5 -R ../dimlp/datafiles/Mnist")

                    if res_fid == -1:
                        return -1

                    img_fidex_file = image_folder + "/imgFidexrule.txt"
                    with open(img_fidex_file, "r") as my_fidex_file:
                        for line in my_fidex_file:
                            if "->" in line:
                                rule = line.strip()
                                break
                        my_fidex_file.close()
                    print("Computation of result for this rule")
                    go = False
                """
                elif "->" in line and has_global_rule == 0:
                    has_global_rule = 1
                    rule = line.strip()
                    print("Computation of result for rule :")
                    print(rule)

                if has_global_rule == 1:
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

                    # Convertir la liste en un tableau NumPy de forme (28, 28)
                    image_array = np.array(base_images[sample_count]).reshape(28, 28)
                    # Créer une image à partir du tableau NumPy
                    image = Image.fromarray(image_array.astype('uint8'))
                    # Save and show image
                    base_image_path = image_save_folder + '/img_'+ str(id_samples[sample_count]) + '_in.png'
                    image.save(base_image_path)
                    if show_images:
                        image.show()
                    colorimage = [[v,v,v] for v in base_images[sample_count]]

                    for antecedant in antecedants:
                        if antecedant["inequality"] == "<":
                            colorimage[antecedant["attribute"]]=[255,0,0]
                        else:
                            colorimage[antecedant["attribute"]]=[0,255,0]
                    colorimage_array = np.array(colorimage).reshape(28, 28, 3)
                    colorimage = Image.fromarray(colorimage_array.astype('uint8'))
                    image_path = image_save_folder + '/img_'+ str(id_samples[sample_count]) + '_out.png'
                    colorimage.save(image_path)
                    if show_images:
                        colorimage.show()

                    has_global_rule = -1
"""
            my_file.close()







        end_time = time.time()
        full_time = end_time - start_time
        full_time = "{:.6f}".format(full_time).rstrip("0").rstrip(".")

        print(f"\nFull execution time = {full_time} sec")
        return 0


    except ValueError as error:
        print(error)
        return -1


image_analyser()
