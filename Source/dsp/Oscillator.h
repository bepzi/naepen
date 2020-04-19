#pragma once

/**
 * Abstract interface defining a simple oscillator.
 */
class Oscillator {
public:
    /**
     * Retrieves the next sample from this oscillator.
     * This method will likely be called repeatedly from the audio thread,
     * so don't do anything slow when implementing it!
     */
    [[nodiscard]] inline virtual float get_next_sample() = 0;

    /**
     * Informs this oscillator what frequency in Hz it will be expected to produce.
     */
    virtual void set_freq(double freq_hz) = 0;

    /**
     * Informs this oscillator what the current audio sample rate is.
     */
    virtual void set_sample_rate(double sample_rate) = 0;

    virtual ~Oscillator() = default;
};