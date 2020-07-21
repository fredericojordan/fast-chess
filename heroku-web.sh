#!/bin/sh
set -e

apt-get update
apt-get install -y g++ cmake libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev
cmake .
make
pip install -r lichess/requirements.txt
python lichess/lichess.py
