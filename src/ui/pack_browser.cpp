#include <imgui.h>
#include "pack_browser.hpp"

std::optional<std::string> PackBrowser::Draw(SARC &currentSarc) {
    // TODO replace "Files" with pack name
    std::optional<std::string> openFile;
    if (ImGui::TreeNodeEx("Files", ImGuiTreeNodeFlags_DefaultOpen)) {
        openFile = DrawPackTree(currentSarc.GetFileList());
        ImGui::TreePop();
    }
    return openFile;
}

std::optional<std::string> PackBrowser::DrawPackTree(const std::vector<std::string> &fileList) {
    // FIXME don't sort every frame?
    // Sort file list so that the drawing algorithm works correctly
    // (and also so that the files are in alphabetical order lol)
    std::vector<std::string> sortedFileList = fileList;
    std::sort(sortedFileList.begin(), sortedFileList.end());

    // Keep track of opened folders
    std::vector<std::string> currPath;
    std::vector<bool> isOpened;
    isOpened.push_back(true);

    std::optional<std::string> selectedFile;
    for (const std::string &filePath : sortedFileList) {
        // Split path into its components
        std::vector<std::string> path;
        std::istringstream iss(filePath);
        std::string token;
        while (std::getline(iss, token, '/')) {
            path.push_back(token);
        }

        for (size_t i = 0; i < path.size(); i++) {
            // Navigate to the correct folder
            if (currPath.size() > i) {
                if (currPath[i] == path[i]) {
                    continue;
                }
                while (currPath.size() > i) {
                    currPath.pop_back();
                    if (isOpened.back()) {
                        ImGui::TreePop();
                    }
                    isOpened.pop_back();
                }
            }
            currPath.push_back(path[i]);

            // Draw the folder node
            if (isOpened.back()) {
                ImGuiTreeNodeFlags nodeFlags = (i == path.size() - 1) ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_None;
                bool openRes = ImGui::TreeNodeEx(path[i].c_str(), nodeFlags);
                if (i == path.size() - 1 && ImGui::IsItemClicked()) {
                    selectedFile = filePath;
                }
                isOpened.push_back(openRes);
            } else {
                // If parent folder is closed, the child folder must be closed too
                isOpened.push_back(false);
            }
        }
    }
    // Finish popping off the tree node stack
    while (currPath.size() > 0) {
        currPath.pop_back();
        if (isOpened.back()) {
            ImGui::TreePop();
        }
        isOpened.pop_back();
    }

    return selectedFile;
}
