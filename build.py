import os
import platform
import subprocess
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
        Pybind11Extension(
            "hyperLocus",
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
    # Build the C++ library
    os.makedirs("build", exist_ok=True)
    os.chdir("build")
    subprocess.run(["cmake", ".."])
    subprocess.run(["cmake", "--build", "."])

    # Return to the project root directory
    os.chdir("..")


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
