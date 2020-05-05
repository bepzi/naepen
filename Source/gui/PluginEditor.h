#pragma once

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

    Slider master_gain_slider {Slider::LinearVertical, Slider::TextBoxBelow};
    AudioProcessorValueTreeState::SliderAttachment master_gain_slider_attachment;

    OscillatorComponent osc_one_component;

    MidiKeyboardComponent midi_keyboard;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NaepenAudioProcessorEditor)
};
