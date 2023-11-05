#include "file_index_cache.hpp"
#include "file_formats/zstd.hpp"

#include <iostream>

static FileIndexCache fileIndexCache = FileIndexCache();
FileIndexCache& FileIndexCache::Get(void) { return fileIndexCache; }

std::filesystem::path FileIndexCache::GetRomfsPath() { return romfsPath; }
void FileIndexCache::SetRomfsPath(const std::filesystem::path _romfsPath) { romfsPath = _romfsPath; }

// TODO handle game+version specific root+pack locations, or truly crawl everything
void FileIndexCache::CrawlPacks() {
    // Index special "Root" namespace like a pack
    for (auto const& cat: {"AI", "Logic", "Sequence"}) {
        auto const packFile = "Root";
        for (auto const& entry: std::filesystem::directory_iterator(romfsPath / cat)) {
            if (!entry.is_regular_file() || entry.path().extension() != ".ainb") { continue; }
            std::string fName = std::filesystem::relative(entry.path(), romfsPath).string();
            std::cout << fName << "\n";
            this->types["ainb"].packs[packFile].files.try_emplace(fName, fName, packFile); // FileIndexCacheEntry
        }
    }

    // AI/Global pack
    IndexPack(romfsPath / "Pack/AI.Global.Product.100.pack.zs");

    // Actor packs
    for (auto const& entry: std::filesystem::directory_iterator(romfsPath / "Pack" / "Actor")) {
        if (!entry.is_regular_file() || !entry.path().string().ends_with(".pack.zs")) { continue; }
        // std::cout << entry.path() << "\n";
        IndexPack(entry.path());
    }
}

void FileIndexCache::IndexPack(const std::filesystem::path packPath) {
    std::string packFile = std::filesystem::relative(packPath, romfsPath).string();
    auto archive = ZSTD_ReaderPool::OpenPackFile(packPath);
    for (auto const& intName: archive.GetFileList()) {
        if (intName.size() > 0 && intName.ends_with(".ainb")) {
            this->types["ainb"].packs[packFile].files.try_emplace(intName, intName, packFile); // FileIndexCacheEntry
        } else {
            //
        }
    }
}
