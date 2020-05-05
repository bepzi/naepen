#pragma once

#include "dsp/BandlimitedOscillator.h"
#include "dsp/OscillatorVoice.h"
#include "dsp/SvfFilter.h"
#include "gui/WaveformVisualizerComponent.h"

#include <JuceHeader.h>

/**
 * Handles audio, MIDI I/O, and processing logic.
 */
class NaepenAudioProcessor : public AudioProcessor {
public:
    //==============================================================================
    NaepenAudioProcessor();

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout &layouts) const override;

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

    // Holds all of our state, including the automatable parameters
    AudioProcessorValueTreeState state;

    // State for the on-screen MIDI keyboard
    MidiKeyboardState virtual_keyboard_state;

private:
    using AudioGraphIOProcessor = AudioProcessorGraph::AudioGraphIOProcessor;
    using Node = AudioProcessorGraph::Node;

    AudioProcessorGraph processor_graph;
    void initialize_graph();
    Node::Ptr audio_output_node;
    Node::Ptr osc_one_node;

    MidiMessageCollector midi_collector;

    std::atomic<float> *master_gain;

    /**
     * Sets up the automatable parameters for the synth.
     * Parameters _cannot_ be added outside of calling this method!
     */
    static AudioProcessorValueTreeState::ParameterLayout create_parameter_layout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NaepenAudioProcessor)
};
