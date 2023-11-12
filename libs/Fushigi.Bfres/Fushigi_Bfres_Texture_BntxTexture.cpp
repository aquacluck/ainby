#include "Fushigi_Bfres_Texture_BntxTexture.hpp"
#include "Fushigi_Bfres_Texture_Swizzling_TegraX1Swizzle.hpp"
#include <iostream>

namespace Fushigi_Bfres::Texture
{
    void BntxTexture::Read(std::basic_istream<uint8_t> &reader)
    {
        reader.read((uint8_t*)&Header, sizeof(Fushigi_Bfres::Common::Structs::TextureHeader));
        std::cout << Header.Magic << " " << Header.NameOffset << "\n";

        reader.seekg(Header.NameOffset);
        uint16_t nameLen;
        reader.read((uint8_t*)&nameLen, sizeof(nameLen));
        Name.resize(nameLen);
        reader.read((uint8_t*)Name.data(), nameLen);
        std::cout << Name << "\n";

        reader.seekg(Header.ImageDataTableOffset);
        MipOffsets.resize(Header.MipCount);
        if (Header.MipCount > 0) {
            reader.read((uint8_t*)MipOffsets.data(), Header.MipCount*sizeof(uint64_t));
        }
        //std::cout << MipOffsets[0] << "\n";

        ChannelRed = Fushigi_Bfres::Common::Enum::ChannelType((Header.ChannelSwizzle >> 0) & 0xff);
        ChannelGreen = Fushigi_Bfres::Common::Enum::ChannelType((Header.ChannelSwizzle >> 8) & 0xff);
        ChannelBlue = Fushigi_Bfres::Common::Enum::ChannelType((Header.ChannelSwizzle >> 16) & 0xff);
        ChannelAlpha = Fushigi_Bfres::Common::Enum::ChannelType((Header.ChannelSwizzle >> 24) & 0xff);
        BlockHeightLog2 = Header.TextureLayout1 & 7;
        /*
        std::cout << Header.ChannelSwizzle << "\n";
        std::cout << (ChannelRed == Fushigi_Bfres::Common::Enum::ChannelType::Red) << "\n";
        std::cout << (ChannelGreen == Fushigi_Bfres::Common::Enum::ChannelType(3)) << "\n";
        std::cout << (ChannelBlue == Fushigi_Bfres::Common::Enum::ChannelType(4)) << "\n";
        std::cout << (ChannelAlpha == Fushigi_Bfres::Common::Enum::ChannelType(5)) << "\n";
        */

        uint64_t ArrayOffset = 0;
        for (int a = 0; a < Header.ArrayCount; a++)
        {
            reader.seekg(ArrayOffset + MipOffsets[0]);
            auto arrSize = (int)(Header.ImageSize / Header.ArrayCount);
            auto existingTexSize = TextureData.size();
            TextureData.resize(existingTexSize + arrSize);
            reader.read(TextureData.data() + existingTexSize, arrSize);
            ArrayOffset += arrSize;
        }
        //std::cout << TextureData.size() << "\n";
    }

    std::vector<uint8_t> BntxTexture::DeswizzleSurface(int32_t surface_level /*= 0*/, int32_t mip_level /*= 0*/)
    {
        //std::vector<uint8_t> sd; return sd;
        return Fushigi_Bfres::Texture::Swizzling::TegraX1Swizzle::GetSurface(*this, surface_level, mip_level);
    }

    // TODO DecodeAstc
}
