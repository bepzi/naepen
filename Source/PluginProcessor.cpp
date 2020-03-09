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
    // HACK: I shouldn't have to do this, esp. not here
    auto sine_wave = WavetableOsc<2048>(make_sine<2048>());
    auto triangle_wave = WavetableOsc<2048>(make_triangle<2048>(20.0));
    auto square_wave = WavetableOsc<2048>(make_square<2048>(20.0));
    auto sawtooth_wave = WavetableOsc<2048>(make_sawtooth<2048>(20.0));

    sine_wave.replace_table(make_sine<2048>());
    auto wt = Wavetable<2048>({sine_wave, triangle_wave, square_wave, sawtooth_wave});

    for (size_t i = 0; i < MAX_POLYPHONY; ++i) {
        synth.addVoice(new WavetableVoice<2048>(std::make_unique<Wavetable<2048>>(wt)));
    }

    synth.addSound(new WavetableSound());

    set_table_index(0.5);
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

static forcedinline void apply_master_gain(AudioBuffer<float> &buffer, float gain)
{
    {
        float *c0_buf = buffer.getWritePointer(0);
        for (int i = 0; i < buffer.getNumSamples(); ++i) {
            c0_buf[i] *= gain;
        }
    }

    const float *c0_buf = buffer.getReadPointer(0);
    for (int channel = 1; channel < buffer.getNumChannels(); ++channel) {
        float *buf = buffer.getWritePointer(channel);
        std::memcpy(buf, c0_buf, sizeof(float) * buffer.getNumSamples());
    }
}

void NaepenAudioProcessor::processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages)
{
    ScopedNoDenormals noDenormals;

    buffer.clear();

    if (getTotalNumOutputChannels() < 1) {
        return;
    }

    keyboard_state.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);
    midi_collector.removeNextBlockOfMessages(midiMessages, buffer.getNumSamples());
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    filter.set_params(filter_params, getSampleRate());
    filter.apply_to_buffer(buffer);

    visualizer.pushBuffer(buffer);

    apply_master_gain(buffer, gain);
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
