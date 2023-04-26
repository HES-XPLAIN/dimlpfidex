import os
import sys

if sys.platform == 'win32':
    dir_path = os.path.abspath('dimlpfidex')
    os.add_dll_directory(dir_path)

# from dimlpfidex import example
from dimlpfidex import dimlp

def main():
    # print(example.cpp_hello())
    # print(example.cpp_add(7, 5))
    # print(example.cpp_fib(7))
    print(dir(dimlp))


if __name__ == '__main__':
    main()
