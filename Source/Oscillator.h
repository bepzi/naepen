#pragma once

template<size_t T = 2048>
std::array<double, T> make_sine() noexcept
{
    std::array<double, T> out = {};

    double phase = 0.0f;
    double phase_delta = MathConstants<double>::twoPi / (double)(T);

    for (auto &f : out) {
        f += std::sin(phase);
        phase += phase_delta;
    }

    return out;
}

template<size_t T = 2048>
std::array<double, T> make_triangle(double top_freq) noexcept
{
    std::array<double, T> out = {};

    double phase = 0.0f;
    double phase_delta = MathConstants<double>::twoPi / (double)(T);
    auto num_harmonics = (size_t)(48000.0 / (2.0 * top_freq));

    for (auto &f : out) {
        for (size_t i = 1; i <= num_harmonics; i += 2) {
            float amp = 1.0f / (float)(i * i);
            f += std::sin(phase * i) * amp;
        }

        phase += phase_delta;
    }

    return out;
}

template<size_t T = 2048>
std::array<double, T> make_square(double top_freq) noexcept
{
    std::array<double, T> out = {};

    double phase = 0.0f;
    double phase_delta = MathConstants<double>::twoPi / (double)(T);
    auto num_harmonics = (size_t)(48000.0 / (2.0 * top_freq));

    for (auto &f : out) {
        for (size_t i = 1; i <= num_harmonics; i += 2) {
            float amp = 1.0f / i;
            f += std::sin(phase * i) * amp;
        }

        phase += phase_delta;
    }

    return out;
}

template<size_t T = 2048>
std::array<double, T> make_sawtooth(double top_freq) noexcept
{
    std::array<double, T> out = {};

    double phase = 0.0f;
    double phase_delta = MathConstants<double>::twoPi / (double)(T);
    auto num_harmonics = (size_t)(48000.0 / (2.0 * top_freq));

    for (auto &f : out) {
        for (size_t i = 1; i <= num_harmonics; ++i) {
            float amp = 1.0f / i;
            f += std::sin(phase * i) * amp;
        }

        // Sine wave summation makes an "inverted" sawtooth, so flip it back over
        f = -f;

        phase += phase_delta;
    }

    return out;
}

static auto sine_wave = Wavetable<2048>(make_sine<2048>());
static auto triangle_wave = Wavetable<2048>(make_triangle<2048>(20.0));
static auto square_wave = Wavetable<2048>(make_square<2048>(20.0));
static auto sawtooth_wave = Wavetable<2048>(make_sawtooth<2048>(20.0));
