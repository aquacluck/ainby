#include "zstd.hpp"

#include <cstring>
#include <fstream>
#include <iostream>
#include <mutex>
#include <span>
#include <thread>

void ZSTD::Read(std::istream &szFile) {
    data = ZSTD_ReaderPool::DecompressPackStream(szFile);
}

const u8 *ZSTD::GetData(size_t &size) const {
    size = data.size();
    return data.data();
}

std::basic_spanstream<u8> ZSTD::GetStream(void) {
    return std::basic_spanstream(std::span{(u8*)data.data(), data.size()});
}

void ZSTD::Write(std::ostream &szFile, const u8 *data, size_t size, int compressionLevel) {
    size_t szCompressedSize = ZSTD_compressBound(size);
    std::vector<u8> buffer(szCompressedSize);
    size_t res = ZSTD_compress(buffer.data(), szCompressedSize, data, size, compressionLevel);
    if (ZSTD_isError(res)) {
        throw std::runtime_error("Could not compress SZ file: " + std::string(ZSTD_getErrorName(res)));
    }
    szFile.write((char *) buffer.data(), res);
}

// ZSTD_ReaderPool

ZSTD_DDict* ZSTD_ReaderPool::packDict = nullptr;
ZSTD_DDict* ZSTD_ReaderPool::productZstdDict = nullptr;
ZSTD_DDict* ZSTD_ReaderPool::bcettBymlDict = nullptr;
ZSTD_DCtx* ZSTD_ReaderPool::mainThreadDCtx = ZSTD_createDCtx();
std::vector<ZSTD_DCtx*> ZSTD_ReaderPool::threadDCtxPool;

void ZSTD_ReaderPool::LoadDDictsFromSarc(const std::filesystem::path zsDicPackPath) {
    auto compressedBytes = ReadFile(zsDicPackPath);
    const auto sarcSize = ZSTD_getFrameContentSize(compressedBytes.data(), compressedBytes.size());
    if (ZSTD_isError(sarcSize)) {
        throw std::runtime_error("Could not decompress ZS file: " + std::string(ZSTD_getErrorName(sarcSize)));
    }
    std::vector<u8> sarcBuff;
    sarcBuff.resize(sarcSize);
    auto res = ZSTD_decompress(sarcBuff.data(), sarcSize, compressedBytes.data(), compressedBytes.size());
    if (ZSTD_isError(res)) {
        throw std::runtime_error("Could not decompress ZS file: " + std::string(ZSTD_getErrorName(res)));
    }
    //std::cout << compressedBytes.size() << " -> " << sarcSize << "\n";

    std::basic_spanstream sarcStream(std::span{(char*)sarcBuff.data(), sarcBuff.size()});
    SARC zsDicPack;
    zsDicPack.Read(sarcStream); // Another copy

    const u8 *internalFileBuffer;
    u32 internalFileSize;

    // Make a ddict for each packed zsdic, afaik ro+threadsafe
    internalFileBuffer = zsDicPack.GetFileByPath("pack.zsdic", internalFileSize);
    packDict = ZSTD_createDDict(internalFileBuffer, internalFileSize);

    internalFileBuffer = zsDicPack.GetFileByPath("zs.zsdic", internalFileSize);
    productZstdDict = ZSTD_createDDict(internalFileBuffer, internalFileSize);

    internalFileBuffer = zsDicPack.GetFileByPath("bcett.byml.zsdic", internalFileSize);
    bcettBymlDict = ZSTD_createDDict(internalFileBuffer, internalFileSize);
}

void ZSTD_ReaderPool::AllocDCtxPool(const int maxThreads) {
    threadDCtxPool.reserve(maxThreads);
    for(auto i=0; i < maxThreads; i++) {
        threadDCtxPool.push_back(ZSTD_createDCtx());
    }
}

std::vector<u8> ZSTD_ReaderPool::DecompressPackFile(const std::filesystem::path zsFile, ZSTD_DCtx* dctx /*=nullptr*/) {
    return DecompressPackVec(ReadFile(zsFile), dctx);
}

std::vector<u8> ZSTD_ReaderPool::DecompressPackStream(std::istream &zsStream, ZSTD_DCtx* dctx /*=nullptr*/) {
    return DecompressPackVec(ReadStream(zsStream), dctx);
}

std::vector<u8> ZSTD_ReaderPool::DecompressPackVec(std::vector<u8> compressedBytes, ZSTD_DCtx* dctx /*=nullptr*/) {
    // When no dctx provided, assume we're in a blocking main thread call and always use this one
    if (dctx == nullptr) { dctx = mainThreadDCtx; }

    std::vector<u8> sarcBuff;
    const auto sarcSize = ZSTD_getFrameContentSize(compressedBytes.data(), compressedBytes.size());
    if (ZSTD_isError(sarcSize)) {
        throw std::runtime_error("Could not decompress ZS file: " + std::string(ZSTD_getErrorName(sarcSize)));
    }
    sarcBuff.resize(sarcSize);
    size_t res;
    if (packDict) {
        res = ZSTD_decompress_usingDDict(dctx, sarcBuff.data(), sarcSize, compressedBytes.data(), compressedBytes.size(), packDict);
    } else {
        res = ZSTD_decompressDCtx(dctx, sarcBuff.data(), sarcSize, compressedBytes.data(), compressedBytes.size());
    }

    if (ZSTD_isError(res)) {
        throw std::runtime_error("Could not decompress ZS file: " + std::string(ZSTD_getErrorName(res)));
    }
    return sarcBuff;
}

SARC ZSTD_ReaderPool::OpenPackFile(const std::filesystem::path zsFile) {
    auto sarcBuff = DecompressPackFile(zsFile);
    std::basic_spanstream sarcStream(std::span{(char*)sarcBuff.data(), sarcBuff.size()});
    SARC pack;
    pack.Read(sarcStream); // Another copy
    return pack;
}

void ZSTD_ReaderPool::OpenPackFile(const std::filesystem::path zsFile, SARC& output) {
    auto sarcBuff = DecompressPackFile(zsFile);
    std::basic_spanstream sarcStream(std::span{(char*)sarcBuff.data(), sarcBuff.size()});
    output.Read(sarcStream); // Another copy
}

std::map<std::filesystem::path, SARC> ZSTD_ReaderPool::OpenPackFileBatch(std::vector<std::filesystem::path> packPathList) {
    std::map<std::filesystem::path, SARC> output;
    std::mutex outputMutex;
    std::vector<std::thread> threads;
    auto workCount = packPathList.size();
    if (workCount > threadDCtxPool.size()) {
        throw std::runtime_error("Pack batch size exceeds available DCtxs/maxDecompressionThreads");
    }

    for (auto i=0; i < workCount; i++) {
        auto packPath = packPathList[i];
        auto dctx = threadDCtxPool[i];
        threads.emplace_back([&] (std::filesystem::path pp, ZSTD_DCtx* dctx) {
            auto sarcBuff = DecompressPackFile(pp, dctx);
            std::basic_spanstream sarcStream(std::span{(char*)sarcBuff.data(), sarcBuff.size()});
            // Lock and write to output
            std::scoped_lock lk(outputMutex);
            // XXX can we do some Read before locking, then std::move or copy or something faster than the Read?
            output[pp].Read(sarcStream);
        }, packPath, dctx);
    }

    for (auto i=0; i < workCount; i++) {
        threads[i].join();
    }

    return output;
}

std::vector<u8> ZSTD_ReaderPool::ReadFile(const std::filesystem::path zsFile) {
    auto is = std::ifstream(zsFile, std::ios::binary);
    return ReadStream(is);
}

// TODO benchmark+tune against romfs
// TODO any error handling here?
std::vector<u8> ZSTD_ReaderPool::ReadStream(std::istream &zsStream) {
    const auto blockSize = std::size_t{65536};
    std::vector<u8> blockBuff(blockSize);
    auto blockAddr = reinterpret_cast<char*>(blockBuff.data());

    std::vector<u8> output;
    std::size_t totalRead = 0;

    zsStream.seekg(0);
    while (true) {
        zsStream.read(blockAddr, blockSize);
        auto gcount = zsStream.gcount();
        if (gcount == 0) { break; }
        output.resize(totalRead + gcount); // Alters output.data location
        std::memcpy(reinterpret_cast<unsigned char*>(output.data()) + totalRead, blockAddr, gcount);
        totalRead += gcount;
        zsStream.seekg(totalRead);
    }

    //output.shrink_to_fit(); // unnecessary unless memcpy fails, we only resize to what we need
    //std::cout << zsFile << " raw: " << output.size() << "\n";
    return output;
}
