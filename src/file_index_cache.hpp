#pragma once

#include <filesystem>
#include <map>
#include <string>

// per filetype index of everything inside packs (but just ainb for now)
class FileIndexCacheEntry {
public:
    std::string file; // relative to romfs root or pack root
    std::string packFile; // "Root" or containing pack relative to romfs root
    FileIndexCacheEntry(std::string a_file, std::string a_packFile) {
        file = a_file;
        packFile = a_packFile;
    }
};

class FileIndexCachePack {
public:
    // key is `FileIndexCacheEntry.file`
    std::map<std::string, FileIndexCacheEntry> files;
};

class FileIndexCacheType {
public:
    // key is "Root" or `FileIndexCacheEntry.packFile`
    std::map<std::string, FileIndexCachePack> packs;
};

class FileIndexCache {
public:
    static FileIndexCache& Get(void); // singleton or whatever for now

    // should find nicer get/set patterns
    std::filesystem::path GetRomfsPath();
    void SetRomfsPath(const std::filesystem::path _romfsPath);

    // key is "ainb", TODO make a filetype enum or something
    std::map<std::string, FileIndexCacheType> types;

    void CrawlPacks();

private:
    std::filesystem::path romfsPath;
    void IndexPack(const std::filesystem::path packPath);
};
