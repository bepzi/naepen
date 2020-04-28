#pragma once

#include "OscOneComponent.h"
#include "PluginProcessor.h"

#include <JuceHeader.h>

class NaepenAudioProcessorEditor : public AudioProcessorEditor {
public:
    explicit NaepenAudioProcessorEditor(NaepenAudioProcessor &processor);
    ~NaepenAudioProcessorEditor() override = default;

    void paint(Graphics &) override;
    void resized() override;

private:
    NaepenAudioProcessor &naepenProcessor;

    Slider master_gain_slider {Slider::LinearVertical, Slider::TextBoxBelow};
    APVTS::SliderAttachment master_gain_slider_attachment;

    OscOneComponent osc_one;

    MidiKeyboardComponent midi_keyboard;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NaepenAudioProcessorEditor)
};
