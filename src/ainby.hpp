#pragma once

#include <filesystem>
#include <string>
#include <optional>

#include "ainb_editor/ainb_editor.hpp"
#include "file_formats/ainb.hpp"
#include "file_formats/sarc.hpp"
#include "file_formats/zstd.hpp"
#include "file_index_cache.hpp"
#include "project_config.hpp"
#include "style.hpp"

// Main editor class
class AINBY {
private:
    AINBEditor editor;

    //FileIndexCacheEntry currentIndexEntry;

    SARC currentSarc;
    std::string currentSarcName; // romfs relative path to pack.zs
    bool sarcLoaded = false;

    AINB currentAinb;
    bool ainbLoaded = false;

    bool shouldOpenErrorPopup = false;
    std::string fileOpenErrorMessage = "";

    bool firstFrame = true;

    void DrawMainWindow();

    void DrawPackBrowser();
    std::string DrawPackTree(const std::vector<std::string> &fileList);

    void HandleOpenRequest(FileIndexCacheEntry selectedEntry);

public:
    void Draw();

    bool shouldClose = false;
};
