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
        state, DatabaseIdentifiers::OSC_ONE_FILTER_Q.toString(), filter_q_slider)
{
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

void OscOneComponent::paint(Graphics &g)
{
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    g.setColour(Colours::lightgrey);
    g.drawRoundedRectangle(from_rect_int(getLocalBounds()), 8, 2);
}

void OscOneComponent::resized()
{
    auto area = getLocalBounds();
    area.reduce(6, 6);

    {
        auto adsr_area = area.removeFromTop((area.getHeight() / 2) - 8);
        auto width = adsr_area.getWidth() / 4;
        gain_attack_slider.setBounds(adsr_area.removeFromLeft(width));
        gain_decay_slider.setBounds(adsr_area.removeFromLeft(width));
        gain_sustain_slider.setBounds(adsr_area.removeFromLeft(width));
        gain_release_slider.setBounds(adsr_area.removeFromLeft(width));
    }
    area.removeFromTop(8);

    {
        auto filter_area = area;

        auto width = filter_area.getWidth() / 4;
        filter_enabled_button.setBounds(filter_area.removeFromLeft(width));
        filter_area.removeFromLeft(width);
        filter_cutoff_slider.setBounds(filter_area.removeFromLeft(width));
        filter_q_slider.setBounds(filter_area.removeFromLeft(width));
    }
}
