#pragma once

#include "Wavetable.h"

#include <JuceHeader.h>

// TODO: Fix glitches/popping and duplicate noises when rapidly playing notes

class WavetableSound : public SynthesiserSound {
public:
    WavetableSound() noexcept {}

    bool appliesToNote(int /* midi_note_number */) override
    {
        return true;
    }

    bool appliesToChannel(int /* midi_channel */) override
    {
        return true;
    }

private:
};

template<size_t T = 2048>
class WavetableVoice : public SynthesiserVoice {
public:
    explicit WavetableVoice(std::unique_ptr<Wavetable<T>> table) noexcept
    {
        sample_rate = getSampleRate();
        adsr_envelope.setSampleRate(sample_rate);
        adsr_envelope.setParameters({0.01f, 0.0f, 1.0f, 0.2f});
        this->table = std::move(table);
    }

    bool canPlaySound(SynthesiserSound *sound) override
    {
        return dynamic_cast<WavetableSound *>(sound) != nullptr;
    }

    void startNote(
        int midi_note_number, float velocity, SynthesiserSound *,
        int /*currentPitchWheelPosition*/) override
    {
        adsr_envelope.noteOn();

        level = velocity * 0.15f;

        auto freq_hz = MidiMessage::getMidiNoteInHertz(midi_note_number);
        table->set_freq(freq_hz, sample_rate);
    }

    void stopNote(float /* velocity */, bool allow_tail_off) override
    {
        if (allow_tail_off) {
            adsr_envelope.noteOff();
        } else {
            clearCurrentNote();
        }
    }

    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}

    void
    renderNextBlock(AudioBuffer<float> &output_buffer, int start_sample, int num_samples) override
    {
        if (!adsr_envelope.isActive()) {
            clearCurrentNote();
            return;
        }

        for (int i = start_sample; i < start_sample + num_samples; ++i) {
            auto sample = table->get_next_sample() * level;
            for (int chan = 0; chan < output_buffer.getNumChannels(); ++chan) {
                output_buffer.addSample(chan, i, sample);
            }
        }

        adsr_envelope.applyEnvelopeToBuffer(output_buffer, start_sample, num_samples);
    }

private:
    double sample_rate;
    float level = 0.0f;

    ADSR adsr_envelope;
    std::unique_ptr<Wavetable<T>> table = nullptr;
};
