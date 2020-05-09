#pragma once

#include "ProcessorBase.h"
#include "dsp/BandlimitedOscillator.h"
#include "dsp/Oscillator.h"

#include <JuceHeader.h>

/**
 * Handles parameters, wavetables, etc. for a generic "Oscillator".
 *
 * Add one of these to the master AudioProcessorGraph in order to
 * effectively add a new oscillator+parameters to the synth.
 */
class OscillatorAudioProcessor : public ProcessorBase, public ValueTree::Listener {
public:
    OscillatorAudioProcessor(
        AudioProcessorValueTreeState &apvts, const String &waveform_id, const String &gain_id,
        const String &gain_attack_id, const String &gain_decay_id, const String &gain_sustain_id,
        const String &gain_release_id, const String &filter_enabled_id,
        const String &filter_cutoff_id, const String &filter_q_id);

    ~OscillatorAudioProcessor();

    void prepareToPlay(double sample_rate, int samples_per_block) override;
    void releaseResources() override;

    void processBlock(AudioBuffer<float> &, MidiBuffer &) override;
    void valueTreePropertyChanged(
        ValueTree &treeWhosePropertyHasChanged, const Identifier &property) override;

private:
    Synthesiser synth;

    // Oscillator parameters
    std::atomic<float> *gain;

    std::atomic<float> *gain_attack;
    std::atomic<float> *gain_decay;
    std::atomic<float> *gain_sustain;
    std::atomic<float> *gain_release;

    std::atomic<float> *filter_enabled;
    std::atomic<float> *filter_cutoff;
    std::atomic<float> *filter_q;

    const String &waveform_id;
    std::shared_ptr<const BandlimitedOscillator::LookupTable> current_waveform;

    // Waveform lookup tables, computed once at startup
    // and then copied around (by shared_ptr) to many oscillators/voices
    std::shared_ptr<const BandlimitedOscillator::LookupTable> sine_table;
    std::shared_ptr<const BandlimitedOscillator::LookupTable> triangle_table;
    std::shared_ptr<const BandlimitedOscillator::LookupTable> square_table;
    std::shared_ptr<const BandlimitedOscillator::LookupTable> engineers_sawtooth_table;
};
