#include <GUI.hpp>

GUI::GUI(std::string title, int width, int height){
    if (!glfwInit()){
        throw std::runtime_error(
            "GLFW could not init!"
        );
    }
    if ((width <= 0) || (height <= 0)){
        wm_ctx = glfwCreateWindow(800, 640, title.c_str(), nullptr, nullptr);
        glfwMaximizeWindow(wm_ctx);
    } else {
        wm_ctx = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    }
    frame_rate = glfwGetVideoMode(glfwGetPrimaryMonitor())->refreshRate;

#if defined(IMGUI_IMPL_OPENGL_ES2)
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
#endif
    glfwSwapInterval(1);
    glfwMakeContextCurrent(wm_ctx);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(wm_ctx, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::StyleColorsDark();
    font_scale = io.FontGlobalScale * 2;
    io.FontGlobalScale = font_scale;

    if (!bt_adapter->isApapterFound()){
        UI_State = NO_ADAPTER;
    }
}

void GUI::createBaseWindow(){
    imgui_vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(imgui_vp->Pos);
    ImGui::SetNextWindowSize(imgui_vp->Size);

    ImGui::Begin(
        "#", 
        nullptr, 
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove
    );
    ImGui::GetStyle().WindowBorderSize = 0.0f;
}

void GUI::endBaseWindow(){
    ImGui::End();
}

void GUI::styleButtonWhite(){
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0, 0.0, 0.0, 1.0));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.9, 0.9, 0.9, 1.0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9, 0.9, 0.9, 0.8));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0, 0.0, 0.0, 1.0));
}

void GUI::drawError(std::string msg, std::string button, std::function<void()> lambda){
    ImGui::OpenPopup("##error");
    if (ImGui::BeginPopupModal(
        "##error",
        nullptr,
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoTitleBar
    )){
        ImGui::Text(msg.c_str(), font_scale);
        if (button.length() > 0){
            if (ImGui::Button(button.c_str())){
                lambda();
            }
        } else {
            lambda();
        }
        ImGui::SetWindowPos(ImVec2(
            imgui_vp->GetWorkCenter().x - (ImGui::GetWindowSize().x*0.5),
            imgui_vp->GetWorkCenter().y - (ImGui::GetWindowSize().y*0.5)
        ));
        ImGui::EndPopup();
    }    
}

void GUI::drawLeftDevice(){
    ImVec2 buttonPos;
    ImVec2 buttonSize;
    buttonPos.x =  imgui_vp->GetCenter().x * 0.25;
    buttonPos.y =  imgui_vp->Size.y * 0.15;
    buttonSize.x = imgui_vp->GetCenter().x * 0.5;
    buttonSize.y = buttonSize.x;
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0, 0.0, 1.0, 1.0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0, 0.0, 1.0, 0.8));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0, 0.0, 0.0, 1.0));
    if (!devices.left->isConnected()){
        ImGui::SetCursorPos(buttonPos);
        if (ImGui::Button("Left Not Paired", buttonSize)){
            inScan = true;
        }
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_None)){
            ImGui::SetTooltip("Click me to scan");
        }
    } else {
        if (ImGui::Button("Connected", buttonSize)){
        }
    }
    ImGui::PopStyleColor(3);
}

void GUI::drawRightDevice(){
    ImVec2 buttonPos;
    ImVec2 buttonSize;
    buttonPos.x =  (imgui_vp->GetCenter().x * 0.25) + imgui_vp->GetCenter().x;
    buttonPos.y =  imgui_vp->Size.y * 0.15;
    buttonSize.x = imgui_vp->GetCenter().x * 0.5;
    buttonSize.y = buttonSize.x;
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0, 0.0, 0.0, 1.0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0, 0.0, 0.0, 0.8));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0, 0.0, 0.0, 1.0));
    if (!devices.right->isConnected()){
        ImGui::SetCursorPos(buttonPos);
        if (ImGui::Button("Right Not Paired", buttonSize)){
            inScan = true;
        }
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_None)){
            ImGui::SetTooltip("Click me to scan");
        }
    } else {
        if (ImGui::Button("Connected", buttonSize)){
        }
    }
    ImGui::PopStyleColor(3);
}

void GUI::drawScanMenu(){
    if (!bt_adapter->isScanning()){
        bt_adapter->startScan();
        timer = precise_clock::now();
    }
    if (std::chrono::duration_cast<std::chrono::seconds>(precise_clock::now() - timer).count() >= 1){
        bt_adapter->updateScanResults();
    }
    std::vector<ASHA::ScanPeer> results = bt_adapter->getLastScan();
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(1.0, 1.0, 1.0, 1.0));
    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(1.0, 1.0, 1.0, 1.0));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0, 0.0, 0.0, 1.0));
    ImGui::OpenPopup("Select An ASHA Device");
    if (!ImGui::BeginPopupModal(
        "Select An ASHA Device",
        nullptr,
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove
    )){ ImGui::PopStyleColor(3); return; }
    ImGui::PopStyleColor(3);
    styleButtonWhite();
    if (results.size() == 0){
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 1.0, 1.0, 1.0));
        ImGui::Text("Searching for devices...", font_scale);
        ImGui::PopStyleColor();
    } else {
        ImVec2 buttonSize = ImVec2(
            imgui_vp->Size.x * 0.3,
            imgui_vp->Size.y * 0.1
        );
        for (ASHA::ScanPeer peer : results){
            if (ImGui::Button(peer.name.c_str(), buttonSize)){
                if (!peer.peer->isConnectable()){
                    
                }
                if (peer.peer->isASHA()){
                    // if (peer.peer->getReadOnlyProperties()){
                    //     if (peer.peer->getSide() == ASHA::Side::LEFT){
                    //         devices.left = peer.peer;
                    //     } else {
                    //         devices.right = peer.peer;
                    //     }
                    // }
                } else {
                    UI_State = NOT_ASHA;
                }
            }
        }
    }
    if (ImGui::Button("Stop")){
        inScan = false;
    }
    ImGui::PopStyleColor(4);
    ImGui::SetWindowPos(ImVec2(
        imgui_vp->GetWorkCenter().x - (ImGui::GetWindowSize().x*0.5),
        imgui_vp->GetWorkCenter().y - (ImGui::GetWindowSize().y*0.5)
    ));
    ImGui::EndPopup();
}

void GUI::setNoAdapterFound(){
    UI_State = NO_ADAPTER;
}

void GUI::setBluetoothOff(){
    UI_State = BT_OFF;
}

void GUI::run(){
    bool open = true;
    ImVec4 clear_color = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    int display_w, display_h;

    while (!glfwWindowShouldClose(wm_ctx) && !should_exit){
        glfwPollEvents();
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwGetFramebufferSize(wm_ctx, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        createBaseWindow();
        switch (UI_State)
        {
        case (NO_ADAPTER):
            drawError(
                "No bluetooth adapter found", 
                "Exit", 
                [this](){this->should_exit = true;}
            );
            break;
        case (BT_OFF):
            drawError(
                "Bluetooth off, please turn back on", 
                "", 
                [this](){
                    if (ASHA::Adapter::isBluetoothOn()){
                        this->UI_State = STANDARD;
                    }
                }
            );
            break;
        case (NOT_ASHA):
            drawError(
                "Not an ASHA device", 
                "OK", 
                [this](){this->UI_State = STANDARD;}
            );
            break;
        default:
            if (!ASHA::Adapter::isBluetoothOn()){
                UI_State = BT_OFF;
                break;
            }
            drawLeftDevice();
            drawRightDevice();
            if (inScan){
                drawScanMenu();
            }
            break;
        }
        endBaseWindow();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(wm_ctx);
        std::this_thread::sleep_for(
            std::chrono::milliseconds(int(1000/frame_rate))
        );
    }

    exit();
}

void GUI::exit(){
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}