cmake_minimum_required(VERSION 3.10.1)

project(ratioQualityModeling)

# set(CMAKE_CXX_STANDARD 14)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

set(CXX_STANDARD_REQUIRED ON) #Make C++20 a hard requirement
set(CMAKE_CXX_STANDARD 14) # Default C++ standard for targets


add_library(predictRatio STATIC src/predictRatio.cpp)
target_include_directories(predictRatio PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(predictRatio PUBLIC cxx_std_14)


###########
add_executable(ratioQualityModeling main.cpp)
target_link_libraries(ratioQualityModeling PUBLIC predictRatio)