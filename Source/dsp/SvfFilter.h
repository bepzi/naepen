#pragma once

#include <JuceHeader.h>

class SvfFilter {
public:
    struct Params {
        double cutoff;
        double Q;
    };

    enum class Type { LOWPASS = 1, HIGHPASS, BANDPASS };

    void set_type(SvfFilter::Type type) noexcept
    {
        this->current_type = type;
        reset();
    }

    void set_params(const SvfFilter::Params &params, double sample_rate) noexcept
    {
        g = std::tan((MathConstants<double>::pi * params.cutoff) / sample_rate);
        k = 1 / params.Q;
        a1 = 1 / (1 + g * (g + k));
        a2 = g * a1;
        a3 = g * a2;

        switch (current_type) {
        case Type::LOWPASS:
            m0 = 0.0;
            m1 = 0.0;
            m2 = 1.0;
            break;
        case Type::HIGHPASS:
            m0 = 1.0;
            m1 = -k;
            m2 = -1.0;
            break;
        case Type::BANDPASS:
            m0 = 0.0;
            m1 = 1.0;
            m2 = 0.0;
            break;
        }
    }

    [[nodiscard]] float forcedinline get_next_sample(float curr_sample) noexcept
    {
        double v3 = curr_sample - ic2eq;
        double v1 = (a1 * ic1eq) + (a2 * v3);
        double v2 = ic2eq + (a2 * ic1eq) + (a3 * v3);
        ic1eq = (2 * v1) - ic1eq;
        ic2eq = (2 * v2) - ic2eq;

        return (float)((m0 * curr_sample) + (m1 * v1) + (m2 * v2));
    }

    void reset() noexcept
    {
        ic1eq = 0.0;
        ic2eq = 0.0;
    }

    void forcedinline apply_to_buffer(AudioBuffer<float> &buffer) noexcept
    {
        {
            float *c0_buf = buffer.getWritePointer(0);
            for (int i = 0; i < buffer.getNumSamples(); ++i) {
                c0_buf[i] = get_next_sample(c0_buf[i]);
            }
        }

        const float *c0_buf = buffer.getReadPointer(0);
        for (int channel = 1; channel < buffer.getNumChannels(); ++channel) {
            float *buf = buffer.getWritePointer(channel);
            std::memcpy(buf, c0_buf, sizeof(float) * buffer.getNumSamples());
        }
    }

private:
    Type current_type = Type::LOWPASS;

    double ic1eq = 0.0, ic2eq = 0.0;
    double g = 0.0, k = 0.0, a1 = 0.0, a2 = 0.0, a3 = 0.0;
    double m0 = 0.0, m1 = 0.0, m2 = 0.0;
};
