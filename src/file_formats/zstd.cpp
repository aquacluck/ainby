#include "zstd.hpp"

#include <cstring>
#include <fstream>
#include <iostream>
#include <strstream>

void ZSTD::Read(std::istream &szFile) {
    data = ZSTD_ReaderPool::DecompressPackStream(szFile);
}

const u8 *ZSTD::GetData(size_t &size) const {
    size = data.size();
    return data.data();
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

// TODO needs dctx per thread, just make in threadpool? not specific to packs or ddicts either
ZSTD_DCtx* ZSTD_ReaderPool::packDCtx = ZSTD_createDCtx();

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

    // TODO spanstream should eliminate this copy, the deprecation, the silly cast
    std::istrstream sarcStream(reinterpret_cast<const char*>(sarcBuff.data()), sarcBuff.size());
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

std::vector<u8> ZSTD_ReaderPool::DecompressPackFile(const std::filesystem::path zsFile) {
    return DecompressPackVec(ReadFile(zsFile));
}

std::vector<u8> ZSTD_ReaderPool::DecompressPackStream(std::istream &zsStream) {
    return DecompressPackVec(ReadStream(zsStream));
}

std::vector<u8> ZSTD_ReaderPool::DecompressPackVec(std::vector<u8> compressedBytes) {
    std::vector<u8> sarcBuff;
    const auto sarcSize = ZSTD_getFrameContentSize(compressedBytes.data(), compressedBytes.size());
    if (ZSTD_isError(sarcSize)) {
        throw std::runtime_error("Could not decompress ZS file: " + std::string(ZSTD_getErrorName(sarcSize)));
    }
    sarcBuff.resize(sarcSize);
    size_t res;
    if (packDict) {
        res = ZSTD_decompress_usingDDict(packDCtx, sarcBuff.data(), sarcSize, compressedBytes.data(), compressedBytes.size(), packDict);
    } else {
        res = ZSTD_decompressDCtx(packDCtx, sarcBuff.data(), sarcSize, compressedBytes.data(), compressedBytes.size());
    }
    if (ZSTD_isError(res)) {
        throw std::runtime_error("Could not decompress ZS file: " + std::string(ZSTD_getErrorName(res)));
    }
    return sarcBuff;
}

SARC ZSTD_ReaderPool::OpenPackFile(const std::filesystem::path zsFile) {
    auto sarcBuff = DecompressPackFile(zsFile);
    // TODO spanstream should eliminate this copy, the deprecation, the silly cast
    std::istrstream sarcStream(reinterpret_cast<const char*>(sarcBuff.data()), sarcBuff.size());
    SARC pack;
    pack.Read(sarcStream); // Another copy
    return pack;
}

void ZSTD_ReaderPool::OpenPackFile(const std::filesystem::path zsFile, SARC& output) {
    auto sarcBuff = DecompressPackFile(zsFile);
    // TODO spanstream should eliminate this copy, the deprecation, the silly cast
    std::istrstream sarcStream(reinterpret_cast<const char*>(sarcBuff.data()), sarcBuff.size());
    output.Read(sarcStream); // Another copy
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
