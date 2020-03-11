#include "WavetableVisualizerComponent.h"

static auto vertex_shader = String(R"(
attribute vec4 position;

uniform mat4 projection_mat;
uniform mat4 view_mat;

void main()
{
    gl_Position = projection_mat * view_mat * position;
}
)");

static auto fragment_shader = String(R"(
void main()
{
    gl_FragColor = vec4(0.95, 0.57, 0.03, 0.7);
}
)");

WavetableVisualizerComponent::WavetableVisualizerComponent()
{
    gl.setRenderer(this);
    gl.attachTo(*this);
    //    gl.setContinuousRepainting(true);
}

WavetableVisualizerComponent::~WavetableVisualizerComponent()
{
    gl.detach();
}

void WavetableVisualizerComponent::newOpenGLContextCreated()
{
    std::unique_ptr<OpenGLShaderProgram> shader_program(new OpenGLShaderProgram(gl));
    if (!shader_program->addVertexShader(OpenGLHelpers::translateVertexShaderToV3(vertex_shader)) ||
        !shader_program->addFragmentShader(
            OpenGLHelpers::translateFragmentShaderToV3(fragment_shader)) ||
        !shader_program->link()) {
        std::cerr << "Shader compilation error: " << shader_program->getLastError() << std::endl;
        return;
    }

    //    shape.reset();
    //    attributes.reset();
    //    uniforms.reset();

    shader = std::move(shader_program);
    shader->use();

    //    shape.reset(new Shape(gl));
    //    attributes.reset(new Attributes(gl, *shader));
    //    uniforms.reset(new Uniforms(gl, *shader));

    std::cout << "GLSL: v" << String(OpenGLShaderProgram::getLanguageVersion(), 2) << std::endl;
}

void WavetableVisualizerComponent::renderOpenGL()
{
    OpenGLHelpers::clear(Colours::darkblue);
}

void WavetableVisualizerComponent::openGLContextClosing() {}

void WavetableVisualizerComponent::update_wavetable_model(
    const std::vector<std::array<float, 2048>> &mesh)
{
}