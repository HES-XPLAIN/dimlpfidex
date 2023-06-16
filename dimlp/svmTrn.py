import argparse

def check_strictly_positive(variable):
    if isinstance(variable, (float,int)) and variable > 0:
        return True
    return False


def svmTrn(*args, **kwargs):
    if args or not kwargs:
        print("---------------------------------------------------------------------")
        print("Please specify arguments using named parameters.")
        print("----------------------------")
        print("Obligatory parameters :")
        print("train_data : train data file")
        print("train_class : train class file")
        print("test_data : test data file")
        print("----------------------------")
        print("Optional parameters :")
        print("test_class : test class file")
        print("K : Parameter to improve dynamics (=1 by default)")
        print("----------------------------")
        print("Here is an example, keep same parameter names :")
        print('svmTrn(trainData="trainDataFile.txt", testData="testDataFile.txt")')
        print("---------------------------------------------------------------------")
    else:

        # Get parameters
        train_data = kwargs.get('train_data')
        train_class = kwargs.get('train_class')
        test_data = kwargs.get('test_data')
        test_class = kwargs.get('test_class')
        K = kwargs.get('K')

        if train_data is None :
            print('Error : train data file missing, add it with option train_data="your_train_data_file"')
            return
        if train_class is None :
            print('Error : train class file missing, add it with option train_class="your_train_class_file"')
            return
        if test_data is None :
            print('Error : test data file missing, add it with option test_data="your_test_data_file"')
            return
        if K is None:
            K = 1
        else:
            if (not check_strictly_positive(K)):
                print('Error, parameter K is not a strictly positive number')
                return

        print(train_data)
        print(test_data)
        print(K)
