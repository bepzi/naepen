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
    keyboard_component(keyboard_state, MidiKeyboardComponent::horizontalKeyboard)
{
    gain_label.setJustificationType(Justification::horizontallyCentred);

    gain_slider.setRange(0.0, 1.0);
    gain_slider.setSkewFactorFromMidPoint(0.1);
    gain_slider.setValue(1.0, dontSendNotification);

    addAndMakeVisible(gain_label);
    addAndMakeVisible(gain_slider);
    addAndMakeVisible(visualizer);
    addAndMakeVisible(keyboard_component);

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

    auto top_right = top_row;
    gain_label.setBounds(top_right.removeFromBottom(24));
    gain_slider.setBounds(top_right);

    keyboard_component.setBounds(area.removeFromBottom(72));
    area.removeFromBottom(8);

    visualizer.setBounds(area);
}

void NaepenAudioProcessorEditor::sliderValueChanged(Slider *slider)
{
    if (slider == &gain_slider) {
        processor.set_gain((float)gain_slider.getValue());
    }
}

void NaepenAudioProcessorEditor::comboBoxChanged(ComboBox *menu)
{
    if (menu == &waveform_selector) {
        //        auto id = static_cast<WaveformId>(waveform_selector.getSelectedId());
        // TODO: Trigger switching waveforms for current synth
    }
}
