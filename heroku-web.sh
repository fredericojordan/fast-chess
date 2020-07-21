#!/bin/sh
set -e

cmake .
make
pip install -r lichess/requirements.txt
python lichess/lichess.py
