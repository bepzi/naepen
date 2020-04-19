#include "InterpolatingOscillator.h"

#include <cassert>
#include <cmath>

InterpolatingOscillator::InterpolatingOscillator(const std::vector<BandlimitedOscillator> &oscs)
{
    assert(!oscs.empty());
    oscillators = oscs;
}

float InterpolatingOscillator::get_next_sample()
{
    auto idx0 = (size_t)std::floor(idx);
    auto idx1 = (size_t)std::ceil(idx);

    if (idx0 == idx1) {
        // Don't call get_next_sample() twice because it's a stateful call
        return oscillators[idx0].get_next_sample();
    }

    auto weight = (float)(idx - (double)idx0);
    float sample0 = oscillators[idx0].get_next_sample();
    float sample1 = oscillators[idx1].get_next_sample();

    return ((1.0f - weight) * sample0) + (weight * sample1);
}

void InterpolatingOscillator::set_freq(double freq_hz)
{
    for (auto &osc : oscillators) {
        osc.set_freq(freq_hz);
    }
}

void InterpolatingOscillator::set_sample_rate(double sample_rate)
{
    for (auto &osc : oscillators) {
        osc.set_sample_rate(sample_rate);
    }
}

void InterpolatingOscillator::set_index(double index)
{
    assert(index >= 0.0 && index <= 1.0);
    idx = index * (double)(oscillators.size() - 1);
}