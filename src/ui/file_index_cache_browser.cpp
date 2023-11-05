#include <algorithm>
#include <imgui.h>
#include "file_index_cache_browser.hpp"
#include "project_config.hpp"

std::optional<FileIndexCacheEntry> FileIndexCacheBrowser::Draw(FileIndexCache& index) {
    // TODO quick search filter
    auto rootPack = index.types["ainb"].packs["Root"];
    auto globalAinbPacks = ProjectConfig::Get().globalAinbPacks;
    std::optional<FileIndexCacheEntry> selectedEntry;

    // Render special packs first
    if (ImGui::TreeNode("Root")) {
        for (std::pair kv: rootPack.files) {
            ImGui::TreeNodeEx(kv.second.file.c_str(), ImGuiTreeNodeFlags_Leaf);
            if (ImGui::IsItemClicked()) {
                selectedEntry = kv.second;
            }
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }

    for (auto aiPackName: globalAinbPacks) {
        auto aiProductPack = index.types["ainb"].packs[aiPackName];
        if (ImGui::TreeNode(aiPackName.c_str())) {
            for (std::pair kv: aiProductPack.files) {
                ImGui::TreeNodeEx(kv.second.file.c_str(), ImGuiTreeNodeFlags_Leaf);
                if (ImGui::IsItemClicked()) {
                    selectedEntry = kv.second;
                }
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }
    }

    for (std::pair pack_kv: index.types["ainb"].packs) {
        // Exclude special packs
        if (pack_kv.first == "Root") { continue; }
        // Skip if packFile is in global pack list
        if ( std::find(globalAinbPacks.begin(), globalAinbPacks.end(), pack_kv.first) != globalAinbPacks.end() ) {
            continue;
        }

        if (ImGui::TreeNode(pack_kv.first.c_str())) {
            for (std::pair kv: pack_kv.second.files) {
                ImGui::TreeNodeEx(kv.second.file.c_str(), ImGuiTreeNodeFlags_Leaf);
                if (ImGui::IsItemClicked()) {
                    selectedEntry = kv.second;
                }
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }
    }

    return selectedEntry;
}
