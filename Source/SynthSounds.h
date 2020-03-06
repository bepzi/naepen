#pragma once

#include "WavetableOsc.h"

#include <JuceHeader.h>

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
    explicit WavetableVoice(std::unique_ptr<WavetableOsc<T>> table) noexcept
    {
        this->table = std::move(table);

        adsr_envelope.setSampleRate(getSampleRate());
        adsr_envelope.setParameters(adsr_params);
    }

    bool canPlaySound(SynthesiserSound *sound) override
    {
        return dynamic_cast<WavetableSound *>(sound) != nullptr;
    }

    void startNote(
        int midi_note_number, float velocity, SynthesiserSound *,
        int /*currentPitchWheelPosition*/) override
    {
        level = velocity * 0.5;
        auto freq_hz = MidiMessage::getMidiNoteInHertz(midi_note_number) / 2;
        table->set_freq(freq_hz, getSampleRate());

        adsr_envelope.setSampleRate(getSampleRate());
        adsr_envelope.setParameters(adsr_params);
        adsr_envelope.noteOn();
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
        for (int i = start_sample; i < start_sample + num_samples; ++i) {
            if (!adsr_envelope.isActive()) {
                clearCurrentNote();
                break;
            }

            auto sample = table->get_next_sample() * level * adsr_envelope.getNextSample();
            for (int channel = 0; channel < output_buffer.getNumChannels(); ++channel) {
                output_buffer.addSample(channel, i, sample);
            }
        }
    }

    void set_adsr(const ADSR::Parameters &params) noexcept
    {
        adsr_params = params;
    }

private:
    float level = 0.0f;

    ADSR adsr_envelope;
    ADSR::Parameters adsr_params = {0.005f, 0.25f, 1.0f, 0.1f};

    std::unique_ptr<WavetableOsc<T>> table = nullptr;
};
