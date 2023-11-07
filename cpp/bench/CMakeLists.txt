add_executable(circuit_model_bench circuit_model_bench.cpp)
target_link_libraries(circuit_model_bench PRIVATE benchmark::benchmark)

# WDF Configuration
target_link_libraries(circuit_model_bench PRIVATE chowdsp::chowdsp_wdf)

# NDK Configuration
target_sources(circuit_model_bench PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../ndk/TS_NDK.cpp)
target_link_libraries(circuit_model_bench PRIVATE Eigen3::Eigen)

# RNN Configuration
target_link_libraries(circuit_model_bench PRIVATE RTNeural rnn-resources)
