#include "PluginEditor.h"
#include "DatabaseIdentifiers.h"

static constexpr auto EDITOR_WIDTH = 800;
static constexpr auto EDITOR_HEIGHT = 800;

static constexpr auto PADDING = 8;

NaepenAudioProcessorEditor::NaepenAudioProcessorEditor(NaepenAudioProcessor &proc) :
    AudioProcessorEditor(proc),
    naepenProcessor(proc),
    main_mixer_component(naepenProcessor.state),
    osc_one_component(
        naepenProcessor.state,

        DatabaseIdentifiers::OSC_ONE_WAVEFORM,

        DatabaseIdentifiers::OSC_ONE_DETUNE_SEMITONES.toString(),
        DatabaseIdentifiers::OSC_ONE_DETUNE_CENTS.toString(),

        DatabaseIdentifiers::OSC_ONE_PAN.toString(),

        DatabaseIdentifiers::OSC_ONE_GAIN_ATTACK.toString(),
        DatabaseIdentifiers::OSC_ONE_GAIN_DECAY.toString(),
        DatabaseIdentifiers::OSC_ONE_GAIN_SUSTAIN.toString(),
        DatabaseIdentifiers::OSC_ONE_GAIN_RELEASE.toString(),

        DatabaseIdentifiers::OSC_ONE_FILTER_TYPE.toString(),
        DatabaseIdentifiers::OSC_ONE_FILTER_ENABLED.toString(),
        DatabaseIdentifiers::OSC_ONE_FILTER_CUTOFF.toString(),
        DatabaseIdentifiers::OSC_ONE_FILTER_Q.toString()),

    osc_two_component(
        naepenProcessor.state,

        DatabaseIdentifiers::OSC_TWO_WAVEFORM,

        DatabaseIdentifiers::OSC_TWO_DETUNE_SEMITONES.toString(),
        DatabaseIdentifiers::OSC_TWO_DETUNE_CENTS.toString(),

        DatabaseIdentifiers::OSC_TWO_PAN.toString(),

        DatabaseIdentifiers::OSC_TWO_GAIN_ATTACK.toString(),
        DatabaseIdentifiers::OSC_TWO_GAIN_DECAY.toString(),
        DatabaseIdentifiers::OSC_TWO_GAIN_SUSTAIN.toString(),
        DatabaseIdentifiers::OSC_TWO_GAIN_RELEASE.toString(),

        DatabaseIdentifiers::OSC_TWO_FILTER_TYPE.toString(),
        DatabaseIdentifiers::OSC_TWO_FILTER_ENABLED.toString(),
        DatabaseIdentifiers::OSC_TWO_FILTER_CUTOFF.toString(),
        DatabaseIdentifiers::OSC_TWO_FILTER_Q.toString()),
    midi_keyboard(naepenProcessor.virtual_keyboard_state, MidiKeyboardComponent::horizontalKeyboard)
{
    midi_keyboard.setOctaveForMiddleC(4);

    addAndMakeVisible(midi_keyboard);

    addAndMakeVisible(main_mixer_component);

    addAndMakeVisible(osc_one_component);
    addAndMakeVisible(osc_two_component);

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

    main_mixer_component.setBounds(
        area.removeFromLeft(area.getWidth() / 3).removeFromBottom(area.getHeight() / 4));

    osc_one_component.setBounds(area.removeFromTop(area.getHeight() / 2));
    osc_two_component.setBounds(area);
}
