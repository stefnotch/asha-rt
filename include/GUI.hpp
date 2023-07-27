#ifdef _WIN32
    #include <windows.h>
    #include <gl/GL.h>
#endif

#ifdef __APPLE__
    #include <OpenGL/gl.h>
#endif

#include <array>
#include <asha.hpp>
#include <chrono>
#ifdef __linux__
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#endif
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

typedef std::chrono::high_resolution_clock precise_clock;

class GUI {
private:
    bool                       should_exit = false;
    int                        frame_rate;
    float                      font_scale;
    GLFWwindow                *wm_ctx;
    ImGuiContext              *im_ctx;
    ImGuiViewport             *imgui_vp;
    ASHA::Adapter             *bt_adapter;
    ASHA::DevicePair           devices;
    bool                       inScan      = false;
    precise_clock::time_point  timer;
    enum {
        NO_ADAPTER, BT_OFF, NOT_ASHA, STANDARD
    } UI_State;
    void createBaseWindow();
    void endBaseWindow();

    void styleButtonWhite();

    void drawError(std::string, std::string, std::function<void()>);
    void drawLeftDevice();
    void drawRightDevice();
    void drawScanMenu();
    void exit();
protected:
public:
    GUI(std::string, int, int);
    void setNoAdapterFound();
    void setBluetoothOff();
    void assignAdapter(ASHA::Adapter*);
    void run();
};