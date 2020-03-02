#pragma once

#include "Wavetable.h"

#include <JuceHeader.h>

// https://www.earlevel.com/main/2012/05/25/a-wavetable-oscillator%e2%80%94the-code/

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

template<unsigned int T = 1024>
class WavetableVoice : public SynthesiserVoice {
public:
    explicit WavetableVoice(std::unique_ptr<Wavetable<T>> table) noexcept
    {
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
        level = velocity * 0.15f;

        atk_curr_samples = 0;
        release_curr_samples = release_total_samples;

        auto freq_hz = MidiMessage::getMidiNoteInHertz(midi_note_number);
        table->set_freq((float)freq_hz, (int)getSampleRate());
    }

    void stopNote(float /* velocity */, bool allow_tail_off) override
    {
        /*
        atk_curr_samples = atk_total_samples;

        if (allow_tail_off) {
            release_curr_samples = 0;
        } else {
            release_curr_samples = release_total_samples;
            clearCurrentNote();
            table->set_freq(0.0f, 1);
        }
        */

        clearCurrentNote();
        table->set_freq(0.0f, 1);
    }

    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}

    void
    renderNextBlock(AudioBuffer<float> &output_buffer, int start_sample, int num_samples) override
    {
        while (--num_samples >= 0) {
            auto current_sample = table->get_next_sample() * level;

            /*
            if (atk_curr_samples < atk_total_samples) {
                // Apply attack
                current_sample *= attack_function();
                ++atk_curr_samples;

            } else if (release_curr_samples < release_total_samples) {
                // Apply release
                current_sample *= release_function();
                ++release_curr_samples;

                if (release_curr_samples >= release_total_samples) {
                    // Finished playing the note, shut it down
                    clearCurrentNote();
                    table->set_freq(0.0f, 1);
                    break;
                }
            }
            */

            for (auto i = 0; i < output_buffer.getNumChannels(); ++i) {
                output_buffer.addSample(i, start_sample, current_sample);
            }
            ++start_sample;
        }
    }

private:
    size_t atk_total_samples = ms_to_samples(40.0f, getSampleRate());
    size_t atk_curr_samples = 0;

    size_t release_total_samples = ms_to_samples(200.0f, getSampleRate());
    size_t release_curr_samples = release_total_samples;

    float level = 0.0f;

    std::unique_ptr<Wavetable<T>> table = nullptr;

    static forcedinline size_t ms_to_samples(float time_ms, double sample_rate)
    {
        return (size_t)std::ceil(time_ms * 0.001f * sample_rate);
    }

    // Exponential gain: a^x - 1, where a > 1. The closer a is to 1, the slower the attack.
    // Exponential decay: a^x, where a < 1. The closer a is to 1, the slower the decay.
    forcedinline float attack_function()
    {
        // TODO: Make this exponential, not linear
        return (float)atk_curr_samples / (float)atk_total_samples;
    }

    forcedinline float release_function()
    {
        // TODO: Make this exponential, not linear
        return (1.0f - ((float)release_curr_samples / (float)release_total_samples));
    }
};
