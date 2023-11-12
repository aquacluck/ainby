#include <imgui.h>
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_opengl3_loader.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

#ifdef WIN32
#include <ShellScalingAPI.h>
#include <Windows.h>
#endif

#include "ainby.hpp"

#define AINBY_VERSION "v0.1-beta"

static const char* ImGuiGetClipboardText(void *) { return SDL_GetClipboardText(); }
static void ImGuiSetClipboardText(void *, const char *text) { SDL_SetClipboardText(text); }

int main() {
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL Init Error: %s\n", SDL_GetError());
        return -1;
    }

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    SDL_Window* window = SDL_CreateWindow("ainby | " AINBY_VERSION, 1080, 720, window_flags);
    if (window == NULL) {
        fprintf(stderr, "SDL Create Window Error: %s\n", SDL_GetError());
        return -1;
    }

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable VSync

    // Initialize & configure ImGui

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable;
    io.SetClipboardTextFn = ImGuiSetClipboardText;
    io.GetClipboardTextFn = ImGuiGetClipboardText;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImGuiStyle &style = ImGui::GetStyle();
    const SDL_DisplayMode *dm = SDL_GetDesktopDisplayMode(SDL_GetDisplayForWindow(window));
    float highDPIScaleFactor = 1.0f;
    if (dm != nullptr) {
        highDPIScaleFactor = dm->pixel_density;
    }
    style.ScaleAllSizes(highDPIScaleFactor);
    style.WindowRounding = 6;

    ImFontConfig fontUiConfig;
    fontUiConfig.GlyphOffset.y = highDPIScaleFactor;

    ImFontConfig fontNodeGraphConfig;
    fontNodeGraphConfig.GlyphOffset.y = highDPIScaleFactor;
    fontNodeGraphConfig.OversampleH = 2;
    fontNodeGraphConfig.OversampleV = 2;

    // 16 for main text, 14@1.0f for node editor
    Style::fonts[Style::FONT_UI] = io.Fonts->AddFontFromFileTTF("sarasa-term-j-regular.ttf", 16 * highDPIScaleFactor, &fontUiConfig, io.Fonts->GetGlyphRangesJapanese());
    Style::fonts[Style::FONT_NODE_GRAPH] = io.Fonts->AddFontFromFileTTF("sarasa-term-j-regular.ttf", 14 * highDPIScaleFactor, &fontNodeGraphConfig, io.Fonts->GetGlyphRangesJapanese());
    io.Fonts->Build();

    json fakeWonderConfig = json::parse(R"(
      {
        "appCachePath": "/home/user/ainby/project/smw_eg/cache",
        "romfsPath": "/home/user/ainby/project/smw_eg/romfs",
        "modRomfsPath": "/home/user/ainby/project/smw_eg/output_romfs",

        "titleName": "wonder_100",
        "globalAinbPacks": ["Pack/AIGameCommon.pack.zs"],
        "rootAinbFolders": ["AI"],
        "zsdicPackFile": "",

        "maxDecompressionThreads": 8
      }
    )");

    json fakeTotkConfig = json::parse(R"(
      {
        "appCachePath": "/home/user/ainby/project/zelda_eg/cache",
        "romfsPath": "/home/user/ainby/project/zelda_eg/romfs",
        "modRomfsPath": "/home/user/ainby/project/zelda_eg/output_romfs",

        "titleName": "totk_100",
        "globalAinbPacks": ["Pack/AI.Global.Product.100.pack.zs"],
        "rootAinbFolders": ["AI", "Logic", "Sequence"],
        "zsdicPackFile": "Pack/ZsDic.pack.zs",

        "maxDecompressionThreads": 32
      }
    )");

    // Load paths, prepare zstd, crawl romfs
    // TODO actually read this from somewhere
    ProjectConfig::LoadFromJson(fakeWonderConfig);
    auto pc = ProjectConfig::Get();
    if (pc.zsdicPackFile.size() > 0) {
        ZSTD_ReaderPool::LoadDDictsFromSarc(pc.romfsPath / pc.zsdicPackFile);
    }
    ZSTD_ReaderPool::AllocDCtxPool(pc.maxDecompressionThreads);
    FileIndexCache::Get().CrawlPacks();
    if (ProjectConfig::Get().titleName == TITLE_TOTK_100) {
        //TotkMap2DView::Init();
    }

    // Render loop
    AINBY ainby;
    SDL_Event event;
    bool shouldClose = false;
    while (!shouldClose && !ainby.shouldClose) {
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
                shouldClose = true;
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
                shouldClose = true;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        ainby.Draw();

        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
