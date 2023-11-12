#include "ui/totk_map_2d_view.hpp"
#include "file_formats/zstd.hpp"
#include "imgui.h"
#include "imgui_memory_editor.h"
#include "project_config.hpp"
#include "types.h"

#include <iostream>
#include <string>
#include <vector>

static MemoryEditor memEdit;

static std::vector<u8> bntxData;
static std::vector<u8> deswizData;
static Fushigi_Bfres::Texture::BntxFile bntxFile;

void TotkMap2DView::Init() {
    auto zstdFileName = ProjectConfig::Get().romfsPath / "UI/Map/MainField/G_06-05_00_TTTT.bntx.zs";
    bntxData = ZSTD_ReaderPool::DecompressPackFile(zstdFileName);

    ZSTD zs;
    auto ifs = std::ifstream(zstdFileName, std::ios::binary);
    zs.Read(ifs);
    auto is = zs.GetStream();

    //auto bntxFile = Fushigi_Bfres::Texture::BntxFile(decompressedStream);
    //Fushigi_Bfres::Texture::BntxFile bntxFile;
    bntxFile.Read(is);

    deswizData = bntxFile.Textures[0].DeswizzleSurface();
}

void TotkMap2DView::Draw() {
    //memEdit.DrawContents(bntxData.data(), bntxData.size());

    // still very different from toolbox astc out. missing swizzle?
    //auto texVec = bntxFile.Textures[0].TextureData;
    //memEdit.DrawContents(texVec.data(), texVec.size());

    // missing some header: 13 ab a1 5c 0c 0c 01 b8  0b 00 b8 0b 00 01 00 00
    // has large 0x7dc0 zero pad out to 0xfc000, but otherwise exact match for astc out!
    // TODO we probably just need AstcFile::SaveAstc
    memEdit.DrawContents(deswizData.data(), deswizData.size());

    //memEdit.DrawContents(&bntxFile.BinHeader.Magic, 4);
    //memEdit.DrawContents(&bntxFile.BinHeader, 32);
    //memEdit.DrawContents(&bntxFile.Header, 56);
}
