cmake_minimum_required(VERSION 3.25)
project(netpbm LANGUAGES C)

set(CMAKE_C_STANDARD 23)

set(SOURCE_FILES ppm.c pgm.c pbm.c)
set(HEADER_FILES bayer.h types/ppm.h types/pgm.h types/pbm.h types/pixel.h)
set_source_files_properties(${SOURCE_FILES} PROPERTIES LANGUAGE C)
set_source_files_properties(${HEADER_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)

# Add the executable
add_executable(run main.c)

# Add the library
add_library(netpbm SHARED ${SOURCE_FILES} ${HEADER_FILES})

# Include the math library
target_link_libraries(netpbm m)

# Include the OpenMP library
find_package(OpenMP)
if (OpenMP_C_FOUND)
    target_link_libraries(netpbm OpenMP::OpenMP_C)
endif ()

# Include netpbm library to the executable
target_link_libraries(run netpbm)