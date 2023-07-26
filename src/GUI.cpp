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

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    glfwSwapInterval(1);
    glfwMakeContextCurrent(wm_ctx);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(wm_ctx, true);
    ImGui_ImplOpenGL3_Init("#version 410");
    ImGui::StyleColorsDark();
    font_scale = io.FontGlobalScale * 2;
    io.FontGlobalScale = font_scale;
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

void GUI::noAdapter(){
    ImGui::OpenPopup("##bt_off");
    if (ImGui::BeginPopupModal(
        "##bt_off",
        nullptr,
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove
    )){
        ImGui::Text("No bluetooth adapter found", font_scale);
        if (ImGui::Button("Exit")){
            should_exit = true;
        }
        ImGui::SetWindowPos(ImVec2(
            imgui_vp->GetWorkCenter().x - (ImGui::GetWindowSize().x*0.5),
            imgui_vp->GetWorkCenter().y - (ImGui::GetWindowSize().y*0.5)
        ));
        ImGui::EndPopup();
    }
}

void GUI::bluetoothOff(){
    ImGui::OpenPopup("##bt_off");
    if (ImGui::BeginPopupModal(
        "##bt_off",
        nullptr,
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_AlwaysUseWindowPadding
    )){
        ImGui::Text("Bluetooth off, please turn it on...", font_scale);
        ImGui::SetWindowPos(ImVec2(
            imgui_vp->GetWorkCenter().x - (ImGui::GetWindowSize().x*0.5),
            imgui_vp->GetWorkCenter().y - (ImGui::GetWindowSize().y*0.5)
        ));
        if (ASHA::Adapter::isBluetoothOn()){
            UI_State = STANDARD;
        }
        ImGui::EndPopup();
    }
}

void GUI::notASHA(){
    ImGui::OpenPopup("##not_ASHA");
    if (ImGui::BeginPopupModal(
        "##not_ASHA",
        nullptr,
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoTitleBar
    )){
        ImGui::Text("Not an ASHA device...", font_scale * 2);
        if (ImGui::Button("Ok")){
            UI_State = STANDARD;
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
    if (!devices.left.isConnected()){
        ImGui::SetCursorPos(buttonPos);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0, 0.0, 1.0, 1.0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0, 0.0, 1.0, 0.8));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0, 0.0, 0.0, 1.0));
        if (ImGui::Button("Left Not Paired", buttonSize) || inLeftScan){
            inLeftScan = true;
            drawLeftSelect();
        }
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_None)){
            ImGui::SetTooltip("Click me to scan");
        }
        ImGui::PopStyleColor(3);
    }
}

void GUI::drawLeftSelect(){
    if (!bt_adapter->isScanning()){
        bt_adapter->startScan();
        timer = precise_clock::now();
    }
    if (std::chrono::duration_cast<std::chrono::seconds>(precise_clock::now() - timer).count() >= 5){
        bt_adapter->updateScanResults();
    }
    std::vector<ASHA::ScanPeer> results = bt_adapter->getLastScan();
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.0, 0.0, 1.0, 1.0));
    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.0, 0.0, 1.0, 1.0));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8, 0.8, 0.8, 1.0));
    ImGui::OpenPopup("Select Left Device");
    if (!ImGui::BeginPopupModal(
        "Select Left Device",
        nullptr,
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove
    )){ ImGui::PopStyleColor(3); return; }
    if (results.size() == 0){
        ImGui::Text("Searching for devices...", font_scale);
    } else {
        ImVec2 buttonSize = ImVec2(
            imgui_vp->Size.x * 0.3,
            imgui_vp->Size.y * 0.1
        );
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0, 0.0, 0.0, 1.0));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.9, 0.9, 0.9, 1.0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9, 0.9, 0.9, 0.8));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0, 0.0, 0.0, 1.0));
        for (ASHA::ScanPeer peer : results){
            if (ImGui::Button(peer.name.c_str(), buttonSize)){
                if (peer.peer.isASHA()){
                    devices.left = peer.peer;
                } else {
                    UI_State = NOT_ASHA;
                }
            }
        }
        ImGui::PopStyleColor(4);
    }
    if (ImGui::Button("Stop")){
        inLeftScan = false;
    }
    ImGui::SetWindowPos(ImVec2(
        imgui_vp->GetWorkCenter().x - (ImGui::GetWindowSize().x*0.5),
        imgui_vp->GetWorkCenter().y - (ImGui::GetWindowSize().y*0.5)
    ));
    ImGui::EndPopup();
    ImGui::PopStyleColor(3);
}

void GUI::drawRightDevice(){
    ImVec2 buttonPos;
    ImVec2 buttonSize;
    buttonPos.x =  (imgui_vp->GetCenter().x * 0.25) + imgui_vp->GetCenter().x;
    buttonPos.y =  imgui_vp->Size.y * 0.15;
    buttonSize.x = imgui_vp->GetCenter().x * 0.5;
    buttonSize.y = buttonSize.x;
    if (!devices.right.isConnected()){
        ImGui::SetCursorPos(buttonPos);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0, 0.0, 0.0, 1.0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0, 0.0, 0.0, 0.8));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0, 0.0, 0.0, 1.0));
        if (ImGui::Button("Right Not Paired", buttonSize) || inRightScan){
            inRightScan = true;
            drawRightSelect();
        }
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_None)){
            ImGui::SetTooltip("Click me to scan");
        }
        ImGui::PopStyleColor(3);
    }
}

void GUI::drawRightSelect(){
    if (!bt_adapter->isScanning()){
        bt_adapter->startScan();
        timer = precise_clock::now();
    }
    if (std::chrono::duration_cast<std::chrono::seconds>(precise_clock::now() - timer).count() >= 5){
        bt_adapter->updateScanResults();
    }
    std::vector<ASHA::ScanPeer> results = bt_adapter->getLastScan();
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(1.0, 0.0, 0.0, 1.0));
    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(1.0, 0.0, 0.0, 1.0));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8, 0.8, 0.8, 1.0));
    ImGui::OpenPopup("Select Right Device");
    if (!ImGui::BeginPopupModal(
        "Select Right Device",
        nullptr,
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove
    )){ ImGui::PopStyleColor(3); return; }
    if (results.size() == 0){
        ImGui::Text("Searching for devices...", font_scale);
    } else {
        ImVec2 buttonSize = ImVec2(
            imgui_vp->Size.x * 0.3,
            imgui_vp->Size.y * 0.1
        );
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0, 0.0, 0.0, 1.0));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.9, 0.9, 0.9, 1.0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9, 0.9, 0.9, 0.8));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0, 0.0, 0.0, 1.0));
        for (ASHA::ScanPeer peer : results){
            if (ImGui::Button(peer.name.c_str(), buttonSize)){
                if (peer.peer.isASHA()){
                    devices.right = peer.peer;
                } else {
                    UI_State = NOT_ASHA;
                }
            }
        }
        ImGui::PopStyleColor(4);
    }
    if (ImGui::Button("Stop")){
        inRightScan = false;
    }
    ImGui::SetWindowPos(ImVec2(
        imgui_vp->GetWorkCenter().x - (ImGui::GetWindowSize().x*0.5),
        imgui_vp->GetWorkCenter().y - (ImGui::GetWindowSize().y*0.5)
    ));
    ImGui::EndPopup();
    ImGui::PopStyleColor(3);
}

void GUI::setNoAdapterFound(){
    UI_State = NO_ADAPTER;
}

void GUI::setBluetoothOff(){
    UI_State = BT_OFF;
}

void GUI::assignAdapter(ASHA::Adapter *adapter){
    bt_adapter = adapter;
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
            noAdapter();
            break;
        case (BT_OFF):
            bluetoothOff();
            break;
        case (NOT_ASHA):
            notASHA();
            break;
        default:
            if (!ASHA::Adapter::isBluetoothOn()){
                UI_State = BT_OFF;
                break;
            }
            drawLeftDevice();
            drawRightDevice();
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