#pragma once

#include "PluginProcessor.h"

#include <JuceHeader.h>

/**
 * Handles on-screen GUI controls and visualizations.
 */
class NaepenAudioProcessorEditor : public AudioProcessorEditor, public Slider::Listener {
public:
    explicit NaepenAudioProcessorEditor(NaepenAudioProcessor &);
    ~NaepenAudioProcessorEditor() override;

    //==============================================================================
    void paint(Graphics &) override;
    void resized() override;

    void sliderValueChanged(Slider *slider) override;

private:
    // This reference is provided as a quick way for the editor to
    // access the processor object that created it.
    NaepenAudioProcessor &processor;

    Slider freq_slider;
    AudioVisualiserComponent visualizer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NaepenAudioProcessorEditor)
};
