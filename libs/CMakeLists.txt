# SDL3
set(SDL3_DIR "SDL")
set(SDL_SHARED OFF)
set(SDL_STATIC ON)
add_subdirectory(${SDL3_DIR})

# OpenGL
find_package(OpenGL REQUIRED)

# ImGui
set(IMGUI_DIR "imgui")
add_library(IMGUI STATIC)

target_sources(IMGUI
    PRIVATE
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imgui.cpp

    PRIVATE
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
        ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
)

target_include_directories(IMGUI
    PUBLIC ${IMGUI_DIR}
    PUBLIC ${IMGUI_DIR}/backends
    PUBLIC ${SDL3_DIR}/include
)

add_definitions(-DIMGUI_USER_CONFIG="${PROJECT_SOURCE_DIR}/src/ainby_imgui_config.h")

target_link_libraries(IMGUI PRIVATE SDL3::SDL3-static)

# json
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory("json")

# tinyfiledialogs
set(TINYFILEDIALOGS_DIR "tinyfd")

add_library(TINYFILEDIALOGS STATIC
    ${TINYFILEDIALOGS_DIR}/tinyfiledialogs.c
)

target_include_directories(TINYFILEDIALOGS
    PUBLIC ${TINYFILEDIALOGS_DIR}
)

# zstd
set(ZSTD_DIR "zstd")
add_subdirectory(${ZSTD_DIR}/build/cmake)

# astc-encoder
set(ASTCENC_DIR "astc-encoder")
add_subdirectory(${ASTCENC_DIR})

# fastc
#set(FASTC_DIR "FasTC")
#add_subdirectory(${FASTC_DIR})

# fushigi_bfres
set(FUSHIGI_BFRES_DIR "Fushigi.Bfres")
add_library(FUSHIGI_BFRES STATIC)

target_sources(FUSHIGI_BFRES
    PRIVATE
        ${FUSHIGI_BFRES_DIR}/Fushigi_Bfres_Texture_BntxFile.cpp
        ${FUSHIGI_BFRES_DIR}/Fushigi_Bfres_Texture_BntxTexture.cpp
        ${FUSHIGI_BFRES_DIR}/Fushigi_Bfres_Texture_Swizzling_TegraX1Swizzle.cpp
)

target_include_directories(FUSHIGI_BFRES
    PUBLIC ${FUSHIGI_BFRES_DIR}
)

# smhasher (MurmurHash3)
set(SMHASHER_DIR "smhasher")

add_library(SMHASHER STATIC
    ${SMHASHER_DIR}/src/MurmurHash3.cpp
)

target_include_directories(SMHASHER
    PUBLIC ${SMHASHER_DIR}/src
)
