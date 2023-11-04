#include "../wdf/ts_wdf.hpp"
#include <iostream>
#include <sndfile.h>

template <typename T>
std::tuple<std::vector<T>, int> read_file (const char* path)
{
    SF_INFO file_info {};
    auto* file_handle = sf_open (path, SFM_READ, &file_info);
    if (file_handle == nullptr)
    {
        std::cout << "Unable to open input file!" << std::endl;
        std::cout << "libsndfile error: " << sf_strerror (nullptr) << std::endl;
        return {};
    }

    if (file_info.channels != 1)
    {
        sf_close (file_handle);
        std::cout << "Input files are required to be mono!" << std::endl;
        return {};
    }

    std::vector<T> data {};
    data.resize (file_info.frames);
    if constexpr (std::is_same_v<T, float>)
        sf_read_float (file_handle, data.data(), file_info.frames);
    else if constexpr (std::is_same_v<T, double>)
        sf_read_double (file_handle, data.data(), file_info.frames);

    sf_close (file_handle);

    return { data, file_info.samplerate };
}

template <typename T>
void write_file (const char* path, std::span<const T> data, int samplerate)
{
    SF_INFO file_info {
        .frames = static_cast<sf_count_t> (data.size()),
        .samplerate = samplerate,
        .channels = 1,
        .format = SF_FORMAT_WAV | SF_FORMAT_FLOAT,
    };
    auto* file_handle = sf_open (path, SFM_WRITE, &file_info);
    if (file_handle == nullptr)
    {
        std::cout << "Unable to open output file!" << std::endl;
        std::cout << "libsndfile error: " << sf_strerror (nullptr) << std::endl;
        return;
    }

    if constexpr (std::is_same_v<T, float>)
        sf_write_float (file_handle, data.data(), data.size());
    else if constexpr (std::is_same_v<T, double>)
        sf_write_double (file_handle, data.data(), data.size());

    sf_close (file_handle);
}

int main (int argc, char* argv[])
{
    //    if (argc != 4 || std::string { argv[1] } == "--help")
    //    {
    //        std::cout << "Usage: data_generator <data_dir> <dist_value>" << std::endl;
    //        return 1;
    //    }

    static constexpr int sample_rate = 96000;
    static constexpr int N = 4 * sample_rate;
    std::vector<float> in_data;
    in_data.resize ((size_t) N, 0.0f);
    for (size_t n = 0; n < N; ++n)
    {
        const auto freq = [n]
        {
            if (n < (size_t) sample_rate)
                return 50.0f;
            else if (n < 2 * (size_t) sample_rate)
                return 100.0f;
            else if (n < 3 * (size_t) sample_rate)
                return 250.0f;
            else
                return 500.0f;
        }();
        in_data[n] = 0.999f * std::sin (2.0f * (float) M_PI * (float) n * freq / sample_rate);
    }

    write_file<float> ("train_input_96k.wav", in_data, sample_rate);

    TS_WDF ts_wdf {};
    for (std::pair<float, const char*> config : { std::make_pair (0.0f, "out_00.wav"),
                                                  std::make_pair (0.001f, "out_0001.wav"),
                                                  std::make_pair (0.01f, "out_001.wav"),
                                                  std::make_pair (0.1f, "out_01.wav"),
                                                  std::make_pair (0.3f, "out_03.wav"),
                                                  std::make_pair (0.7f, "out_07.wav"),
                                                  std::make_pair (1.0f, "out_10.wav") })
    {
        std::vector<float> out_data { in_data.begin(), in_data.end() };
        std::transform (out_data.begin(), out_data.end(), out_data.begin(), [] (float x)
                        { return 0.2f * x + 4.5f; });

        ts_wdf.set_distortion (config.first);
        ts_wdf.prepare (static_cast<float> (sample_rate));
        ts_wdf.process (out_data, out_data);

        std::transform (out_data.begin(), out_data.end(), out_data.begin(), [] (float x)
                        { return (x - 4.5f) / 2.3f; });
        std::cout << *std::max_element (out_data.begin(), out_data.end()) << ", ";
        std::cout << *std::min_element (out_data.begin(), out_data.end()) << std::endl;
        write_file<float> (config.second, out_data, sample_rate);
    }

    return 0;
}
