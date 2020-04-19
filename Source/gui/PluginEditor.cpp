#include "PluginEditor.h"

static constexpr auto EDITOR_WIDTH = 640;
static constexpr auto EDITOR_HEIGHT = 640;

static constexpr auto PADDING = 8;

NaepenAudioProcessorEditor::NaepenAudioProcessorEditor(NaepenAudioProcessor &proc) :
    AudioProcessorEditor(proc),
    processor(proc),
    master_gain_slider_attachment(processor.state, "MasterGain", master_gain_slider)
{
    master_gain_slider.setRange(0.0, 1.0, 0.0001);
    master_gain_slider.setValue(1.0, dontSendNotification);
    addAndMakeVisible(master_gain_slider);

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

    master_gain_slider.setBounds(area);
}

void NaepenAudioProcessorEditor::sliderValueChanged(Slider *slider)
{
    ignoreUnused(slider);
}
