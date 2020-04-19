#include "OscillatorVoice.h"

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

OscillatorVoice::OscillatorVoice(std::unique_ptr<Oscillator> oscillator) :
    osc(std::move(oscillator))
{
    osc->set_sample_rate(getSampleRate());

    adsr_envelope.setSampleRate(getSampleRate());
    adsr_envelope.setParameters(adsr_params);
}

bool OscillatorVoice::canPlaySound(SynthesiserSound *sound)
{
    return dynamic_cast<OscillatorSound *>(sound) != nullptr;
}

void OscillatorVoice::startNote(
    int midi_note_number, float velocity, SynthesiserSound *sound, int currentPitchWheelPosition)
{
    ignoreUnused(sound, currentPitchWheelPosition);

    level = velocity * 0.3f;
    auto freq_hz = MidiMessage::getMidiNoteInHertz(midi_note_number);
    osc->set_freq(freq_hz);

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

void OscillatorVoice::pitchWheelMoved(int) {}
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
}