#include "OscillatorAudioProcessor.h"

#include "dsp/OscillatorVoice.h"

static constexpr size_t MAX_POLYPHONY = 16;

OscillatorAudioProcessor::OscillatorAudioProcessor(
    AudioProcessorValueTreeState &apvts, const String &gain_id, const String &gain_attack_id,
    const String &gain_decay_id, const String &gain_sustain_id, const String &gain_release_id,
    const String &filter_enabled_id, const String &filter_cutoff_id, const String &filter_q_id) :
    ProcessorBase(apvts)
{
    sine_table = std::make_shared<const BandlimitedOscillator::LookupTable>(make_sine());
    synth.addSound(new OscillatorSound());

    gain = state.getRawParameterValue(gain_id);

    gain_attack = state.getRawParameterValue(gain_attack_id);
    gain_decay = state.getRawParameterValue(gain_decay_id);
    gain_sustain = state.getRawParameterValue(gain_sustain_id);
    gain_release = state.getRawParameterValue(gain_release_id);

    filter_enabled = state.getRawParameterValue(filter_enabled_id);
    filter_cutoff = state.getRawParameterValue(filter_cutoff_id);
    filter_q = state.getRawParameterValue(filter_q_id);
}

void OscillatorAudioProcessor::prepareToPlay(double sample_rate, int samples_per_block)
{
    ignoreUnused(samples_per_block);

    //    triangle_table =
    //        std::make_shared<const BandlimitedOscillator::LookupTable>(make_triangle(20.0,
    //        sampleRate));
    //    square_table =
    //        std::make_shared<const BandlimitedOscillator::LookupTable>(make_square(16.0,
    //        sample_rate));
    //    engineers_sawtooth_table = std::make_shared<const BandlimitedOscillator::LookupTable>(
    //        make_engineers_sawtooth(20.0, sampleRate));
    //    musicians_sawtooth_table = std::make_shared<const BandlimitedOscillator::LookupTable>(
    //        make_musicians_sawtooth(20.0, sampleRate));

    synth.clearVoices();
    synth.setCurrentPlaybackSampleRate(sample_rate);
    for (size_t i = 0; i < MAX_POLYPHONY; ++i) {
        synth.addVoice(new OscillatorVoice(
            std::make_unique<BandlimitedOscillator>(sine_table), state, gain_attack, gain_decay,
            gain_sustain, gain_release, filter_enabled, filter_cutoff, filter_q));
    }
}

void OscillatorAudioProcessor::releaseResources() {}

void OscillatorAudioProcessor::processBlock(AudioBuffer<float> &buffer, MidiBuffer &midi_messages)
{
    buffer.clear();
    synth.renderNextBlock(buffer, midi_messages, 0, buffer.getNumSamples());
    buffer.applyGain(*gain);
}
