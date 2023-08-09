import os
import re
import subprocess
import sys
from glob import glob
from pybind11.setup_helpers import Pybind11Extension, build_ext


def build(setup_kwargs):
    # Define the extension ext_modules
    ext_modules = [
        Pybind11Extension(
            "example",
            sorted(glob("cpp/src/*.cpp")),
            extra_compile_args=['-O3', '-pthread'],
            language='c++',
            cxx_std=11
        ),
        Pybind11Extension(
            "dimlp",
            sorted(glob("cpp/src/*.cpp") + glob("cpp/src/*.h")),
            extra_compile_args=['-O3', '-pthread'],
            language='c++',
            cxx_std=11
        ),
        Pybind11Extension(
            "fidex",
            sorted(glob("cpp/src/*.cpp") + glob("cpp/src/*.h")),
            extra_compile_args=['-O3', '-pthread'],
            language='c++',
            cxx_std=11
        ),
        Pybind11Extension(
            "fidexGlo",
            sorted(glob("cpp/src/*.cpp") + glob("cpp/src/*.h")),
            extra_compile_args=['-O3', '-pthread'],
            language='c++',
            cxx_std=11
        ),
    ]

    setup_kwargs.update(
        {
            "ext_modules": [ext_modules],
            "cmd_class": {"build_ext": build_ext},
            "zip_safe": False,
        }
    )


def build_package():
    global cpu_count, cmake_command

    os.makedirs("build", exist_ok=True)
    os.chdir("build")

    # Platform-specific compilation commands
    if sys.platform.startswith('linux'):
        cmake_command = ['poetry', 'run', 'cmake', '..']
        cpu_count = len(os.sched_getaffinity(0))
    elif sys.platform == 'darwin':  # macOS
        cmake_command = ['poetry', 'run', 'cmake', '..']
        cpu_count = os.cpu_count()
    elif sys.platform == 'win32':  # Windows
        output = subprocess.check_output("poetry env info", shell=True, text=True)
        path = re.search(r"Path:\s+(.*)", output).group(1) if re.search(r"Path:\s+(.*)", output) else None
        cmake_command = ['poetry', 'run', 'cmake', '-G', 'MinGW Makefiles', '-DCMAKE_PREFIX_PATH=' + path, '..']
        cpu_count = os.cpu_count()

    print("CPUs count:", cpu_count)
    build_command = ['poetry', 'run', 'cmake', '--build', '.', '-j', str(cpu_count)]

    # Run cmake
    subprocess.check_call(cmake_command)
    subprocess.check_call(build_command)


if __name__ == "__main__":
    # # Define the setup parameters
    # setup_kwargs = {
    #     "name": "my_package",
    #     "version": "0.1.0",
    #     "description": "My package description",
    #     # "ext_modules": [extension_mod],
    #     # "zip_safe": False,
    # }
    # build(setup_kwargs)

    # build with cmake
    build_package()
