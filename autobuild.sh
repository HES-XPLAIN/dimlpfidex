#!/bin/bash

ncpu=$(nproc --all)

[ ! -d ".venv" ] && python3 -m venv .venv

# git submodule init && git submodule update

source .venv/bin/activate && \
pip install . && \
mkdir -p build && \
cd build && \
cmake .. && \
cmake --build . -j $ncpu && \
cd .. && \
python3 -m build