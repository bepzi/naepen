#include "PluginEditor.h"
#include "PluginProcessor.h"

#define DRAW_COMPONENT_BOUNDS false

NaepenAudioProcessorEditor::NaepenAudioProcessorEditor(
    NaepenAudioProcessor &p, AudioVisualiserComponent &vis, MidiKeyboardState &key_state) :
    AudioProcessorEditor(&p),
    processor(p),
    gain_label("", "Master Gain"),
    gain_slider(Slider::RotaryHorizontalVerticalDrag, Slider::TextBoxBelow),
    visualizer(vis),
    keyboard_state(key_state),
    keyboard_component(keyboard_state, MidiKeyboardComponent::horizontalKeyboard),
    attack_label("", "Attack"),
    attack_slider(Slider::LinearBarVertical, Slider::TextBoxBelow),
    decay_label("", "Decay"),
    decay_slider(Slider::LinearBarVertical, Slider::TextBoxBelow),
    sustain_label("", "Sustain"),
    sustain_slider(Slider::LinearBarVertical, Slider::TextBoxBelow),
    release_label("", "Release"),
    release_slider(Slider::LinearBarVertical, Slider::TextBoxBelow)
{
    gain_label.setJustificationType(Justification::horizontallyCentred);

    gain_slider.setRange(0.0, 1.0);
    gain_slider.setSkewFactorFromMidPoint(0.1);
    gain_slider.setValue(1.0, dontSendNotification);

    attack_slider.setValue(0.005);
    attack_slider.setRange(0.0, 5.0, 0.001);
    attack_slider.setSkewFactorFromMidPoint(1.0);
    attack_label.setJustificationType(Justification::horizontallyCentred);
    decay_slider.setValue(0.25);
    decay_slider.setRange(0.0, 5.0, 0.001);
    decay_slider.setSkewFactorFromMidPoint(1.0);
    decay_label.setJustificationType(Justification::horizontallyCentred);
    sustain_slider.setValue(1.0);
    sustain_slider.setRange(0.0, 1.0, 0.001);
    sustain_slider.setSkewFactorFromMidPoint(0.2);
    sustain_label.setJustificationType(Justification::horizontallyCentred);
    release_slider.setValue(0.1);
    release_slider.setRange(0.0, 5.0, 0.001);
    release_slider.setSkewFactorFromMidPoint(1.0);
    release_label.setJustificationType(Justification::horizontallyCentred);

    addAndMakeVisible(gain_label);
    addAndMakeVisible(gain_slider);
    addAndMakeVisible(visualizer);
    addAndMakeVisible(keyboard_component);

    addAndMakeVisible(attack_label);
    addAndMakeVisible(attack_slider);
    addAndMakeVisible(decay_label);
    addAndMakeVisible(decay_slider);
    addAndMakeVisible(sustain_label);
    addAndMakeVisible(sustain_slider);
    addAndMakeVisible(release_label);
    addAndMakeVisible(release_slider);

    gain_slider.addListener(this);

    attack_slider.addListener(this);
    decay_slider.addListener(this);
    sustain_slider.addListener(this);
    release_slider.addListener(this);

    setSize(640, 480);
    setVisible(true);
}

NaepenAudioProcessorEditor::~NaepenAudioProcessorEditor() = default;

//==============================================================================
void NaepenAudioProcessorEditor::paint(Graphics &g)
{
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

#if DRAW_COMPONENT_BOUNDS == true
    g.setColour(Colours::red);
    g.drawRect(gain_label.getBoundsInParent());
    g.drawRect(gain_slider.getBoundsInParent());
    g.drawRect(visualizer.getBoundsInParent());
#endif
}

void NaepenAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    area.reduce(8, 8);

    auto top_row = area.removeFromTop(128);
    area.removeFromTop(8);

    auto top_left = top_row.removeFromLeft((top_row.getWidth() / 2));
    gain_label.setBounds(top_left.removeFromBottom(24));
    gain_slider.setBounds(top_left);

    auto top_right = top_row;
    auto quarter_top_right = (top_right.getWidth() / 4) - 8;

    auto attack_area = top_right.removeFromLeft(quarter_top_right);
    top_right.removeFromLeft(8);
    attack_label.setBounds(attack_area.removeFromBottom(24));
    attack_slider.setBounds(attack_area);

    auto decay_area = top_right.removeFromLeft(quarter_top_right);
    top_right.removeFromLeft(8);
    decay_label.setBounds(decay_area.removeFromBottom(24));
    decay_slider.setBounds(decay_area);

    auto sustain_area = top_right.removeFromLeft(quarter_top_right);
    top_right.removeFromLeft(8);
    sustain_label.setBounds(sustain_area.removeFromBottom(24));
    sustain_slider.setBounds(sustain_area);

    auto release_area = top_right.removeFromLeft(quarter_top_right);
    release_label.setBounds(release_area.removeFromBottom(24));
    release_slider.setBounds(release_area);

    keyboard_component.setBounds(area.removeFromBottom(72));
    area.removeFromBottom(8);

    visualizer.setBounds(area);
}

void NaepenAudioProcessorEditor::sliderValueChanged(Slider *slider)
{
    if (slider == &gain_slider) {
        processor.set_gain((float)gain_slider.getValue());
    } else if (
        slider == &attack_slider || slider == &decay_slider || slider == &sustain_slider ||
        slider == &release_slider) {
        processor.set_adsr(
            attack_slider.getValue(), decay_slider.getValue(), sustain_slider.getValue(),
            release_slider.getValue());
    }
}

void NaepenAudioProcessorEditor::comboBoxChanged(ComboBox *menu)
{
    if (menu == &waveform_selector) {
        //        auto id = static_cast<WaveformId>(waveform_selector.getSelectedId());
        // TODO: Trigger switching waveforms for current synth
    }
}
