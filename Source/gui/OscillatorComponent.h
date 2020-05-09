#pragma once

#include "WaveformSelectorListBoxModel.h"

#include <JuceHeader.h>

class OscillatorComponent : public Component {
public:
    OscillatorComponent(
        AudioProcessorValueTreeState &state, const Identifier &waveform_id, const String &pan_id,
        const String &gain_attack_id, const String &gain_decay_id, const String &gain_sustain_id,
        const String &gain_release_id, const String &filter_type_id,
        const String &filter_enabled_id, const String &filter_cutoff_id, const String &filter_q_id);

    void paint(Graphics &) override;
    void resized() override;

private:
    using APVTS = juce::AudioProcessorValueTreeState;

    // TODO: Perhaps this should be done with a TreeView?
    std::unique_ptr<WaveformSelectorListBoxModel> waveform_selector_model;
    ListBox waveform_selector;

    Slider pan_slider {Slider::RotaryHorizontalVerticalDrag, Slider::TextBoxBelow};
    APVTS::SliderAttachment pan_slider_attachment;

    Slider gain_attack_slider {Slider::RotaryHorizontalVerticalDrag, Slider::TextBoxBelow};
    Slider gain_decay_slider {Slider::RotaryHorizontalVerticalDrag, Slider::TextBoxBelow};
    Slider gain_sustain_slider {Slider::RotaryHorizontalVerticalDrag, Slider::TextBoxBelow};
    Slider gain_release_slider {Slider::RotaryHorizontalVerticalDrag, Slider::TextBoxBelow};
    APVTS::SliderAttachment gain_attack_slider_attachment;
    APVTS::SliderAttachment gain_decay_slider_attachment;
    APVTS::SliderAttachment gain_sustain_slider_attachment;
    APVTS::SliderAttachment gain_release_slider_attachment;

    ComboBox filter_type_combo_box;
    APVTS::ComboBoxAttachment filter_type_combo_box_attachment;
    ToggleButton filter_enabled_button {"Filter Enabled"};
    Slider filter_cutoff_slider {Slider::RotaryHorizontalVerticalDrag, Slider::TextBoxBelow};
    Slider filter_q_slider {Slider::RotaryHorizontalVerticalDrag, Slider::TextBoxBelow};
    APVTS::ButtonAttachment filter_enabled_button_attachment;
    APVTS::SliderAttachment filter_cutoff_slider_attachment;
    APVTS::SliderAttachment filter_q_slider_attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscillatorComponent)
};
