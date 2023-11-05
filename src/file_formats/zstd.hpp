#pragma once
#include <filesystem>
#include <string>
#include <iostream>
#include <vector>
#include <zstd.h>

#include "sarc.hpp"
#include "types.h"

// Owns an anonymous decompressed buffer
class ZSTD {
public:
    void Read(std::istream &szFile);
    const u8 *GetData(size_t &size) const;

    static void Write(std::ostream &szFile, const u8 *data, size_t size, int compressionLevel = 19);

private:
    std::vector<u8> data;
};


// Non-owning calls intended eventually for fast bulk loads
class ZSTD_ReaderPool {
public:
    // Unused in wonder, totk has 3 zstd dicts. Also why we have pack specific decompression calls
    static void LoadDDictsFromSarc(const std::filesystem::path zsDicPackPath);

    static std::vector<u8> DecompressPackFile(const std::filesystem::path zsFile);
    static std::vector<u8> DecompressPackStream(std::istream &zsStream);
    static std::vector<u8> DecompressPackVec(std::vector<u8> compressedBytes);

    static SARC OpenPackFile(const std::filesystem::path zsFile);
    static void OpenPackFile(const std::filesystem::path zsFile, SARC& output);

private:
    // DDicts specific to totk, TODO map<string, ZSTD_DDict>?
    static ZSTD_DDict* packDict;
    static ZSTD_DDict* productZstdDict;
    static ZSTD_DDict* bcettBymlDict;

    static ZSTD_DCtx* packDCtx;

    static std::vector<u8> ReadFile(const std::filesystem::path zsFile);
    static std::vector<u8> ReadStream(std::istream &zsStream);
};
