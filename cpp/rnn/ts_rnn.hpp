#pragma once

#include <cmrc/cmrc.hpp>
#include <RTNeural/RTNeural.h>
#include <span>
#include <string_view>

CMRC_DECLARE (rnn_files);

template <int hidden_size>
struct TS_RNN
{
    explicit TS_RNN (const std::string& model_file_name)
    {
        const auto fs = cmrc::rnn_files::get_filesystem();
        const auto model_file = fs.open (model_file_name);
        const auto model_json = nlohmann::json::parse (std::string_view { model_file.begin(), model_file.size() });

        const auto& state_dict = model_json.at ("state_dict");
        RTNeural::torch_helpers::loadLSTM<float> (state_dict, "rec.", model.template get<0>());
        RTNeural::torch_helpers::loadDense<float> (state_dict, "lin.", model.template get<1>());
    }

    void prepare (double sample_rate)
    {
        assert (sample_rate == 96000.0);
        model.reset();
    }

    void set_distortion (float dist_01)
    {
        dist_param = dist_01;
    }

    void process (std::span<const float> in_data, std::span<float> out_data) noexcept
    {
        alignas (16) std::array<float, xsimd::batch<float>::size> model_in {};
        model_in[1] = dist_param;

        for (size_t n = 0; n < out_data.size(); ++n)
        {
            model_in[0] = 5.0f * (in_data[n] - 4.5f);
            out_data[n] = model.forward (model_in.data()) + model_in[0];
            out_data[n] = out_data[n] * 2.3f + 4.5f;
        }
    }

    float dist_param = 0.0;

    static constexpr int num_conditions = 1;
    static constexpr int num_inputs = num_conditions + 1;
    static constexpr int num_outputs = 1;
    RTNeural::ModelT<float,
                     num_inputs,
                     num_outputs,
                     RTNeural::LSTMLayerT<float, num_inputs, hidden_size>,
                     RTNeural::DenseT<float, hidden_size, num_outputs>>
        model;
};
