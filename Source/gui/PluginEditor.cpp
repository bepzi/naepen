#include "PluginEditor.h"
#include "DatabaseIdentifiers.h"

static constexpr auto EDITOR_WIDTH = 800;
static constexpr auto EDITOR_HEIGHT = 800;

static constexpr auto PADDING = 8;

NaepenAudioProcessorEditor::NaepenAudioProcessorEditor(NaepenAudioProcessor &proc) :
    AudioProcessorEditor(proc),
    naepenProcessor(proc),
    master_gain_slider_attachment(
        naepenProcessor.state, DatabaseIdentifiers::MASTER_GAIN.toString(), master_gain_slider),
    osc_one_component(
        naepenProcessor.state, DatabaseIdentifiers::OSC_ONE_GAIN_ATTACK.toString(),
        DatabaseIdentifiers::OSC_ONE_GAIN_DECAY.toString(),
        DatabaseIdentifiers::OSC_ONE_GAIN_SUSTAIN.toString(),
        DatabaseIdentifiers::OSC_ONE_GAIN_RELEASE.toString(),
        DatabaseIdentifiers::OSC_ONE_FILTER_ENABLED.toString(),
        DatabaseIdentifiers::OSC_ONE_FILTER_CUTOFF.toString(),
        DatabaseIdentifiers::OSC_ONE_FILTER_Q.toString()),
    midi_keyboard(naepenProcessor.virtual_keyboard_state, MidiKeyboardComponent::horizontalKeyboard)
{
    midi_keyboard.setOctaveForMiddleC(4);

    addAndMakeVisible(master_gain_slider);

    addAndMakeVisible(midi_keyboard);
    addAndMakeVisible(osc_one_component);

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

    midi_keyboard.setBounds(area.removeFromBottom(64));
    area.removeFromBottom(PADDING);

    master_gain_slider.setBounds(area.removeFromLeft(128));
    area.removeFromLeft(128);

    osc_one_component.setBounds(area.removeFromTop(area.getHeight() / 2));
}
