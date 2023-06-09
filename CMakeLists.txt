cmake_minimum_required(VERSION 3.25)
project(netpbm LANGUAGES C)

set(CMAKE_C_STANDARD 23)

set(SOURCE_FILES main.c ppm.h pgm.h pbm.h bayer.h types/ppm.h types/pgm.h types/pbm.h types/pixel.h types/sat.h sat.h)
set(HEADER_FILES ppm.h pgm.h pbm.h bayer.h sat.h types/ppm.h types/pgm.h types/pbm.h types/pixel.h types/sat.h)
set_source_files_properties(${SOURCE_FILES} PROPERTIES LANGUAGE C)
set_source_files_properties(${HEADER_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)

add_executable(netpbm ${SOURCE_FILES})

# Include the math library
target_link_libraries(netpbm m)

# Include the OpenMP library
find_package(OpenMP)
if (OpenMP_C_FOUND)
    target_link_libraries(netpbm OpenMP::OpenMP_C)
endif ()
