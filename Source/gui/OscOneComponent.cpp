#include "OscOneComponent.h"
#include "../DatabaseIdentifiers.h"

#include <JuceHeader.h>

OscOneComponent::OscOneComponent(APVTS &state) :
    gain_attack_slider_attachment(
        state, DatabaseIdentifiers::OSC_ONE_GAIN_ATTACK.toString(), gain_attack_slider),
    gain_decay_slider_attachment(
        state, DatabaseIdentifiers::OSC_ONE_GAIN_DECAY.toString(), gain_decay_slider),
    gain_sustain_slider_attachment(
        state, DatabaseIdentifiers::OSC_ONE_GAIN_SUSTAIN.toString(), gain_sustain_slider),
    gain_release_slider_attachment(
        state, DatabaseIdentifiers::OSC_ONE_GAIN_RELEASE.toString(), gain_release_slider),

    filter_enabled_button_attachment(
        state, DatabaseIdentifiers::OSC_ONE_FILTER_ENABLED.toString(), filter_enabled_button),
    filter_cutoff_slider_attachment(
        state, DatabaseIdentifiers::OSC_ONE_FILTER_CUTOFF.toString(), filter_cutoff_slider),
    filter_q_slider_attachment(
        state, DatabaseIdentifiers::OSC_ONE_FILTER_Q.toString(), filter_q_slider),

    filter_attack_slider_attachment(
        state, DatabaseIdentifiers::OSC_ONE_FILTER_ATTACK.toString(), filter_attack_slider),
    filter_decay_slider_attachment(
        state, DatabaseIdentifiers::OSC_ONE_FILTER_DECAY.toString(), filter_decay_slider),
    filter_sustain_slider_attachment(
        state, DatabaseIdentifiers::OSC_ONE_FILTER_SUSTAIN.toString(), filter_sustain_slider),
    filter_release_slider_attachment(
        state, DatabaseIdentifiers::OSC_ONE_FILTER_RELEASE.toString(), filter_release_slider)
{
    addAndMakeVisible(gain_attack_slider);
    addAndMakeVisible(gain_decay_slider);
    addAndMakeVisible(gain_sustain_slider);
    addAndMakeVisible(gain_release_slider);

    addAndMakeVisible(filter_enabled_button);
    addAndMakeVisible(filter_cutoff_slider);
    addAndMakeVisible(filter_q_slider);

    addAndMakeVisible(filter_attack_slider);
    addAndMakeVisible(filter_decay_slider);
    addAndMakeVisible(filter_sustain_slider);
    addAndMakeVisible(filter_release_slider);
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

void OscOneComponent::paint(Graphics &g)
{
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    g.setColour(Colours::lightgrey);
    g.drawRoundedRectangle(from_rect_int(getLocalBounds()), 8, 2);
}

// TODO: Rework this, this is just temporary -- probably use Grid/Flexbox?
void OscOneComponent::resized()
{
    auto area = getLocalBounds();
    area.reduce(6, 6);

    auto row_height = area.getHeight() / 4;
    auto col_width = area.getWidth() / 4;

    auto gain_controls = area.removeFromTop(row_height);
    auto filter_controls = area.removeFromBottom(row_height * 2);

    {
        gain_attack_slider.setBounds(gain_controls.removeFromLeft(col_width));
        gain_decay_slider.setBounds(gain_controls.removeFromLeft(col_width));
        gain_sustain_slider.setBounds(gain_controls.removeFromLeft(col_width));
        gain_release_slider.setBounds(gain_controls.removeFromLeft(col_width));
    }

    {
        auto adsr_area = filter_controls.removeFromTop(row_height);

        filter_attack_slider.setBounds(adsr_area.removeFromLeft(col_width));
        filter_decay_slider.setBounds(adsr_area.removeFromLeft(col_width));
        filter_sustain_slider.setBounds(adsr_area.removeFromLeft(col_width));
        filter_release_slider.setBounds(adsr_area.removeFromLeft(col_width));

        filter_enabled_button.setBounds(filter_controls.removeFromLeft(col_width));
        filter_controls.removeFromLeft(col_width);
        filter_cutoff_slider.setBounds(filter_controls.removeFromLeft(col_width));
        filter_q_slider.setBounds(filter_controls.removeFromLeft(col_width));
    }
}
