#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

// arbitrary strings for title+version
const auto TITLE_TOTK_100 = "totk_100";
const auto TITLE_WONDER_100 = "wonder_100";

class ProjectConfig {
public:
    static ProjectConfig& Get(void); // singleton or whatever for now
    static void LoadFromJson(const json& j);

    std::filesystem::path appCachePath; // Persistent scratch space
    std::filesystem::path romfsPath; // TODO we should enforce this path is ro
    //std::filesystem::path modRomfsPath; // TODO all writes go here. Overlay for reads too?

    std::string titleName; // "totk_100", "wonder_100", unused for now but intended for hardcoded app logic
    std::vector<std::string> globalAinbPacks; // romfs-relative pack files to index (Pack/Actor is always crawled)
    std::vector<std::string> rootAinbFolders; // romfs-relative folders to index
    std::string zsdicPackFile; // romfs-relative zsdic pack (only for totk). Internal filenames+handling is still hardcoded

    int maxDecompressionThreads; // minimum 2 for no good reason
};
