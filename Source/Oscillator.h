#pragma once

#include <JuceHeader.h>

template<unsigned int T = 1024, unsigned int R = 44100>
class Oscillator {
public:
    Oscillator<T, R>() noexcept = default;

    void set_freq(float freq) noexcept
    {
        static_assert(R > 0);
        idx_delta = freq * ((float)T / R);
    }

    [[nodiscard]] forcedinline float get_next_sample() noexcept
    {
        // Linearly interpolate between two sample values
        auto idx0 = (unsigned int)std::floor(idx);
        auto idx1 = idx0 + 1;

        float lower = buf[idx0];
        float upper = buf[idx1];

        float frac = idx - (float)idx0;
        float sample = lower + frac * (upper - lower);

        if ((idx += idx_delta) > (float)T) {
            idx -= (float)T;
        }

        return sample;
    }

    [[nodiscard]] constexpr unsigned int get_table_size() const noexcept
    {
        return T;
    }

    [[nodiscard]] unsigned int get_num_harmonics() const noexcept
    {
        return num_harmonics;
    }

protected:
    float buf[T + 1] = {};

    /// How many harmonics were used to synthesize the values in the buffer.
    ///
    /// If this number is too high and you try to play a high-frequency tone,
    /// you may get aliasing. Make sure the frequencies you want to play are
    /// compatible with this table.
    unsigned int num_harmonics = 1;

    float idx = 0.0f;
    float idx_delta = 0.0f;
};

template<unsigned int T = 1024, unsigned int R = 44100>
class SineOscillator : public Oscillator<T, R> {
public:
    SineOscillator<T, R>() noexcept
    {
        static_assert(T > 1);

        float phase = 0.0f;
        float phase_delta = MathConstants<float>::twoPi / (float)(T - 1);

        for (unsigned int i = 0; i < T; ++i) {
            this->buf[i] = std::sin(phase);
            phase += phase_delta;
        }

        this->buf[T] = this->buf[0];
    }
};

template<unsigned int T = 1024, unsigned int R = 44100>
class SquareOscillator : public Oscillator<T, R> {
public:
    explicit SquareOscillator<T, R>(unsigned int num_harmonics) noexcept
    {
        static_assert(T > 1);
        jassert(num_harmonics >= 1);
        this->num_harmonics = num_harmonics;

        float phase = 0.0f;
        float phase_delta = MathConstants<float>::twoPi / (float)(T - 1);

        for (auto i = 0; i < T; ++i) {
            for (auto h = 1; h <= num_harmonics; h += 2) {
                float amp = 1.0f / h;
                this->buf[i] += std::sin(phase * h) * amp;
            }

            phase += phase_delta;
        }

        this->buf[T] = this->buf[0];
    }
};

template<unsigned int T = 1024, unsigned int R = 44100>
class SawtoothOscillator : public Oscillator<T, R> {
public:
    explicit SawtoothOscillator<T, R>(unsigned int num_harmonics) noexcept
    {
        static_assert(T > 1);
        jassert(num_harmonics >= 1);
        this->num_harmonics = num_harmonics;

        float phase = 0.0f;
        float phase_delta = MathConstants<float>::twoPi / (float)(T - 1);

        // TODO: Account for the Gibbs phenomenon

        for (auto i = 0; i < T; ++i) {
            for (auto h = 1; h <= num_harmonics; ++h) {
                float amp = 1.0f / h;
                this->buf[i] += std::sin(phase * h) * amp;
            }

            phase += phase_delta;
        }

        this->buf[T] = this->buf[0];
    }
};

template<unsigned int T = 1024, unsigned int R = 44100>
class TriangleOscillator : public Oscillator<T, R> {
public:
    explicit TriangleOscillator<T, R>(unsigned int num_harmonics) noexcept
    {
        static_assert(T > 1);
        jassert(num_harmonics >= 1);
        this->num_harmonics = num_harmonics;

        float phase = 0.0f;
        float phase_delta = MathConstants<float>::twoPi / (float)(T - 1);

        for (auto i = 0; i < T; ++i) {
            for (auto h = 1; h <= num_harmonics; h += 2) {
                float amp = 1.0f / (float)(h * h);
                this->buf[i] += std::sin(phase * h) * amp;
            }

            phase += phase_delta;
        }

        this->buf[T] = this->buf[0];
    }
};