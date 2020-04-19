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
    //    visualizer.setBufferSize(samplesPerBlock);
    //    visualizer.setSamplesPerBlock(samplesPerBlock / 64);

    triangle_table =
        std::make_shared<const BandlimitedOscillator::LookupTable>(make_triangle(20.0, sampleRate));
    square_table =
        std::make_shared<const BandlimitedOscillator::LookupTable>(make_square(20.0, sampleRate));
    sawtooth_table =
        std::make_shared<const BandlimitedOscillator::LookupTable>(make_sawtooth(20.0, sampleRate));

    synth.clearVoices();
    for (size_t i = 0; i < MAX_POLYPHONY; ++i) {
        synth.addVoice(new OscillatorVoice(std::make_unique<BandlimitedOscillator>(square_table)));
    }
    synth.setCurrentPlaybackSampleRate(sampleRate);

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
    // In this template code we only support mono or stereo.
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

    float master_gain = state.getParameter(DatabaseIdentifiers::MASTER_GAIN)->getValue();
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        float *buf = buffer.getWritePointer(channel);
        for (int i = 0; i < buffer.getNumSamples(); ++i) {
            buf[i] *= master_gain;
        }
    }

    //    filter.set_params(filter_params, getSampleRate());
    //    filter.apply_to_buffer(buffer);
    //
    //    visualizer.pushBuffer(buffer);
    //
    //    apply_master_gain(buffer, gain);
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
    auto xml = state.state.toXmlString();
    dest_data.append(xml.getCharPointer(), xml.getNumBytesAsUTF8());
}

void NaepenAudioProcessor::setStateInformation(const void *data, int size_in_bytes)
{
    auto xml = String::fromUTF8((const char *)data, size_in_bytes);
    state.replaceState(ValueTree::fromXml(xml));
}

APVTS::ParameterLayout NaepenAudioProcessor::create_parameter_layout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back(std::make_unique<AudioParameterFloat>(
        DatabaseIdentifiers::MASTER_GAIN.toString(), "Master Gain", 0.0, 1.0, 1.0));

    return {params.begin(), params.end()};
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new NaepenAudioProcessor();
}
