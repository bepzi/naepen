#pragma once

#include "MainMixerComponent.h"
#include "OscillatorComponent.h"
#include "PluginProcessor.h"

#include <JuceHeader.h>

class NaepenAudioProcessorEditor : public AudioProcessorEditor {
public:
    explicit NaepenAudioProcessorEditor(NaepenAudioProcessor &processor);

    void paint(Graphics &) override;
    void resized() override;

private:
    NaepenAudioProcessor &naepenProcessor;

    MainMixerComponent main_mixer_component;

    OscillatorComponent osc_one_component;
    OscillatorComponent osc_two_component;

    MidiKeyboardComponent midi_keyboard;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NaepenAudioProcessorEditor)
};
