# Normalizes Windows drive letters in compile_commands.json by lowercasing them.
# Usage: cmake -DCC_SRC=... -DCC_DST=... -P normalize_compile_commands.cmake

if(NOT DEFINED CC_SRC OR CC_SRC STREQUAL "")
  message(FATAL_ERROR "CC_SRC must be provided")
endif()

if(NOT DEFINED CC_DST OR CC_DST STREQUAL "")
  message(FATAL_ERROR "CC_DST must be provided")
endif()

if(NOT EXISTS "${CC_SRC}")
  message(FATAL_ERROR "compile_commands.json not found at ${CC_SRC}")
endif()

file(READ "${CC_SRC}" _contents)

# Normalize drive letters to lowercase (applies globally)
foreach(
  _drive_letter
  A
  B
  C
  D
  E
  F
  G
  H
  I
  J
  K
  L
  M
  N
  O
  P
  Q
  R
  S
  T
  U
  V
  W
  X
  Y
  Z)
  string(TOLOWER "${_drive_letter}" _drive_letter_lower)
  string(REPLACE "${_drive_letter}:" "${_drive_letter_lower}:" _contents
                 "${_contents}")
endforeach()

get_filename_component(_cc_dst_dir "${CC_DST}" DIRECTORY)
if(NOT _cc_dst_dir STREQUAL "")
  file(MAKE_DIRECTORY "${_cc_dst_dir}")
endif()

file(WRITE "${CC_DST}" "${_contents}")
message(STATUS "Wrote ${CC_DST}")
