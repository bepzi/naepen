#pragma once

#include "dsp/BandlimitedOscillator.h"
#include "dsp/Oscillator.h"

#include <JuceHeader.h>

/**
 * Handles parameters, wavetables, etc. for a generic "Oscillator".
 *
 * Add one of these to the master AudioProcessorGraph in order to
 * effectively add a new oscillator+parameters to the synth.
 */
class OscillatorAudioProcessor : public AudioProcessor {
public:
    explicit OscillatorAudioProcessor(AudioProcessorValueTreeState &apvts);

    void prepareToPlay(double sample_rate, int samples_per_block) override;
    void releaseResources() override;

    void processBlock(AudioBuffer<float> &, MidiBuffer &) override;

    // =============================================================
    const String getName() const override;
    double getTailLengthSeconds() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    AudioProcessorEditor *createEditor() override;
    bool hasEditor() const override;
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const String getProgramName(int index) override;
    void changeProgramName(int index, const String &newName) override;
    void getStateInformation(MemoryBlock &destData) override;
    void setStateInformation(const void *data, int sizeInBytes) override;

private:
    AudioProcessorValueTreeState &state;

    Synthesiser synth;

    // Waveform lookup tables, computed once at startup
    // and then copied around (by shared_ptr) to many oscillators/voices
    std::shared_ptr<const BandlimitedOscillator::LookupTable> sine_table;
    std::shared_ptr<const BandlimitedOscillator::LookupTable> triangle_table;
    std::shared_ptr<const BandlimitedOscillator::LookupTable> square_table;
    std::shared_ptr<const BandlimitedOscillator::LookupTable> engineers_sawtooth_table;
    std::shared_ptr<const BandlimitedOscillator::LookupTable> musicians_sawtooth_table;
};
