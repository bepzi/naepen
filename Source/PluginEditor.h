#pragma once

#include "PluginProcessor.h"

#include <JuceHeader.h>

/**
 * Handles on-screen GUI controls and visualizations.
 */
class NaepenAudioProcessorEditor : public AudioProcessorEditor, public Slider::Listener {
public:
    NaepenAudioProcessorEditor(NaepenAudioProcessor &, AudioVisualiserComponent &);
    ~NaepenAudioProcessorEditor() override;

    //==============================================================================
    void paint(Graphics &) override;
    void resized() override;

    void sliderValueChanged(Slider *slider) override;

private:
    // This reference is provided as a quick way for the editor to
    // access the processor object that created it.
    NaepenAudioProcessor &processor;

    Label freq_label;
    Label gain_label;

    Slider freq_slider;
    Slider gain_slider;

    AudioVisualiserComponent &visualizer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NaepenAudioProcessorEditor)
};
