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

# UI library - contains all UI components
add_library(
  slayergit_ui STATIC
  src/ui/window.cpp
  src/ui/window_manager.cpp
  src/ui/tabs/unstaged_tab.cpp
  src/ui/tabs/staged_tab.cpp
  src/ui/tabs/local_branches_tab.cpp
  src/ui/tabs/remote_branches_tab.cpp
  src/ui/tabs/tags_tab.cpp
  src/ui/tabs/commits_tab.cpp
  src/ui/tabs/reflog_tab.cpp
  src/ui/tabs/stashes_view.cpp
  src/ui/tabs/details_view.cpp)

target_link_libraries(slayergit_ui PUBLIC ftxui::screen ftxui::dom
                                          ftxui::component)

target_include_directories(slayergit_ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Main executable
add_executable(slayergit src/main.cpp)

# Link UI library
target_link_libraries(slayergit PRIVATE slayergit_ui)
