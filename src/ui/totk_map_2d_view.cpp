#include "ui/totk_map_2d_view.hpp"
#include "file_formats/zstd.hpp"
#include "imgui.h"
#include "imgui_memory_editor.h"
#include "project_config.hpp"
#include "types.h"
#include "astcenc.h"

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <string>
#include <vector>

static MemoryEditor memEdit;

static std::vector<u8> bntxData;
static std::vector<u8> deswizData;
static Fushigi_Bfres::Texture::BntxFile bntxFile;
static astcenc_image img;
static astcenc_swizzle swz;
static astcenc_swizzle swizzle { ASTCENC_SWZ_R, ASTCENC_SWZ_G, ASTCENC_SWZ_B, ASTCENC_SWZ_A };
static astcenc_context* ctx;

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

    auto tex = bntxFile.Textures[0];
    deswizData = tex.DeswizzleSurface();

    // TODO configure ctx from profile+blocksize in bntx, package all this up nicer
    astcenc_config my_config;
    astcenc_config_init(ASTCENC_PRF_LDR_SRGB, 12, 12, 1, ASTCENC_PRE_MEDIUM, ASTCENC_FLG_DECOMPRESS_ONLY, &my_config);
    //astcenc_context* ctx;
    auto threadCount = 1;
    astcenc_context_alloc(&my_config, threadCount, &ctx);

    // we already unswizzled? so this should be identity swizzle
    //astcenc_swizzle swz;
    swz.r = ASTCENC_SWZ_R;
    swz.g = ASTCENC_SWZ_G;
    swz.b = ASTCENC_SWZ_B;
    swz.a = ASTCENC_SWZ_A;

    //astcenc_image img;
    img.dim_x = 3000;
    img.dim_y = 3000;
    img.dim_z = 1;
    img.data_type = ASTCENC_TYPE_F32; // F32 per component * 4 = 16B per pixel
    //img.data_type = ASTCENC_TYPE_U8; // F32 per component * 4 = 16B per pixel
    //img.data = (void**)(new f32[4 * 3000 * 3000]);
    //img.data = (void**)(new u8[16 * 250 * 250]); // block count * 16B each
    //f32* alo = (new f32[4 * 3000 * 3000]);
    //u8* alo = (new u8[16 * 3000 * 3000]);

    img.data = reinterpret_cast<void**>(new void*);
    *img.data = (new f32[4 * 3000 * 3000]);
    //f32* alo = (new f32[4 * 3000 * 3000]);
    //img.data = reinterpret_cast<void**>(&alo);

    auto err = astcenc_decompress_image(ctx, deswizData.data(), deswizData.size(), &img, &swizzle, 0);
    if (err != ASTCENC_SUCCESS) {
        std::cout << astcenc_get_error_string(err) << "\n";
    }

    // bad green output, only using 16b per channel, half the given size
    // #include "astcenccli_image_load_store.h"
    // store_dds_uncompressed_image(&img, uncompressedOutFilename, 0);

    // std::cout << std::hex << bntxFile.Textures[0].Header.Width << " " << bntxFile.Textures[0].Header.Height << "\n";
    // std::cout << std::hex << img.dim_x << " " << img.dim_y << " " << img.dim_z << " " << img.data_type << "\n";

    // it works! gimp chokes, ps can load it
    // https://stackoverflow.com/questions/16636311/what-is-the-simplest-rgb-image-format/49658800#49658800
    /*
    // #include <stdio.h>
    // #include <string.h>
    const auto width = 3000;
    const auto height = 3000;
    static unsigned char pixels[width * height * 3];
    static unsigned char tga[18];
    unsigned char *p = pixels;
    for (auto y = 0; y < height; y++) {
        for (auto x = 0; x < width; x++) {
            f32* pix = static_cast<f32*>((*(img.data)) + (4*4*(y*height + x)));
            // std::cout << (*(pix+0)) << " " << (*(pix+1)) << " " << (*(pix+2)) << " " << (*(pix+3)) << "\n";
            *p++ = 255 * (*(pix+2)); // g
            *p++ = 255 * (*(pix+1)); // b
            *p++ = 255 * (*(pix+0)); // r
        }
    }
    tga[2] = 2;
    tga[12] = 255 & width;
    tga[13] = 255 & (width >> 8);
    tga[14] = 255 & height;
    tga[15] = 255 & (height >> 8);
    tga[16] = 24;
    tga[17] = 32;
    FILE* f1 = fopen("/tmp/booyah.tga", "wb");
    fwrite(tga, sizeof(tga), 1, f1);
    fwrite(pixels, sizeof(pixels), 1, f1);
    */

}

void TotkMap2DView::Draw() {
    //memEdit.DrawContents(bntxData.data(), bntxData.size());

    //memEdit.DrawContents(&bntxFile.BinHeader.Magic, 4);
    //memEdit.DrawContents(&bntxFile.BinHeader, 32);
    //memEdit.DrawContents(&bntxFile.Header, 56);

    // still very different from toolbox astc out. missing swizzle?
    //auto texVec = bntxFile.Textures[0].TextureData;
    //memEdit.DrawContents(texVec.data(), texVec.size());

    // missing some header: 13 ab a1 5c 0c 0c 01 b8  0b 00 b8 0b 00 01 00 00
    // has large 0x7dc0 zero pad out to 0xfc000, but otherwise exact match for astc out!
    // TODO we probably just need AstcFile::SaveAstc
    //memEdit.DrawContents(deswizData.data(), deswizData.size());

    // decompressed texture data
    //memEdit.DrawContents(*(img.data), 16 * img.dim_x * img.dim_y * img.dim_z);
    memEdit.DrawContents(*(img.data), 16 * 3000 * 3000);
}
