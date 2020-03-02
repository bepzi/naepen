#pragma once

#include "Oscillator.h"

#include <JuceHeader.h>

//#include <fftw3.h>
//template<size_t T>
//static fftw_complex *from_waveform(std::array<double, T> waveform)
//{
//    fftw_complex *out = fftw_alloc_complex(T);
//    fftw_plan p = fftw_plan_dft_r2c_1d(T, waveform.data(), out, FFTW_ESTIMATE);
//    fftw_execute(p);
//
//    fftw_destroy_plan(p);
//
//    return out;
//}
//
//template<size_t T>
//static std::array<double, T> to_waveform(fftw_complex *&in)
//{
//    std::array<double, T> out = {};
//    fftw_plan p = fftw_plan_dft_c2r_1d((T / 2) + 1, in, out.data(), FFTW_ESTIMATE);
//    fftw_execute(p);
//
//    fftw_destroy_plan(p);
//    fftw_free(in);
//    in = nullptr;
//
//    return out;
//}
//
//template<size_t T = 2048>
//class GoodWavetable {
//public:
//    /**
//     * Given a full-bandwidth, single-cycle waveform, creates a set of
//     * progressively band-limited oscillators in a wavetable.
//     *
//     * Design and implementation inspired by
//     *  - https://www.earlevel.com/main/2013/03/03/replicating-wavetables/ and
//     *  - https://www.vast-dynamics.com/?q=node/181
//     */
//    explicit GoodWavetable(std::array<double, T> waveform) noexcept
//    {
//        tables.reserve(num_tables);
//        tables[0] = waveform;
//
//        // Calculate top harmonic for initial wavetable
//        size_t max_harmonic = T / 2;
//        {
//            auto fft = from_waveform(waveform);
//
//            // Zero the DC-offset and Nyquist
//            fft[0][0] = fft[1][0] = 0.0;
//            fft[0][T / 2] = fft[1][T / 2] = 0.0;
//
//            const double min_val = 0.000001;  // -120 dB
//            while ((std::abs(fft[0][max_harmonic]) + std::abs(fft[1][max_harmonic]) < min_val) &&
//                   max_harmonic) {
//                --max_harmonic;
//            }
//
//            to_waveform<T>(fft);  // Clean up allocated memory
//        }
//
//        for (size_t i = 1; i < num_tables; ++i) {
//            if (max_harmonic == 0) {
//                break;
//            }
//
//            // Zero-out the upper harmonics
//            auto fft = from_waveform(tables[i - 1]);
//            for (size_t h = max_harmonic; h < (T / 2) + 1; ++i) {
//                fft[0][h] = fft[1][h] = 0.0;
//            }
//
//            tables.push_back(to_waveform<T>(fft));
//            max_harmonic /= 2;
//        }
//
//        std::cout << "It did not crash!" << std::endl;
//    }
//
//    virtual ~GoodWavetable() noexcept = default;
//
//    void set_freq(double freq, double sample_rate) noexcept
//    {
//        phase_incr = sample_rate / (2.0 * freq);
//    }
//
//private:
//    double phase = 0.0;
//    double phase_incr = 0.0;
//
//    static const size_t num_tables = 32;
//    std::vector<std::array<double, T>> tables;
//};

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
        osc_idx =
            (unsigned int)std::floor((float)sample_rate / (2.0f * (freq < 1.0f ? 1.0f : freq)));

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
    size_t osc_idx;
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