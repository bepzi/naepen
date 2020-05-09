#include "OscillatorAudioProcessor.h"

#include "dsp/OscillatorVoice.h"

static constexpr size_t MAX_POLYPHONY = 16;

OscillatorAudioProcessor::OscillatorAudioProcessor(
    AudioProcessorValueTreeState &apvts, const String &waveform_id, const String &gain_id,
    const String &gain_attack_id, const String &gain_decay_id, const String &gain_sustain_id,
    const String &gain_release_id, const String &filter_enabled_id, const String &filter_cutoff_id,
    const String &filter_q_id) :
    ProcessorBase(apvts), waveform_id(waveform_id)
{
    sine_table = std::make_shared<const BandlimitedOscillator::LookupTable>(make_sine());
    current_waveform = sine_table;

    synth.addSound(new OscillatorSound());

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

    triangle_table = std::make_shared<const BandlimitedOscillator::LookupTable>(
        make_triangle(20.0, sample_rate));
    square_table =
        std::make_shared<const BandlimitedOscillator::LookupTable>(make_square(20.0, sample_rate));
    engineers_sawtooth_table = std::make_shared<const BandlimitedOscillator::LookupTable>(
        make_engineers_sawtooth(20.0, sample_rate));

    current_waveform = sine_table;
    auto waveform_name = state.state.getProperty(waveform_id, "Sine").toString();
    if (waveform_name == "Sine") {
        current_waveform = sine_table;
    } else if (waveform_name == "Triangle") {
        current_waveform = triangle_table;
    } else if (waveform_name == "Square") {
        current_waveform = square_table;
    } else if (waveform_name == "EngineersSawtooth") {
        current_waveform = engineers_sawtooth_table;
    }

    synth.clearVoices();
    synth.setCurrentPlaybackSampleRate(sample_rate);
    for (size_t i = 0; i < MAX_POLYPHONY; ++i) {
        synth.addVoice(new OscillatorVoice(
            std::make_unique<BandlimitedOscillator>(current_waveform), state, gain_attack,
            gain_decay, gain_sustain, gain_release, filter_enabled, filter_cutoff, filter_q));
    }
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

    if (property.toString() != waveform_id) {
        return;
    }

    // TODO: Just use a hashmap <String, shared_ptr>
    auto waveform_name = state.state.getProperty(property, "Sine").toString();
    if (waveform_name == "Sine") {
        current_waveform = sine_table;
    } else if (waveform_name == "Triangle") {
        current_waveform = triangle_table;
    } else if (waveform_name == "Square") {
        current_waveform = square_table;
    } else if (waveform_name == "EngineersSawtooth") {
        current_waveform = engineers_sawtooth_table;
    } else {
        // Waveform name in the ValueTree that we don't recognize
        return;
    }

    synth.clearVoices();
    synth.setCurrentPlaybackSampleRate(getSampleRate());
    for (size_t i = 0; i < MAX_POLYPHONY; ++i) {
        synth.addVoice(new OscillatorVoice(
            std::make_unique<BandlimitedOscillator>(current_waveform), state, gain_attack,
            gain_decay, gain_sustain, gain_release, filter_enabled, filter_cutoff, filter_q));
    }
}
