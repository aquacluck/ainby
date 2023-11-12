#pragma once
#include <filesystem>
#include <iostream>
#include <memory>
#include <spanstream>
#include <string>
#include <vector>

#include <zstd.h>
#include "sarc.hpp"
#include "types.h"


// Owns an anonymous decompressed buffer
class ZSTD {
public:
    void Read(std::istream &szFile);
    const u8 *GetData(size_t &size) const;
    std::basic_spanstream<u8> GetStream(void);

    static void Write(std::ostream &szFile, const u8 *data, size_t size, int compressionLevel = 19);

private:
    std::vector<u8> data;
};


// Non-owning calls intended eventually for fast bulk loads
class ZSTD_ReaderPool {
public:
    // Unused in wonder, totk has 3 zstd dicts. Also why we have pack specific decompression calls
    static void LoadDDictsFromSarc(const std::filesystem::path zsDicPackPath);
    static void AllocDCtxPool(const int maxThreads);

    static std::vector<u8> DecompressPackFile(const std::filesystem::path zsFile, ZSTD_DCtx* dctx=nullptr);
    static std::vector<u8> DecompressPackStream(std::istream &zsStream, ZSTD_DCtx* dctx=nullptr);
    static std::vector<u8> DecompressPackVec(std::vector<u8> compressedBytes, ZSTD_DCtx* dctx=nullptr);


    static SARC OpenPackFile(const std::filesystem::path zsFile);
    static void OpenPackFile(const std::filesystem::path zsFile, SARC& output);
    static std::map<std::filesystem::path, SARC> OpenPackFileBatch(std::vector<std::filesystem::path> packPathList);

    // Just file reads, these do not decompress
    static std::vector<u8> ReadFile(const std::filesystem::path zsFile);
    static std::vector<u8> ReadStream(std::istream &zsStream);
private:
    // DDicts specific to totk, TODO map<string, ZSTD_DDict>?
    static ZSTD_DDict* packDict;
    static ZSTD_DDict* productZstdDict;
    static ZSTD_DDict* bcettBymlDict;

    // Default for plain synchronous calls + ProjectConfig.maxDecompressionThreads
    static ZSTD_DCtx* mainThreadDCtx;
    static std::vector<ZSTD_DCtx*> threadDCtxPool;
};
