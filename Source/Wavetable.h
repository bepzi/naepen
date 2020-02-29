#pragma once

#include "Oscillator.h"

template<unsigned int T = 1024, unsigned int R = 44100>
class Wavetable {
public:
    Wavetable<T, R>() noexcept = default;
    virtual ~Wavetable<T, R>() noexcept = default;

    [[nodiscard]] float get_next_sample() noexcept
    {
        if (use_secondary_osc) {
            float primary_sample = weight * primary->get_next_sample();
            float secondary_sample = (1 - weight) * secondary->get_next_sample();
            return primary_sample + secondary_sample;
        } else {
            float sample = primary->get_next_sample();
            return sample;
        }
    }

    void set_freq(float freq) noexcept
    {
        jassert(!oscillators.empty());
        if (oscillators.size() == 1) {
            // Don't do any interpolation, just set the frequency
            oscillators[0].second->set_freq(freq);
            primary = oscillators[0].second;
            secondary = nullptr;
            use_secondary_osc = false;
            return;
        }

        unsigned int lower_idx = 0;
        for (auto i = 0; i < oscillators.size(); ++i) {
            if (oscillators[i].first > freq) {
                break;
            } else {
                lower_idx = i;
            }
        }

        unsigned int upper_idx = (lower_idx + 1 < oscillators.size()) ? lower_idx + 1 : lower_idx;
        if (lower_idx != upper_idx) {
            auto lower = oscillators[lower_idx];
            auto upper = oscillators[upper_idx];

            if (lower.first < freq && upper.first > freq) {
                primary = oscillators[lower_idx].second;
                secondary = oscillators[upper_idx].second;
                primary->set_freq(freq);
                secondary->set_freq(freq);
                use_secondary_osc = true;
                weight = (upper.first - freq) / (upper.first - lower.first);

            } else {
                primary = oscillators[lower_idx].second;
                primary->set_freq(freq);
                secondary = nullptr;
                use_secondary_osc = false;
            }
        } else {
            primary = oscillators[lower_idx].second;
            primary->set_freq(freq);
            secondary = nullptr;
            use_secondary_osc = false;
        }
    }

protected:
    bool use_secondary_osc = false;
    Oscillator<T, R> *primary = nullptr;
    Oscillator<T, R> *secondary = nullptr;
    float weight = 0.0;

    std::vector<std::pair<float, Oscillator<T, R> *>> oscillators;
};

template<unsigned int T = 1024, unsigned int R = 44100>
class SawtoothWavetable : public Wavetable<T, R> {
public:
    SawtoothWavetable<T, R>() noexcept
    {
        this->oscillators.push_back(std::make_pair(50.0f, &sawtooth_oscs[0]));
        this->oscillators.push_back(std::make_pair(100.0f, &sawtooth_oscs[1]));
        this->oscillators.push_back(std::make_pair(200.0f, &sawtooth_oscs[2]));
        this->oscillators.push_back(std::make_pair(400.0f, &sawtooth_oscs[3]));
        this->oscillators.push_back(std::make_pair(800.0f, &sawtooth_oscs[4]));
        this->oscillators.push_back(std::make_pair(1000.0f, &sawtooth_oscs[5]));
        this->oscillators.push_back(std::make_pair(2000.0f, &sawtooth_oscs[6]));
        this->oscillators.push_back(std::make_pair(4000.0f, &sawtooth_oscs[7]));
    }

private:
    SawtoothOscillator<T, R> sawtooth_oscs[8] = {
        SawtoothOscillator<T, R>(50.0f),
        SawtoothOscillator<T, R>(100.0f),
        SawtoothOscillator<T, R>(200.0f),
        SawtoothOscillator<T, R>(400.0f),
        SawtoothOscillator<T, R>(800.0f),
        SawtoothOscillator<T, R>(1000.0f),
        SawtoothOscillator<T, R>(2000.0f),
        SawtoothOscillator<T, R>(4000.0f)};
};

template<unsigned int T = 1024, unsigned int R = 44100>
class SineWavetable : public Wavetable<T, R> {
public:
    SineWavetable<T, R>() noexcept
    {
        this->oscillators.push_back(std::make_pair(440.0f, &sine_oscs[0]));
    }

private:
    SineOscillator<T, R> sine_oscs[1] = {SineOscillator<T, R>()};
};