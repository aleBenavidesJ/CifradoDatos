# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\CifradoDatos_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\CifradoDatos_autogen.dir\\ParseCache.txt"
  "CifradoDatos_autogen"
  )
endif()
