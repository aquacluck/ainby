#pragma once

#include <cstdint>
#include <map>
#include <vector>
#include "Fushigi_Bfres_Common_Enum.hpp"
#include "Fushigi_Bfres_Texture_BntxTexture.hpp"

namespace Fushigi_Bfres::Texture::Swizzling
{
    class FormatInfo
    {
    public:
        uint32_t BytesPerPixel;
        uint32_t BlockWidth;
        uint32_t BlockHeight;

        FormatInfo(uint32_t bpp, uint32_t blockW, uint32_t blockH);
    };

    class TegraX1Swizzle
    {
    public:
        static std::vector<uint8_t> GetSurface(Fushigi_Bfres::Texture::BntxTexture &texture, int32_t array_level, int32_t mip_level, int32_t target = 1);

        static uint32_t CalculateMipOffset(int32_t level, uint32_t alignment, uint32_t width, uint32_t height, uint32_t blkWidth, uint32_t blkHeight, uint32_t bpp, int32_t blockHeightLog2);

        static uint32_t CalculateSurfaceSize(uint32_t width, uint32_t height, uint32_t blkWidth, uint32_t blkHeight, uint32_t bpp, int32_t blockHeightLog2);
        static int32_t CalculateBlockHeightLog(uint32_t blockHeightLog2, uint32_t width, uint32_t bllWidth);


        static uint32_t GetBytesPerPixel(Fushigi_Bfres::Common::Enum::SurfaceFormat format);
        static uint32_t GetBlockWidth(Fushigi_Bfres::Common::Enum::SurfaceFormat format);
        static uint32_t GetBlockHeight(Fushigi_Bfres::Common::Enum::SurfaceFormat format);


        //static std::map<Fushigi_Bfres::Common::Enum::SurfaceFormat, FormatInfo> FormatList;
        static FormatInfo GetFormat(Fushigi_Bfres::Common::Enum::SurfaceFormat); // FIXME can these be constexpr?

        /*---------------------------------------
         * 
         * Code ported from AboodXD's BNTX Extractor https://github.com/aboood40091/BNTX-Extractor/blob/master/swizzle.py
         * 
         *---------------------------------------*/

        static uint32_t GetBlockHeight(uint32_t height);
        static uint32_t DIV_ROUND_UP(uint32_t n, uint32_t d);
        static uint32_t round_up(uint32_t x, uint32_t y);
        static uint32_t pow2_round_up(uint32_t x);

        static std::vector<uint8_t> deswizzle(uint32_t width, uint32_t height, uint32_t depth, uint32_t blkWidth, uint32_t blkHeight, uint32_t blkDepth, int32_t roundPitch, uint32_t bpp, uint32_t tileMode, int32_t size_range, std::vector<uint8_t> data);
        static std::vector<uint8_t> swizzle(uint32_t width, uint32_t height, uint32_t depth, uint32_t blkWidth, uint32_t blkHeight, uint32_t blkDepth, int32_t roundPitch, uint32_t bpp, uint32_t tileMode, int32_t size_range, std::vector<uint8_t> data);

    private:
        static std::vector<uint8_t> _swizzle(uint32_t width, uint32_t height, uint32_t depth, uint32_t blkWidth, uint32_t blkHeight, uint32_t blkDepth, int32_t roundPitch, uint32_t bpp, uint32_t tileMode, int32_t blockHeightLog2, std::vector<uint8_t> data, int32_t toSwizzle);
        static uint32_t getAddrBlockLinear(uint32_t x, uint32_t y, uint32_t width, uint32_t bytes_per_pixel, uint32_t base_address, uint32_t block_height);
    };
}
