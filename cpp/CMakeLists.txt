cmake_minimum_required(VERSION 3.21)
project(acd_va_models VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 20)

# Add CPM dependencies
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/)
include(CPM)
include(CMakeRC)
CPMAddPackage(
    NAME chowdsp_wdf
    GITHUB_REPOSITORY Chowdhury-DSP/chowdsp_wdf
    GIT_TAG 0dfd1bdf98d4276a637d314f6a97ea74778740bd
)
CPMAddPackage(
    NAME matplotlib-cpp
    GIT_REPOSITORY https://github.com/jatinchowdhury18/matplotlib-cpp
    GIT_TAG main
)
CPMAddPackage(
    NAME eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen
    GIT_TAG master
)
CPMAddPackage(
    NAME RTNeural
    GIT_REPOSITORY https://github.com/jatinchowdhury18/RTNeural
    GIT_TAG main
    OPTIONS "RTNEURAL_XSIMD ON"
)

# Set up plotting tool
add_executable(circuit_model_plotter circuit_model_plotter.cpp)
target_link_libraries(circuit_model_plotter PRIVATE matplotlib-cpp)

# WDF Configuration
target_link_libraries(circuit_model_plotter PRIVATE chowdsp::chowdsp_wdf)

# NDK Configuration
target_sources(circuit_model_plotter PRIVATE ndk/TS_NDK.cpp)
target_link_libraries(circuit_model_plotter PRIVATE Eigen3::Eigen)

# RNN Configuration
cmrc_add_resource_library(rnn-resources
    NAMESPACE rnn_files
    WHENCE rnn
    rnn/model_best_24.json
    rnn/model_best_12.json
)
set_target_properties(rnn-resources PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
target_link_libraries(circuit_model_plotter PRIVATE RTNeural rnn-resources)

# DWDF Configuration
cmrc_add_resource_library(dwdf-resources
    NAMESPACE dwdf_files
    WHENCE wdf
    wdf/1N4148_2x8.json
)
set_target_properties(dwdf-resources PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
target_link_libraries(circuit_model_plotter PRIVATE dwdf-resources)

# Configure other targets...
add_subdirectory(data_generator)
add_subdirectory(bench)
