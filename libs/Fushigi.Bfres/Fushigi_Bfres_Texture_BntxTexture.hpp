#pragma once

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include "Fushigi_Bfres_Common_Enum.hpp"
#include "Fushigi_Bfres_Common_Structs.hpp"

namespace Fushigi_Bfres::Texture
{
    class BntxTexture
    {
    public:
        std::string Name;
        Fushigi_Bfres::Common::Structs::TextureHeader Header; // XXX

        uint32_t BlockHeightLog2;
        std::vector<uint8_t> TextureData; // or span?
        //List<Memory<byte>> TextureData { get; set; } = new List<Memory<byte>>();
        std::vector<uint64_t> MipOffsets;
        //ulong[] MipOffsets { get; set; } = new ulong[0];

        Fushigi_Bfres::Common::Enum::ChannelType ChannelRed;
        Fushigi_Bfres::Common::Enum::ChannelType ChannelGreen;
        Fushigi_Bfres::Common::Enum::ChannelType ChannelBlue;
        Fushigi_Bfres::Common::Enum::ChannelType ChannelAlpha;

        void Read(std::basic_istream<uint8_t> &reader);
        //void Read(BinaryReader reader);

        std::vector<uint8_t> DeswizzleSurface(int32_t surface_level = 0, int32_t mip_level = 0);
        //byte[] DeswizzleSurface(int surface_level = 0, int mip_level = 0);
        // TODO DecodeAstc, IsAstc, IsBCNCompressed
    };
}
