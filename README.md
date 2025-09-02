# Chemgine

**Real-time laboratory scale chemical simulation engine.**

# How to Build

Chemgine can be built using [CMake](https://cmake.org/download/) (v3.20+).
Bellow are a few common build configurations:

<details>
  <summary><strong><span style="font-size:1.2em;">Windows (MSVC)</span></strong></summary>

```sh
# Clone
git clone https://github.com/filip256/chemgine.git
cd chemgine

# Configure & Build
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config=Release
```
</details>

<details>
  <summary><strong><span style="font-size:1.2em;">Ubuntu (GCC/Clang)</span></strong></summary>

```sh
# Clone
git clone https://github.com/filip256/chemgine.git
cd chemgine

# Install dependencies
chmod +x setup_ubuntu.sh
./setup_ubuntu.sh

# Configure & Build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel $(nproc)
```
</details>

# How to Debug

<details>
  <summary><strong><span style="font-size:1.2em;">Ubuntu (GCC) using VS Code</span></strong></summary>

1. Ensure `gdb` is installed:
```sh
sudo apt update
sudo apt install gdb
```

2. Install the ```C/C++ Debug (gdb)``` *VS Code* extension.

3. Build in `Debug` mode:
```sh
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel $(nproc)
```

4. Create `.vscode/launch.json` from the preset [launch.json](dev/presets/launch.json):
```sh
cp dev/presets/launch.json .vscode
```

5. In *VS Code* go to `Run and Debug` (*Ctrl+Shift+D*), select the desired configuration and click run.

</details>

# How to Contribute
### Pre-commit:
```sh
pip install pre-commit
pre-commit --version  # v4.3.0+

cd <chemgine-project-root>
pre-commit install
```

### Visual Studio:
#### Automatic formatting:
[*Microsoft Visual Studio*](https://visualstudio.microsoft.com/) ships together with a *clang-format* version which is usually older than the latest version. This may cause issues with some of the rules defined in [.clang-format](.clang-format). To fix them:
 1. Download the latest [LLVM release](https://github.com/llvm/llvm-project/releases) (look for `clang+llvm-20.1.8-x86_64-pc-windows-msvc.tar.xz`)
 2. Extract `bin/clang-format.exe`
 3. In *Visual Studio* go to `Tools -> Options -> Text Editor -> C/C++ -> Code Style -> Formatting -> General`, tick `Use custom path to clang-format.exe` and browse for the updated `clang-format.exe`.
