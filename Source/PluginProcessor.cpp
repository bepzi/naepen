#include "PluginProcessor.h"
#include "PluginEditor.h"

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
    table.set_freq(0.0f);
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

void NaepenAudioProcessor::setCurrentProgram(int index) {}

const String NaepenAudioProcessor::getProgramName(int index)
{
    return {};
}

void NaepenAudioProcessor::changeProgramName(int index, const String &newName) {}

//==============================================================================
void NaepenAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    visualizer.setBufferSize(samplesPerBlock);
    visualizer.setSamplesPerBlock(samplesPerBlock / 64);
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
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    if (totalNumOutputChannels < 1) {
        return;
    }

    // Do the computations for channel 0
    {
        float *c0_buf = buffer.getWritePointer(0);
        for (int i = 0; i < buffer.getNumSamples(); ++i) {
            c0_buf[i] = table.get_next_sample() * gain;
        }
    }

    // Copy-paste the computations from channel 0 to the others
    const float *c0_buf = buffer.getReadPointer(0);
    for (int channel = 1; channel < totalNumOutputChannels; ++channel) {
        float *buf = buffer.getWritePointer(channel);
        std::memcpy(buf, c0_buf, sizeof(float) * buffer.getNumSamples());
    }

    visualizer.pushBuffer(buffer);
}

//==============================================================================
bool NaepenAudioProcessor::hasEditor() const
{
    return true;  // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor *NaepenAudioProcessor::createEditor()
{
    return new NaepenAudioProcessorEditor(*this, visualizer);
}

//==============================================================================
void NaepenAudioProcessor::getStateInformation(MemoryBlock &destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void NaepenAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new NaepenAudioProcessor();
}
