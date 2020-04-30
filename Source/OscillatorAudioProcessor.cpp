#include "OscillatorAudioProcessor.h"

#include "dsp/OscillatorVoice.h"

static constexpr size_t MAX_POLYPHONY = 16;

OscillatorAudioProcessor::OscillatorAudioProcessor(AudioProcessorValueTreeState &apvts) :
    AudioProcessor(),
    state(apvts)
{
    sine_table = std::make_shared<const BandlimitedOscillator::LookupTable>(make_sine());
    synth.addSound(new OscillatorSound());
}

void OscillatorAudioProcessor::prepareToPlay(double sample_rate, int samples_per_block)
{
    //    triangle_table =
    //        std::make_shared<const BandlimitedOscillator::LookupTable>(make_triangle(20.0,
    //        sampleRate));
    square_table =
        std::make_shared<const BandlimitedOscillator::LookupTable>(make_square(16.0, sample_rate));
    //    engineers_sawtooth_table = std::make_shared<const BandlimitedOscillator::LookupTable>(
    //        make_engineers_sawtooth(20.0, sampleRate));
    //    musicians_sawtooth_table = std::make_shared<const BandlimitedOscillator::LookupTable>(
    //        make_musicians_sawtooth(20.0, sampleRate));

    synth.clearVoices();
    synth.setCurrentPlaybackSampleRate(sample_rate);
    for (size_t i = 0; i < MAX_POLYPHONY; ++i) {
        synth.addVoice(
            new OscillatorVoice(std::make_unique<BandlimitedOscillator>(square_table), state));
    }
}

void OscillatorAudioProcessor::releaseResources() {}

void OscillatorAudioProcessor::processBlock(AudioBuffer<float> &buffer, MidiBuffer &midi_messages)
{
    synth.renderNextBlock(buffer, midi_messages, 0, buffer.getNumSamples());
}

// ============================================================

const String OscillatorAudioProcessor::getName() const
{
    return String();
}
double OscillatorAudioProcessor::getTailLengthSeconds() const
{
    return 0;
}
bool OscillatorAudioProcessor::acceptsMidi() const
{
    return false;
}
bool OscillatorAudioProcessor::producesMidi() const
{
    return false;
}
AudioProcessorEditor *OscillatorAudioProcessor::createEditor()
{
    return nullptr;
}
bool OscillatorAudioProcessor::hasEditor() const
{
    return false;
}
int OscillatorAudioProcessor::getNumPrograms()
{
    return 1;
}
int OscillatorAudioProcessor::getCurrentProgram()
{
    return 0;
}
void OscillatorAudioProcessor::setCurrentProgram(int index) {}
const String OscillatorAudioProcessor::getProgramName(int index)
{
    return String();
}
void OscillatorAudioProcessor::changeProgramName(int index, const String &newName) {}
void OscillatorAudioProcessor::getStateInformation(MemoryBlock &destData) {}
void OscillatorAudioProcessor::setStateInformation(const void *data, int sizeInBytes) {}
