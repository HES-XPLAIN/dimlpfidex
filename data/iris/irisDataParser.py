#Iris data parser

import random

with open("iris.data", "r") as f:
    datas = []
    line = f.readline()
    while line:
        line = line.strip()
        line = line.split(",")
        datas.append(line)
        line = f.readline()
    del datas[-1]
    random.shuffle(datas)
    f.close()


classes = [d[-1] for d in datas]
attributs = [d[:-1] for d in datas]

#4/5ème des données vont dans l'entrainement
#1/5 vont dans le test

nbSamples = len(classes)
cutoff = int((4/5)*nbSamples)

trainClasses = classes[:cutoff]
trainAttributs = attributs[:cutoff]
testClasses = classes[cutoff:]
testAttributs = attributs[cutoff:]

f = open("irisTrainClass.txt", "w")
for classe in trainClasses:
    match classe:
        case "Iris-setosa":
            f.write("1 0 0\n")
        case "Iris-versicolor":
            f.write("0 1 0\n")
        case "Iris-virginica":
            f.write("0 0 1\n")
f.close()

f = open("irisTestClass.txt", "w")
for classe in testClasses:
        match classe:
            case "Iris-setosa":
                f.write("1 0 0\n")
            case "Iris-versicolor":
                f.write("0 1 0\n")
            case "Iris-virginica":
                f.write("0 0 1\n")
f.close()

f = open("irisTrainData.txt", "w")
for data in trainAttributs:
    for attribut in data:
        f.write(attribut+" ")
    f.write("\n")
f.close()

f = open("irisTestData.txt", "w")
for data in testAttributs:
    for attribut in data:
        f.write(attribut+" ")
    f.write("\n")
f.close()

