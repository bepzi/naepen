#pragma once

#include "dsp/BandlimitedOscillator.h"
#include "dsp/OscillatorVoice.h"
#include "dsp/SvfFilter.h"
#include "gui/WaveformVisualizerComponent.h"

#include <JuceHeader.h>

using APVTS = juce::AudioProcessorValueTreeState;

/**
 * Handles audio, MIDI I/O, and processing logic.
 */
class NaepenAudioProcessor : public AudioProcessor {
public:
    //==============================================================================
    NaepenAudioProcessor();
    ~NaepenAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout &layouts) const override;
#endif

    void processBlock(AudioBuffer<float> &, MidiBuffer &) override;

    //==============================================================================
    AudioProcessorEditor *createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const String getProgramName(int index) override;
    void changeProgramName(int index, const String &newName) override;

    //==============================================================================
    void getStateInformation(MemoryBlock &destData) override;
    void setStateInformation(const void *data, int sizeInBytes) override;

    APVTS state;

private:
    Synthesiser synth;

    // Waveform lookup tables, computed once at startup
    // and then copied around (by shared_ptr) to many oscillators/voices
    std::shared_ptr<const BandlimitedOscillator::LookupTable> sine_table;
    std::shared_ptr<const BandlimitedOscillator::LookupTable> triangle_table;
    std::shared_ptr<const BandlimitedOscillator::LookupTable> square_table;
    std::shared_ptr<const BandlimitedOscillator::LookupTable> sawtooth_table;

    SvfFilter filter;
    MidiMessageCollector midi_collector;
    MidiKeyboardState keyboard_state;

    /**
     * Sets up the automatable parameters for the synth.
     * Parameters _cannot_ be added outside of calling this method!
     */
    static APVTS::ParameterLayout create_parameter_layout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NaepenAudioProcessor)
};
