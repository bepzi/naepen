#pragma once

#include <JuceHeader.h>

template<unsigned int T = 1024>
class Oscillator {
public:
    Oscillator() noexcept = default;

    /**
     * Copy constructor for creating a new oscillator with the same values as the other.
     */
    Oscillator(const Oscillator &other) noexcept :
        num_harmonics(other.num_harmonics), idx(other.idx), idx_delta(other.idx_delta)
    {
        std::memcpy(this->buf, other.buf, sizeof(float) * (T + 1));
    }

    virtual ~Oscillator() noexcept = default;

    void set_freq(float freq, int sample_rate) noexcept
    {
        if (sample_rate <= 0 || freq < 1.0f) {
            // Treat this as a "reset", we don't want to generate any more samples
            idx_delta = 0.0f;
            return;
        }

        idx_delta = freq * ((float)T / (float)sample_rate);
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

    /*
    [[nodiscard]] constexpr unsigned int get_table_size() const noexcept
    {
        return T;
    }

    [[nodiscard]] unsigned int get_num_harmonics() const noexcept
    {
        return num_harmonics;
    }
    */

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

template<unsigned int T = 1024>
class SineOscillator : public Oscillator<T> {
public:
    SineOscillator() noexcept
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

template<unsigned int T = 1024>
class SquareOscillator : public Oscillator<T> {
public:
    explicit SquareOscillator(unsigned int num_harmonics) noexcept
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

template<unsigned int T = 1024>
class SawtoothOscillator : public Oscillator<T> {
public:
    explicit SawtoothOscillator(unsigned int num_harmonics) noexcept
    {
        static_assert(T > 1);
        jassert(num_harmonics >= 1);
        this->num_harmonics = num_harmonics;

        float phase = 0.0f;
        float phase_delta = MathConstants<float>::twoPi / (float)(T - 1);

        // TODO: Account for the Gibbs phenomenon

        for (unsigned i = 0; i < T; ++i) {
            for (unsigned int h = 1; h <= num_harmonics; ++h) {
                float amp = 1.0f / h;
                this->buf[i] += std::sin(phase * h) * amp;
            }

            phase += phase_delta;
        }

        this->buf[T] = this->buf[0];
    }
};

template<unsigned int T = 1024>
class TriangleOscillator : public Oscillator<T> {
public:
    explicit TriangleOscillator(unsigned int num_harmonics) noexcept
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