#pragma once

#include "Oscillator.h"

#include <fftw3.h>

#include <array>
#include <memory>
#include <vector>

/**
 * A wavetable-backed oscillator that can be safely played at any frequency without aliasing.
 */
class BandlimitedOscillator : public Oscillator {
public:
    static constexpr size_t T = 2048;

    /**
     * Alias for a collection of sample tables, each tagged with the
     * highest frequency at which they can be safely used without aliasing.
     */
    using LookupTable = std::vector<std::pair<float, std::array<float, T + 1>>>;

    explicit BandlimitedOscillator(std::shared_ptr<const LookupTable> t);
    BandlimitedOscillator(const BandlimitedOscillator &other);
    ~BandlimitedOscillator() override = default;

    [[nodiscard]] float get_next_sample() override;

    void set_freq(double freq_hz) override;

    void set_sample_rate(double sr) override;

    /**
     * Alias for a function that can create a single part of
     * a LookupTable given a top frequency in Hz and the sample rate.
     *
     * Note that the returned array is of size (T+1) - this is because
     * we want to always be able to look at "the next element"
     * when interpolating between samples, so when making one of these you
     * typically just do 'arr[T] = arr[0]' before returning the array.
     */
    typedef std::array<float, BandlimitedOscillator::T + 1> (*make_table_func)(double, double);

    /**
     * Creates a sample lookup table, specifically a progressively band-limited
     * table, from a function that can compute a single part of that table
     * given a top frequency.
     */
    static LookupTable from_function(make_table_func table_func, double sample_rate);

    /**
     * Creates a sample lookup table, specifically a progressively band-limited
     * table, from a full-bandwidth, single-cycle waveform.
     *
     * This function allows you to create a large lookup table (ideally once, at startup) and
     * then share the table across many different oscillator implementations without having to
     * recalculate or copy the table around.
     *
     * Design and implementation inspired by
     *  - https://www.earlevel.com/main/2013/03/03/replicating-wavetables/ and
     *  - https://www.vast-dynamics.com/?q=node/181
     */
    static LookupTable from_full_bandwidth(std::array<double, T> waveform);

private:
    static constexpr size_t MAX_TABLES = 16;

    std::shared_ptr<const LookupTable> lookup_table;
    double phase = 0.0;
    double phase_incr = 0.0;
    double sample_rate = 48000.0;

    // Which part of the lookup table we're currently pulling samples from
    size_t idx = 0;

    [[nodiscard]] static inline float
    get_table_sample(double index, const std::array<float, T + 1> &table);

    /**
     * Converts from the frequency domain to the time domain.
     */
    static std::array<float, T + 1> to_waveform(fftw_complex *in);

    /**
     * Normalizes the values within the band-limited table so
     * that all the samples are between [-1, 1].
     */
    static void normalize(LookupTable &table);
};

BandlimitedOscillator::LookupTable make_sine();
std::array<float, BandlimitedOscillator::T + 1> make_triangle(double top_freq, double sample_rate);
std::array<float, BandlimitedOscillator::T + 1> make_square(double top_freq, double sample_rate);
std::array<float, BandlimitedOscillator::T + 1>
make_engineers_sawtooth(double top_freq, double sample_rate);