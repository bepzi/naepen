#include "OscillatorVoice.h"

#include "../DatabaseIdentifiers.h"

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
    std::unique_ptr<Oscillator> oscillator, AudioProcessorValueTreeState &apvts) :
    osc(std::move(oscillator)), state(apvts)
{
    osc_one_filter_enabled =
        state.getRawParameterValue(DatabaseIdentifiers::OSC_ONE_FILTER_ENABLED);
    osc_one_filter_cutoff = state.getRawParameterValue(DatabaseIdentifiers::OSC_ONE_FILTER_CUTOFF);
    osc_one_filter_q = state.getRawParameterValue(DatabaseIdentifiers::OSC_ONE_FILTER_Q);
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
        state.getParameterAsValue(DatabaseIdentifiers::OSC_ONE_GAIN_ATTACK).getValue(),
        state.getParameterAsValue(DatabaseIdentifiers::OSC_ONE_GAIN_DECAY).getValue(),
        state.getParameterAsValue(DatabaseIdentifiers::OSC_ONE_GAIN_SUSTAIN).getValue(),
        state.getParameterAsValue(DatabaseIdentifiers::OSC_ONE_GAIN_RELEASE).getValue(),
    };
    osc_one_gain_envelope.setParameters(osc_one_gain_params);
    osc_one_gain_envelope.noteOn();

    ADSR::Parameters osc_one_filter_params = {
        state.getParameterAsValue(DatabaseIdentifiers::OSC_ONE_FILTER_ATTACK).getValue(),
        state.getParameterAsValue(DatabaseIdentifiers::OSC_ONE_FILTER_DECAY).getValue(),
        state.getParameterAsValue(DatabaseIdentifiers::OSC_ONE_FILTER_SUSTAIN).getValue(),
        state.getParameterAsValue(DatabaseIdentifiers::OSC_ONE_FILTER_RELEASE).getValue(),
    };
    osc_one_filter_envelope.setParameters(osc_one_filter_params);
    osc_one_filter_envelope.noteOn();
}

void OscillatorVoice::stopNote(float velocity, bool allow_tail_off)
{
    ignoreUnused(velocity);

    if (allow_tail_off) {
        osc_one_gain_envelope.noteOff();
        osc_one_filter_envelope.noteOff();
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
    for (int i = start_sample; i < start_sample + num_samples; ++i) {
        if (!osc_one_gain_envelope.isActive()) {
            clearCurrentNote();
            break;
        }

        auto gain_env_sample = osc_one_gain_envelope.getNextSample();
        auto sample = osc->get_next_sample() * level * gain_env_sample;

        // Optionally apply the filter
        if (osc_one_filter_envelope.isActive()) {
            auto filter_env_sample = osc_one_filter_envelope.getNextSample();

            if (*osc_one_filter_enabled > 0.5f) {
                // TODO: This crackles
                float cutoff = *osc_one_filter_cutoff * filter_env_sample;

                osc_one_filter.set_params({cutoff, *osc_one_filter_q}, getSampleRate());
                sample = osc_one_filter.get_next_sample(sample);
            }
        }

        for (int channel = 0; channel < output_buffer.getNumChannels(); ++channel) {
            output_buffer.addSample(channel, i, sample);
        }
    }
}

void OscillatorVoice::setCurrentPlaybackSampleRate(double new_rate)
{
    osc->set_sample_rate(new_rate);

    osc_one_gain_envelope.setSampleRate(new_rate);
    osc_one_filter_envelope.setSampleRate(new_rate);
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
