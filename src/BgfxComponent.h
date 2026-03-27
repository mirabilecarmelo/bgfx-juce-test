#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/math.h>

#if JUCE_WINDOWS
 #include <windows.h>
#endif

class BgfxComponent : public juce::Component, private juce::Timer
{
public:
    BgfxComponent();
    ~BgfxComponent() override;

    void resized() override;
    void paint(juce::Graphics& g) override;

    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& w) override;

private:
    void timerCallback() override;
    void initBgfx();
    void shutdownBgfx();
    void createGeometry();
    void renderFrame();

    bool bgfxInitialized = false;

#if JUCE_WINDOWS
    HWND childHwnd = nullptr;
    void createChildWindow();
    void destroyChildWindow();
#endif

    bgfx::VertexBufferHandle cubeVBH = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle  cubeIBH = BGFX_INVALID_HANDLE;
    bgfx::VertexBufferHandle gridVBH = BGFX_INVALID_HANDLE;

    float cameraDistance  = 8.0f;
    float cameraAzimuth   = 45.0f;
    float cameraElevation = 30.0f;
    juce::Point<float> lastMousePos;
    float rotationAngle = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BgfxComponent)
};
