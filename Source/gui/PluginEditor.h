#pragma once

#include "../PluginProcessor.h"
#include "OscOneComponent.h"

#include <JuceHeader.h>

class NaepenAudioProcessorEditor : public AudioProcessorEditor, public Slider::Listener {
public:
    explicit NaepenAudioProcessorEditor(NaepenAudioProcessor &processor);
    ~NaepenAudioProcessorEditor() override = default;

    void paint(Graphics &) override;
    void resized() override;

    void sliderValueChanged(Slider *slider) override;

private:
    NaepenAudioProcessor &processor;

    Slider master_gain_slider {Slider::LinearVertical, Slider::TextBoxBelow};
    APVTS::SliderAttachment master_gain_slider_attachment;

    OscOneComponent osc_one;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NaepenAudioProcessorEditor)
};
