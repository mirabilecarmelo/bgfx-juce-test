# bgfx + JUCE Test

Minimal test project: JUCE window with bgfx 3D rendering via native child window.
bgfx auto-selects the best GPU backend per platform — **zero code changes** between platforms.

| Platform | GPU Backend | Status |
|----------|-------------|--------|
| Windows  | Direct3D 11/12 | Tested |
| macOS    | Metal | To verify |
| Linux    | OpenGL | Untested |

## Prerequisites

### Windows
- **Visual Studio 2022** (Community edition is fine)
- **CMake** 3.22+ (included with VS2022 or install separately)
- **JUCE** — download from [juce.com](https://juce.com/download) and extract (e.g. `C:\JUCE`)

### macOS
- **Xcode** 14+ (install from App Store)
- **CMake** 3.22+ (`brew install cmake`)
- **JUCE** — download from [juce.com](https://juce.com/download) and extract (e.g. `~/JUCE`)

## Clone

```bash
git clone --recursive https://github.com/mirabilecarmelo/bgfx-juce-test.git
cd bgfx-juce-test
```

If you forgot `--recursive`:
```bash
git submodule update --init --recursive
```

## Build on Windows

Open **Developer Command Prompt for VS 2022** (or any terminal with CMake in PATH):

```bash
cmake -B build -G "Visual Studio 17 2022" -A x64 -DJUCE_PATH="C:/path/to/JUCE"
cmake --build build --config Release --target bgfx_juce_test
```

Run:
```bash
build\bgfx_juce_test_artefacts\Release\bgfx JUCE Test.exe
```

Expected output: window with dark background, debug text showing `Renderer: Direct3D 11`.

## Build on macOS

### Option 1: Xcode project

```bash
cmake -B build -G Xcode -DJUCE_PATH="$HOME/JUCE"
open build/bgfx_juce_test.xcodeproj
```

Then in Xcode: select **bgfx_juce_test** scheme, Product > Run.

### Option 2: Command line (Unix Makefiles)

```bash
cmake -B build -DJUCE_PATH="$HOME/JUCE" -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(sysctl -n hw.ncpu)
```

Run:
```bash
open build/bgfx_juce_test_artefacts/bgfx\ JUCE\ Test.app
```

Expected output: window with dark background, debug text showing `Renderer: Metal`.

## Controls

- **Right-drag**: orbit camera (azimuth / elevation)
- **Scroll wheel**: zoom in / out

## Project Structure

```
bgfx-juce-test/
├── CMakeLists.txt       # Build config (JUCE + bgfx)
├── bgfx.cmake/          # bgfx library (git submodule)
└── src/
    ├── Main.cpp          # JUCE Application entry point
    ├── MainComponent.h   # Container component
    ├── BgfxComponent.h   # bgfx rendering component
    └── BgfxComponent.cpp # Child window + bgfx init + render loop
```

## How it works

1. `BgfxComponent` creates a **native child window** (HWND on Windows) inside the JUCE component
2. bgfx renders to the child window — no conflict with JUCE's own painting
3. `bgfx::renderFrame()` called before `bgfx::init()` enables **single-threaded mode**
4. Timer at 60Hz drives the render loop
5. bgfx auto-selects the best renderer: DX11/12 (Win), Metal (Mac), GL (Linux)
