cmake_minimum_required(VERSION 3.10.1)

project(estimateRatio)

# set(CMAKE_CXX_STANDARD 14)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

set(CXX_STANDARD_REQUIRED ON) #Make C++20 a hard requirement
set(CMAKE_CXX_STANDARD 14) # Default C++ standard for targets


add_library(obtainCompressionResults STATIC src/obtainCompressionResults.cpp)
target_include_directories(obtainCompressionResults PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(obtainCompressionResults PUBLIC cxx_std_14)

add_library(extractFeatures STATIC src/extractFeatures.cpp)
target_include_directories(extractFeatures PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(extractFeatures PUBLIC cxx_std_14)




###########
add_executable(estimateRatio main.cpp)
target_link_libraries(estimateRatio PUBLIC extractFeatures obtainCompressionResults)