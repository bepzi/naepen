#pragma once

#include <JuceHeader.h>

class WaveformVisualizerComponent : public AudioVisualiserComponent {
public:
    explicit WaveformVisualizerComponent(int initialNumChannels) :
        AudioVisualiserComponent(initialNumChannels)
    {
        // Do nothing
    }

    void paintChannel(
        Graphics &g, Rectangle<float> bounds, const Range<float> *levels, int numLevels,
        int nextSample) override
    {
        g.setColour(Colours::dimgrey);
        g.drawHorizontalLine((int)bounds.getCentre().y, bounds.getX(), bounds.getRight());

        g.setColour(Colours::white);
        AudioVisualiserComponent::paintChannel(g, bounds, levels, numLevels, nextSample);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformVisualizerComponent)
};