#include "PluginProcessor.h"
#include "dsp/OscillatorVoice.h"
#include "gui/PluginEditor.h"

#include "DatabaseIdentifiers.h"

static constexpr size_t MAX_POLYPHONY = 12;

//==============================================================================
NaepenAudioProcessor::NaepenAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    :
    AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                       .withInput("Input", AudioChannelSet::stereo(), true)
#endif
                       .withOutput("Output", AudioChannelSet::stereo(), true)
#endif
                       ),
#endif
    state(
        *this, nullptr, Identifier(DatabaseIdentifiers::DATABASE_TYPE_ID),
        create_parameter_layout())
{
    sine_table = std::make_shared<const BandlimitedOscillator::LookupTable>(make_sine());
    synth.addSound(new OscillatorSound());

    master_gain = state.getRawParameterValue(DatabaseIdentifiers::MASTER_GAIN);
}

NaepenAudioProcessor::~NaepenAudioProcessor() = default;

//==============================================================================
const String NaepenAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool NaepenAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool NaepenAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool NaepenAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double NaepenAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int NaepenAudioProcessor::getNumPrograms()
{
    return 1;  // NB: some hosts don't cope very well if you tell them there are 0 programs,
               // so this should be at least 1, even if you're not really implementing programs.
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
    ignoreUnused(samplesPerBlock);

    triangle_table =
        std::make_shared<const BandlimitedOscillator::LookupTable>(make_triangle(20.0, sampleRate));
    square_table =
        std::make_shared<const BandlimitedOscillator::LookupTable>(make_square(20.0, sampleRate));
    engineers_sawtooth_table = std::make_shared<const BandlimitedOscillator::LookupTable>(
        make_engineers_sawtooth(20.0, sampleRate));
    musicians_sawtooth_table = std::make_shared<const BandlimitedOscillator::LookupTable>(
        make_musicians_sawtooth(20.0, sampleRate));

    synth.clearVoices();
    synth.setCurrentPlaybackSampleRate(sampleRate);
    for (size_t i = 0; i < MAX_POLYPHONY; ++i) {
        synth.addVoice(new OscillatorVoice(
            std::make_unique<BandlimitedOscillator>(musicians_sawtooth_table), state));
    }

    midi_collector.reset(sampleRate);
}

void NaepenAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool NaepenAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
#if JucePlugin_IsMidiEffect
    ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // We only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono() &&
        layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

        // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void NaepenAudioProcessor::processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages)
{
    ScopedNoDenormals noDenormals;

    buffer.clear();

    midi_collector.removeNextBlockOfMessages(midiMessages, buffer.getNumSamples());
    keyboard_state.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

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
#ifdef DEBUG
// Don't save/load patch state when compiling in debug mode, for development purposes
void NaepenAudioProcessor::getStateInformation(MemoryBlock &dest_data)
{
    ignoreUnused(dest_data);
}

void NaepenAudioProcessor::setStateInformation(const void *data, int size_in_bytes)
{
    ignoreUnused(data, size_in_bytes);
}
#else
void NaepenAudioProcessor::getStateInformation(MemoryBlock &dest_data)
{
    auto copied_state = state.copyState();
    std::unique_ptr<XmlElement> xml(copied_state.createXml());
    copyXmlToBinary(*xml, dest_data);
}

void NaepenAudioProcessor::setStateInformation(const void *data, int size_in_bytes)
{
    std::unique_ptr<XmlElement> xml(getXmlFromBinary(data, size_in_bytes));
    if (xml) {
        if (xml->hasTagName(state.state.getType())) {
            state.replaceState(ValueTree::fromXml(*xml));
        }
    }
}
#endif

APVTS::ParameterLayout NaepenAudioProcessor::create_parameter_layout()
{
    // Global parameters
    // ====================================================
    auto master_gain_param = std::make_unique<AudioParameterFloat>(
        DatabaseIdentifiers::MASTER_GAIN.toString(), "Master Gain",
        (NormalisableRange<float>) {0.0f, 1.0f}, 1.0f);

    // Parameters for Oscillator 1
    // ====================================================
    auto osc_one_group = std::make_unique<AudioProcessorParameterGroup>(
        DatabaseIdentifiers::OSC_ONE_GROUP.toString(), "Oscillator 1", "|");
    {
        NormalisableRange<float> adr_range = {0.0f, 5.0f};
        adr_range.setSkewForCentre(0.75f);

        auto osc_one_gain_attack = std::make_unique<AudioParameterFloat>(
            DatabaseIdentifiers::OSC_ONE_GAIN_ATTACK.toString(), "Osc 1 Gain Attack", adr_range,
            0.05f, "s");
        auto osc_one_gain_decay = std::make_unique<AudioParameterFloat>(
            DatabaseIdentifiers::OSC_ONE_GAIN_DECAY.toString(), "Osc 1 Gain Decay", adr_range,
            0.25f, "s");
        auto osc_one_gain_sustain = std::make_unique<AudioParameterFloat>(
            DatabaseIdentifiers::OSC_ONE_GAIN_SUSTAIN.toString(), "Osc 1 Gain Sustain",
            (NormalisableRange<float>) {0.0f, 1.0f}, 1.0f);
        auto osc_one_gain_release = std::make_unique<AudioParameterFloat>(
            DatabaseIdentifiers::OSC_ONE_GAIN_RELEASE.toString(), "Osc 1 Gain Release", adr_range,
            0.15f, "s");

        osc_one_group->addChild(
            std::move(osc_one_gain_attack), std::move(osc_one_gain_decay),
            std::move(osc_one_gain_sustain), std::move(osc_one_gain_release));

        auto osc_one_filter_enabled = std::make_unique<AudioParameterBool>(
            DatabaseIdentifiers::OSC_ONE_FILTER_ENABLED.toString(), "Osc 1 Filter Enabled", false);

        NormalisableRange<float> cutoff_range = {1.0f, 10000.0f};
        cutoff_range.setSkewForCentre(1000.0f);
        auto osc_one_filter_cutoff = std::make_unique<AudioParameterFloat>(
            DatabaseIdentifiers::OSC_ONE_FILTER_CUTOFF.toString(), "Osc 1 Filter Cutoff",
            cutoff_range, 5000.0f, "Hz");

        NormalisableRange<float> q_range = {0.5f, 20.0f};
        q_range.setSkewForCentre(5.0f);
        auto osc_one_filter_q = std::make_unique<AudioParameterFloat>(
            DatabaseIdentifiers::OSC_ONE_FILTER_Q.toString(), "Osc 1 Filter Q", q_range, 0.5f);

        osc_one_group->addChild(
            std::move(osc_one_filter_enabled), std::move(osc_one_filter_cutoff),
            std::move(osc_one_filter_q));

        //        auto osc_one_filter_attack = std::make_unique<AudioParameterFloat>(
        //            DatabaseIdentifiers::OSC_ONE_FILTER_ATTACK.toString(), "Osc 1 Filter Attack",
        //            adr_range, 0.05f, "s");
        //
        //        auto osc_one_filter_decay = std::make_unique<AudioParameterFloat>(
        //            DatabaseIdentifiers::OSC_ONE_FILTER_DECAY.toString(), "Osc 1 Filter Decay",
        //            adr_range, 0.25f, "s");
        //
        //        auto osc_one_filter_sustain = std::make_unique<AudioParameterFloat>(
        //            DatabaseIdentifiers::OSC_ONE_FILTER_SUSTAIN.toString(), "Osc 1 Filter
        //            Sustain", (NormalisableRange<float>) {0.0f, 1.0f}, 1.0f);
        //
        //        auto osc_one_filter_release = std::make_unique<AudioParameterFloat>(
        //            DatabaseIdentifiers::OSC_ONE_FILTER_RELEASE.toString(), "Osc 1 Filter
        //            Release", adr_range, 0.15f, "s");
        //
        //        osc_one_group->addChild(
        //            std::move(osc_one_filter_attack), std::move(osc_one_filter_decay),
        //            std::move(osc_one_filter_sustain), std::move(osc_one_filter_release));
    }

    return {std::move(master_gain_param), std::move(osc_one_group)};
}

//==============================================================================
AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new NaepenAudioProcessor();
}
