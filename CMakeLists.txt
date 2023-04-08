cmake_minimum_required(VERSION 3.25)
project(Netpbm LANGUAGES C)

set(CMAKE_C_STANDARD 23)

set(SOURCE_FILES main.c ppm.h pgm.h pbm.h)
set(HEADER_FILES ppm.h pgm.h pbm.h)
set_source_files_properties(${SOURCE_FILES} PROPERTIES LANGUAGE C)
set_source_files_properties(${HEADER_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)

add_executable(Netpbm ${SOURCE_FILES})

# Include the math library
target_link_libraries(Netpbm m)