#pragma once

#include <fftw3.h>

#include <JuceHeader.h>

template<size_t T = 2048>
class WavetableOsc {
public:
    /**
     * Given a full-bandwidth, single-cycle waveform, creates a set of
     * progressively band-limited oscillators in a wavetable.
     *
     * Design and implementation inspired by
     *  - https://www.earlevel.com/main/2013/03/03/replicating-wavetables/ and
     *  - https://www.vast-dynamics.com/?q=node/181
     */
    explicit WavetableOsc(std::array<double, T> waveform) noexcept
    {
        tables.reserve(max_tables);

        auto fft = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * ((T / 2) + 1));
        fftw_plan p = fftw_plan_dft_r2c_1d(T, waveform.data(), (fftw_complex *)fft, FFTW_ESTIMATE);
        fftw_execute(p);
        fftw_destroy_plan(p);

        // Zero the DC-offset and Nyquist
        fft[0][0] = fft[1][0] = 0.0;
        fft[0][T / 2] = fft[1][T / 2] = 0.0;

        // Calculate top harmonic and freq for initial wavetable
        size_t max_harmonic = T / 2;
        const double min_val = 0.000001;  // -120 dB

        while ((std::abs(fft[0][max_harmonic]) + std::abs(fft[1][max_harmonic]) < min_val) &&
               max_harmonic > 0) {
            --max_harmonic;
        }
        jassert(max_harmonic > 0);

        double top_freq = 2.0 / 3.0 / max_harmonic;

        while (max_harmonic != 0 && tables.size() <= max_tables) {
            // TODO: Do this with std::memset?
            for (size_t h = max_harmonic + 1; h < (T / 2) + 1; ++h) {
                fft[0][h] = /* fft[1][h] = */ 0.0;
            }

            Table t(to_waveform(fft), top_freq);
            tables.push_back(t);

            top_freq *= 2.0;
            max_harmonic /= 2;
        }

        normalize();

        fftw_free(fft);
    }

    WavetableOsc(const WavetableOsc<T> &other) noexcept
    {
        this->phase = other.phase;
        this->phase_incr = other.phase_incr;
        this->idx = other.idx;
        this->tables = other.tables;
    }

    // HACK to allow for having a single sinewave as a wavetable
    void replace_table(std::array<double, T> waveform) noexcept
    {
        tables.clear();

        Table t(waveform, 1.0);
        tables.push_back(t);

        normalize();
    }

    void set_freq(double freq_hz, double sample_rate) noexcept
    {
        if (freq_hz <= 0.0 || sample_rate < 1.0) {
            // Turn the wavetable off
            phase_incr = 0.0;
            return;
        }

        phase_incr = freq_hz * (T / sample_rate);

        for (idx = 0; idx < tables.size(); ++idx) {
            if (idx == tables.size() - 1) {
                // No more tables left, use the last one
                break;
            }

            double top_freq = tables[idx].get_top_freq_hz(sample_rate);

            if (top_freq >= freq_hz) {
                // We've found our table
                break;
            }
        }

        jassert(idx <= tables.size() - 1);
    }

    [[nodiscard]] forcedinline float get_next_sample() noexcept
    {
        auto sample = tables[idx].get_sample(phase);

        if ((phase += phase_incr) > (float)T) {
            phase -= (float)T;
        }

        return sample;
    }

private:
    class Table;

    double phase = 0.0;
    double phase_incr = 0.0;
    size_t idx = 0;

    static const size_t max_tables = 20;
    std::vector<Table> tables;

    class Table {
    public:
        /**
         * @param waveform A single cycle of a waveform
         * @param top_freq The highest frequency that can be safely played (not enforced)
         */
        explicit Table(const std::array<double, T> &waveform, double top_freq) noexcept
        {
            std::copy(waveform.begin(), waveform.end(), table.begin());
            this->top_freq = top_freq;

            // Minor optimization that ensures we can always
            // look at "the next element" when interpolating
            // samples between two indices in this table
            table[T] = table[0];
        }

        Table(const Table &other) noexcept
        {
            this->top_freq = other.top_freq;
            this->table = other.table;
        }

        [[nodiscard]] forcedinline float get_sample(double index) const noexcept
        {
            // Linearly interpolate between two sample values
            auto idx0 = (size_t)std::floor(index);
            auto idx1 = idx0 + 1;

            float lower = table[idx0];
            float upper = table[idx1];

            float frac = (float)index - idx0;
            float sample = lower + frac * (upper - lower);

            return sample;
        }

        [[nodiscard]] double get_top_freq_hz(double sr) const
        {
            return top_freq * sr;
        }

        [[nodiscard]] float get_max() const noexcept
        {
            return *std::max_element(table.begin(), table.end());
        }

        void scale_by(float scale) noexcept
        {
            for (float &f : table) {
                f /= scale;
            }
        }

    private:
        std::array<float, T + 1> table;
        double top_freq;
    };

    static std::array<double, T> to_waveform(fftw_complex *in)
    {
        std::array<double, T> out = {};

        fftw_plan p = fftw_plan_dft_c2r_1d((T / 2) + 1, in, out.data(), FFTW_ESTIMATE);
        fftw_execute(p);
        fftw_destroy_plan(p);

        return out;
    }

    void normalize() noexcept
    {
        jassert(!tables.empty());
        auto global_max = tables.at(0).get_max();
        for (Table &t : tables) {
            t.scale_by(global_max);
        }
    }
};

template<size_t T = 2048>
std::array<double, T> make_sine() noexcept
{
    std::array<double, T> out = {};

    double phase = 0.0;
    double phase_delta = MathConstants<double>::twoPi / (double)(T);

    for (auto &f : out) {
        f = std::sin(phase);
        phase += phase_delta;
    }

    return out;
}

template<size_t T = 2048>
std::array<double, T> make_triangle(double top_freq) noexcept
{
    std::array<double, T> out = {};

    double phase = 0.0;
    double phase_delta = MathConstants<double>::twoPi / (double)(T);
    auto num_harmonics = (size_t)(48000.0 / (2.0 * top_freq));

    for (auto &f : out) {
        for (size_t i = 1; i <= num_harmonics; i += 2) {
            float amp = 1.0f / (float)(i * i);
            f += std::sin(phase * i) * amp;
        }

        phase += phase_delta;
    }

    return out;
}

template<size_t T = 2048>
std::array<double, T> make_square(double top_freq) noexcept
{
    std::array<double, T> out = {};

    double phase = 0.0;
    double phase_delta = MathConstants<double>::twoPi / (double)(T);
    auto num_harmonics = (size_t)(48000.0 / (2.0 * top_freq));

    for (auto &f : out) {
        for (size_t i = 1; i <= num_harmonics; i += 2) {
            float amp = 1.0f / i;
            f += std::sin(phase * i) * amp;
        }

        phase += phase_delta;
    }

    return out;
}

template<size_t T = 2048>
std::array<double, T> make_sawtooth(double top_freq) noexcept
{
    std::array<double, T> out = {};

    double phase = 0.0;
    double phase_delta = MathConstants<double>::twoPi / (double)(T);
    auto num_harmonics = (size_t)(48000.0 / (2.0 * top_freq));

    for (auto &f : out) {
        for (size_t i = 1; i <= num_harmonics; ++i) {
            float amp = 1.0f / i;
            f += std::sin(phase * i) * amp;
        }

        // Sine wave summation makes an "inverted" sawtooth, so flip it back over
        f = -f;

        phase += phase_delta;
    }

    return out;
}
