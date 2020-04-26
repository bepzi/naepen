#pragma once

#include "Oscillator.h"
#include "SvfFilter.h"

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
        int midi_note_number, float velocity, SynthesiserSound *sound,
        int pitch_wheel_pos) override;

    void stopNote(float /* velocity */, bool allow_tail_off) override;

    void pitchWheelMoved(int) override;
    void controllerMoved(int, int) override;

    void
    renderNextBlock(AudioBuffer<float> &output_buffer, int start_sample, int num_samples) override;

    void setCurrentPlaybackSampleRate(double new_rate) override;

    /**
     * Calculates the note frequency (in Hz) given a MIDI note number
     * and the position of the pitch wheel.
     */
    static double calc_freq(int midi_nn, int pitch_wheel_pos);

private:
    std::unique_ptr<Oscillator> osc;
    AudioProcessorValueTreeState &state;

    float level = 0.0f;

    ADSR osc_one_gain_envelope;

    SvfFilter osc_one_filter;
    std::atomic<float> *osc_one_filter_enabled;
    std::atomic<float> *osc_one_filter_cutoff;
    std::atomic<float> *osc_one_filter_q;
    ADSR osc_one_filter_envelope;
};
