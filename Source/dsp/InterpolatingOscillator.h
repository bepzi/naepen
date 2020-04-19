#pragma once

#include "BandlimitedOscillator.h"
#include "Oscillator.h"

/**
 * An oscillator backed by multiple band-limited wavetables,
 * which will be interpolated between when generating samples.
 */
class InterpolatingOscillator : public Oscillator {
public:
    explicit InterpolatingOscillator(const std::vector<BandlimitedOscillator> &oscs);
    ~InterpolatingOscillator() override = default;

    float get_next_sample() override;

    void set_freq(double freq_hz) override;

    void set_sample_rate(double sample_rate) override;

    /**
     * Changes which wavetables will be sampled and interpolated from when generating samples.
     * A value of 0 represents the first wavetable, and a value of 1 represents the
     * last wavetable in the array of wavetables.
     *
     * @param index value between 0 and 1 (inclusive)
     */
    void set_index(double index);

private:
    std::vector<BandlimitedOscillator> oscillators;

    // Which oscillators we'll sample and interpolate from
    double idx = 0.0;
};