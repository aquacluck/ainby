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
            std::cout << fName << "\n";
            this->types["ainb"].packs[packFile].files.try_emplace(fName, fName, packFile); // FileIndexCacheEntry
        }
    }

    // Global AI pack
    for (auto aiPackName: pc.globalAinbPacks) {
        IndexPack(pc.romfsPath / aiPackName, pc);
    }

    // Actor packs
    for (auto const& entry: std::filesystem::directory_iterator(pc.romfsPath / "Pack" / "Actor")) {
        if (!entry.is_regular_file() || !entry.path().string().ends_with(".pack.zs")) { continue; }
        // std::cout << entry.path() << "\n";
        IndexPack(entry.path(), pc);
    }
}

void FileIndexCache::IndexPack(const std::filesystem::path packPath, ProjectConfig &pc) {
    std::string packFile = std::filesystem::relative(packPath, pc.romfsPath).string();
    auto archive = ZSTD_ReaderPool::OpenPackFile(packPath);
    for (auto const& intName: archive.GetFileList()) {
        if (intName.size() > 0 && intName.ends_with(".ainb")) {
            this->types["ainb"].packs[packFile].files.try_emplace(intName, intName, packFile); // FileIndexCacheEntry
        } else {
            //
        }
    }
}
