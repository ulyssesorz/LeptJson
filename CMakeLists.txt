project(Test)
cmake_minimum_required(VERSION 3.1.0)

# Use C++17
add_compile_options(-std=c++17 -g)
 
aux_source_directory(. DIR_SRCS)
message(${DIR_SRCS})
 
INCLUDE_DIRECTORIES(
include
)
add_subdirectory(src)
 
 
add_executable(${PROJECT_NAME} ${DIR_SRCS})
target_link_libraries(${PROJECT_NAME} static_lib )

enable_testing()
find_package(GTest REQUIRED) # Find the google testing framework on your system
include_directories(${GTEST_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${GTEST_LIBRARIES}) # Replace ${PROJECT_NAME} with your target name
