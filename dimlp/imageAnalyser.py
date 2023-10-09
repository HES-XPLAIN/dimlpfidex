import time
import re
from PIL import Image
import numpy as np

from dimlpfidex import fidex, fidexGlo

def image_analyser():
    try:

        start_time = time.time()

        res_fid_glo = fidexGlo.fidexGlo("fidexGlo -S testSampleData.txt -p testSamplePred.txt -R withValid/globalRules.txt -O explanation.txt -F ../dimlp/datafiles/Mnist -r imgExplanationResult.txt")
        if res_fid_glo == -1:
            return -1

        no_rule_found_sentence = "We couldn't find any global explanation for this sample."
        has_global_rule = True
        rule = ""
        with open("datafiles/Mnist/explanation.txt", "r") as my_file:
            for line in my_file:
                if line.strip() == no_rule_found_sentence:
                    rule = False
                    break
                elif "->" in line:
                    rule = line.strip()
                    break

            my_file.close()

        if has_global_rule:
            print("Computation of result for rule :")
            print(rule)
        else:
            print("No rule global rule found. We launch Fidex.")
            res_fid = fidex.fidex("fidex -T mnistTrainData.txt -P predTrain.out -C mnistTrainClass.txt -S testSampleData.txt -c testSampleClass.txt -p testSamplePred.txt -W weights.wts -O imgFidexrule.txt -s imgFidexStats.txt -Q 50 -I 5 -i 100 -v 2 -d 0.5 -h 0.5 -R ../dimlp/datafiles/Mnist")
            if res_fid == -1:
                return -1

            with open("datafiles/Mnist/imgFidexrule.txt", "r") as my_file:
                for line in my_file:
                    if "->" in line:
                        rule = line.strip()
                        break
                my_file.close()

            print("Computation of result for this rule")

        # Get rule antecedants
        antecedants = []

        # Regular expression pattern to match antecedants
        pattern = r'X(\d+)\s*([<>]=?)\s*([\d.]+)'

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


        with open ("datafiles/Mnist/testSampleData.txt", "r") as my_file:
            line = my_file.readline().strip()
            baseimage = line.split(" ")
            my_file.close()
        # Convertir la liste en un tableau NumPy de forme (28, 28)
        image_array = np.array(baseimage).reshape(28, 28)

        # Créer une image à partir du tableau NumPy
        image = Image.fromarray(image_array.astype('uint8'))

        # Save and show image
        base_image_path = 'datafiles/Mnist/input_image.png'
        image.save(base_image_path)
        image.show()

        colorimage = [[v,v,v] for v in baseimage]

        for antecedant in antecedants:
            print(antecedant)
            if antecedant["inequality"] == "<":
                colorimage[antecedant["attribute"]]=[255,0,0]
            else:
                colorimage[antecedant["attribute"]]=[0,255,0]

        colorimage_array = np.array(colorimage).reshape(28, 28, 3)
        colorimage = Image.fromarray(colorimage_array.astype('uint8'))

        image_path = 'datafiles/Mnist/output_image.png'
        colorimage.save(image_path)
        colorimage.show()


        end_time = time.time()
        full_time = end_time - start_time
        full_time = "{:.6f}".format(full_time).rstrip("0").rstrip(".")

        print(f"\nFull execution time = {full_time} sec")
        return 0


    except ValueError as error:
        print(error)
        return -1


image_analyser()
