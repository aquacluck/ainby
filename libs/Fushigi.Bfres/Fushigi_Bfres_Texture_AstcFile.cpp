#include "Fushigi_Bfres_Texture_AstcFile.hpp"
//#include "Fushigi_Bfres_Texture_Swizzling_TegraX1Swizzle.hpp"

namespace Fushigi_Bfres::Texture
{

    void AstcFile::SaveAstc(std::string filePath, Fushigi_Bfres::Common::Enum::SurfaceFormat format, uint32_t width, uint32_t height, std::vector<uint8_t> data)
    {
        /* uint8_t blockDimX = (uint8_t)TegraX1Swizzle.GetBlockWidth(format); */
        /* uint8_t blockDimY = (uint8_t)TegraX1Swizzle.GetBlockHeight(format); */

        /* SaveAstc(filePath, blockDimX, blockDimY, width, height, data); */
    }

    void AstcFile::SaveAstc(std::string filePath, uint8_t blockDimX, uint8_t blockDimY, uint32_t width, uint32_t height, std::vector<uint8_t> data)
    {
        /* var mem = new MemoryStream(); */
        /* var writer = mem.AsBinaryWriter(); */

        /* writer.Write(MagicFileConstant); */
        /* writer.Write(blockDimX); */
        /* writer.Write(blockDimY); */
        /* writer.Write((uint8_t)1); */
        /* writer.Write(IntTo3Bytes((int)width)); */
        /* writer.Write(IntTo3Bytes((int)height)); */
        /* writer.Write(IntTo3Bytes((int)1)); */
        /* writer.Write(data); */

        /* File.WriteAllBytes(filePath, mem.ToArray()); */
    }

    std::vector<uint8_t> AstcFile::IntTo3Bytes(int value)
    {
        std::vector<uint8_t> newValue;
        newValue.resize(3);
        newValue[0] = (uint8_t)(value & 0xFF);
        newValue[1] = (uint8_t)((value >> 8) & 0xFF);
        newValue[2] = (uint8_t)((value >> 16) & 0xFF);
        return newValue;
    }
}
