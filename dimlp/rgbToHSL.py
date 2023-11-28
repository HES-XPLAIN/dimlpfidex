import numpy as np
import colorsys

def get_data(file_name): # Get data from file
    try:
        with open(file_name, "r") as my_file:
            data = []
            line = my_file.readline()
            while line:
                line = line.strip()  # Remove the line break at the end of the line
                di = [float(elt) for elt in line.split(" ")]
                data.append(di)
                line = my_file.readline()
            my_file.close()
        return data
    except (FileNotFoundError):
        raise ValueError(f"Error : File {file_name} not found.")
    except (IOError):
        raise ValueError(f"Error : Couldn't open file {file_name}.")

def output_data(data, data_file):
    try:
        with open(data_file, "w") as predFile:
            for var in data:
                for val in var:
                    predFile.write(str(val) + " ")
                predFile.write("\n")
            predFile.close()

    except (FileNotFoundError):
        raise ValueError(f"Error : File {data_file} not found.")
    except (IOError):
        raise ValueError(f"Error : Couldn't open file {data_file}.")

def rgb_to_hsl_fun(rgb):
    r, g, b = [x / 255.0 for x in rgb]
    h, l, s = colorsys.rgb_to_hls(r, g, b)
    return (h, s, l)

def rgb_to_hsl(data):
    hsl_data = np.apply_along_axis(rgb_to_hsl_fun, -1, data)
    return hsl_data


datafile = "datafiles/Cifar10/trainData.txt"
hslfile = "datafiles/Cifar10/trainDataHSL.txt"
size1d = 32
nb_channels=3

data = np.array(get_data(datafile))

data = data.reshape(data.shape[0], size1d, size1d, nb_channels)
data = rgb_to_hsl(data)
data = data.reshape(data.shape[0], size1d*size1d*nb_channels)
output_data(data, hslfile)
