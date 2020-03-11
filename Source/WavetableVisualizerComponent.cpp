#include "WavetableVisualizerComponent.h"

WavetableVisualizerComponent::WavetableVisualizerComponent()
{
    gl.setRenderer(this);
    gl.attachTo(*this);
}

WavetableVisualizerComponent::~WavetableVisualizerComponent()
{
    gl.detach();
}

void WavetableVisualizerComponent::newOpenGLContextCreated() {}

void WavetableVisualizerComponent::renderOpenGL()
{
    OpenGLHelpers::clear(Colours::darkblue);
}

void WavetableVisualizerComponent::openGLContextClosing() {}
