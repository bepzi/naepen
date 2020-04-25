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

    ADSR::Parameters adsr_params = {
        state.getParameterAsValue(DatabaseIdentifiers::OSC_ONE_GAIN_ATTACK).getValue(),
        state.getParameterAsValue(DatabaseIdentifiers::OSC_ONE_GAIN_DECAY).getValue(),
        state.getParameterAsValue(DatabaseIdentifiers::OSC_ONE_GAIN_SUSTAIN).getValue(),
        state.getParameterAsValue(DatabaseIdentifiers::OSC_ONE_GAIN_RELEASE).getValue(),
    };
    adsr_envelope.setParameters(adsr_params);
    adsr_envelope.noteOn();
}

void OscillatorVoice::stopNote(float velocity, bool allow_tail_off)
{
    ignoreUnused(velocity);

    if (allow_tail_off) {
        adsr_envelope.noteOff();
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
        if (!adsr_envelope.isActive()) {
            clearCurrentNote();
            break;
        }

        auto sample = osc->get_next_sample() * level * adsr_envelope.getNextSample();
        for (int channel = 0; channel < output_buffer.getNumChannels(); ++channel) {
            output_buffer.addSample(channel, i, sample);
        }
    }
}

void OscillatorVoice::setCurrentPlaybackSampleRate(double new_rate)
{
    osc->set_sample_rate(new_rate);

    adsr_envelope.setSampleRate(new_rate);
    ADSR::Parameters adsr_params = {
        state.getParameterAsValue(DatabaseIdentifiers::OSC_ONE_GAIN_ATTACK).getValue(),
        state.getParameterAsValue(DatabaseIdentifiers::OSC_ONE_GAIN_DECAY).getValue(),
        state.getParameterAsValue(DatabaseIdentifiers::OSC_ONE_GAIN_SUSTAIN).getValue(),
        state.getParameterAsValue(DatabaseIdentifiers::OSC_ONE_GAIN_RELEASE).getValue(),
    };
    adsr_envelope.setParameters(adsr_params);
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
