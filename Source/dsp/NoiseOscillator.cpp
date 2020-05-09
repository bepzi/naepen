#include "NoiseOscillator.h"

NoiseOscillator::NoiseOscillator() : rand() {}

NoiseOscillator::NoiseOscillator(const NoiseOscillator &other)
{
    rand = other.rand;
}

// TODO: Look into Professor Haken's paper on generating "good" white noise
float NoiseOscillator::get_next_sample()
{
    return (rand.nextFloat() * 2.0f) - 1.0f;
}

void NoiseOscillator::set_freq(double freq_hz)
{
    (void)freq_hz;
}

void NoiseOscillator::set_sample_rate(double sample_rate)
{
    (void)sample_rate;
}
