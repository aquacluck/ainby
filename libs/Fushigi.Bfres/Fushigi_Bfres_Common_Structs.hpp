#pragma once

#include <cstdint>
#include "Fushigi_Bfres_Common_Enum.hpp"

namespace Fushigi_Bfres::Common::Structs
{
    struct TextureHeader
    {
        uint32_t Magic; //BRTI  
        uint32_t NextBlockOffset;
        uint32_t BlockSize;
        uint32_t Reserved;

        uint8_t Flag;
        Fushigi_Bfres::Common::Enum::Dim Dim;

        Fushigi_Bfres::Common::Enum::TileMode TileMode;
        uint16_t Swizzle;
        uint16_t MipCount;
        uint32_t SampleCount;

        Fushigi_Bfres::Common::Enum::SurfaceFormat Format;
        Fushigi_Bfres::Common::Enum::AccessFlags AccessFlags;
        uint32_t Width;
        uint32_t Height;
        uint32_t Depth;
        uint32_t ArrayCount;

        uint32_t TextureLayout1;
        uint32_t TextureLayout2;

        uint64_t Reserved1;
        uint64_t Reserved2;

        uint32_t ImageSize;
        uint32_t Alignment;

        uint32_t ChannelSwizzle;

        Fushigi_Bfres::Common::Enum::SurfaceDim TextureDim;
        uint8_t Padding0;
        uint16_t Padding2;

        uint64_t NameOffset;
        uint64_t TextureContainerOffset;
        uint64_t ImageDataTableOffset;
        uint64_t UserDataOffset;
        uint64_t RuntimePointer;
        uint64_t RuntimeViewPointer;
        uint64_t DescriptorPointer;
        uint64_t UserDataDictionaryOffset;
    };

    struct BinaryHeader //A header shared between bntx and other formats
    {
        uint64_t Magic; //MAGIC + padding

        uint8_t VersionMicro;
        uint8_t VersionMinor;
        uint16_t VersionMajor;

        uint16_t ByteOrder;
        uint8_t Alignment;
        uint8_t TargetAddressSize;

        uint32_t NameOffset;
        uint16_t Flag;
        uint16_t BlockOffset;

        uint32_t RelocationTableOffset;
        uint32_t FileSize;
    };

    struct BntxHeader
    {
        uint32_t Target; //NX 
        uint32_t TextureCount;
        uint64_t TextureTableOffset;
        uint64_t TextureArrayOffset;
        uint64_t TextureDictionaryOffset;
        uint64_t MemoryPoolOffset;
        uint64_t RuntimePointer;
        uint32_t Padding1;
        uint32_t Padding2;
    };


#ifdef NOPE

    [StructLayout(LayoutKind.Sequential, Size = 0x10)]
    public struct ResHeader
    {
        public ulong NameOffset;

        public ulong ModelOffset;
        public ulong ModelDictionaryOffset;

        public ulong Reserved0;
        public ulong Reserved1;
        public ulong Reserved2;
        public ulong Reserved3;

        public ulong SkeletalAnimOffset;
        public ulong SkeletalAnimDictionaryOffset;
        public ulong MaterialAnimOffset;
        public ulong MaterialAnimDictionarymOffset;
        public ulong BoneVisAnimOffset;
        public ulong BoneVisAnimDictionarymOffset;
        public ulong ShapeAnimOffset;
        public ulong ShapeAnimDictionarymOffset;
        public ulong SceneAnimOffset;
        public ulong SceneAnimDictionarymOffset;

        public ulong MemoryPoolOffset;
        public ulong MemoryPoolInfoOffset;

        public ulong EmbeddedFilesOffset;
        public ulong EmbeddedFilesDictionaryOffset;

        public ulong UserPointer;

        public ulong StringTableOffset;
        public uint StringTableSize;

        public ushort ModelCount;

        public ushort Reserved4;
        public ushort Reserved5;

        public ushort SkeletalAnimCount;
        public ushort MaterialAnimCount;
        public ushort BoneVisAnimCount;
        public ushort ShapeAnimCount;
        public ushort SceneAnimCount;
        public ushort EmbeddedFileCount;

        public byte ExternalFlags;
        public byte Reserved6;
    }

    [StructLayout(LayoutKind.Sequential, Size = 0x10)]
    public struct BufferMemoryPool
    {
        public uint Flag;
        public uint Size;
        public ulong Offset;

        public ulong Reserved1;
        public ulong Reserved2;
    }

    [StructLayout(LayoutKind.Sequential, Size = 0x10)]
    public struct ModelHeader
    {
        public uint Magic;
        public uint Reserved;
        public ulong NameOffset;
        public ulong PathOffset;

        public ulong SkeletonOffset;
        public ulong VertexArrayOffset;
        public ulong ShapeArrayOffset;
        public ulong ShapeDictionaryOffset;
        public ulong MaterialArrayOffset;
        public ulong MaterialDictionaryOffset;
        public ulong ShaderAssignArrayOffset;

        public ulong UserDataArrayOffset;
        public ulong UserDataDictionaryOffset;

        public ulong UserPointer;

        public ushort VertexBufferCount;
        public ushort ShapeCount;
        public ushort MaterialCount;
        public ushort ShaderAssignCount;
        public ushort UserDataCount;

        public ushort Reserved1;
        public uint Reserved2;
    }

    [StructLayout(LayoutKind.Sequential, Size = 0x10)]
    public struct VertexBufferHeader
    {
        public uint Magic;
        public uint Reserved;

        public ulong AttributeArrayOffset;
        public ulong AttributeArrayDictionary;

        public ulong MemoryPoolPointer;

        public ulong RuntimeBufferArray;
        public ulong UserBufferArray;

        public ulong VertexBufferInfoArray;
        public ulong VertexBufferStrideArray;
        public ulong UserPointer;

        public uint BufferOffset;
        public byte VertexAttributeCount;
        public byte VertexBufferCount;

        public ushort Index;
        public uint VertexCount;

        public ushort Reserved1;
        public ushort VertexBufferAlignment;
    }

    [StructLayout(LayoutKind.Sequential, Size = 0x10)]
    public struct ShapeHeader
    {
        public uint Magic;
        public uint Flags;
        public ulong NameOffset;
        public ulong PathOffset;

        public ulong MeshArrayOffset;
        public ulong SkinBoneIndicesOffset;

        public ulong KeyShapeArrayOffset;
        public ulong KeyShapeDictionaryOffset;

        public ulong BoundingBoxOffset;
        public ulong BoundingSphereOffset;

        public ulong UserPointer;

        public ushort Index;
        public ushort MaterialIndex;
        public ushort BoneIndex;
        public ushort VertexBufferIndex;
        public ushort SkinBoneIndex;

        public byte MaxSkinInfluence;
        public byte MeshCount;
        public byte KeyShapeCount;
        public byte KeyAttributeCount;

        public ushort Reserved;
    }

    [StructLayout(LayoutKind.Sequential, Size = 0x10)]
    public struct ShapeRadius
    {
        public float CenterX;
        public float CenterY;
        public float CenterZ;

        public float Radius;
    }


    [StructLayout(LayoutKind.Sequential, Size = 0x10)]
    public struct MeshHeader
    {
        public ulong SubMeshArrayOffset;
        public ulong MemoryPoolOffset;
        public ulong BufferRuntimeOffset;
        public ulong BufferInfoOffset;

        public uint BufferOffset;

        public BfresPrimitiveType PrimType;
        public BfresIndexFormat IndexFormat;
        public uint IndexCount;
        public uint BaseIndex;
        public ushort SubMeshCount;
        public ushort Reserved;
    }


    [StructLayout(LayoutKind.Sequential, Size = 0x10)]
    public struct MaterialHeader
    {
        public uint Magic; //FMAT
        public uint Flags;
        public ulong NameOffset;

        public ulong ShaderInfoOffset;

        public ulong TextureRuntimeDataOffset;
        public ulong TextureNamesOffset;
        public ulong SamplerRuntimeDataOffset;
        public ulong SamplerOffset;
        public ulong SamplerDictionaryOffset;
        public ulong RenderInfoBufferOffset;
        public ulong RenderInfoNumOffset;
        public ulong RenderInfoDataOffsetTable;
        public ulong ParamDataOffset;
        public ulong ParamIndicesOffset;
        public ulong Reserved;
        public ulong UserDataOffset;
        public ulong UserDataDictionaryOffset;
        public ulong VolatileFlagsOffset;
        public ulong UserPointer;
        public ulong SamplerIndicesOffset;
        public ulong TextureIndicesOffset;

        public ushort Index;
        public byte SamplerCount;
        public byte TextureRefCount;
        public ushort Reserved1;
        public ushort UserDataCount;

        public ushort RenderInfoDataSize;
        public ushort Reserved2;
        public uint Reserved3;
    }
#endif
}
