#include <imgui.h>
#include "file_index_cache_browser.hpp"

std::optional<FileIndexCacheEntry> FileIndexCacheBrowser::Draw(FileIndexCache& index) {
    // TODO per game options to display special packs first
    // TODO quick search filter
    auto rootPack = index.types["ainb"].packs["Root"];
    auto aiProductPack = index.types["ainb"].packs["Pack/AI.Global.Product.100.pack.zs"];

    std::optional<FileIndexCacheEntry> selectedEntry;

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

    if (ImGui::TreeNode("Pack/AI.Global.Product.100.pack.zs")) {
        for (std::pair kv: aiProductPack.files) {
            ImGui::TreeNodeEx(kv.second.file.c_str(), ImGuiTreeNodeFlags_Leaf);
            if (ImGui::IsItemClicked()) {
                selectedEntry = kv.second;
            }
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }

    for (std::pair pack_kv: index.types["ainb"].packs) {
        if (pack_kv.first == "Root" || pack_kv.first == "Pack/AI.Global.Product.100.pack.zs") { continue; }
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
