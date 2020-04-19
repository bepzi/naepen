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
        state, DatabaseIdentifiers::OSC_ONE_GAIN_RELEASE.toString(), gain_release_slider)
{
    gain_attack_slider.setSkewFactorFromMidPoint(0.75);
    addAndMakeVisible(gain_attack_slider);

    gain_decay_slider.setSkewFactorFromMidPoint(0.75);
    addAndMakeVisible(gain_decay_slider);

    addAndMakeVisible(gain_sustain_slider);

    gain_release_slider.setSkewFactorFromMidPoint(0.75);
    addAndMakeVisible(gain_release_slider);
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

    auto width = area.getWidth() / 4;
    gain_attack_slider.setBounds(area.removeFromLeft(width));
    gain_decay_slider.setBounds(area.removeFromLeft(width));
    gain_sustain_slider.setBounds(area.removeFromLeft(width));
    gain_release_slider.setBounds(area.removeFromLeft(width));
}
