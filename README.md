# itsamonster

This is a simple C++ project using CMake for build management.

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

---
Feel free to modify or extend this README as needed for your project!
