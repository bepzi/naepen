#include "PluginEditor.h"
#include "PluginProcessor.h"

NaepenAudioProcessorEditor::NaepenAudioProcessorEditor(NaepenAudioProcessor &p) :
    AudioProcessorEditor(&p), processor(p), visualizer(2), freq_slider(Slider::RotaryHorizontalVerticalDrag, Slider::TextBoxBelow)
{
    visualizer.setBufferSize(512); // TODO: No, don't do this here
    visualizer.setSamplesPerBlock(8);

    freq_slider.setRange(0.0, 5000.0);
    freq_slider.setSkewFactorFromMidPoint(500.0);

    addAndMakeVisible(visualizer);
    addAndMakeVisible(freq_slider);

    freq_slider.addListener(this);

    setSize(640, 480);
    setVisible(true);
}

NaepenAudioProcessorEditor::~NaepenAudioProcessorEditor() = default;

//==============================================================================
void NaepenAudioProcessorEditor::paint(Graphics &g)
{
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
}

void NaepenAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    area.reduce(8, 8);

    freq_slider.setBounds(area.removeFromTop(128));
    visualizer.setBounds(area);
}

void NaepenAudioProcessorEditor::sliderValueChanged(Slider *slider) {
    if (slider == &freq_slider) {
        processor.table.set_freq((float)freq_slider.getValue());
    }
}
