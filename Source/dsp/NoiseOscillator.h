#pragma once

#include "Oscillator.h"

#include <JuceHeader.h>

/**
 * A white noise oscillator.
 */
class NoiseOscillator : public Oscillator {
public:
    NoiseOscillator();
    explicit NoiseOscillator(const NoiseOscillator &other);
    ~NoiseOscillator() override = default;

    float get_next_sample() override;
    void set_freq(double freq_hz) override;
    void set_sample_rate(double sample_rate) override;

private:
    Random rand;
};
