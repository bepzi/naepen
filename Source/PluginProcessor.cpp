#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "SynthSounds.h"
#include "WavetableOsc.h"

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
    visualizer(2)
{
    for (size_t i = 0; i < MAX_POLYPHONY; ++i) {
        synth.addVoice(new WavetableVoice<2048>(std::make_unique<WavetableOsc<2048>>(square_wave)));
    }

    synth.addSound(new WavetableSound());
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
    visualizer.setBufferSize(samplesPerBlock);
    visualizer.setSamplesPerBlock(samplesPerBlock / 64);

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

    if (getTotalNumOutputChannels() < 1) {
        return;
    }

    midi_collector.removeNextBlockOfMessages(midiMessages, buffer.getNumSamples());
    keyboard_state.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);

    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    // Apply master gain
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        float *buf = buffer.getWritePointer(channel);
        for (int i = 0; i < buffer.getNumSamples(); ++i) {
            buf[i] *= gain;
        }
    }

    visualizer.pushBuffer(buffer);
}

//==============================================================================
bool NaepenAudioProcessor::hasEditor() const
{
    return true;
}

AudioProcessorEditor *NaepenAudioProcessor::createEditor()
{
    return new NaepenAudioProcessorEditor(*this, visualizer, keyboard_state);
}

//==============================================================================
void NaepenAudioProcessor::getStateInformation(MemoryBlock &dest_data)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    ignoreUnused(dest_data);
}

void NaepenAudioProcessor::setStateInformation(const void *data, int size_in_bytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    ignoreUnused(data, size_in_bytes);
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new NaepenAudioProcessor();
}
