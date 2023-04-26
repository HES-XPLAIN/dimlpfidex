# dimlpfidex
Discretized Interpretable Multi Layer Perceptron (DIMLP) and related algorithms

## Contribution

### Install C++ toolchain

#### Linux, macOS, Windows/WSL

Install with your package manager:

* a C++ compiler (gcc/g++)
* make
* cmake

#### Windows

* Install [MinGW-W64](https://www.mingw-w64.org/) with the [online installer](https://github.com/Vuniverse0/mingwInstaller/releases) (64 bit, POSIX, ucrt)
* Install CMake:

```shell
winget install Kitware.CMake
```

Ensure `mingw32-make.exe`, `cmake.exe` are accessible in the `$PATH` environment variable.

```shell
mingw32-make.exe --version
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
winget install Python.Python.3.11
```

* Install [poetry](https://python-poetry.org/docs/#installation) and add it to your PATH.

Ensure `python.exe` and `poetry.exe` are accessible in the `$PATH` environment variable.

To check the installation, check these commands return an output:

```shell
python.exe --version
poetry.exe --version
```

### Compile

Install python dependencies and activate the virtualenv:

```shell
poetry install
poetry shell
```

#### Linux, macOS, Windows/WSL

At the **root** of the project, compile the submodules:
```
mkdir build && cd build
cmake ..
cmake --build .
```

#### Windows

Get your virtualenv path:
```shell
poetry env info | Select-String -Pattern "Path:" | Select-String -Pattern "virtualenvs"
```

At the **root** of the project, compile the submodules:
```shell
mkdir build && cd build
cmake.exe -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="<virtualenv path>" ..
cmake.exe --build .
```

### Install Pre-commit hooks

Git hooks are used to ensure quality checks are run by all developers every time
before a commit.

```shell
pre-commit install
```

Pre-commit hooks can be run manually with:

```shell
pre-commit run --all-files
```
