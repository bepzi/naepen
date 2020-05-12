#pragma once

#include "Oscillator.h"
#include "SvfFilter.h"

#include <JuceHeader.h>

class OscillatorSound : public SynthesiserSound {
public:
    bool appliesToNote(int /* midi_note_number */) override;
    bool appliesToChannel(int /* midi_channel */) override;
};

class OscillatorVoice : public SynthesiserVoice, public ValueTree::Listener {
public:
    OscillatorVoice(
        std::unique_ptr<Oscillator> oscillator, AudioProcessorValueTreeState &apvts,
        std::atomic<float> *detune_semitones, std::atomic<float> *detune_cents,
        std::atomic<float> *pan, std::atomic<float> *gain_attack, std::atomic<float> *gain_decay,
        std::atomic<float> *gain_sustain, std::atomic<float> *gain_release,
        Identifier filter_type_id, std::atomic<float> *filter_enabled,
        std::atomic<float> *filter_cutoff, std::atomic<float> *filter_q);
    ~OscillatorVoice() override;

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
    static double calc_freq(int midi_nn, int pitch_wheel_pos, int cents_detune);

    // =======================================================================
    void valueTreePropertyChanged(
        ValueTree &treeWhosePropertyHasChanged, const Identifier &property) override;

private:
    std::unique_ptr<Oscillator> osc;
    AudioProcessorValueTreeState &state;

    float level = 0.0f;

    std::atomic<float> *detune_semitones;
    std::atomic<float> *detune_cents;

    std::atomic<float> *pan;

    ADSR gain_envelope;
    std::atomic<float> *gain_attack;
    std::atomic<float> *gain_decay;
    std::atomic<float> *gain_sustain;
    std::atomic<float> *gain_release;

    SvfFilter filter;
    Identifier filter_type_id;
    std::atomic<float> *filter_enabled;
    std::atomic<float> *filter_cutoff;
    std::atomic<float> *filter_q;
};
