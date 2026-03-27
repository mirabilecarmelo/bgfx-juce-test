# bgfx + JUCE Test

Minimal test project: JUCE window with bgfx 3D rendering via native child window.
bgfx auto-selects the best GPU backend: **Direct3D11/12** on Windows, **Metal** on macOS, **OpenGL** on Linux.

## Prerequisites

- **CMake** 3.22+
- **JUCE** installed locally
- **C++20** compiler (VS2022 on Windows, Xcode 14+ on macOS)

## Clone

```bash
git clone --recursive https://github.com/MudiStudio/bgfx-juce-test.git
cd bgfx-juce-test
```

If you forgot `--recursive`:
```bash
git submodule update --init --recursive
```

## Build on Windows

```bash
cmake -B build -G "Visual Studio 17 2022" -A x64 -DJUCE_PATH="C:/path/to/JUCE"
cmake --build build --config Release --target bgfx_juce_test
```

Executable: `build/bgfx_juce_test_artefacts/Release/bgfx JUCE Test.exe`

## Build on macOS

```bash
cmake -B build -G Xcode -DJUCE_PATH="$HOME/JUCE"
cmake --build build --config Release
```

Or with Makefiles:
```bash
cmake -B build -DJUCE_PATH="$HOME/JUCE"
cmake --build build --config Release
```

## Controls

- **Right-drag**: orbit camera
- **Scroll wheel**: zoom in/out
