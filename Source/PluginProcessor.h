#pragma once

#include "SynthSounds.h"
#include "WaveformVisualizer.h"
#include "WavetableOsc.h"

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

    void set_gain(float gain)
    {
        this->gain = gain;
    }

    void set_adsr(double attack, double decay, double sustain, double release)
    {
        for (int i = 0; i < synth.getNumVoices(); ++i) {
            // TODO: Not a fan of needing to do this dynamic cast. Is there a better way to update the global ADSR? Maybe I need to subclass JUCE::Synthesiser...
            auto wavetable_voice = dynamic_cast<WavetableVoice<2048> *>(synth.getVoice(i));
            if (wavetable_voice != nullptr) {
                wavetable_voice->set_adsr(
                    {(float)attack, (float)decay, (float)sustain, (float)release});
            }
        }
    }

private:
    float gain = 1.0f;

    WaveformVisualiser visualizer;

    Synthesiser synth;

    MidiMessageCollector midi_collector;
    MidiKeyboardState keyboard_state;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NaepenAudioProcessor)
};
