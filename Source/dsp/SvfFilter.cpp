#include "SvfFilter.h"

#include <cassert>
#include <cmath>

void SvfFilter::set_type(SvfFilter::Type type) noexcept
{
    this->current_type = type;
    reset();
}

void SvfFilter::set_params(const SvfFilter::Params &params, double sample_rate) noexcept
{
    assert(sample_rate > 0.0);
    assert(params.Q > 0.0);

    g = std::tan((M_PI * params.cutoff) / sample_rate);
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

[[nodiscard]] inline float SvfFilter::get_next_sample(float curr_sample) noexcept
{
    double v3 = curr_sample - ic2eq;
    double v1 = (a1 * ic1eq) + (a2 * v3);
    double v2 = ic2eq + (a2 * ic1eq) + (a3 * v3);
    ic1eq = (2 * v1) - ic1eq;
    ic2eq = (2 * v2) - ic2eq;

    return (float)((m0 * curr_sample) + (m1 * v1) + (m2 * v2));
}

void SvfFilter::reset() noexcept
{
    ic1eq = 0.0;
    ic2eq = 0.0;
}