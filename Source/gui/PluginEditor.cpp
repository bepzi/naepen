#include "PluginEditor.h"
#include "../DatabaseIdentifiers.h"

static constexpr auto EDITOR_WIDTH = 640;
static constexpr auto EDITOR_HEIGHT = 640;

static constexpr auto PADDING = 8;

NaepenAudioProcessorEditor::NaepenAudioProcessorEditor(NaepenAudioProcessor &proc) :
    AudioProcessorEditor(proc),
    processor(proc),
    master_gain_slider_attachment(
        processor.state, DatabaseIdentifiers::MASTER_GAIN.toString(), master_gain_slider),
    osc_one(processor.state)
{
    addAndMakeVisible(master_gain_slider);

    addAndMakeVisible(osc_one);

    setSize(EDITOR_WIDTH, EDITOR_HEIGHT);
}

void NaepenAudioProcessorEditor::paint(Graphics &g)
{
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
}

void NaepenAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    area.reduce(PADDING, PADDING);

    master_gain_slider.setBounds(area.removeFromLeft(128));
    area.removeFromLeft(128);
    osc_one.setBounds(area);
}

void NaepenAudioProcessorEditor::sliderValueChanged(Slider *slider)
{
    ignoreUnused(slider);
}
