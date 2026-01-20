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

# Copy compile_commands.json to the repository root for tools that expect it
# there. On Windows, normalize drive letter casing to avoid path-case
# inconsistencies in tooling.
if(CMAKE_EXPORT_COMPILE_COMMANDS)
  set(_cc_src "${CMAKE_BINARY_DIR}/compile_commands.json")
  set(_cc_dst "${CMAKE_SOURCE_DIR}/compile_commands.json")

  if(WIN32)
    add_custom_target(
      slayergit_copy_compile_commands ALL
      COMMAND
        ${CMAKE_COMMAND} -DCC_SRC:FILEPATH=${_cc_src}
        -DCC_DST:FILEPATH=${_cc_dst} -P
        ${CMAKE_SOURCE_DIR}/cmake/normalize_compile_commands.cmake
      BYPRODUCTS ${_cc_dst}
      DEPENDS ${_cc_src}
      VERBATIM)
  else()
    add_custom_target(
      slayergit_copy_compile_commands ALL
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${_cc_src} ${_cc_dst}
      BYPRODUCTS ${_cc_dst}
      DEPENDS ${_cc_src}
      VERBATIM)
  endif()
endif()

# Add FTXUI submodule
add_subdirectory(submodules/FTXUI)

# UI library - contains all UI components
add_library(
  slayergit_ui STATIC src/ui/window_tab.cpp src/ui/window.cpp
                      src/ui/window_manager.cpp src/ui/input_handler.cpp)

target_link_libraries(slayergit_ui PUBLIC ftxui::screen ftxui::dom
                                          ftxui::component)

target_include_directories(slayergit_ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Main executable
add_executable(slayergit src/main.cpp)

# Link UI library
target_link_libraries(slayergit PRIVATE slayergit_ui)
