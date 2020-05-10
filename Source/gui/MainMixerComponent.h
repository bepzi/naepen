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
    Label master_gain_label {"", "Master"};

    Slider osc_one_gain_slider {Slider::LinearBarVertical, Slider::TextBoxBelow};
    APVTS::SliderAttachment osc_one_gain_slider_attachment;
    Label osc_one_gain_label {"", "Osc One"};

    Slider osc_two_gain_slider {Slider::LinearBarVertical, Slider::TextBoxBelow};
    APVTS::SliderAttachment osc_two_gain_slider_attachment;
    Label osc_two_gain_label {"", "Osc Two"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainMixerComponent)
};
