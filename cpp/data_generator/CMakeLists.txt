find_package(SndFile REQUIRED)
if(NOT SndFile_FOUND)
    message(STATUS "Could not find libsndfile, skipping data_generator configuration")
    return()
endif()

message(STATUS "Found libsndfile, configuring data_generator tool...")

add_executable(data_generator data_generator.cpp)
target_link_libraries(data_generator PRIVATE SndFile::sndfile)

# WDF Configuration
target_link_libraries(data_generator PRIVATE chowdsp::chowdsp_wdf)
