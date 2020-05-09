#include "MainMixerComponent.h"

#include "DatabaseIdentifiers.h"

MainMixerComponent::MainMixerComponent(AudioProcessorValueTreeState &state) :
    master_gain_slider_attachment(
        state, DatabaseIdentifiers::MASTER_GAIN.toString(), master_gain_slider),
    osc_one_gain_slider_attachment(
        state, DatabaseIdentifiers::OSC_ONE_GAIN.toString(), osc_one_gain_slider),
    osc_two_gain_slider_attachment(
        state, DatabaseIdentifiers::OSC_TWO_GAIN.toString(), osc_two_gain_slider)
{
    addAndMakeVisible(master_gain_slider);

    addAndMakeVisible(osc_one_gain_slider);
    addAndMakeVisible(osc_two_gain_slider);
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

void MainMixerComponent::paint(Graphics &g)
{
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    g.setColour(Colours::lightgrey);
    g.drawRoundedRectangle(from_rect_int(getLocalBounds()), 8, 2);
}

void MainMixerComponent::resized()
{
    auto area = getLocalBounds();
    area.reduce(6, 6);

    auto width = area.getWidth() / 4;

    master_gain_slider.setBounds(area.removeFromLeft(width));
    area.removeFromLeft(width);

    osc_one_gain_slider.setBounds(area.removeFromLeft(width));
    osc_two_gain_slider.setBounds(area);
}
