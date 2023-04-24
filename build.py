from glob import glob
from pybind11.setup_helpers import Pybind11Extension, build_ext


def build(setup_kwargs):
    ext_modules = [
        Pybind11Extension(
            "example",
            sorted(glob("cpp/src/*.cpp"))
        ),
        Pybind11Extension(
            "dimlp",
            sorted(glob("cpp/src/*.cpp") + glob("cpp/src/*.h"))
        ),
        Pybind11Extension(
            "fidex",
            sorted(glob("cpp/src/*.cpp") + glob("cpp/src/*.h"))
        ),
        Pybind11Extension(
            "fidexGlo",
            sorted(glob("cpp/src/*.cpp") + glob("cpp/src/*.h"))
        ),
        Pybind11Extension(
            "hyperLocus",
            sorted(glob("cpp/src/*.cpp") + glob("cpp/src/*.h"))
        ),
    ]
    setup_kwargs.update({
        "ext_modules": ext_modules,
        "cmd_class": {"build_ext": build_ext},
        "zip_safe": False,
    })
