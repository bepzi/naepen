#include "PluginProcessor.h"

#include "gui/PluginEditor.h"
#include "processors/OscillatorAudioProcessor.h"

#include "DatabaseIdentifiers.h"

using APVTS = juce::AudioProcessorValueTreeState;

//==============================================================================
NaepenAudioProcessor::NaepenAudioProcessor() :
    AudioProcessor(BusesProperties().withOutput("Output", AudioChannelSet::stereo(), true)),
    state(
        *this, nullptr, Identifier(DatabaseIdentifiers::DATABASE_TYPE_ID),
        create_parameter_layout())
{
    master_gain = state.getRawParameterValue(DatabaseIdentifiers::MASTER_GAIN);
}

//==============================================================================
const String NaepenAudioProcessor::getName() const
{
    return JucePlugin_Name;
}
bool NaepenAudioProcessor::acceptsMidi() const
{
    return true;
}
bool NaepenAudioProcessor::producesMidi() const
{
    return false;
}
bool NaepenAudioProcessor::isMidiEffect() const
{
    return false;
}
double NaepenAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}
int NaepenAudioProcessor::getNumPrograms()
{
    return 1;
}
int NaepenAudioProcessor::getCurrentProgram()
{
    return 0;
}
void NaepenAudioProcessor::setCurrentProgram(int index)
{
    ignoreUnused(index);
}
const String NaepenAudioProcessor::getProgramName(int index)
{
    ignoreUnused(index);
    return {};
}
void NaepenAudioProcessor::changeProgramName(int index, const String &new_name)
{
    ignoreUnused(index, new_name);
}

//==============================================================================
void NaepenAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    processor_graph.setPlayConfigDetails(
        getMainBusNumInputChannels(), getMainBusNumOutputChannels(), sampleRate, samplesPerBlock);
    processor_graph.prepareToPlay(sampleRate, samplesPerBlock);

    initialize_graph();

    midi_collector.reset(sampleRate);
}

void NaepenAudioProcessor::releaseResources()
{
    processor_graph.releaseResources();
}

bool NaepenAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
    if (layouts.getMainOutputChannelSet() == AudioChannelSet::disabled()) {
        return false;
    }

    return layouts.getMainOutputChannelSet() == AudioChannelSet::stereo();
}

void NaepenAudioProcessor::processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages)
{
    ScopedNoDenormals noDenormals;

    midi_collector.removeNextBlockOfMessages(midiMessages, buffer.getNumSamples());
    virtual_keyboard_state.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);

    processor_graph.processBlock(buffer, midiMessages);

    buffer.applyGain(*master_gain);
}

//==============================================================================
bool NaepenAudioProcessor::hasEditor() const
{
    return true;
}

AudioProcessorEditor *NaepenAudioProcessor::createEditor()
{
    return new NaepenAudioProcessorEditor(*this);
}

//==============================================================================
void NaepenAudioProcessor::getStateInformation(MemoryBlock &dest_data)
{
    auto copied_state = state.copyState();
    std::unique_ptr<XmlElement> xml(copied_state.createXml());
    copyXmlToBinary(*xml, dest_data);
}

void NaepenAudioProcessor::setStateInformation(const void *data, int size_in_bytes)
{
    std::unique_ptr<XmlElement> xml(getXmlFromBinary(data, size_in_bytes));
    if (xml && xml->hasTagName(state.state.getType())) {
        state.replaceState(ValueTree::fromXml(*xml));
    }
}

void NaepenAudioProcessor::initialize_graph()
{
    processor_graph.clear();

    audio_output_node =
        processor_graph.addNode(std::make_unique<AudioProcessorGraph::AudioGraphIOProcessor>(
            AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode));
    midi_input_node =
        processor_graph.addNode(std::make_unique<AudioProcessorGraph::AudioGraphIOProcessor>(
            AudioProcessorGraph::AudioGraphIOProcessor::midiInputNode));

    osc_one_node = processor_graph.addNode(std::make_unique<OscillatorAudioProcessor>(
        state, DatabaseIdentifiers::OSC_ONE_WAVEFORM,

        DatabaseIdentifiers::OSC_ONE_DETUNE_SEMITONES.toString(),
        DatabaseIdentifiers::OSC_ONE_DETUNE_CENTS.toString(),

        DatabaseIdentifiers::OSC_ONE_PAN.toString(),

        DatabaseIdentifiers::OSC_ONE_GAIN.toString(),
        DatabaseIdentifiers::OSC_ONE_GAIN_ATTACK.toString(),
        DatabaseIdentifiers::OSC_ONE_GAIN_DECAY.toString(),
        DatabaseIdentifiers::OSC_ONE_GAIN_SUSTAIN.toString(),
        DatabaseIdentifiers::OSC_ONE_GAIN_RELEASE.toString(),

        DatabaseIdentifiers::OSC_ONE_FILTER_TYPE,
        DatabaseIdentifiers::OSC_ONE_FILTER_ENABLED.toString(),
        DatabaseIdentifiers::OSC_ONE_FILTER_CUTOFF.toString(),
        DatabaseIdentifiers::OSC_ONE_FILTER_Q.toString()));
    osc_one_node->getProcessor()->setPlayConfigDetails(
        getMainBusNumInputChannels(), getMainBusNumOutputChannels(), getSampleRate(),
        getBlockSize());

    osc_two_node = processor_graph.addNode(std::make_unique<OscillatorAudioProcessor>(
        state, DatabaseIdentifiers::OSC_TWO_WAVEFORM,

        DatabaseIdentifiers::OSC_TWO_DETUNE_SEMITONES.toString(),
        DatabaseIdentifiers::OSC_TWO_DETUNE_CENTS.toString(),

        DatabaseIdentifiers::OSC_TWO_PAN.toString(),

        DatabaseIdentifiers::OSC_TWO_GAIN.toString(),
        DatabaseIdentifiers::OSC_TWO_GAIN_ATTACK.toString(),
        DatabaseIdentifiers::OSC_TWO_GAIN_DECAY.toString(),
        DatabaseIdentifiers::OSC_TWO_GAIN_SUSTAIN.toString(),
        DatabaseIdentifiers::OSC_TWO_GAIN_RELEASE.toString(),

        DatabaseIdentifiers::OSC_TWO_FILTER_TYPE,
        DatabaseIdentifiers::OSC_TWO_FILTER_ENABLED.toString(),
        DatabaseIdentifiers::OSC_TWO_FILTER_CUTOFF.toString(),
        DatabaseIdentifiers::OSC_TWO_FILTER_Q.toString()));
    osc_two_node->getProcessor()->setPlayConfigDetails(
        getMainBusNumInputChannels(), getMainBusNumOutputChannels(), getSampleRate(),
        getBlockSize());

    for (int channel = 0; channel < getMainBusNumOutputChannels(); ++channel) {
        // TODO: Be more robust against failure to connect graph nodes
        processor_graph.addConnection(
            {{osc_one_node->nodeID, channel}, {audio_output_node->nodeID, channel}});
        processor_graph.addConnection(
            {{osc_two_node->nodeID, channel}, {audio_output_node->nodeID, channel}});
    }

    // NOTE: We don't actually have to wire up the MIDI input node to the osc_one_component node,
    // probably because osc_one_component AudioProcessor _doesn't_ accept MIDI and we handle
    // retrieving messages, the virtual keyboard, etc. in this AudioProcessor before passing it on
    // to the graph to handle. I think.
}

// TODO: Add static methods to each automatable component to generate their own parameters
APVTS::ParameterLayout NaepenAudioProcessor::create_parameter_layout()
{
    NormalisableRange<float> gain_range = {0.0f, 1.0f, 0.001f};

    const StringArray filter_type_choices = {"Lowpass", "Highpass", "Bandpass"};

    NormalisableRange<float> adr_range = {0.0f, 5.0f, 0.001f};
    adr_range.setSkewForCentre(0.75f);

    NormalisableRange<float> sustain_range = {0.0f, 1.0f, 0.01f};

    NormalisableRange<int> detune_semitones_range = {-12, 12};
    NormalisableRange<int> detune_cents_range = {-100, 100};

    NormalisableRange<float> pan_range = {0.0f, 1.0f, 0.001f};

    // Global parameters
    // ====================================================
    auto master_gain_param = std::make_unique<AudioParameterFloat>(
        DatabaseIdentifiers::MASTER_GAIN.toString(), "Master Gain", gain_range, 0.5f);

    // Parameters for Oscillator 1
    // ====================================================
    auto osc_one_group = std::make_unique<AudioProcessorParameterGroup>(
        DatabaseIdentifiers::OSC_ONE_GROUP.toString(), "Oscillator 1", "|");
    {
        auto osc_one_detune_semitones = std::make_unique<AudioParameterInt>(
            DatabaseIdentifiers::OSC_ONE_DETUNE_SEMITONES.toString(), "Osc 1 Detune Semitones",
            detune_semitones_range.start, detune_semitones_range.end, 0);
        auto osc_one_detune_cents = std::make_unique<AudioParameterInt>(
            DatabaseIdentifiers::OSC_ONE_DETUNE_CENTS.toString(), "Osc 1 Detune Cents",
            detune_cents_range.start, detune_cents_range.end, 0);

        auto osc_one_pan = std::make_unique<AudioParameterFloat>(
            DatabaseIdentifiers::OSC_ONE_PAN.toString(), "Osc 1 Pan", pan_range, 0.5f);

        auto osc_one_gain = std::make_unique<AudioParameterFloat>(
            DatabaseIdentifiers::OSC_ONE_GAIN.toString(), "Osc 1 Gain", gain_range, 0.5f);
        auto osc_one_gain_attack = std::make_unique<AudioParameterFloat>(
            DatabaseIdentifiers::OSC_ONE_GAIN_ATTACK.toString(), "Osc 1 Gain Attack", adr_range,
            0.05f, "s");
        auto osc_one_gain_decay = std::make_unique<AudioParameterFloat>(
            DatabaseIdentifiers::OSC_ONE_GAIN_DECAY.toString(), "Osc 1 Gain Decay", adr_range,
            0.25f, "s");
        auto osc_one_gain_sustain = std::make_unique<AudioParameterFloat>(
            DatabaseIdentifiers::OSC_ONE_GAIN_SUSTAIN.toString(), "Osc 1 Gain Sustain",
            sustain_range, 1.0f);
        auto osc_one_gain_release = std::make_unique<AudioParameterFloat>(
            DatabaseIdentifiers::OSC_ONE_GAIN_RELEASE.toString(), "Osc 1 Gain Release", adr_range,
            0.15f, "s");

        osc_one_group->addChild(
            std::move(osc_one_detune_semitones), std::move(osc_one_detune_cents),
            std::move(osc_one_pan), std::move(osc_one_gain), std::move(osc_one_gain_attack),
            std::move(osc_one_gain_decay), std::move(osc_one_gain_sustain),
            std::move(osc_one_gain_release));

        auto osc_one_filter_type = std::make_unique<AudioParameterChoice>(
            DatabaseIdentifiers::OSC_ONE_FILTER_TYPE.toString(), "Osc 1 Filter Type",
            filter_type_choices, 0);
        auto osc_one_filter_enabled = std::make_unique<AudioParameterBool>(
            DatabaseIdentifiers::OSC_ONE_FILTER_ENABLED.toString(), "Osc 1 Filter Enabled", false);

        NormalisableRange<float> cutoff_range = {1.0f, 10000.0f, 0.1f};
        cutoff_range.setSkewForCentre(700.0f);
        auto osc_one_filter_cutoff = std::make_unique<AudioParameterFloat>(
            DatabaseIdentifiers::OSC_ONE_FILTER_CUTOFF.toString(), "Osc 1 Filter Cutoff",
            cutoff_range, 5000.0f, "Hz");
        NormalisableRange<float> q_range = {0.5f, 20.0f, 0.01f};
        q_range.setSkewForCentre(5.0f);
        auto osc_one_filter_q = std::make_unique<AudioParameterFloat>(
            DatabaseIdentifiers::OSC_ONE_FILTER_Q.toString(), "Osc 1 Filter Q", q_range, 0.5f);

        osc_one_group->addChild(
            std::move(osc_one_filter_type), std::move(osc_one_filter_enabled),
            std::move(osc_one_filter_cutoff), std::move(osc_one_filter_q));
    }

    // Parameters for Oscillator 2
    // ====================================================
    auto osc_two_group = std::make_unique<AudioProcessorParameterGroup>(
        DatabaseIdentifiers::OSC_TWO_GROUP.toString(), "Oscillator 2", "|");
    {
        auto osc_two_detune_semitones = std::make_unique<AudioParameterInt>(
            DatabaseIdentifiers::OSC_TWO_DETUNE_SEMITONES.toString(), "Osc 2 Detune Semitones",
            detune_semitones_range.start, detune_semitones_range.end, 0);
        auto osc_two_detune_cents = std::make_unique<AudioParameterInt>(
            DatabaseIdentifiers::OSC_TWO_DETUNE_CENTS.toString(), "Osc 2 Detune Cents",
            detune_cents_range.start, detune_cents_range.end, 0);

        auto osc_two_pan = std::make_unique<AudioParameterFloat>(
            DatabaseIdentifiers::OSC_TWO_PAN.toString(), "Osc 2 Pan", pan_range, 0.5f);

        auto osc_two_gain = std::make_unique<AudioParameterFloat>(
            DatabaseIdentifiers::OSC_TWO_GAIN.toString(), "Osc 2 Gain", gain_range, 0.0f);
        auto osc_two_gain_attack = std::make_unique<AudioParameterFloat>(
            DatabaseIdentifiers::OSC_TWO_GAIN_ATTACK.toString(), "Osc 2 Gain Attack", adr_range,
            0.05f, "s");
        auto osc_two_gain_decay = std::make_unique<AudioParameterFloat>(
            DatabaseIdentifiers::OSC_TWO_GAIN_DECAY.toString(), "Osc 2 Gain Decay", adr_range,
            0.25f, "s");
        auto osc_two_gain_sustain = std::make_unique<AudioParameterFloat>(
            DatabaseIdentifiers::OSC_TWO_GAIN_SUSTAIN.toString(), "Osc 2 Gain Sustain",
            sustain_range, 1.0f);
        auto osc_two_gain_release = std::make_unique<AudioParameterFloat>(
            DatabaseIdentifiers::OSC_TWO_GAIN_RELEASE.toString(), "Osc 2 Gain Release", adr_range,
            0.15f, "s");

        osc_two_group->addChild(
            std::move(osc_two_detune_semitones), std::move(osc_two_detune_cents),
            std::move(osc_two_pan), std::move(osc_two_gain), std::move(osc_two_gain_attack),
            std::move(osc_two_gain_decay), std::move(osc_two_gain_sustain),
            std::move(osc_two_gain_release));

        auto osc_two_filter_type = std::make_unique<AudioParameterChoice>(
            DatabaseIdentifiers::OSC_TWO_FILTER_TYPE.toString(), "Osc 2 Filter Type",
            filter_type_choices, 0);
        auto osc_two_filter_enabled = std::make_unique<AudioParameterBool>(
            DatabaseIdentifiers::OSC_TWO_FILTER_ENABLED.toString(), "Osc 2 Filter Enabled", false);

        NormalisableRange<float> cutoff_range = {1.0f, 10000.0f, 0.1f};
        cutoff_range.setSkewForCentre(700.0f);
        auto osc_two_filter_cutoff = std::make_unique<AudioParameterFloat>(
            DatabaseIdentifiers::OSC_TWO_FILTER_CUTOFF.toString(), "Osc 2 Filter Cutoff",
            cutoff_range, 5000.0f, "Hz");
        NormalisableRange<float> q_range = {0.5f, 20.0f, 0.01f};
        q_range.setSkewForCentre(5.0f);
        auto osc_two_filter_q = std::make_unique<AudioParameterFloat>(
            DatabaseIdentifiers::OSC_TWO_FILTER_Q.toString(), "Osc 2 Filter Q", q_range, 0.5f);

        osc_two_group->addChild(
            std::move(osc_two_filter_type), std::move(osc_two_filter_enabled),
            std::move(osc_two_filter_cutoff), std::move(osc_two_filter_q));
    }

    return {std::move(master_gain_param), std::move(osc_one_group), std::move(osc_two_group)};
}

//==============================================================================
AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new NaepenAudioProcessor();
}
