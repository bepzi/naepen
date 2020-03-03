#pragma once

template<size_t T = 2048>
std::array<double, T> make_sawtooth() noexcept
{
    // Lowest freq we support is 20Hz
    std::array<double, T> out = {};

    double phase = 0.0f;
    double phase_delta = MathConstants<double>::twoPi / (double)(T);
    auto num_harmonics = (size_t)(48000.0 / (2.0 * 20.0));

    for (auto &f : out) {
        for (size_t i = 1; i <= num_harmonics; ++i) {
            float amp = 1.0f / i;
            f += std::sin(phase * i) * amp;
        }
        f /= 1.57415942;

        phase += phase_delta;
    }

    return out;
}

static auto sawtooth = make_sawtooth<2048>();