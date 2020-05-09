#pragma once

/**
 * Implements a filtering algorithm developed by Andrew Simper of Cytomic:
 * https://cytomic.com/files/dsp/SvfLinearTrapOptimised2.pdf
 */
class SvfFilter {
public:
    struct Params {
        double cutoff;
        double Q;
    };

    enum class Type { LOWPASS = 0, HIGHPASS, BANDPASS };

    void set_type(SvfFilter::Type type) noexcept;

    SvfFilter::Type get_type() const noexcept;

    void set_params(const SvfFilter::Params &params, double sample_rate) noexcept;

    [[nodiscard]] float get_next_sample(float curr_sample) noexcept;

    void reset() noexcept;

private:
    Type current_type = Type::LOWPASS;

    double ic1eq = 0.0, ic2eq = 0.0;
    double g = 0.0, k = 0.0, a1 = 0.0, a2 = 0.0, a3 = 0.0;
    double m0 = 0.0, m1 = 0.0, m2 = 0.0;
};
