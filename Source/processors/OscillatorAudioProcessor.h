#pragma once

#include "ProcessorBase.h"

#include "dsp/BandlimitedOscillator.h"
#include "dsp/NoiseOscillator.h"
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
        AudioProcessorValueTreeState &apvts, Identifier oscillator_id, const String &gain_id,
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

    Identifier oscillator_id;

    // TODO: Can we share these between AudioProcessors?
    std::shared_ptr<Oscillator> sine_osc;
    std::shared_ptr<Oscillator> triangle_osc;
    std::shared_ptr<Oscillator> square_osc;
    std::shared_ptr<Oscillator> engineers_sawtooth_osc;

    std::shared_ptr<Oscillator> white_noise_osc;

    void update_current_oscillator();
};
