#pragma once

#include "Oscillator.h"

#include <JuceHeader.h>

class OscillatorSound : public SynthesiserSound {
public:
    OscillatorSound() noexcept = default;

    bool appliesToNote(int /* midi_note_number */) override;
    bool appliesToChannel(int /* midi_channel */) override;
};

class OscillatorVoice : public SynthesiserVoice {
public:
    OscillatorVoice(std::unique_ptr<Oscillator> oscillator, AudioProcessorValueTreeState &apvts);

    bool canPlaySound(SynthesiserSound *sound) override;

    void startNote(
        int midi_note_number, float velocity, SynthesiserSound *,
        int /*currentPitchWheelPosition*/) override;

    void stopNote(float /* velocity */, bool allow_tail_off) override;

    // TODO: Implement pitch wheel bending
    void pitchWheelMoved(int) override;
    void controllerMoved(int, int) override;

    void
    renderNextBlock(AudioBuffer<float> &output_buffer, int start_sample, int num_samples) override;

    void setCurrentPlaybackSampleRate(double new_rate) override;

private:
    std::unique_ptr<Oscillator> osc;
    AudioProcessorValueTreeState &state;

    float level = 0.0f;

    ADSR adsr_envelope;
};

//    void set_adsr(const ADSR::Parameters &params) noexcept
//    {
//        adsr_params = params;
//    }
//
//    void set_table_idx(double idx) noexcept
//    {
//        table->set_index(idx * table->get_max_index());
//    }
