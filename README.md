# dimlpfidex
Discretized Interpretable Multi Layer Perceptron (DIMLP) and related algorithms

## Contribution

### Install C++ toolchain

#### Linux, macOS, Windows/WSL

Install with your package manager:

* a C++ compiler (gcc/g++)
* cmake

#### Windows

* Install [MinGW-W64](https://www.mingw-w64.org/) with the [online installer](https://github.com/Vuniverse0/mingwInstaller/releases) (64 bit, win32, ucrt)
* Install [CMake](https://cmake.org/):

```shell
winget install Kitware.CMake
```

Ensure `cmake.exe` is accessible in the `$PATH` environment variable.

```shell
cmake.exe --version
```

### Install Python and Poetry

#### Linux, macOS, Windows/WSL

Use your package manager to install:

* python
* poetry

#### Windows

* Install [Python](https://www.python.org/)

```shell
winget install Python.Python.3.10
```

* Install [poetry](https://python-poetry.org/docs/#installation) and add it to your PATH.

Ensure `python.exe` and `poetry.exe` are accessible in the `$PATH` environment variable.

To check the installation, check these commands return an output:

```shell
python.exe --version
poetry.exe --version
```

### Compile

Activate the virtual environment:

```shell
poetry shell
```

Install python dependencies and compile:

```shell
poetry install
poetry build
```

If CMake complains about not finding pybind11, ensure to activate the shell first.

**Note**: If you still have an issue on Windows, try the following commands to
build manually:

```shell
mkdir build && cd build
$path = (poetry env info | Select-String -Pattern 'Path:\s+(.*)').Matches.Groups[1].Value
cmake.exe -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="$path" ..
cmake.exe --build .
```

### Install Pre-commit hooks

Git hooks are used to ensure quality checks are run by all developers every time
before a commit.

```shell
poetry shell
pre-commit install
```

Pre-commit hooks can be run manually with:

```shell
pre-commit run --all-files
```

## Release

To publish the package on [PyPI](https://pypi.org/project/dimlpfidex/), refer to [RELEASE](RELEASE.md).
