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
    ~BandlimitedOscillator() override = default;

    [[nodiscard]] inline float get_next_sample() override;

    void set_freq(double freq_hz) override;

    void set_sample_rate(double sr) override;

    /**
     * Creates a sample lookup table, specifically a progressively band-limited
     * table from a full-bandwidth, single-cycle waveform.
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
BandlimitedOscillator::LookupTable make_triangle(double top_freq, double sample_rate);
BandlimitedOscillator::LookupTable make_square(double top_freq, double sample_rate);
BandlimitedOscillator::LookupTable make_sawtooth(double top_freq, double sample_rate);