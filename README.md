# dimlpfidex ![build](https://github.com/HES-XPLAIN/dimlpfidex/actions/workflows/build.yml/badge.svg)
Discretized Interpretable Multi Layer Perceptron (DIMLP) and related algorithms

## Contribution

### Get the project code

To get the latest source code, install [git](https://git-scm.com/) and [clone](https://docs.github.com/en/get-started/getting-started-with-git/about-remote-repositories) the repository:

```sh
$ git clone https://github.com/HES-XPLAIN/dimlpfidex.git
```

To download the required dependencies on your system, run:

```sh
$ git submodule init
$ git submodule update
```

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

### Install Python

Install [Python](https://www.python.org/):

#### Manually

* **Linux, macOS, Windows/WSL**: Use your package manager to install `python3` and `python3-dev`
* **Windows**: `winget install Python.Python.3.11`

> [!IMPORTANT]
> On Windows, avoid installing Python through the Microsoft Store as the package has additional permission restrictions.

#### Using Rye

- Install [Rye](https://rye-up.com/) and [add shims](https://rye-up.com/guide/installation/) to your PATH.

Ensure `rye` is accessible in the `$PATH` environment variable.
Rye will automatically download the suitable Python toolchain as needed.

To check the installation, check the following commands return an output:

```shell
rye --version
```

### Install Python dependencies

#### Using pip

```shell
python -m venv .venv
source .venv/bin/activate
pip install .
```

> [!NOTE]
> On Windows, use `.venv\Scripts\activate` instead.

#### Using Rye

Install python dependencies and create a virtualenv in `.venv`:

```shell
rye sync
```

#### Add dependencies

To add new dependencies to the project, either add them to the `pyproject.toml` file or use `rye add <dependency>`.
To add them to the virtualenv, use `pip install .` or `rye sync`.

### Work with virtualenv

To activate the virtualenv, use the standard methods:

* Unix: `source .venv/bin/activate`
* Windows: `.venv\Scripts\activate`

To leave the virtualenv, use `deactivate`.

### Compile

To compile:

```shell
python buildcpp.py
```

If CMake complains about not finding pybind11, ensure to activate the shell first.

**Note**: If you still have an issue on Windows, try the following commands to
build manually:

```shell
mkdir build && cd build
$path = 'C:\\Users\\<Username>\\<path_to>\\.venv\\Lib\\site-packages'
cmake.exe -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="$path" ..
cmake.exe --build .
```

Adjust the path to the virtual environment accordingly.

### Package

Create archives for distribution:

```shell
python -m build
```

### Install Pre-commit hooks

Git hooks are used to ensure quality checks are run by all developers every time
before a commit.

Install with `pip install pre-commit` or`rye sync`.

To enable pre-commit:

```shell
pre-commit install
```

Pre-commit hooks can be run manually with:

```shell
pre-commit run --all-files
```

## Release

To publish the package on [PyPI](https://pypi.org/project/dimlpfidex/), refer to [RELEASE](RELEASE.md).

## Credits
Our test suite is using [Obesity or CVD risk dataset](https://www.kaggle.com/datasets/aravindpcoder/obesity-or-cvd-risk-classifyregressorcluster) from [AravindPCoder](https://www.kaggle.com/aravindpcoder) (under CC BY-SA 4.0 license)
