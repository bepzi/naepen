#pragma once

#include <JuceHeader.h>

/*
static double ic1eq = 0;
static double ic2eq = 0;

static double cutoff = 3000.0;
static double Q = 2;

double g = std::tan(MathConstants<double>::pi * cutoff / getSampleRate());
double k = 1 / Q;
double a1 = 1 / (1 + g * (g + k));
double a2 = g * a1;
double a3 = g * a2;
double m0 = 0;
double m1 = 0;
double m2 = 1;

// TICK:
double v0 = c0_buf[i];
double v3 = v0 - ic2eq;
double v1 = (a1 * ic1eq) + (a3 * v3);
double v2 = ic2eq + (a2 * ic1eq) + (a3 * v3);
ic1eq = (2 * v1) - ic1eq;
ic2eq = (2 * v2) - ic2eq;

c0_buf[i] = (m0 * v0) + (m1 * v1) + (m2 * v2);
*/

class Filter {
public:
    struct Params {
        double cutoff;
        double Q;
    };

    virtual void set_params(const Filter::Params &params, double sample_rate) noexcept = 0;

    [[nodiscard]] virtual float forcedinline get_next_sample(float curr_sample) noexcept = 0;

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

protected:
    double ic1eq = 0.0;
    double ic2eq = 0.0;
};

class LowPassFilter : public Filter {
public:
    void set_params(const Filter::Params &params, double sample_rate) noexcept override
    {
        g = std::tan(MathConstants<double>::pi * params.cutoff / sample_rate);
        k = 1 / params.Q;
        a1 = 1 / (1 + g * (g + k));
        a2 = g * a1;
        a3 = g * a2;
    }

    [[nodiscard]] float forcedinline get_next_sample(float curr_sample) noexcept override
    {
        double v3 = curr_sample - ic2eq;
        double v2 = ic2eq + (a2 * ic1eq) + (a3 * v3);
        ic2eq = (2 * v2) - ic2eq;

        return (float)(m2 * v2);
    }

private:
    double g = 0.0, k = 0.0, a1 = 0.0, a2 = 0.0, a3 = 0.0;
    double m2 = 1.0;
};