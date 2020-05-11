#include "MainMixerComponent.h"

#include "DatabaseIdentifiers.h"
#include "NaepenLookAndFeel.h"

static const int TEXT_HEIGHT = 24;

MainMixerComponent::MainMixerComponent(AudioProcessorValueTreeState &state) :
    master_gain_slider_attachment(
        state, DatabaseIdentifiers::MASTER_GAIN.toString(), master_gain_slider),
    osc_one_gain_slider_attachment(
        state, DatabaseIdentifiers::OSC_ONE_GAIN.toString(), osc_one_gain_slider),
    osc_two_gain_slider_attachment(
        state, DatabaseIdentifiers::OSC_TWO_GAIN.toString(), osc_two_gain_slider)
{
    addAndMakeVisible(master_gain_slider);
    master_gain_label.setJustificationType(Justification::centred);
    addAndMakeVisible(master_gain_label);

    addAndMakeVisible(osc_one_gain_slider);
    osc_one_gain_label.setJustificationType(Justification::centred);
    addAndMakeVisible(osc_one_gain_label);

    addAndMakeVisible(osc_two_gain_slider);
    osc_two_gain_label.setJustificationType(Justification::centred);
    addAndMakeVisible(osc_two_gain_label);
}

void MainMixerComponent::paint(Graphics &g)
{
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    g.setColour(Colour(NaepenLookAndFeel::outline_argb));
    g.drawRoundedRectangle(getLocalBounds().toFloat(), 8, 3);
}

void MainMixerComponent::resized()
{
    auto area = getLocalBounds();
    area.reduce(6, 6);

    auto width = area.getWidth() / 4;

    auto region = area.removeFromLeft(width);
    master_gain_label.setBounds(region.removeFromBottom(TEXT_HEIGHT));
    master_gain_slider.setBounds(region);

    area.removeFromLeft(width);

    region = area.removeFromLeft(width);
    osc_one_gain_label.setBounds(region.removeFromBottom(TEXT_HEIGHT));
    osc_one_gain_slider.setBounds(region);

    region = area.removeFromLeft(width);
    osc_two_gain_label.setBounds(region.removeFromBottom(TEXT_HEIGHT));
    osc_two_gain_slider.setBounds(region);
}
