#pragma once

#include "Wavetable.h"
#include <JuceHeader.h>

class WavetableVisualizerComponent : public Component, private OpenGLRenderer {
public:
    WavetableVisualizerComponent();
    ~WavetableVisualizerComponent() override;

    void newOpenGLContextCreated() override;
    void renderOpenGL() override;
    void openGLContextClosing() override;

    OpenGLContext gl;

    void update_wavetable_model(const std::vector<std::array<float, 2048>> &mesh);

private:
    std::unique_ptr<OpenGLShaderProgram> shader;
    //    std::unique_ptr<Attributes> attributes;
    //    std::unique_ptr<Uniforms> uniforms;
    //    std::unique_ptr<Shape> grid;
};