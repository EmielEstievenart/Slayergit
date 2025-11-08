# SlayerGit

A blazing-fast, intuitive terminal UI (TUI) for Git operations, built with modern C++ and FTXUI.

## 🚀 Quick Start

### Prerequisites

- CMake 3.21 or higher
- C++17 compatible compiler
- Ninja build system (recommended)
- Git

### Building the Project

#### Using CMake Presets (Recommended)

```powershell
# Configure for debug build
cmake --preset debug

# Build
cmake --build build/debug

# Run
.\build\debug\slayergit.exe
```

#### Available Presets

- **debug** - Debug build with full symbols
- **release** - Optimized release build
- **relwithdebinfo** - Optimized build with debug symbols

#### Alternative Build Commands

```powershell
# Configure for release
cmake --preset release

# Build release
cmake --build build/release

# Run release build
.\build\release\slayergit.exe
```

### First Time Setup

After cloning, initialize the FTXUI submodule:

```powershell
git submodule update --init --recursive
```

## 📁 Project Structure

```
SlayerGit/
├── CMakeLists.txt          # Root CMake configuration
├── CMakePresets.json       # CMake presets for easy configuration
├── src/
│   └── main.cpp            # Application entry point
├── submodules/
│   └── FTXUI/              # FTXUI library (git submodule)
├── docs/                   # Architecture and design documents
└── examples/               # Example implementations
```

## 🎯 Architecture

SlayerGit follows a clean, layered architecture:

- **UI Layer** - FTXUI components and rendering
- **Application Layer** - State management and orchestration
- **Core Layer** - Git operations and business logic
- **Infrastructure Layer** - Git process execution and parsing

See `docs/00-architecture.md` for complete architectural details.

## 🔧 Development

### CMake Configuration

The project uses modern CMake practices with:
- Target-based configuration (no global settings)
- CMakePresets.json for consistent builds
- C++17 standard
- Automatic compile commands export (for IDE/clangd)

### Build Options

CMake automatically configures FTXUI with:
- Examples disabled
- Tests disabled
- Documentation disabled
- Only library components built

## 📚 Documentation

- [Architecture](docs/00-architecture.md) - Comprehensive system design
- [Decisions & Rationale](docs/decisions_rationale.md) - Why we made specific choices
- [Kickoff Document](docs/kickoff.md) - Project overview and goals

## ⚡ Current Status

This is the initial CMake foundation with a simple FTXUI "Hello World" to verify the build system works correctly.

Press 'q' to quit the application.

## 📝 License

See LICENSE file for details.
