#pragma once

#include "Oscillator.h"

// TODO: I'm not happy with the fact that you HAVE to heap-allocate the oscillators, and that the Wavetable itself will call delete on those oscillators

template<unsigned int T = 1024>
class Wavetable {
public:
    Wavetable() noexcept : osc_idx(0), oscillators() {}

    /**
     * Copy constructor, useful for making copies of a wavetable
     * without re-computing it from scratch.
     */
    Wavetable(const Wavetable &other) noexcept : osc_idx(other.osc_idx)
    {
        oscillators.resize(other.oscillators.size(), nullptr);
        for (size_t i = 1; i < oscillators.size(); ++i) {
            auto osc_copy = new Oscillator<T>(*other.oscillators[i]);
            oscillators[i] = osc_copy;
        }
    }

    virtual ~Wavetable() noexcept
    {
        for (auto &osc : this->oscillators) {
            delete osc;
        }
        this->oscillators.clear();
    }

    [[nodiscard]] forcedinline float get_next_sample() noexcept
    {
        return oscillators.at(osc_idx)->get_next_sample();
    }

    void set_freq(float freq, int sample_rate) noexcept
    {
        // Find the best-fitting oscillator for this
        // frequency, and prepare that oscillator for playback.
        osc_idx = std::floor((float)sample_rate / (2.0f * (freq < 1.0f ? 1.0f : freq)));

        if (osc_idx < 1) {
            // We don't have an oscillator with too few harmonics, choose the next-best one
            osc_idx = 1;
        } else if (osc_idx >= oscillators.size()) {
            // We don't have an oscillator with enough harmonics, choose the next-best one
            osc_idx = oscillators.size() - 1;
        }

        oscillators.at(osc_idx)->set_freq(freq, sample_rate);
    }

    /**
     * Computes the amount of space (in bytes) this wavetable takes up in memory.
     */
    [[nodiscard]] size_t get_space_occupied() const noexcept
    {
        return (T + 1) * oscillators.size() * sizeof(float);
    }

protected:
    unsigned int osc_idx;
    std::vector<Oscillator<T> *> oscillators;
};

template<unsigned int T = 1024>
class SawtoothWavetable : public Wavetable<T> {
public:
    SawtoothWavetable() noexcept : Wavetable<T>()
    {
        this->oscillators.resize(num_harmonics + 1, nullptr);

        for (unsigned int i = 1; i <= num_harmonics; ++i) {
            auto *osc = new SawtoothOscillator<T>(i);
            this->oscillators[i] = osc;
        }

        jassert(!this->oscillators.empty());
    }

private:
    static const unsigned int num_harmonics = 100;
};

template<unsigned int T = 1024>
class SineWavetable : public Wavetable<T> {
public:
    SineWavetable() noexcept : Wavetable<T>()
    {
        this->oscillators.resize(num_harmonics + 1, nullptr);
        auto *osc = new SineOscillator<T>();
        this->oscillators[1] = (osc);
        jassert(!this->oscillators.empty());
    }

private:
    static const unsigned int num_harmonics = 1;
};