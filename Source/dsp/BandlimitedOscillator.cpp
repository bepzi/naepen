#include "BandlimitedOscillator.h"

#include <algorithm>
#include <cassert>
#include <cmath>

BandlimitedOscillator::BandlimitedOscillator(std::shared_ptr<const LookupTable> t) :
    lookup_table(std::move(t))
{
}

BandlimitedOscillator::BandlimitedOscillator(const BandlimitedOscillator &other)
{
    lookup_table = other.lookup_table;

    phase = other.phase;
    phase_incr = other.phase_incr;
    sample_rate = other.sample_rate;
    idx = other.idx;
}

float BandlimitedOscillator::get_next_sample()
{
    auto sample = get_table_sample(phase, (*lookup_table)[idx].second);

    if ((phase += phase_incr) > (double)T) {
        phase -= (double)T;
    }

    return sample;
}

void BandlimitedOscillator::set_freq(double freq_hz)
{
    if (freq_hz <= 0.0 || sample_rate < 1.0) {
        // Turn the wavetable off
        phase_incr = 0.0;
        return;
    }

    phase_incr = freq_hz * (T / sample_rate);

    for (idx = 0; idx < lookup_table->size(); ++idx) {
        if (idx == lookup_table->size() - 1) {
            // No more tables left, use the last one
            break;
        }

        double top_freq = (*lookup_table)[idx].first * sample_rate;
        if (top_freq >= freq_hz) {
            // We've found our table
            break;
        }
    }

    assert(idx <= lookup_table->size() - 1);
}

void BandlimitedOscillator::set_sample_rate(double sr)
{
    sample_rate = sr;
}

BandlimitedOscillator::LookupTable
BandlimitedOscillator::from_full_bandwidth(std::array<double, T> waveform)
{
    LookupTable table;
    table.reserve(MAX_TABLES);

    auto fft = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * ((T / 2) + 1));
    fftw_plan p = fftw_plan_dft_r2c_1d(T, waveform.data(), (fftw_complex *)fft, FFTW_ESTIMATE);
    fftw_execute(p);
    fftw_destroy_plan(p);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Warray-bounds"
    // Zero the DC-offset and Nyquist
    fft[0][0] = fft[1][0] = 0.0;
    fft[0][T / 2] = fft[1][T / 2] = 0.0;
#pragma clang diagnostic pop

    // Calculate top harmonic and freq for the initial wavetable
    size_t max_harmonic = T / 2;
    const double min_val = 0.000001;  // -120 dB

    while ((std::abs(fft[0][max_harmonic]) + std::abs(fft[1][max_harmonic]) < min_val) &&
           max_harmonic > 0) {
        --max_harmonic;
    }
    assert(max_harmonic > 0);

    // Allows for partial overlap between the tables -- this lets us smoothly switch between
    // tables without any hiccups, at the cost of a tiny amount of aliasing
    double top_freq = 2.0 / 3.0 / max_harmonic;

    while (max_harmonic != 0 && table.size() <= MAX_TABLES) {
        for (size_t h = max_harmonic + 1; h < (T / 2) + 1; ++h) {
            fft[0][h] = fft[1][h] = 0.0;
        }

        auto table_piece = std::make_pair<>(top_freq, to_waveform(fft));
        table.push_back(table_piece);

        top_freq *= 2.0;
        max_harmonic /= 2;
    }

    normalize(table);

    fftw_free(fft);

    return table;
}

[[nodiscard]] inline float
BandlimitedOscillator::get_table_sample(double index, const std::array<float, T + 1> &table)
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

std::array<float, BandlimitedOscillator::T + 1> BandlimitedOscillator::to_waveform(fftw_complex *in)
{
    std::array<double, T> waveform = {};

    fftw_plan p = fftw_plan_dft_c2r_1d((T / 2) + 1, in, waveform.data(), FFTW_ESTIMATE);
    fftw_execute(p);
    fftw_destroy_plan(p);

    std::array<float, T + 1> out = {};
    std::copy(waveform.begin(), waveform.end(), out.begin());

    // Minor optimization that ensures we can always
    // look at "the next element" when interpolating
    // samples between two indices in this table
    out[T] = out[0];

    return out;
}

void BandlimitedOscillator::normalize(BandlimitedOscillator::LookupTable &table)
{
    assert(!table.empty());

    float global_max = std::numeric_limits<float>::lowest();
    for (auto &pair : table) {
        float local_max = *std::max_element(pair.second.begin(), pair.second.end());
        if (local_max > global_max) {
            global_max = local_max;
        }
    }

    if (global_max != 0.0f) {
        for (auto &pair : table) {
            for (float &val : pair.second) {
                val /= global_max;
            }
        }
    }
}

BandlimitedOscillator::LookupTable make_sine()
{
    // Ordinarily we'd use our fancy `from_full_bandwidth()`
    // function after creating a 2048-double sine table, but that algorithm
    // doesn't simplify down to a simple sine LookupTable. We're going to cheat
    // and hand-craft the LookupTable ourselves.
    std::array<float, BandlimitedOscillator::T + 1> samples = {};

    double phase = 0.0;
    double phase_delta = (2 * M_PI) / (double)(BandlimitedOscillator::T);

    for (auto &f : samples) {
        f = static_cast<float>(std::sin(phase));
        phase += phase_delta;
    }
    samples[BandlimitedOscillator::T] = samples[0];

    BandlimitedOscillator::LookupTable out;
    out.push_back(std::make_pair<>(1.0f, samples));  // The "top freq" value is irrelevant here
    return out;
}

BandlimitedOscillator::LookupTable make_triangle(double top_freq, double sample_rate)
{
    assert(top_freq > 0.0);
    assert(sample_rate > 0.0);

    std::array<double, BandlimitedOscillator::T> samples = {};

    double phase = 0.0;
    double phase_delta = (2 * M_PI) / (double)(BandlimitedOscillator::T);
    auto num_harmonics = (size_t)(sample_rate / (2.0 * top_freq));

    for (auto &f : samples) {
        for (size_t i = 1; i <= num_harmonics; i += 2) {
            float amp = 1.0f / (float)(i * i);
            f += std::sin(phase * i) * amp;
        }

        phase += phase_delta;
    }

    return BandlimitedOscillator::from_full_bandwidth(samples);
}

BandlimitedOscillator::LookupTable make_square(double top_freq, double sample_rate)
{
    assert(top_freq > 0.0);
    assert(sample_rate > 0.0);

    std::array<double, BandlimitedOscillator::T> samples = {};

    double phase = 0.0;
    double phase_delta = (2 * M_PI) / (double)(BandlimitedOscillator::T);
    auto num_harmonics = (size_t)(sample_rate / (2.0 * top_freq));

    for (auto &f : samples) {
        for (size_t i = 1; i <= num_harmonics; i += 2) {
            float amp = 1.0f / i;
            f += std::sin(phase * i) * amp;
        }

        phase += phase_delta;
    }

    return BandlimitedOscillator::from_full_bandwidth(samples);
}

BandlimitedOscillator::LookupTable make_engineers_sawtooth(double top_freq, double sample_rate)
{
    assert(top_freq > 0.0);
    assert(sample_rate > 0.0);

    std::array<double, BandlimitedOscillator::T> samples = {};

    double phase = 0.0;
    double phase_delta = (2 * M_PI) / (double)(BandlimitedOscillator::T);
    auto num_harmonics = (size_t)(sample_rate / (2.0 * top_freq));

    for (auto &f : samples) {
        for (size_t i = 1; i <= num_harmonics; ++i) {
            float amp = 1.0f / i;
            f += std::sin(phase * i) * amp;
        }

        // Sine wave summation makes an "inverted" sawtooth, so flip it back over
        f = -f;

        phase += phase_delta;
    }

    return BandlimitedOscillator::from_full_bandwidth(samples);
}
