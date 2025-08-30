#!/usr/bin/env bash
set -euo pipefail

DEPS=(
    # General:
    build-essential
    cmake
    git

    # SFML:
    libxrandr-dev
    libxcursor-dev
    libxi-dev
    libudev-dev
    libfreetype-dev
    libflac-dev
    libvorbis-dev
    libgl1-mesa-dev
    libegl1-mesa-dev
)

echo "Installing dependencies..."
sudo apt update
sudo apt install -y "${DEPS[@]}"

echo "Dependencies installed successfully!"
