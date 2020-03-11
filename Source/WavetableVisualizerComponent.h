#pragma once

#include <JuceHeader.h>

class WavetableVisualizerComponent : public Component, private OpenGLRenderer {
public:
    WavetableVisualizerComponent();
    ~WavetableVisualizerComponent() override;

    void newOpenGLContextCreated() override;
    void renderOpenGL() override;
    void openGLContextClosing() override;

    OpenGLContext gl;

private:
};