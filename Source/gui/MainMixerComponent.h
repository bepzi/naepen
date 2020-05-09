#pragma once

#include <JuceHeader.h>

class MainMixerComponent : public Component {
public:
    explicit MainMixerComponent(AudioProcessorValueTreeState &state);

    void paint(Graphics &) override;
    void resized() override;

private:
    using APVTS = juce::AudioProcessorValueTreeState;

    Slider master_gain_slider {Slider::LinearBarVertical, Slider::TextBoxBelow};
    APVTS::SliderAttachment master_gain_slider_attachment;

    Slider osc_one_gain_slider {Slider::LinearBarVertical, Slider::TextBoxBelow};
    APVTS::SliderAttachment osc_one_gain_slider_attachment;
    Slider osc_two_gain_slider {Slider::LinearBarVertical, Slider::TextBoxBelow};
    APVTS::SliderAttachment osc_two_gain_slider_attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainMixerComponent)
};
