#include "ainby.hpp"
#include "ui/file_index_cache_browser.hpp"
#include "ui/pack_browser.hpp"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <strstream>

#include <imgui_internal.h> // Internal header needed for DockSpaceXXX functions
#include <tinyfiledialogs.h>

void AINBY::Draw() {
    // Main Window -- Menu bar + Error popup
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGuiWindowFlags windowFlags =
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus // Disable Ctrl-Tabbing onto this window
        | ImGuiWindowFlags_MenuBar;
    ImGui::Begin("Main Window", nullptr, windowFlags);

    if (shouldOpenErrorPopup) {
        ImGui::OpenPopup("Error##OpenFileError");
        shouldOpenErrorPopup = false;
    }

    DrawMainWindow();

    ImGui::End();
    ImGui::PopStyleVar();

    ImGui::Begin("AINB Index");
    auto index = FileIndexCache::Get();
    auto maybeOpenEntry = FileIndexCacheBrowser::Draw(index);
    if (maybeOpenEntry) {
        HandleOpenRequest(*maybeOpenEntry);
    }
    ImGui::End();

    ImGui::Begin("Pack Browser");
    if (sarcLoaded) {
        auto maybeOpenInternalFilename = PackBrowser::Draw(currentSarc);
        if (maybeOpenInternalFilename) {
            auto entry = FileIndexCacheEntry(*maybeOpenInternalFilename, currentSarcName);
            HandleOpenRequest(entry);
        }
    }
    ImGui::End();

    ImGui::Begin("AINB Inspector");
        if (ainbLoaded) {
            editor.DrawInspector();
        } else {
            ImGui::Text("No .ainb file loaded.");
        }
    ImGui::End();

    ImGui::Begin("Node Viewer");
        if (ainbLoaded) {
            editor.DrawNodeEditor();
        }
    ImGui::End();

    if (ProjectConfig::Get().titleName == TITLE_TOTK_100) {
        //ImGui::Begin("World Map");
        //TotkMap2DView::Draw();
        //ImGui::End();
    }
}

void AINBY::DrawMainWindow() {
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Exit")) {
                shouldClose = true;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }


    // Create the docking layout
    ImGuiID dockSpace = ImGui::DockSpace(ImGui::GetID("DockSpace"));
    if (firstFrame) {
        ImGui::DockBuilderRemoveNode(dockSpace);
        ImGui::DockBuilderAddNode(dockSpace, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockSpace, ImGui::GetMainViewport()->Size);

        ImGuiID dockLeft, dockMiddle, dockRight;
        ImGui::DockBuilderSplitNode(dockSpace, ImGuiDir_Left, 0.2f, &dockLeft, &dockMiddle);
        ImGui::DockBuilderSplitNode(dockMiddle, ImGuiDir_Right, 0.3f, &dockRight, &dockMiddle);

        ImGui::DockBuilderDockWindow("AINB Index", dockLeft);
        ImGui::DockBuilderDockWindow("Pack Browser", dockLeft);
        ImGui::DockBuilderDockWindow("Node Viewer", dockMiddle);
        if (ProjectConfig::Get().titleName == TITLE_TOTK_100) {
            //ImGui::DockBuilderDockWindow("World Map", dockMiddle);
        }
        ImGui::DockBuilderDockWindow("AINB Inspector", dockRight);

        ImGui::DockBuilderFinish(dockSpace);
        firstFrame = false;
    }

    // Error popup
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Error##OpenFileError", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Could not open file!\nError: %s", fileOpenErrorMessage.c_str());
        if (ImGui::Button("OK")) {
            fileOpenErrorMessage = "";
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void AINBY::HandleOpenRequest(FileIndexCacheEntry selectedEntry) {
    editor.UnloadAINB();
    auto pc = ProjectConfig::Get();

    std::optional<std::ifstream> rawFileStream;
    std::optional<std::istrstream> packInternalStream;
    if (selectedEntry.packFile == "Root") {
        currentSarc.Clear();
        sarcLoaded = false;

        rawFileStream = std::ifstream(pc.romfsPath / selectedEntry.file, std::ios::binary);
    } else {
        currentSarcName = selectedEntry.packFile;
        ZSTD_ReaderPool::OpenPackFile(pc.romfsPath / currentSarcName, currentSarc);
        sarcLoaded = true;

        u32 fileSize;
        const u8 *buffer = currentSarc.GetFileByPath(selectedEntry.file, fileSize);
        packInternalStream = std::istrstream((const char *) buffer, fileSize);
    }
    std::istream &is = packInternalStream ? (std::istream&)(*packInternalStream) : (std::istream&)(*rawFileStream);

    try {
        currentAinb.Read(is);
        editor.RegisterAINB(currentAinb);
        ainbLoaded = true;
    } catch (std::exception &e) {
        fileOpenErrorMessage = e.what();
        shouldOpenErrorPopup = true;
        ainbLoaded = false;
        editor.UnloadAINB();
    }
}
