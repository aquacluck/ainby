#include "ainby.hpp"
#include "ui/file_index_cache_browser.hpp"
#include "ui/pack_browser.hpp"

#include <iostream>
#include <algorithm>
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
        HandleOpenRequest(index, *maybeOpenEntry);
    }
    ImGui::End();

    ImGui::Begin("Pack Browser");
    if (sarcLoaded) {
        auto maybeOpenInternalFilename = PackBrowser::Draw(currentSarc);
        if (maybeOpenInternalFilename) {
            auto entry = FileIndexCacheEntry(*maybeOpenInternalFilename, currentSarcName);
            HandleOpenRequest(index, entry);
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
}

void AINBY::DrawMainWindow() {
    // TODO: Make this a simple "Open" item instead and detect the file type automatically
    int openFileType = -1;
    bool savePack = false;
    bool saveSZ = false;
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open .zs")) {
                openFileType = 2;
            }
            if (ImGui::MenuItem("Open .pack")) {
                openFileType = 0;
            }
            if (ImGui::MenuItem("Open .ainb")) {
                openFileType = 1;
            }
            if (ImGui::MenuItem("Save .pack")) {
                savePack = true;
            }
            if (ImGui::MenuItem("Save .zs")) {
                saveSZ = true;
            }
            if (ImGui::MenuItem("Exit")) {
                shouldClose = true;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    if (openFileType != -1) {
        const char *path = tinyfd_openFileDialog("Open file", "", 0, nullptr, nullptr, 0);
        if (path != nullptr) {
            try {
                std::ifstream file(path, std::ios::binary);
                if (openFileType == 0) {
                    currentSarc.Read(file);
                    sarcLoaded = true;
                } else if (openFileType == 1) {
                    currentAinb.Read(file);
                    editor.RegisterAINB(currentAinb);
                    ainbLoaded = true;
                } else {
                    ZSTD zstdFile;
                    zstdFile.Read(file);

                    size_t decompressedSize;
                    const u8 *decompressed = zstdFile.GetData(decompressedSize);

                    std::istrstream stream((const char *) decompressed, decompressedSize);
                    currentSarc.Read(stream);
                    sarcLoaded = true;
                }
            } catch (std::exception &e) {
                fileOpenErrorMessage = e.what();
                shouldOpenErrorPopup = true;
            }
        }
    }

    if (savePack) {
        const char *path = tinyfd_saveFileDialog("Save file", "", 0, nullptr, nullptr);
        if (path != nullptr) {
            try {
                std::ofstream file(path, std::ios::binary);
                currentSarc.Write(file);
            } catch (std::exception &e) {
                fileOpenErrorMessage = e.what();
                shouldOpenErrorPopup = true;
            }
        }
    }

    if (saveSZ) {
        const char *path = tinyfd_saveFileDialog("Save file", "", 0, nullptr, nullptr);
        if (path != nullptr) {
            try {
                std::ostrstream stream;
                currentSarc.Write(stream);

                // FIXME totk needs pack CDict
                std::ofstream file(path, std::ios::binary);
                ZSTD::Write(file, (const u8 *) stream.str(), stream.pcount());

                stream.freeze(false);
            } catch (std::exception &e) {
                fileOpenErrorMessage = e.what();
                shouldOpenErrorPopup = true;
            }
        }
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

void AINBY::HandleOpenRequest(FileIndexCache& index, FileIndexCacheEntry selectedEntry) {
    editor.UnloadAINB();

    std::optional<std::ifstream> rawFileStream;
    std::optional<std::istrstream> packInternalStream;
    if (selectedEntry.packFile == "Root") {
        currentSarc.Clear();
        sarcLoaded = false;

        rawFileStream = std::ifstream(index.GetRomfsPath() / selectedEntry.file, std::ios::binary);
    } else {
        currentSarcName = selectedEntry.packFile;
        ZSTD_ReaderPool::OpenPackFile(index.GetRomfsPath() / currentSarcName, currentSarc);
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
