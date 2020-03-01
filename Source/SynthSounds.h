#pragma once

#include "Wavetable.h"

#include <JuceHeader.h>

// TODO: Pretty sure the wavetable belongs in the Sound, not the Voice.

class SineWaveSound : public SynthesiserSound {
public:
    SineWaveSound() noexcept = default;

    bool appliesToNote(int) override
    {
        return true;
    }
    bool appliesToChannel(int) override
    {
        return true;
    }

private:
};

class SawtoothWaveSound : public SynthesiserSound {
public:
    SawtoothWaveSound() noexcept = default;

    bool appliesToNote(int) override
    {
        return true;
    }
    bool appliesToChannel(int) override
    {
        return true;
    }

private:
};

class SineWaveVoice : public SynthesiserVoice {
public:
    SineWaveVoice() noexcept = default;

    bool canPlaySound(SynthesiserSound *sound) override
    {
        return dynamic_cast<SineWaveSound *>(sound) != nullptr;
    }

    void startNote(
        int midiNoteNumber, float velocity, SynthesiserSound *,
        int /*currentPitchWheelPosition*/) override
    {
        level = velocity * 0.15f;
        tail_off = 0.0f;
        tail_on = 1.0f;

        auto freq_hz = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        table.set_freq(freq_hz);
    }

    void stopNote(float /*velocity*/, bool allow_tail_off) override
    {
        if (allow_tail_off) {
            if (tail_off <= 0.0) {
                tail_off = 1.0;
            }
        } else {
            clearCurrentNote();
            table.set_freq(0.0f);
        }
    }

    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}

    void
    renderNextBlock(AudioSampleBuffer &output_buffer, int start_sample, int num_samples) override
    {
        if (tail_off > 0.0) {
            while (--num_samples >= 0) {
                auto current_sample = table.get_next_sample() * level * tail_off;

                for (auto i = output_buffer.getNumChannels(); --i >= 0;) {
                    output_buffer.addSample(i, start_sample, current_sample);
                }

                ++start_sample;
                tail_off *= 0.99;

                if (tail_off <= 0.005) {
                    clearCurrentNote();
                    table.set_freq(0.0f);
                    break;
                }
            }

        } else if (tail_on > 0) {
            while (--num_samples >= 0) {
                auto current_sample = table.get_next_sample() * level * (1 - tail_on);

                for (auto i = output_buffer.getNumChannels(); --i >= 0;) {
                    output_buffer.addSample(i, start_sample, current_sample);
                }

                ++start_sample;
                tail_on *= 0.999f;

                if (tail_on <= 0.0001f) {
                    tail_on = 0.0f;
//                    break;
                }
            }

        } else {
            while (--num_samples >= 0) {
                auto current_sample = table.get_next_sample() * level;

                for (auto i = output_buffer.getNumChannels(); --i >= 0;) {
                    output_buffer.addSample(i, start_sample, current_sample);
                }

                ++start_sample;
            }
        }
    }

private:
    float level = 0.0f;
    float tail_on = 0.0f;
    float tail_off = 0.0f;

    SineWavetable<1024, 44800> table = {};
};

// HACK! Don't copy-paste code like this!
class SawtoothWaveVoice : public SynthesiserVoice {
public:
    SawtoothWaveVoice() noexcept = default;

    bool canPlaySound(SynthesiserSound *sound) override
    {
        return dynamic_cast<SawtoothWaveSound *>(sound) != nullptr;
    }

    void startNote(
        int midiNoteNumber, float velocity, SynthesiserSound *,
        int /*currentPitchWheelPosition*/) override
    {
        level = velocity * 0.15f;
        tail_off = 0.0f;
        tail_on = 1.0f;

        auto freq_hz = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        table.set_freq(freq_hz);
    }

    void stopNote(float /*velocity*/, bool allow_tail_off) override
    {
        if (allow_tail_off) {
            if (tail_off <= 0.0) {
                tail_off = 1.0;
            }
        } else {
            clearCurrentNote();
            table.set_freq(0.0f);
        }
    }

    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}

    void
    renderNextBlock(AudioSampleBuffer &output_buffer, int start_sample, int num_samples) override
    {
        if (tail_off > 0.0) {
            while (--num_samples >= 0) {
                auto current_sample = table.get_next_sample() * level * tail_off;

                for (auto i = output_buffer.getNumChannels(); --i >= 0;) {
                    output_buffer.addSample(i, start_sample, current_sample);
                }

                ++start_sample;
                tail_off *= 0.99;

                if (tail_off <= 0.005) {
                    clearCurrentNote();
                    table.set_freq(0.0f);
                    break;
                }
            }

        } else if (tail_on > 0) {
            while (--num_samples >= 0) {
                auto current_sample = table.get_next_sample() * level * (1 - tail_on);

                for (auto i = output_buffer.getNumChannels(); --i >= 0;) {
                    output_buffer.addSample(i, start_sample, current_sample);
                }

                ++start_sample;
                tail_on *= 0.999f;

                if (tail_on <= 0.0001f) {
                    tail_on = 0.0f;
//                    break;
                }
            }

        } else {
            while (--num_samples >= 0) {
                auto current_sample = table.get_next_sample() * level;

                for (auto i = output_buffer.getNumChannels(); --i >= 0;) {
                    output_buffer.addSample(i, start_sample, current_sample);
                }

                ++start_sample;
            }
        }
    }

private:
    float level = 0.0f;
    float tail_on = 0.0f;
    float tail_off = 0.0f;

    SawtoothWavetable<1024, 44800> table = {};
};