#include "OscillatorComponent.h"

#include <JuceHeader.h>

OscillatorComponent::OscillatorComponent(
    APVTS &state, const String &gain_attack_id, const String &gain_decay_id,
    const String &gain_sustain_id, const String &gain_release_id, const String &filter_enabled_id,
    const String &filter_cutoff_id, const String &filter_q_id) :
    waveform_selector_model(std::make_unique<WaveformSelectorListBoxModel>()),
    waveform_selector("", waveform_selector_model.get()),

    gain_attack_slider_attachment(state, gain_attack_id, gain_attack_slider),
    gain_decay_slider_attachment(state, gain_decay_id, gain_decay_slider),
    gain_sustain_slider_attachment(state, gain_sustain_id, gain_sustain_slider),
    gain_release_slider_attachment(state, gain_release_id, gain_release_slider),

    filter_enabled_button_attachment(state, filter_enabled_id, filter_enabled_button),
    filter_cutoff_slider_attachment(state, filter_cutoff_id, filter_cutoff_slider),
    filter_q_slider_attachment(state, filter_q_id, filter_q_slider)
{
    populate_waveform_selector();
    addAndMakeVisible(waveform_selector);

    addAndMakeVisible(gain_attack_slider);
    addAndMakeVisible(gain_decay_slider);
    addAndMakeVisible(gain_sustain_slider);
    addAndMakeVisible(gain_release_slider);

    addAndMakeVisible(filter_enabled_button);
    addAndMakeVisible(filter_cutoff_slider);
    addAndMakeVisible(filter_q_slider);
}

/**
 * Stupid helper function because there's no native way in JUCE
 * to convert from Rectangle<int> to Rectangle<float>
 */
static Rectangle<float> from_rect_int(Rectangle<int> int_rect)
{
    return {
        (float)int_rect.getX(), (float)int_rect.getY(), (float)int_rect.getWidth(),
        (float)int_rect.getHeight()};
}

void OscillatorComponent::paint(Graphics &g)
{
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    g.setColour(Colours::lightgrey);
    g.drawRoundedRectangle(from_rect_int(getLocalBounds()), 8, 2);
}

// TODO: Rework this, this is just temporary -- probably use Grid/Flexbox?
void OscillatorComponent::resized()
{
    auto area = getLocalBounds();
    area.reduce(6, 6);

    auto row_height = area.getHeight() / 4;
    auto col_width = area.getWidth() / 4;

    auto gain_controls = area.removeFromTop(row_height);
    auto filter_controls = area.removeFromBottom(row_height);
    auto waveform_controls = area.reduced(0, 32);

    waveform_selector.setBounds(waveform_controls.removeFromLeft(col_width * 2)
                                    .removeFromTop(waveform_controls.getHeight() / 2));

    gain_attack_slider.setBounds(gain_controls.removeFromLeft(col_width));
    gain_decay_slider.setBounds(gain_controls.removeFromLeft(col_width));
    gain_sustain_slider.setBounds(gain_controls.removeFromLeft(col_width));
    gain_release_slider.setBounds(gain_controls.removeFromLeft(col_width));

    filter_enabled_button.setBounds(filter_controls.removeFromLeft(col_width));
    filter_cutoff_slider.setBounds(filter_controls.removeFromLeft(col_width));
    filter_q_slider.setBounds(filter_controls.removeFromLeft(col_width));
}

// ==========================================
void OscillatorComponent::populate_waveform_selector()
{
    waveform_selector_model->add_item("Sine");
    waveform_selector_model->add_item("Triangle");
    waveform_selector_model->add_item("Square");
    waveform_selector_model->add_item("Engineer's Sawtooth");

    waveform_selector.updateContent();
}