cmake_minimum_required(VERSION 3.21)
project(acd_va_models VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/)

include(CPM)

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

add_executable(circuit_model_plotter circuit_model_plotter.cpp)
target_link_libraries(circuit_model_plotter
    PRIVATE
        chowdsp::chowdsp_wdf
        matplotlib-cpp
)
