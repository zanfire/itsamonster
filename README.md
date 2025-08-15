# itsamonster

Fight simulator for monster in Dungeons and Dragons 5ed.
Using C++ and CMake for build management.

## Dependencies
- **C++ Compiler**: Any standard C++ compiler (e.g., MSVC, GCC, Clang)
- **CMake**: Version 3.10 or higher recommended

## Building the Project

1. **Install CMake**
   - Download and install from [cmake.org](https://cmake.org/download/)

2. **Generate Build Files**
   - Open a terminal in the project root directory.
   - Run:
     ```powershell
     cmake -S . -B build
     ```

3. **Build the Project**
   - Run:
     ```powershell
     cmake --build build
     ```

4. **Run the Executable**
   - The built executable will be in `build/Debug/itsamonster.exe` (on Windows, Debug configuration).
   - Run it from the terminal:
     ```powershell
     .\build\Debug\itsamonster.exe
     ```

## Notes
- For release builds, use:
  ```powershell
  cmake --build build --config Release
  ```
- You may need to install a C++ compiler if not already available.

## Project Structure
- `src/` - Source files
- `CMakeLists.txt` - CMake build configuration
- `build/` - Generated build files and binaries

## GoogleTest Submodule Setup
Tests use GoogleTest/GoogleMock provided as a git submodule at `extern/googletest`.

### Cloning (preferred)
```powershell
git clone --recurse-submodules <repo-url>
```

If you already cloned without submodules:
```powershell
git submodule update --init --recursive
```

### Updating googletest later
```powershell
cd extern/googletest
git fetch --tags origin
git checkout v1.15.2   # choose desired tag
cd ../../
git add extern/googletest
git commit -m "Update googletest to v1.15.2"
```

### Reconfigure & build tests
```powershell
cmake -S . -B build -D ITSAMONSTER_ENABLE_TESTS=ON
cmake --build build --config Debug --target itsamonster_tests
ctest --test-dir build -C Debug --output-on-failure
```

If the submodule is missing while tests are enabled, CMake emits a warning.

