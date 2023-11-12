#include "Fushigi_Bfres_Texture_Swizzling_TegraX1Swizzle.hpp"
#include <cstring>
//using Fushigi.Bfres;

namespace Fushigi_Bfres::Texture::Swizzling
{
    FormatInfo::FormatInfo(uint32_t bpp, uint32_t blockW, uint32_t blockH) {
        BytesPerPixel = bpp;
        BlockWidth = blockW;
        BlockHeight = blockH;
    }

    std::vector<uint8_t> TegraX1Swizzle::GetSurface(Fushigi_Bfres::Texture::BntxTexture &texture, int32_t array_level, int32_t mip_level, int32_t target /*= 1*/)
    {
        //Block and bpp format info
        uint32_t bpp = GetBytesPerPixel(texture.Header.Format);
        uint32_t blkWidth = GetBlockWidth(texture.Header.Format);
        uint32_t blkHeight = GetBlockHeight(texture.Header.Format);
        //Tile mode
        uint32_t tileMode = texture.Header.TileMode == Fushigi_Bfres::Common::Enum::TileMode::LinearAligned ? 1u : 0u;
        //Mip sizes
        uint32_t width = texture.Header.Width >> mip_level; if (1 > width) { width = 1; }
        uint32_t height = texture.Header.Height >> mip_level; if (1 > height) { height = 1; }
        //Block height
        int32_t block_height = CalculateBlockHeightLog(texture.BlockHeightLog2, width, blkWidth);
        //Slice mip data
        auto size = CalculateSurfaceSize(width, height, blkWidth, blkHeight, bpp, block_height);
        auto offset = CalculateMipOffset(mip_level, texture.Header.Alignment, width, height, blkWidth, blkHeight, bpp, block_height);
        //Mip data
        //auto mip_data = texture.TextureData[array_level].Slice((int32_t)offset, (int32_t)size);
        //auto mip_data = std::vector<uint8_t>(texture.TextureData.data() + array_level + offset, size);
        std::vector<uint8_t> mip_data;
        mip_data.resize(size);
        std::memcpy(mip_data.data(), texture.TextureData.data() + array_level + offset, size);

        //Deswizzle to proper image
        return deswizzle(
            width,
            height,
            1, blkWidth, blkHeight, 1, target, bpp, tileMode,
            block_height, mip_data);
    }

    uint32_t TegraX1Swizzle::CalculateMipOffset(int32_t level, uint32_t alignment, uint32_t width, uint32_t height, uint32_t blkWidth, uint32_t blkHeight, uint32_t bpp, int32_t blockHeightLog2)
    {
        uint32_t offset = 0;
        for (int32_t i = 0; i < level; i++)
        {
            auto size = CalculateSurfaceSize(width, height, blkWidth, blkHeight, bpp, blockHeightLog2);
            auto alignment_pad = (round_up(size, alignment) - size);
            offset += size + alignment_pad;
        }
        return offset;
    }

    uint32_t TegraX1Swizzle::CalculateSurfaceSize(uint32_t width, uint32_t height, uint32_t blkWidth, uint32_t blkHeight, uint32_t bpp, int32_t blockHeightLog2)
    {
        uint32_t block_height = (uint32_t)(1 << blockHeightLog2);

        auto div_width = DIV_ROUND_UP(width, blkWidth);
        auto div_height = DIV_ROUND_UP(height, blkHeight);

        auto width_in_gobs = round_up(div_width * bpp, 64);
        return width_in_gobs * round_up(div_height, (uint32_t)block_height * 8);
    }

    int32_t TegraX1Swizzle::CalculateBlockHeightLog(uint32_t blockHeightLog2, uint32_t width, uint32_t bllWidth)
    {
        int32_t linesPerBlockHeight = (1 << (int32_t)blockHeightLog2) * 8;

        int32_t blockHeightShift = 0;
        if (pow2_round_up(DIV_ROUND_UP(width, bllWidth)) < linesPerBlockHeight)
            blockHeightShift += 1;

        int32_t ret = blockHeightLog2 - blockHeightShift; if (0 > ret) { ret = 0; } return ret;
    }

    uint32_t TegraX1Swizzle::GetBytesPerPixel(Fushigi_Bfres::Common::Enum::SurfaceFormat format)
    {
        return GetFormat(format).BytesPerPixel;
    }

    uint32_t TegraX1Swizzle::GetBlockWidth(Fushigi_Bfres::Common::Enum::SurfaceFormat format)
    {
        return GetFormat(format).BlockWidth;
    }

    uint32_t TegraX1Swizzle::GetBlockHeight(Fushigi_Bfres::Common::Enum::SurfaceFormat format)
    {
        return GetFormat(format).BlockHeight;
    }

    FormatInfo TegraX1Swizzle::GetFormat(Fushigi_Bfres::Common::Enum::SurfaceFormat format) {
        switch(format) {
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::R32_G32_B32_A32_UNORM:
                return FormatInfo(16, 1,  1);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::R16_G16_B16_A16_UNORM:
                return FormatInfo(8, 1, 1);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::R8_G8_B8_A8_UNORM:
                return FormatInfo(4, 1, 1);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::R8_G8_B8_A8_SRGB:
                return FormatInfo(4, 1, 1);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::R8_G8_B8_A8_SNORM:
                return FormatInfo(4, 1, 1);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::R4_G4_B4_A4_UNORM:
                return FormatInfo(3, 1, 1);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::R32_G32_B32_UNORM:
                return FormatInfo(8, 1, 1);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::R4_G4_UNORM:
                return FormatInfo(1, 1, 1);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::R32_UNORM:
                return FormatInfo(4, 1, 1);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::R16_UNORM:
                return FormatInfo(2, 1, 1);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::R16_UINT:
                return FormatInfo(2, 1, 1);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::R8_UNORM:
                return FormatInfo(1, 1, 1);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::R32_G8_X24_UNORM:
                return FormatInfo(8, 1, 1);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::B8_G8_R8_A8_UNORM:
                return FormatInfo(4, 1, 1);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::B8_G8_R8_A8_SRGB:
                return FormatInfo(4, 1, 1);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::R5_G5_B5_A1_UNORM:
                return FormatInfo(2, 1, 1);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::B5_G5_R5_A1_UNORM:
                return FormatInfo(2, 1, 1);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::R5_G6_B5_UNORM:
                return FormatInfo(2, 1, 1);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::R10_G10_B10_A2_UNORM:
                return FormatInfo(4, 1, 1);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::R11_G11_B10_UNORM:
                return FormatInfo(4, 1, 1);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::A4_B4_G4_R4_UNORM:
                return FormatInfo(2, 1, 1);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::B5_G6_R5_UNORM:
                return FormatInfo(2, 1, 1);

            case Fushigi_Bfres::Common::Enum::SurfaceFormat::BC1_UNORM:
                return FormatInfo(8, 4, 4);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::BC1_SRGB:
                return FormatInfo(8, 4, 4);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::BC2_UNORM:
                return FormatInfo(16, 4, 4);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::BC2_SRGB:
                return FormatInfo(16, 4, 4);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::BC3_UNORM:
                return FormatInfo(16, 4, 4);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::BC3_SRGB:
                return FormatInfo(16, 4, 4);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::BC4_UNORM:
                return FormatInfo(8, 4, 4);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::BC4_SNORM:
                return FormatInfo(8, 4, 4);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::BC5_UNORM:
                return FormatInfo(16, 4, 4);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::BC5_SNORM:
                return FormatInfo(16, 4, 4);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::BC6_UFLOAT:
                return FormatInfo(16, 4, 4);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::BC6_FLOAT:
                return FormatInfo(16, 4, 4);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::BC7_UNORM:
                return FormatInfo(16, 4, 4);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::BC7_SRGB:
                return FormatInfo(16, 4, 4);

            case Fushigi_Bfres::Common::Enum::SurfaceFormat::ASTC_4x4_UNORM:
                return FormatInfo(16, 4, 4);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::ASTC_4x4_SRGB:
                return FormatInfo(16, 4, 4);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::ASTC_5x5_UNORM:
                return FormatInfo(16, 5, 5);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::ASTC_5x5_SRGB:
                return FormatInfo(16, 5, 5);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::ASTC_6x5_UNORM:
                return FormatInfo(16, 6, 5);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::ASTC_6x5_SRGB:
                return FormatInfo(16, 6, 5);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::ASTC_8x5_UNORM:
                return FormatInfo(16, 8, 5);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::ASTC_8x5_SRGB:
                return FormatInfo(16, 8, 5);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::ASTC_8x6_UNORM:
                return FormatInfo(16, 8, 6);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::ASTC_8x6_SRGB:
                return FormatInfo(16, 8, 6);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::ASTC_10x5_UNORM:
                return FormatInfo(16, 10, 5);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::ASTC_10x5_SRGB:
                return FormatInfo(16, 10, 5);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::ASTC_10x6_UNORM:
                return FormatInfo(16, 10, 6);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::ASTC_10x6_SRGB:
                return FormatInfo(16, 10, 6);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::ASTC_10x10_SRGB:
                return FormatInfo(16, 10, 10);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::ASTC_10x10_UNORM:
                return FormatInfo(16, 10, 10);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::ASTC_12x10_SRGB:
                return FormatInfo(16, 12, 10);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::ASTC_12x10_UNORM:
                return FormatInfo(16, 12, 10);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::ASTC_12x12_SRGB:
                return FormatInfo(16, 12, 12);
            case Fushigi_Bfres::Common::Enum::SurfaceFormat::ASTC_12x12_UNORM:
                return FormatInfo(16, 12, 12);
        }
    }


    /*---------------------------------------
     * 
     * Code ported from AboodXD's BNTX Extractor https://github.com/aboood40091/BNTX-Extractor/blob/master/swizzle.py
     * 
     *---------------------------------------*/

    uint32_t TegraX1Swizzle::GetBlockHeight(uint32_t height)
    {
        uint32_t blockHeight = pow2_round_up(height / 8);
        if (blockHeight > 16)
            blockHeight = 16;

        return blockHeight;
    }

    uint32_t TegraX1Swizzle::DIV_ROUND_UP(uint32_t n, uint32_t d)
    {
        return (n + d - 1) / d;
    }
    uint32_t TegraX1Swizzle::round_up(uint32_t x, uint32_t y)
    {
        return ((x - 1) | (y - 1)) + 1;
    }
    uint32_t TegraX1Swizzle::pow2_round_up(uint32_t x)
    {
        x -= 1;
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        return x + 1;
    }

    std::vector<uint8_t> TegraX1Swizzle::_swizzle(uint32_t width, uint32_t height, uint32_t depth, uint32_t blkWidth, uint32_t blkHeight, uint32_t blkDepth, int32_t roundPitch, uint32_t bpp, uint32_t tileMode, int32_t blockHeightLog2, std::vector<uint8_t> data, int32_t toSwizzle)
    {
        uint32_t block_height = (uint32_t)(1 << blockHeightLog2);

        width = DIV_ROUND_UP(width, blkWidth);
        height = DIV_ROUND_UP(height, blkHeight);
        depth = DIV_ROUND_UP(depth, blkDepth);

        uint32_t pitch;
        uint32_t surfSize;
        if (tileMode == 1)
        {
            pitch = width * bpp;

            if (roundPitch == 1)
                pitch = round_up(pitch, 32);

            surfSize = pitch * height;
        }
        else
        {
            pitch = round_up(width * bpp, 64);
            surfSize = pitch * round_up(height, block_height * 8);
        }

        std::vector<uint8_t> result;
        result.resize(surfSize);

        for (uint32_t y = 0; y < height; y++)
        {
            for (uint32_t x = 0; x < width; x++)
            {
                uint32_t pos;
                uint32_t pos_;

                if (tileMode == 1)
                    pos = y * pitch + x * bpp;
                else
                    pos = getAddrBlockLinear(x, y, width, bpp, 0, block_height);

                pos_ = (y * width + x) * bpp;

                if (pos + bpp <= surfSize)
                {
                    // public static void Array::Copy (Array sourceArray, int sourceIndex, Array destinationArray, int destinationIndex, int length);
                    if (toSwizzle == 0)
                        //Array.Copy(data, pos, result, pos_, bpp);
                        std::memcpy(result.data() + pos_, data.data() + pos, bpp);
                    else
                        //Array.Copy(data, pos_, result, pos, bpp);
                        std::memcpy(result.data() + pos, data.data() + pos_, bpp);
                }
            }
        }
        return result;
    }

    std::vector<uint8_t> TegraX1Swizzle::deswizzle(uint32_t width, uint32_t height, uint32_t depth, uint32_t blkWidth, uint32_t blkHeight, uint32_t blkDepth, int32_t roundPitch, uint32_t bpp, uint32_t tileMode, int32_t size_range, std::vector<uint8_t> data)
    {
        return _swizzle(width, height, depth, blkWidth, blkHeight, blkDepth, roundPitch, bpp, tileMode, size_range, data, 0);
    }

    std::vector<uint8_t> TegraX1Swizzle::swizzle(uint32_t width, uint32_t height, uint32_t depth, uint32_t blkWidth, uint32_t blkHeight, uint32_t blkDepth, int32_t roundPitch, uint32_t bpp, uint32_t tileMode, int32_t size_range, std::vector<uint8_t> data)
    {
        return _swizzle(width, height, depth, blkWidth, blkHeight, blkDepth, roundPitch, bpp, tileMode, size_range, data, 1);
    }

    uint32_t TegraX1Swizzle::getAddrBlockLinear(uint32_t x, uint32_t y, uint32_t width, uint32_t bytes_per_pixel, uint32_t base_address, uint32_t block_height)
    {
        /*
          From Tega X1 TRM 
                           */
        uint32_t image_width_in_gobs = DIV_ROUND_UP(width * bytes_per_pixel, 64);


        uint32_t GOB_address = (base_address
                            + (y / (8 * block_height)) * 512 * block_height * image_width_in_gobs
                            + (x * bytes_per_pixel / 64) * 512 * block_height
                            + (y % (8 * block_height) / 8) * 512);

        x *= bytes_per_pixel;

        uint32_t Address = (GOB_address + ((x % 64) / 32) * 256 + ((y % 8) / 2) * 64
                        + ((x % 32) / 16) * 32 + (y % 2) * 16 + (x % 16));
        return Address;
    }
}
