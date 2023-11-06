#include "file_index_cache.hpp"
#include "file_formats/zstd.hpp"

#include <iostream>

static FileIndexCache fileIndexCache = FileIndexCache();
FileIndexCache& FileIndexCache::Get(void) { return fileIndexCache; }

// TODO overlay modRomfsPath on load? or keep it in a separate instance?
void FileIndexCache::CrawlPacks() {
    auto pc = ProjectConfig::Get();

    // Index special "Root" namespace like a pack
    for (auto const& cat: pc.rootAinbFolders) {
        auto const packFile = "Root";
        for (auto const& entry: std::filesystem::directory_iterator(pc.romfsPath / cat)) {
            if (!entry.is_regular_file() || entry.path().extension() != ".ainb") { continue; }
            std::string fName = std::filesystem::relative(entry.path(), pc.romfsPath).string();
            // std::cout << fName << "\n";
            this->types["ainb"].packs[packFile].files.try_emplace(fName, fName, packFile); // FileIndexCacheEntry
        }
    }

    // First batch: globalAinbPacks is always(?) 1, then we blindly add 1 in the actor loop
    if (pc.maxDecompressionThreads < pc.globalAinbPacks.size()+1) {
        throw std::runtime_error("maxDecompressionThreads must be > globalAinbPacks");
    }
    std::vector<std::filesystem::path> batchPaths;
    batchPaths.reserve(pc.maxDecompressionThreads);

    // Global AI pack
    for (auto aiPackName: pc.globalAinbPacks) {
        batchPaths.push_back(pc.romfsPath / aiPackName);
    }

    // Actor packs
    for (auto const& entry: std::filesystem::directory_iterator(pc.romfsPath / "Pack" / "Actor")) {
        if (!entry.is_regular_file() || !entry.path().string().ends_with(".pack.zs")) { continue; }
        batchPaths.push_back(entry.path());
        if (batchPaths.size() == pc.maxDecompressionThreads) {
            IndexPackBatch(batchPaths, pc);
            batchPaths.clear();
        }
        // std::cout << entry.path() << "\n";
    }
    if (batchPaths.size() > 0) { // Finish partial batch
        IndexPackBatch(batchPaths, pc);
    }
}

void FileIndexCache::IndexPackBatch(const std::vector<std::filesystem::path> packPaths, ProjectConfig &pc) {
    // Send batch to reader pool and wait for all of them
    auto archiveBatch = ZSTD_ReaderPool::OpenPackFileBatch(packPaths);

    for(const auto& [packPath, sarc] : archiveBatch) {
        std::string packFile = std::filesystem::relative(packPath, pc.romfsPath).string();
        for (const auto& intName: sarc.GetFileList()) {
            if (intName.size() > 0 && intName.ends_with(".ainb")) {
                this->types["ainb"].packs[packFile].files.try_emplace(intName, intName, packFile); // FileIndexCacheEntry
            } else {
                // other filetypes?
                // if we do any real work on the files to index more data, consider threading that too
            }
        }
    }
}
