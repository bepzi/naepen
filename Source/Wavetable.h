#pragma once

#include <fftw3.h>

#include <JuceHeader.h>

template<size_t T = 2048>
class Wavetable {
public:
    /**
     * Given a full-bandwidth, single-cycle waveform, creates a set of
     * progressively band-limited oscillators in a wavetable.
     *
     * Design and implementation inspired by
     *  - https://www.earlevel.com/main/2013/03/03/replicating-wavetables/ and
     *  - https://www.vast-dynamics.com/?q=node/181
     */
    explicit Wavetable(std::array<double, T> waveform) noexcept
    {
        tables.reserve(max_tables);

        Table first(waveform, T / 2);
        tables.push_back(first);

        auto fft = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * ((T / 2) + 1));
        fftw_plan p = fftw_plan_dft_r2c_1d(T, waveform.data(), (fftw_complex *)fft, FFTW_ESTIMATE);
        fftw_execute(p);
        fftw_destroy_plan(p);

        // Zero the DC-offset and Nyquist
        fft[0][0] = fft[1][0] = 0.0;
        fft[0][T / 2] = fft[1][T / 2] = 0.0;

        // Calculate top harmonic for initial wavetable
        size_t max_harmonic = T / 2;

        const double min_val = 0.000001;  // -120 dB
        while ((std::abs(fft[0][max_harmonic]) + std::abs(fft[1][max_harmonic]) < min_val) &&
               max_harmonic > 0) {
            --max_harmonic;
        }

        for (size_t i = 1; i < max_tables; ++i) {
            for (size_t h = max_harmonic / 2; h < max_harmonic + 1; ++h) {
                fft[0][h] = fft[1][h] = 0.0;
            }

            max_harmonic /= 2;
            if (max_harmonic == 0) {
                // Minor optimization to ensure we can always look at "the next table"
                // when interpolating between two tables.
                tables.push_back(tables.at(tables.size() - 1));
                break;
            }

            Table t(to_waveform(fft), max_harmonic);
            tables.push_back(t);
        }

        fftw_free(fft);
    }

    virtual ~Wavetable() noexcept = default;

    void set_freq(double freq, double sample_rate) noexcept
    {
        phase_incr = (float)(freq * (T / sample_rate));

        size_t highest_harmonic = std::floor(sample_rate / (2 * freq));

        for (idx = 0; idx < tables.size(); ++idx) {
            if (tables[idx].get_highest_safe_harmonic() > highest_harmonic) {
                break;
            }
        }
        jassert(idx <= tables.size() - 1);
    }

    [[nodiscard]] forcedinline float get_next_sample() noexcept
    {
        auto sample = tables[idx].get_sample(phase);
        // TODO: inter-wavetable interpolation?

        if ((phase += phase_incr) > (float)T) {
            phase -= (float)T;
        }

        return sample;
    }

private:
    float phase = 0.0;
    float phase_incr = 0.0;
    size_t idx = 0;

    class Table {
    public:
        /**
         * @param waveform A single cycle of a waveform
         * @param highest_freq_hz The highest harmonic that can be safely played (not enforced)
         */
        explicit Table(const std::array<double, T> &waveform, size_t highest_harmonic) noexcept
        {
            std::copy(waveform.begin(), waveform.end(), table.begin());
            this->highest_harmonic = highest_harmonic;

            // Minor optimization that ensures we can always
            // look at "the next element" when interpolating
            // samples between two indices in this table
            table[T] = table[0];
        }

        [[nodiscard]] forcedinline float get_sample(float index) const noexcept
        {
            // Linearly interpolate between two sample values
            auto idx0 = (int)std::floor(index);
            auto idx1 = idx0 + 1;

            float lower = table[idx0];
            float upper = table[idx1];

            float frac = index - (float)idx0;
            float sample = lower + frac * (upper - lower);

            return sample;
        }

        /**
         * @return The highest frequency that this table can be safely played at before aliasing begins to occur.
         */
        [[nodiscard]] size_t get_highest_safe_harmonic() const
        {
            return highest_harmonic;
        }

    private:
        std::array<float, T + 1> table;
        size_t highest_harmonic;
    };

    static const size_t max_tables = 32;
    std::vector<Table> tables;

    static std::array<double, T> to_waveform(fftw_complex *in)
    {
        std::array<double, T> out = {};

        fftw_plan p = fftw_plan_dft_c2r_1d((T / 2) + 1, in, out.data(), FFTW_ESTIMATE);
        fftw_execute(p);
        fftw_destroy_plan(p);

        return out;
    }
};