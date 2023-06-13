cmake_minimum_required(VERSION 3.25)
project(netpbm LANGUAGES C)

set(CMAKE_C_STANDARD 23)

set(SOURCE_FILES ppm.c pgm.c pbm.c sat.c)
set_source_files_properties(${SOURCE_FILES} PROPERTIES LANGUAGE C)

# Add the library as a target
add_library(netpbm SHARED ${SOURCE_FILES})

# Include the math library
target_link_libraries(netpbm m)

# Include the OpenMP library
find_package(OpenMP)
if (OpenMP_C_FOUND)
  target_link_libraries(netpbm OpenMP::OpenMP_C)
endif ()

# Add the executable as a target
add_executable(netpbm-bin main.c)

# Link the library to the executable
target_link_libraries(netpbm-bin netpbm)
