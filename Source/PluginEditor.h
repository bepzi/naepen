#pragma once

#include "PluginProcessor.h"
#include "WaveformVisualizerComponent.h"
#include "WavetableVisualizerComponent.h"

#include <JuceHeader.h>

/**
 * Handles on-screen GUI controls and visualizations.
 */
class NaepenAudioProcessorEditor :
    public AudioProcessorEditor,
    public Slider::Listener,
    public ComboBox::Listener {
public:
    NaepenAudioProcessorEditor(
        NaepenAudioProcessor &, AudioVisualiserComponent &, MidiKeyboardState &);
    ~NaepenAudioProcessorEditor() override;

    //==============================================================================
    void paint(Graphics &) override;
    void resized() override;

    void sliderValueChanged(Slider *slider) override;
    void comboBoxChanged(ComboBox *menu) override;

private:
    // This reference is provided as a quick way for the editor to
    // access the processor object that created it.
    NaepenAudioProcessor &processor;

    Label gain_label;
    Slider gain_slider;

    Label attack_label;
    Slider attack_slider;
    Label decay_label;
    Slider decay_slider;
    Label sustain_label;
    Slider sustain_slider;
    Label release_label;
    Slider release_slider;

    Label table_idx_label;
    Slider table_idx_slider;

    ComboBox filter_type_selector;
    Label filter_cutoff_label;
    Slider filter_cutoff_slider;
    Label filter_q_label;
    Slider filter_q_slider;

    WavetableVisualizerComponent wavetable_visualizer;

    AudioVisualiserComponent &audio_visualizer;

    MidiKeyboardState &keyboard_state;
    MidiKeyboardComponent keyboard_component;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NaepenAudioProcessorEditor)
};
