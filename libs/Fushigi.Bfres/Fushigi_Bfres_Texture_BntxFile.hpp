#pragma once

#include <iostream>
#include <string>
#include "Fushigi_Bfres_Texture_BntxTexture.hpp"
#include "Fushigi_Bfres_Common_ResDict.hpp"
#include "Fushigi_Bfres_Common_Structs.hpp"

namespace Fushigi_Bfres::Texture
{
    class BntxFile {
    public:
        Fushigi_Bfres::Common::ResDict<BntxTexture> Textures;

        BntxFile(void);
        BntxFile(std::string filePath);
        BntxFile(std::basic_istream<uint8_t> &stream);

        void Read(std::basic_istream<uint8_t> &stream);

    //private:
        Fushigi_Bfres::Common::Structs::BinaryHeader BinHeader; //A header shared between bntx and other formats
        Fushigi_Bfres::Common::Structs::BntxHeader Header; //Bfres header
    };
}
