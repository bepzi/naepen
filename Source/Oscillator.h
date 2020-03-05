#pragma once

template<size_t T = 2048>
std::array<double, T> make_sawtooth() noexcept
{
    const double lowest_freq = 20.0;
    std::array<double, T> out = {};

    double phase = 0.0f;
    double phase_delta = MathConstants<double>::twoPi / (double)(T);
    auto num_harmonics = (size_t)(48000.0 / (2.0 * lowest_freq));

    for (auto &f : out) {
        for (size_t i = 1; i <= num_harmonics; ++i) {
            float amp = 1.0f / i;
            f += std::sin(phase * i) * amp;
        }

        phase += phase_delta;
    }

    return out;
}

static auto sawtooth = make_sawtooth<2048>();