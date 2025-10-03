# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles\\wcam_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\wcam_autogen.dir\\ParseCache.txt"
  "wcam_autogen"
  )
endif()
