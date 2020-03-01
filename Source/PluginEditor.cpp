#include "PluginEditor.h"
#include "PluginProcessor.h"

#define DRAW_COMPONENT_BOUNDS false

NaepenAudioProcessorEditor::NaepenAudioProcessorEditor(
    NaepenAudioProcessor &p, AudioVisualiserComponent &vis, MidiKeyboardState &key_state) :
    AudioProcessorEditor(&p),
    processor(p),
    visualizer(vis),
//    freq_label("", "Frequency"),
    gain_label("", "Master Gain"),
//    freq_slider(Slider::RotaryHorizontalVerticalDrag, Slider::TextBoxBelow),
    gain_slider(Slider::RotaryHorizontalVerticalDrag, Slider::TextBoxBelow),
    keyboard_state(key_state),
    keyboard_component(keyboard_state, MidiKeyboardComponent::horizontalKeyboard)
{
//    freq_label.setJustificationType(Justification::horizontallyCentred);
    gain_label.setJustificationType(Justification::horizontallyCentred);

//    freq_slider.setRange(0.0, 5000.0);
//    freq_slider.setSkewFactorFromMidPoint(500.0);

    gain_slider.setRange(0.0, 1.0);
    gain_slider.setSkewFactorFromMidPoint(0.1);
    gain_slider.setValue(1.0, dontSendNotification);

//    addAndMakeVisible(freq_label);
    addAndMakeVisible(gain_label);
//    addAndMakeVisible(freq_slider);
    addAndMakeVisible(gain_slider);
    addAndMakeVisible(visualizer);
    addAndMakeVisible(keyboard_component);

//    freq_slider.addListener(this);
    gain_slider.addListener(this);

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
    g.drawRect(freq_label.getBoundsInParent());
    g.drawRect(gain_label.getBoundsInParent());
    g.drawRect(freq_slider.getBoundsInParent());
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

//    auto top_left = top_row.removeFromLeft(top_row.getCentreX() - 8);
//    top_row.removeFromLeft(8);
//    freq_label.setBounds(top_left.removeFromBottom(24));
//    freq_slider.setBounds(top_left);

    auto top_right = top_row;
    gain_label.setBounds(top_right.removeFromBottom(24));
    gain_slider.setBounds(top_right);

    keyboard_component.setBounds(area.removeFromBottom(72));
    area.removeFromBottom(8);

    visualizer.setBounds(area);
}

void NaepenAudioProcessorEditor::sliderValueChanged(Slider *slider)
{
    /*if (slider == &freq_slider) {
        processor.set_freq((float)freq_slider.getValue());

    } else*/ if (slider == &gain_slider) {
        processor.set_gain((float)gain_slider.getValue());
    }
}

void NaepenAudioProcessorEditor::comboBoxChanged(ComboBox *menu)
{
    if (menu == &waveform_selector) {
        auto id = static_cast<WaveformId>(waveform_selector.getSelectedId());
    }
}
