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
    if (argc != 4 || std::string { argv[1] } == "--help")
    {
        std::cout << "Usage: data_generator <in_file> <out_file> <dist_value>" << std::endl;
        return 1;
    }

    const auto [in_data, sample_rate] = read_file<float> (argv[1]);
    if (in_data.empty())
        return 1;

    std::vector<float> out_data { in_data.begin(), in_data.end() };
    std::transform (out_data.begin(), out_data.end(), out_data.begin(), [] (float x) { return 0.2f * x + 4.5f; });

    TS_WDF ts_wdf {};
    ts_wdf.set_distortion (static_cast<float> (std::atof (argv[3])));
    ts_wdf.prepare (static_cast<float> (sample_rate));
    ts_wdf.process (out_data, out_data);

    std::transform (out_data.begin(), out_data.end(), out_data.begin(), [] (float x) { return 0.016f * (x - 4.5f); });
    write_file<float> (argv[2], out_data, sample_rate);

    return 0;
}
