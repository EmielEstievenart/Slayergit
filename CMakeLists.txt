cmake_minimum_required(VERSION 3.21)
project(
  SlayerGit
  VERSION 0.1.0
  LANGUAGES CXX)

# Use C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for tooling (clangd, etc.)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add FTXUI submodule
add_subdirectory(submodules/FTXUI)

# Main executable
add_executable(slayergit src/main.cpp)

# Link FTXUI libraries
target_link_libraries(slayergit PRIVATE ftxui::screen ftxui::dom
                                        ftxui::component)

# Ensure proper include directories
target_include_directories(slayergit PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
