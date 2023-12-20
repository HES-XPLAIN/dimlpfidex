import os
import subprocess
import sys


def build_package():
    cpu_count = 1
    cmake_command = []

    os.makedirs("build", exist_ok=True)
    os.chdir("build")

    # Platform-specific compilation commands
    if sys.platform.startswith('linux'):
        cmake_command = ['cmake', '..']
        cpu_count = len(os.sched_getaffinity(0))
    elif sys.platform == 'darwin':  # macOS
        cmake_command = ['cmake', '..']
        cpu_count = os.cpu_count()
    elif sys.platform == 'win32':  # Windows
        path = sys.prefix + '\\Lib\\site-packages'
        cmake_command = ['cmake', '-G', 'MinGW Makefiles', '-DCMAKE_PREFIX_PATH=' + path, '..']
        cpu_count = os.cpu_count()

    print("CPUs count:", cpu_count)
    build_command = ['cmake', '--build', '.', '-j', str(cpu_count)]

    # Run cmake
    subprocess.check_call(cmake_command)
    subprocess.check_call(build_command)


if __name__ == "__main__":
    # build with cmake
    build_package()
