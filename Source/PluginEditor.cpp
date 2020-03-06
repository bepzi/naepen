#include "PluginEditor.h"
#include "PluginProcessor.h"

#define DRAW_COMPONENT_BOUNDS true

constexpr auto PADDING = 8;
constexpr auto COMPONENT_HEIGHT = 24;

NaepenAudioProcessorEditor::NaepenAudioProcessorEditor(
    NaepenAudioProcessor &p, AudioVisualiserComponent &vis, MidiKeyboardState &key_state) :
    AudioProcessorEditor(&p),
    processor(p),
    gain_label("", "Master"),
    gain_slider(Slider::LinearVertical, Slider::TextBoxBelow),
    visualizer(vis),
    keyboard_state(key_state),
    keyboard_component(keyboard_state, MidiKeyboardComponent::horizontalKeyboard),
    attack_label("", "A"),
    attack_slider(Slider::RotaryHorizontalVerticalDrag, Slider::TextBoxBelow),
    decay_label("", "D"),
    decay_slider(Slider::RotaryHorizontalVerticalDrag, Slider::TextBoxBelow),
    sustain_label("", "S"),
    sustain_slider(Slider::RotaryHorizontalVerticalDrag, Slider::TextBoxBelow),
    release_label("", "R"),
    release_slider(Slider::RotaryHorizontalVerticalDrag, Slider::TextBoxBelow),
    table_idx_label("", "Offset"),
    table_idx_slider(Slider::LinearVertical, Slider::TextBoxBelow)
{
    gain_label.setJustificationType(Justification::horizontallyCentred);

    gain_slider.setRange(0.0, 1.25, 0.001);
    gain_slider.setSkewFactorFromMidPoint(0.1);
    gain_slider.setValue(1.0, dontSendNotification);

    attack_slider.setValue(0.005, dontSendNotification);
    attack_slider.setRange(0.0, 5.0, 0.001);
    attack_slider.setSkewFactorFromMidPoint(1.0);
    attack_label.setJustificationType(Justification::horizontallyCentred);
    decay_slider.setValue(0.25, dontSendNotification);
    decay_slider.setRange(0.0, 5.0, 0.001);
    decay_slider.setSkewFactorFromMidPoint(1.0);
    decay_label.setJustificationType(Justification::horizontallyCentred);
    sustain_slider.setValue(1.0, dontSendNotification);
    sustain_slider.setRange(0.0, 1.0, 0.001);
    sustain_slider.setSkewFactorFromMidPoint(0.2);
    sustain_label.setJustificationType(Justification::horizontallyCentred);
    release_slider.setValue(0.1, dontSendNotification);
    release_slider.setRange(0.0, 5.0, 0.001);
    release_slider.setSkewFactorFromMidPoint(1.0);
    release_label.setJustificationType(Justification::horizontallyCentred);

    table_idx_slider.setRange(0.0, 1.0, 0.001);
    table_idx_label.setJustificationType(Justification::horizontallyCentred);

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

    addAndMakeVisible(table_idx_label);
    addAndMakeVisible(table_idx_slider);

    gain_slider.addListener(this);

    attack_slider.addListener(this);
    decay_slider.addListener(this);
    sustain_slider.addListener(this);
    release_slider.addListener(this);

    table_idx_slider.addListener(this);

    setSize(1280, 720);
    setVisible(true);
}

NaepenAudioProcessorEditor::~NaepenAudioProcessorEditor() = default;

//==============================================================================
void NaepenAudioProcessorEditor::paint(Graphics &g)
{
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

#if DRAW_COMPONENT_BOUNDS == true
    std::vector<Component *> drawable_components = {
        &gain_label,     &gain_slider,        &visualizer,       &attack_slider,  &decay_slider,
        &sustain_slider, &release_slider,     &attack_label,     &decay_label,    &sustain_label,
        &release_label,  &keyboard_component, &table_idx_slider, &table_idx_label};

    g.setColour(Colours::red);
    for (const auto *c : drawable_components) {
        g.drawRect(c->getBoundsInParent(), 2);
    }
#endif
}

void NaepenAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    area.reduce(PADDING, PADDING);

    // Keyboard component and waveform visualizer
    {
        auto bottom_row = area.removeFromBottom(COMPONENT_HEIGHT * 3);
        area.removeFromBottom(PADDING);
        keyboard_component.setBounds(
            bottom_row.removeFromLeft(((4 * bottom_row.getWidth()) / 5) - PADDING));
        bottom_row.removeFromLeft(PADDING);
        visualizer.setBounds(bottom_row);
    }

    // ADSR and filter parameters
    {
        auto left_pane = area.removeFromLeft((area.getWidth() / 3) - PADDING);
        area.removeFromLeft(PADDING);

        auto box_height = (left_pane.getHeight() / 5) - (2 * PADDING);

        {
            auto ampl_adsr = left_pane.removeFromTop(box_height);
            left_pane.removeFromTop(PADDING);

            auto gain_box = ampl_adsr.removeFromRight(COMPONENT_HEIGHT * 2);
            gain_label.setBounds(gain_box.removeFromBottom(COMPONENT_HEIGHT));
            gain_slider.setBounds(gain_box);

            auto adsr_slider_width = ampl_adsr.getWidth() / 4;

            auto ampl_attack_box = ampl_adsr.removeFromLeft(adsr_slider_width);
            attack_label.setBounds(ampl_attack_box.removeFromBottom(COMPONENT_HEIGHT));
            attack_slider.setBounds(ampl_attack_box);

            auto ampl_decay_box = ampl_adsr.removeFromLeft(adsr_slider_width);
            decay_label.setBounds(ampl_decay_box.removeFromBottom(COMPONENT_HEIGHT));
            decay_slider.setBounds(ampl_decay_box);

            auto ampl_sustain_box = ampl_adsr.removeFromLeft(adsr_slider_width);
            sustain_label.setBounds(ampl_sustain_box.removeFromBottom(COMPONENT_HEIGHT));
            sustain_slider.setBounds(ampl_sustain_box);

            auto ampl_release_box = ampl_adsr.removeFromLeft(adsr_slider_width);
            release_label.setBounds(ampl_release_box.removeFromBottom(COMPONENT_HEIGHT));
            release_slider.setBounds(ampl_release_box);
        }

        // WAVETABLE ADSR / TABLE INDEX
        {
            auto wavetable_adsr = left_pane.removeFromTop(box_height);
            left_pane.removeFromTop(PADDING);

            auto table_idx_box = wavetable_adsr.removeFromRight(COMPONENT_HEIGHT * 2);
            table_idx_label.setBounds(table_idx_box.removeFromBottom(COMPONENT_HEIGHT));
            table_idx_slider.setBounds(table_idx_box);
        }

        {
            auto filter_settings = left_pane;
        }
    }

    // Misc. visualizations
    auto right_pane = area;
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

    } else if (slider == &table_idx_slider) {
        processor.set_table_index(table_idx_slider.getValue());
    }
}

void NaepenAudioProcessorEditor::comboBoxChanged(ComboBox *menu)
{
    if (menu == &waveform_selector) {
        //        auto id = static_cast<WaveformId>(waveform_selector.getSelectedId());
        // TODO: Trigger switching waveforms for current synth
    }
}
