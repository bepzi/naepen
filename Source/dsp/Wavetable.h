#pragma once

#include "WavetableOsc.h"

template<size_t T = 2048>
class Wavetable {
public:
    explicit Wavetable(const std::vector<WavetableOsc<T>> &oscs) noexcept
    {
        jassert(!oscs.empty());
        this->oscillators = oscs;
    }

    Wavetable(const Wavetable<T> &other) noexcept
    {
        this->oscillators = other.oscillators;
        this->idx = other.idx;
    }

    // TODO: Add "arbitrary waveform" constructor

    void set_freq(double freq_hz, double sample_rate) noexcept
    {
        for (auto &osc : oscillators) {
            osc.set_freq(freq_hz, sample_rate);
        }
    }

    /**
     * Sets which oscillator(s) will be used when getting samples from this wavetable.
     * If a fractional value, linear interpolation will be used when retrieving samples
     * from the two oscillators.
     *
     * Must be between [0, this->get_max_index()], inclusive.
     */
    void set_index(double new_idx) noexcept
    {
        jassert(new_idx >= 0.0 && new_idx <= get_max_index());
        this->idx = new_idx;
    }

    [[nodiscard]] double get_max_index() const noexcept
    {
        return (double)(oscillators.size() - 1);
    }

    [[nodiscard]] forcedinline float get_next_sample() noexcept
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

private:
    std::vector<WavetableOsc<T>> oscillators;
    double idx = 0.0;
};