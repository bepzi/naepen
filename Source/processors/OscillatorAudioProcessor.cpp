#include "OscillatorAudioProcessor.h"

#include "dsp/OscillatorVoice.h"

static constexpr size_t MAX_POLYPHONY = 16;

OscillatorAudioProcessor::OscillatorAudioProcessor(
    AudioProcessorValueTreeState &apvts, Identifier oscillator_id, const String &pan_id,
    const String &gain_id, const String &gain_attack_id, const String &gain_decay_id,
    const String &gain_sustain_id, const String &gain_release_id, Identifier filter_type_id,
    const String &filter_enabled_id, const String &filter_cutoff_id, const String &filter_q_id) :
    ProcessorBase(apvts),

    oscillator_id(oscillator_id),
    filter_type_id(filter_type_id)
{
    sine_osc = std::make_shared<BandlimitedOscillator>(
        std::make_shared<const BandlimitedOscillator::LookupTable>(make_sine()));
    white_noise_osc = std::make_shared<NoiseOscillator>();

    synth.addSound(new OscillatorSound());

    pan = state.getRawParameterValue(pan_id);

    gain = state.getRawParameterValue(gain_id);

    gain_attack = state.getRawParameterValue(gain_attack_id);
    gain_decay = state.getRawParameterValue(gain_decay_id);
    gain_sustain = state.getRawParameterValue(gain_sustain_id);
    gain_release = state.getRawParameterValue(gain_release_id);

    filter_enabled = state.getRawParameterValue(filter_enabled_id);
    filter_cutoff = state.getRawParameterValue(filter_cutoff_id);
    filter_q = state.getRawParameterValue(filter_q_id);

    state.state.addListener(this);
}

OscillatorAudioProcessor::~OscillatorAudioProcessor()
{
    state.state.removeListener(this);
}

void OscillatorAudioProcessor::prepareToPlay(double sample_rate, int samples_per_block)
{
    ignoreUnused(samples_per_block);

    triangle_osc = std::make_shared<BandlimitedOscillator>(
        std::make_shared<const BandlimitedOscillator::LookupTable>(
            make_triangle(20.0, sample_rate)));
    square_osc = std::make_shared<BandlimitedOscillator>(
        std::make_shared<const BandlimitedOscillator::LookupTable>(make_square(20.0, sample_rate)));
    engineers_sawtooth_osc = std::make_shared<BandlimitedOscillator>(
        std::make_shared<const BandlimitedOscillator::LookupTable>(
            make_engineers_sawtooth(20.0, sample_rate)));

    update_current_oscillator();
}

void OscillatorAudioProcessor::releaseResources() {}

void OscillatorAudioProcessor::processBlock(AudioBuffer<float> &buffer, MidiBuffer &midi_messages)
{
    buffer.clear();
    synth.renderNextBlock(buffer, midi_messages, 0, buffer.getNumSamples());
    buffer.applyGain(*gain);
}

void OscillatorAudioProcessor::valueTreePropertyChanged(
    ValueTree &treeWhosePropertyHasChanged, const Identifier &property)
{
    ignoreUnused(treeWhosePropertyHasChanged);

    if (property != oscillator_id) {
        return;
    }

    update_current_oscillator();
}

void OscillatorAudioProcessor::update_current_oscillator()
{
    auto waveform_name = state.state.getProperty(oscillator_id, "Sine").toString();

    synth.clearVoices();
    synth.setCurrentPlaybackSampleRate(getSampleRate());
    for (size_t i = 0; i < MAX_POLYPHONY; ++i) {
        std::unique_ptr<Oscillator> osc_copy = nullptr;

        // TODO: Do a table lookup
        // TODO: This is ugly as sin, redo this
        if (waveform_name == "Sine") {
            osc_copy = std::make_unique<BandlimitedOscillator>(
                *(dynamic_cast<BandlimitedOscillator *>(sine_osc.get())));
        } else if (waveform_name == "Triangle") {
            osc_copy = std::make_unique<BandlimitedOscillator>(
                *(dynamic_cast<BandlimitedOscillator *>(triangle_osc.get())));
        } else if (waveform_name == "Square") {
            osc_copy = std::make_unique<BandlimitedOscillator>(
                *(dynamic_cast<BandlimitedOscillator *>(square_osc.get())));
        } else if (waveform_name == "EngineersSawtooth") {
            osc_copy = std::make_unique<BandlimitedOscillator>(
                *(dynamic_cast<BandlimitedOscillator *>(engineers_sawtooth_osc.get())));
        } else if (waveform_name == "WhiteNoise") {
            osc_copy = std::make_unique<NoiseOscillator>(
                *(dynamic_cast<NoiseOscillator *>(white_noise_osc.get())));
        }

        jassert(osc_copy != nullptr);
        synth.addVoice(new OscillatorVoice(
            std::move(osc_copy), state, pan, gain_attack, gain_decay, gain_sustain, gain_release,
            filter_type_id, filter_enabled, filter_cutoff, filter_q));
    }
}
