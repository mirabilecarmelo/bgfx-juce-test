#include "BgfxComponent.h"
#include <cmath>

// ─── Vertex layout ───
struct PosColorVertex
{
    float x, y, z;
    uint32_t abgr;
    static bgfx::VertexLayout layout;
    static void init()
    {
        layout.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .end();
    }
};
bgfx::VertexLayout PosColorVertex::layout;

static PosColorVertex cubeVertices[] =
{
    {-0.5f,  0.5f,  0.5f, 0xff8b6ce0 },
    { 0.5f,  0.5f,  0.5f, 0xff6c8be0 },
    {-0.5f, -0.5f,  0.5f, 0xff8b6ce0 },
    { 0.5f, -0.5f,  0.5f, 0xff6c8be0 },
    {-0.5f,  0.5f, -0.5f, 0xffe06c8b },
    { 0.5f,  0.5f, -0.5f, 0xffe08b6c },
    {-0.5f, -0.5f, -0.5f, 0xffe06c8b },
    { 0.5f, -0.5f, -0.5f, 0xffe08b6c },
};

static const uint16_t cubeIndices[] =
{
    0, 1, 2, 1, 3, 2,
    4, 6, 5, 5, 6, 7,
    0, 2, 4, 4, 2, 6,
    1, 5, 3, 5, 7, 3,
    0, 4, 1, 4, 5, 1,
    2, 3, 6, 6, 3, 7,
};

// ─── Grid (line list) ───
static constexpr int GRID_LINES = 21;
static constexpr int GRID_VERT_COUNT = GRID_LINES * 2 * 2;
static PosColorVertex gridVertices[GRID_VERT_COUNT];

static void buildGrid()
{
    int idx = 0;
    uint32_t col  = 0xff404048;
    uint32_t colZ = 0xff505058;
    for (int i = 0; i < GRID_LINES; ++i)
    {
        float v = (float)(i - GRID_LINES / 2);
        uint32_t c = (i == GRID_LINES / 2) ? colZ : col;
        gridVertices[idx++] = { -10.0f, 0.0f, v, c };
        gridVertices[idx++] = {  10.0f, 0.0f, v, c };
        gridVertices[idx++] = { v, 0.0f, -10.0f, c };
        gridVertices[idx++] = { v, 0.0f,  10.0f, c };
    }
}

// ─── Child window (Windows) ───
#if JUCE_WINDOWS
void BgfxComponent::createChildWindow()
{
    if (childHwnd != nullptr)
        return;

    HWND parentHwnd = (HWND)getWindowHandle();
    if (parentHwnd == nullptr)
        return;

    auto bounds = getLocalBounds();

    childHwnd = CreateWindowExA(
        0, "STATIC", "",
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
        0, 0, bounds.getWidth(), bounds.getHeight(),
        parentHwnd, nullptr,
        GetModuleHandle(nullptr), nullptr);
}

void BgfxComponent::destroyChildWindow()
{
    if (childHwnd != nullptr)
    {
        DestroyWindow(childHwnd);
        childHwnd = nullptr;
    }
}
#endif

// ─── BgfxComponent ───

BgfxComponent::BgfxComponent()
{
    setOpaque(true);
    startTimerHz(60);
}

BgfxComponent::~BgfxComponent()
{
    stopTimer();
    shutdownBgfx();
#if JUCE_WINDOWS
    destroyChildWindow();
#endif
}

void BgfxComponent::paint(juce::Graphics& g)
{
    if (!bgfxInitialized)
    {
        g.fillAll(juce::Colour(0xff1a1a20));
        g.setColour(juce::Colour(0xff8b6ce0));
        g.setFont(16.0f);
        g.drawText("Initializing bgfx...", getLocalBounds(), juce::Justification::centred);
    }
}

void BgfxComponent::resized()
{
#if JUCE_WINDOWS
    if (childHwnd != nullptr)
    {
        auto b = getLocalBounds();
        MoveWindow(childHwnd, 0, 0, b.getWidth(), b.getHeight(), TRUE);
    }
#endif
    if (bgfxInitialized)
        bgfx::reset((uint32_t)getWidth(), (uint32_t)getHeight(), BGFX_RESET_VSYNC);
}

void BgfxComponent::mouseDown(const juce::MouseEvent& e)
{
    lastMousePos = e.position;
}

void BgfxComponent::mouseDrag(const juce::MouseEvent& e)
{
    auto delta = e.position - lastMousePos;
    lastMousePos = e.position;
    if (e.mods.isRightButtonDown())
    {
        cameraAzimuth   += delta.x * 0.5f;
        cameraElevation -= delta.y * 0.5f;
        cameraElevation  = juce::jlimit(5.0f, 89.0f, cameraElevation);
    }
}

void BgfxComponent::mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails& w)
{
    cameraDistance -= w.deltaY * 2.0f;
    cameraDistance  = juce::jlimit(2.0f, 30.0f, cameraDistance);
}

void BgfxComponent::timerCallback()
{
    if (!bgfxInitialized)
    {
        initBgfx();
        if (!bgfxInitialized)
            return;
    }
    renderFrame();
}

void BgfxComponent::initBgfx()
{
    if (getWindowHandle() == nullptr)
        return;

#if JUCE_WINDOWS
    createChildWindow();
    if (childHwnd == nullptr)
        return;
    void* nwh = (void*)childHwnd;
#else
    void* nwh = (void*)getWindowHandle();
#endif

    bgfx::renderFrame(); // single-threaded mode — call before init

    bgfx::Init init;
    init.type = bgfx::RendererType::Count;
    init.resolution.width  = (uint32_t)getWidth();
    init.resolution.height = (uint32_t)getHeight();
    init.resolution.reset  = BGFX_RESET_VSYNC;
    init.platformData.nwh  = nwh;

    if (!bgfx::init(init))
    {
        DBG("bgfx::init() FAILED");
        return;
    }

    DBG("bgfx initialized — renderer: " << bgfx::getRendererName(bgfx::getRendererType()));

    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x1a1a20ff, 1.0f, 0);
    bgfx::setViewRect(0, 0, 0, (uint16_t)getWidth(), (uint16_t)getHeight());

    createGeometry();
    bgfxInitialized = true;
}

void BgfxComponent::shutdownBgfx()
{
    if (!bgfxInitialized)
        return;
    if (bgfx::isValid(cubeVBH)) bgfx::destroy(cubeVBH);
    if (bgfx::isValid(cubeIBH)) bgfx::destroy(cubeIBH);
    if (bgfx::isValid(gridVBH)) bgfx::destroy(gridVBH);
    bgfx::shutdown();
    bgfxInitialized = false;
}

void BgfxComponent::createGeometry()
{
    PosColorVertex::init();

    cubeVBH = bgfx::createVertexBuffer(
        bgfx::makeRef(cubeVertices, sizeof(cubeVertices)),
        PosColorVertex::layout);
    cubeIBH = bgfx::createIndexBuffer(
        bgfx::makeRef(cubeIndices, sizeof(cubeIndices)));

    buildGrid();
    gridVBH = bgfx::createVertexBuffer(
        bgfx::makeRef(gridVertices, sizeof(gridVertices)),
        PosColorVertex::layout);

    bgfx::setDebug(BGFX_DEBUG_TEXT);
}

void BgfxComponent::renderFrame()
{
    rotationAngle += 0.01f;

    uint16_t w = (uint16_t)getWidth();
    uint16_t h = (uint16_t)getHeight();
    if (w == 0 || h == 0) return;

    bgfx::setViewRect(0, 0, 0, w, h);

    // Orbit camera
    float azRad = cameraAzimuth * bx::kPi / 180.0f;
    float elRad = cameraElevation * bx::kPi / 180.0f;

    float eye[3] = {
        cameraDistance * bx::cos(elRad) * bx::sin(azRad),
        cameraDistance * bx::sin(elRad),
        cameraDistance * bx::cos(elRad) * bx::cos(azRad)
    };

    float view[16];
    bx::mtxLookAt(view, { eye[0], eye[1], eye[2] }, { 0.0f, 0.0f, 0.0f });

    float proj[16];
    bx::mtxProj(proj, 45.0f, float(w) / float(h), 0.1f, 100.0f,
                bgfx::getCaps()->homogeneousDepth);

    bgfx::setViewTransform(0, view, proj);
    bgfx::touch(0);

    // Debug text
    bgfx::dbgTextClear();
    bgfx::dbgTextPrintf(1, 1, 0x0f, "bgfx JUCE Test");
    bgfx::dbgTextPrintf(1, 2, 0x07, "Renderer: %s", bgfx::getRendererName(bgfx::getRendererType()));
    bgfx::dbgTextPrintf(1, 3, 0x07, "Resolution: %dx%d", w, h);
    bgfx::dbgTextPrintf(1, 4, 0x07, "Camera: az=%.0f el=%.0f dist=%.1f",
                        cameraAzimuth, cameraElevation, cameraDistance);
    bgfx::dbgTextPrintf(1, 5, 0x07, "Right-drag to orbit, scroll to zoom");

    bgfx::frame();
}
