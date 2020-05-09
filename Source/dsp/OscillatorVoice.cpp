#include "OscillatorVoice.h"

#include <utility>

#include "DatabaseIdentifiers.h"

bool OscillatorSound::appliesToNote(int midi_note_number)
{
    ignoreUnused(midi_note_number);
    return true;
}

bool OscillatorSound::appliesToChannel(int midi_channel)
{
    ignoreUnused(midi_channel);
    return true;
}

//==============================================================================

OscillatorVoice::OscillatorVoice(
    std::unique_ptr<Oscillator> oscillator, AudioProcessorValueTreeState &apvts,
    std::atomic<float> *pan, std::atomic<float> *gain_attack, std::atomic<float> *gain_decay,
    std::atomic<float> *gain_sustain, std::atomic<float> *gain_release, Identifier filter_type_id,
    std::atomic<float> *filter_enabled, std::atomic<float> *filter_cutoff,
    std::atomic<float> *filter_q) :
    osc(std::move(oscillator)),
    state(apvts),

    pan(pan),

    gain_attack(gain_attack),
    gain_decay(gain_decay),
    gain_sustain(gain_sustain),
    gain_release(gain_release),

    filter_type_id(std::move(filter_type_id)),
    filter_enabled(filter_enabled),
    filter_cutoff(filter_cutoff),
    filter_q(filter_q)
{
}

bool OscillatorVoice::canPlaySound(SynthesiserSound *sound)
{
    return dynamic_cast<OscillatorSound *>(sound) != nullptr;
}

void OscillatorVoice::startNote(
    int midi_note_number, float velocity, SynthesiserSound *sound, int pitch_wheel_pos)
{
    ignoreUnused(sound);

    level = velocity * 0.3f;
    osc->set_freq(calc_freq(midi_note_number, pitch_wheel_pos));

    ADSR::Parameters osc_one_gain_params = {
        *gain_attack, *gain_decay, *gain_sustain, *gain_release};
    gain_envelope.setParameters(osc_one_gain_params);
    gain_envelope.noteOn();
}

void OscillatorVoice::stopNote(float velocity, bool allow_tail_off)
{
    ignoreUnused(velocity);

    if (allow_tail_off) {
        gain_envelope.noteOff();
    } else {
        clearCurrentNote();
    }
}

void OscillatorVoice::pitchWheelMoved(int pitch_wheel_pos)
{
    osc->set_freq(calc_freq(getCurrentlyPlayingNote(), pitch_wheel_pos));
}

void OscillatorVoice::controllerMoved(int, int) {}

void OscillatorVoice::renderNextBlock(
    AudioBuffer<float> &output_buffer, int start_sample, int num_samples)
{
    jassert(output_buffer.getNumChannels() == 2);

    auto filter_type = static_cast<SvfFilter::Type>(
        reinterpret_cast<AudioParameterChoice *>(state.getParameter(filter_type_id))->getIndex());
    if (filter_type != filter.get_type()) {
        filter.set_type(filter_type);
    }

    for (int i = start_sample; i < start_sample + num_samples; ++i) {
        if (!gain_envelope.isActive()) {
            clearCurrentNote();
            break;
        }

        auto gain_env_sample = gain_envelope.getNextSample();
        auto sample = osc->get_next_sample() * level * gain_env_sample;

        // Optionally apply the filter
        if (*filter_enabled > 0.5f) {
            filter.set_params({*filter_cutoff, *filter_q}, getSampleRate());
            sample = filter.get_next_sample(sample);
        }

        output_buffer.addSample(
            0, i, sample * sinf((1 - *pan) * MathConstants<float>::halfPi));  // Left
        output_buffer.addSample(1, i, sample * sinf(*pan * MathConstants<float>::halfPi));  // Right
    }
}

void OscillatorVoice::setCurrentPlaybackSampleRate(double new_rate)
{
    osc->set_sample_rate(new_rate);

    gain_envelope.setSampleRate(new_rate);
}

double OscillatorVoice::calc_freq(int nn, int wheel_pos)
{
    // TODO: BEND_RANGE can be a user-changeable parameter
    static constexpr int BEND_RANGE = 4;  // semitones of pitch bend

    jassert(wheel_pos >= 0);
    if (nn < 0) {
        return 0.0;
    }

    // 0 = pitch down; 8192 = neutral; 16383 = pitch up
    double pitch_ratio = (wheel_pos - 8192) / (double)16383;
    int sign = (pitch_ratio == 0.0) ? 0 : (pitch_ratio < 0 ? -1 : 1);

    double curr_freq = MidiMessage::getMidiNoteInHertz(nn);
    double bent_freq = MidiMessage::getMidiNoteInHertz(nn + (BEND_RANGE * sign));

    // Linearly interpolate between curr/bent freq, up or down
    return ((1.0 - (sign * pitch_ratio)) * curr_freq) + (sign * pitch_ratio * bent_freq);
}
