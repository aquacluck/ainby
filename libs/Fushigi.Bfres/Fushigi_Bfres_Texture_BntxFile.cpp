#include <vector>
#include "Fushigi_Bfres_Texture_BntxFile.hpp"

namespace Fushigi_Bfres::Texture
{
    BntxFile::BntxFile(void) { }

    BntxFile::BntxFile(std::string filePath)
    {
        // FIXME Read(File.OpenRead(filePath));
    }

    BntxFile::BntxFile(std::basic_istream<uint8_t> &stream)
    {
        Read(stream);
    }

    void BntxFile::Read(std::basic_istream<uint8_t> &stream)
    {
        //stream.seekg(0);
        stream.read((uint8_t*)&BinHeader, sizeof(Fushigi_Bfres::Common::Structs::BinaryHeader));
        stream.read((uint8_t*)&Header, sizeof(Fushigi_Bfres::Common::Structs::BntxHeader));
        //std::cout << sizeof(Fushigi_Bfres::Common::Structs::BinaryHeader) << "\n"; // 32
        //std::cout << sizeof(Fushigi_Bfres::Common::Structs::BntxHeader) << "\n"; // 56
        /* std::cout << BinHeader.Magic << "\n"; */
        /* std::cout << BinHeader.VersionMajor << "." << BinHeader.VersionMinor << "." << BinHeader.VersionMicro << "\n"; */
        /* std::cout << Header.TextureTableOffset << "\n"; */
        /* std::cout << Header.TextureCount << "\n"; */

        stream.seekg(Header.TextureTableOffset);
        std::vector<uint64_t> offsets;
        offsets.resize(Header.TextureCount);
        for (auto i=0; i < Header.TextureCount; i++) {
            stream.read((uint8_t*)(offsets.data()+i), sizeof(uint64_t));
        }

        for (auto i = 0; i < Header.TextureCount; i++)
        {
            //std::cout << std::hex << "texture: " << i << " @ " << offsets[i] << "\n";
            //std::cout << std::hex << sizeof(BntxTexture) << "\n";
            stream.seekg(offsets[i]);
            BntxTexture* tex = new BntxTexture();
            tex->Read(stream);
            //std::cout << tex->Name << "\n";
            Textures.resDictMap[tex->Name] = std::move(*tex);
        }
    }
}
