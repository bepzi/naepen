#pragma once

#include "Oscillator.h"

// TODO: Ditch template argument for sample-rate
// TODO: Add copy-constructors for faster construction of the same wavetable multiple times

template<unsigned int T = 1024, unsigned int R = 44100>
class Wavetable {
public:
    Wavetable<T, R>() noexcept = default;
    virtual ~Wavetable<T, R>() noexcept = default;

    [[nodiscard]] forcedinline float get_next_sample() noexcept
    {
        return oscillators.at(osc_idx)->get_next_sample();
    }

    void set_freq(float freq) noexcept
    {
        // Find the best-fitting oscillator for this
        // frequency, and prepare that oscillator for playback.
        osc_idx = std::floor(R / (2.0f * (freq < 1.0f ? 1.0f : freq)));
        jassert(osc_idx >= 1);

        if (osc_idx >= oscillators.size()) {
            // We don't have an oscillator with enough harmonics, choose the next-best one
            osc_idx = oscillators.size() - 1;
        }

        oscillators.at(osc_idx)->set_freq(freq);
    }

    /**
     * Computes the amount of space (in bytes) this wavetable takes up in memory.
     */
    [[nodiscard]] size_t get_space_occupied() const noexcept
    {
        return (T + 1) * oscillators.size() * sizeof(float);
    }

    /**
     * Computes the lowest frequency that can be accurately represented.
     */
    [[nodiscard]] float get_lowest_accurate_freq() const noexcept
    {
        if (oscillators.empty()) {
            return 0.0f;
        }

        return R / (2.0f * (oscillators.size() - 1));
    }

protected:
    unsigned int osc_idx = 0;
    std::vector<Oscillator<T, R> *> oscillators;
};

template<unsigned int T = 1024, unsigned int R = 44100>
class SawtoothWavetable : public Wavetable<T, R> {
public:
    SawtoothWavetable<T, R>() noexcept
    {
        this->oscillators.resize(num_harmonics + 1, nullptr);

        for (unsigned int i = 1; i <= num_harmonics; ++i) {
            auto *osc = new SawtoothOscillator<T, R>(i);
            this->oscillators[i] = osc;
        }

        jassert(!this->oscillators.empty());
    }

    ~SawtoothWavetable<T, R>() noexcept
    {
        for (auto &osc : this->oscillators) {
            delete osc;
        }
        this->oscillators.clear();
    }

private:
    static const unsigned int num_harmonics = 100;
};

template<unsigned int T = 1024, unsigned int R = 44100>
class SineWavetable : public Wavetable<T, R> {
public:
    SineWavetable<T, R>() noexcept
    {
        this->oscillators.resize(num_harmonics + 1, nullptr);
        this->oscillators[1] = (&sine_osc);
        jassert(!this->oscillators.empty());
    }

private:
    static const unsigned int num_harmonics = 1;

    SineOscillator<T, R> sine_osc = {};
};