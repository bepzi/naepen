#pragma once

#include "MainMixerComponent.h"
#include "NaepenLookAndFeel.h"
#include "OscillatorComponent.h"
#include "PluginProcessor.h"

#include <JuceHeader.h>

class NaepenAudioProcessorEditor : public AudioProcessorEditor {
public:
    explicit NaepenAudioProcessorEditor(NaepenAudioProcessor &processor);
    ~NaepenAudioProcessorEditor() override;

    void paint(Graphics &) override;
    void resized() override;

private:
    NaepenLookAndFeel look_and_feel;

    NaepenAudioProcessor &naepen_processor;

    MainMixerComponent main_mixer_component;
    OscillatorComponent osc_one_component;
    OscillatorComponent osc_two_component;

    MidiKeyboardComponent midi_keyboard;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NaepenAudioProcessorEditor)
};
