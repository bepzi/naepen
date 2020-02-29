#pragma once

#include "Wavetable.h"

#include <JuceHeader.h>

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

    void set_freq(float freq)
    {
        table.set_freq(freq);
    }

    void set_gain(float gain)
    {
        this->gain = gain;
    }

private:
    float gain = 0.0f;
    SineWavetable<1024, 44100> table;

    AudioVisualiserComponent visualizer;

    //==============================================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NaepenAudioProcessor)
};
