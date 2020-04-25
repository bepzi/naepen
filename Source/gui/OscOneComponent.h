#pragma once

#include <JuceHeader.h>

using APVTS = juce::AudioProcessorValueTreeState;

class OscOneComponent : public Component {
public:
    OscOneComponent(APVTS &state);
    ~OscOneComponent() override = default;

    void paint(Graphics &) override;
    void resized() override;

private:
    Slider gain_attack_slider {Slider::RotaryHorizontalVerticalDrag, Slider::TextBoxBelow};
    Slider gain_decay_slider {Slider::RotaryHorizontalVerticalDrag, Slider::TextBoxBelow};
    Slider gain_sustain_slider {Slider::RotaryHorizontalVerticalDrag, Slider::TextBoxBelow};
    Slider gain_release_slider {Slider::RotaryHorizontalVerticalDrag, Slider::TextBoxBelow};
    APVTS::SliderAttachment gain_attack_slider_attachment;
    APVTS::SliderAttachment gain_decay_slider_attachment;
    APVTS::SliderAttachment gain_sustain_slider_attachment;
    APVTS::SliderAttachment gain_release_slider_attachment;

    ToggleButton filter_enabled_button {"Filter Enabled"};
    Slider filter_cutoff_slider {Slider::RotaryHorizontalVerticalDrag, Slider::TextBoxBelow};
    Slider filter_q_slider {Slider::RotaryHorizontalVerticalDrag, Slider::TextBoxBelow};
    APVTS::ButtonAttachment filter_enabled_button_attachment;
    APVTS::SliderAttachment filter_cutoff_slider_attachment;
    APVTS::SliderAttachment filter_q_slider_attachment;

    Slider filter_attack_slider {Slider::RotaryHorizontalVerticalDrag, Slider::TextBoxBelow};
    Slider filter_decay_slider {Slider::RotaryHorizontalVerticalDrag, Slider::TextBoxBelow};
    Slider filter_sustain_slider {Slider::RotaryHorizontalVerticalDrag, Slider::TextBoxBelow};
    Slider filter_release_slider {Slider::RotaryHorizontalVerticalDrag, Slider::TextBoxBelow};
    APVTS::SliderAttachment filter_attack_slider_attachment;
    APVTS::SliderAttachment filter_decay_slider_attachment;
    APVTS::SliderAttachment filter_sustain_slider_attachment;
    APVTS::SliderAttachment filter_release_slider_attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscOneComponent)
};
