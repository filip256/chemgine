# Chemgine

Real-time laboratory scale chemical simulation engine.

## How to Build

### Windows (MSVC)
```sh
git clone https://github.com/filip256/chemgine.git
cd chemgine

cmake -B build -G "Visual Studio 17 2022 -A x64"
cmake --build build --config=Release
```

### Ubuntu (GCC/Clang)
```sh
git clone https://github.com/filip256/chemgine.git
cd chemgine

chmod +x setup_ubuntu.sh
./setup_ubuntu.sh

cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 16
```
