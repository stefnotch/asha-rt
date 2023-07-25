#include <array>
#include <asha.hpp>
#include <chrono>
#include <GLES2/gl2.h>
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
    bool                       inLeftScan;
    bool                       inRightScan;
    precise_clock::time_point  timer;
    enum {
        NO_ADAPTER, BT_OFF, STANDARD
    } UI_State;
    void createBaseWindow();
    void endBaseWindow();
    void noAdapter();
    void bluetoothOff();
    void drawLeftDevice();
    void drawLeftSelect();
    void drawRightDevice();
    void drawRightSelect();
    void exit();
protected:
public:
    GUI(std::string, int, int);
    void setNoAdapterFound();
    void setBluetoothOff();
    void assignAdapter(ASHA::Adapter*);
    void run();
};