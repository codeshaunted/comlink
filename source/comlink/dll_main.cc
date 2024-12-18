#include <cstdio>
#include <d3d9.h>
#include <format>
#include <iostream>
#include <thread>
#include <vector>
#include <Windows.h>

#include "backends/imgui_impl_dx9.h"
#include "backends/imgui_impl_win32.h"
#include "imgui.h"
#include "MinHook.h"

#include "api.hh"
#include "util.hh"

struct d3d9_dll {
    HMODULE dll;
    FARPROC OrignalD3DPERF_BeginEvent;
    FARPROC OrignalD3DPERF_EndEvent;
    FARPROC OrignalD3DPERF_GetStatus;
    FARPROC OrignalD3DPERF_QueryRepeatFrame;
    FARPROC OrignalD3DPERF_SetMarker;
    FARPROC OrignalD3DPERF_SetOptions;
    FARPROC OrignalD3DPERF_SetRegion;
    FARPROC OrignalDebugSetLevel;
    FARPROC OrignalDebugSetMute;
    FARPROC OrignalDirect3D9EnableMaximizedWindowedModeShim;
    FARPROC OrignalDirect3DCreate9;
    FARPROC OrignalDirect3DCreate9Ex;
    FARPROC OrignalDirect3DShaderValidatorCreate9;
    FARPROC OrignalPSGPError;
    FARPROC OrignalPSGPSampleTexture;
} d3d9;

/*
HMODULE dinput8;
FARPROC dinput8_functions[5];

__declspec(naked) void __stdcall jumpDirectInput8Create() {
    _asm {
        jmp dinput8_functions[0];
    }
}

__declspec(naked) void __stdcall jumpDllCanUnloadNow() {
    _asm {
        jmp dinput8_functions[1];
    }
}

__declspec(naked) void __stdcall jumpDllGetClassObject() {
    _asm {
        jmp dinput8_functions[2];
    }
}

__declspec(naked) void __stdcall jumpDllRegisterServer() {
    _asm {
        jmp dinput8_functions[3];
    }
}

__declspec(naked) void __stdcall jumpDllUnregisterServer() {
    _asm {
        jmp dinput8_functions[4];
    }
}
*/
void initializeConsole() {
    AllocConsole(); // spawns console on main process; todo: make this configurable

    // redirect stdin, stdout and stderr so we control them
    FILE* dummy_file;
    freopen_s(&dummy_file, "CONIN$", "r", stdin);
    freopen_s(&dummy_file, "CONOUT$", "w", stdout);
    freopen_s(&dummy_file, "CONOUT$", "w", stderr);
}

void initializeDLLProxy() {
    d3d9.dll = LoadLibraryA("C:\\Windows\\System32\\d3d9.dll"); // todo: check if this actually loads

    if (d3d9.dll) {
        //TRAVELLER_LOG("Successfully loaded dinput8.dll for call redirects.");
    }
    else {
        //TRAVELLER_LOG_ERROR("Failed to load dinput8.dll for call redirects!");
    }

    // assign addresses of functions
    /*
    dinput8_functions[0] = GetProcAddress(dinput8, "DirectInput8Create");
    dinput8_functions[1] = GetProcAddress(dinput8, "DllCanUnloadNow");
    dinput8_functions[2] = GetProcAddress(dinput8, "DllGetClassObject");
    dinput8_functions[3] = GetProcAddress(dinput8, "DllRegisterServer");
    dinput8_functions[4] = GetProcAddress(dinput8, "DllUnregisterServer");*/

    d3d9.OrignalD3DPERF_BeginEvent = GetProcAddress(d3d9.dll, "D3DPERF_BeginEvent");
    d3d9.OrignalD3DPERF_EndEvent = GetProcAddress(d3d9.dll, "D3DPERF_EndEvent");
    d3d9.OrignalD3DPERF_GetStatus = GetProcAddress(d3d9.dll, "D3DPERF_GetStatus");
    d3d9.OrignalD3DPERF_QueryRepeatFrame = GetProcAddress(d3d9.dll, "D3DPERF_QueryRepeatFrame");
    d3d9.OrignalD3DPERF_SetMarker = GetProcAddress(d3d9.dll, "D3DPERF_SetMarker");
    d3d9.OrignalD3DPERF_SetOptions = GetProcAddress(d3d9.dll, "D3DPERF_SetOptions");
    d3d9.OrignalD3DPERF_SetRegion = GetProcAddress(d3d9.dll, "D3DPERF_SetRegion");
    d3d9.OrignalDebugSetLevel = GetProcAddress(d3d9.dll, "DebugSetLevel");
    d3d9.OrignalDebugSetMute = GetProcAddress(d3d9.dll, "DebugSetMute");
    d3d9.OrignalDirect3D9EnableMaximizedWindowedModeShim = GetProcAddress(d3d9.dll, "Direct3D9EnableMaximizedWindowedModeShim");
    d3d9.OrignalDirect3DCreate9 = GetProcAddress(d3d9.dll, "Direct3DCreate9");
    d3d9.OrignalDirect3DCreate9Ex = GetProcAddress(d3d9.dll, "Direct3DCreate9Ex");
    d3d9.OrignalDirect3DShaderValidatorCreate9 = GetProcAddress(d3d9.dll, "Direct3DShaderValidatorCreate9");
    d3d9.OrignalPSGPError = GetProcAddress(d3d9.dll, "PSGPError");
    d3d9.OrignalPSGPSampleTexture = GetProcAddress(d3d9.dll, "PSGPSampleTexture");
}

void doGUI() {
    ImGui::Text("Hello, World!");
    if (ImGui::Button("Click Me")) {
        // Button logic
    }
}

typedef HRESULT(WINAPI* EndSceneSignature)(IDirect3DDevice9*);
EndSceneSignature original_end_scene = nullptr;
HRESULT WINAPI hookedEndScene(IDirect3DDevice9* device) {
    if (!ImGui::GetCurrentContext()) {
        // initialize imgui context and bindings
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(*comlink::api::_g_hMainhWnd);
        ImGui_ImplDX9_Init(device);
    }

    // start a new imgui frame
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    doGUI();

    // Render the imgui frame
    ImGui::Render();
    device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE); // disable depth testing for ImGui
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);  // restore depth testing

    return original_end_scene(device);
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandlerEx(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, ImGuiIO& io);
WNDPROC original_wnd_proc;
LRESULT CALLBACK hookedWndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam) {
    std::cerr << "asdf" << std::endl;
    if (ImGui::GetCurrentContext()) {
        ImGuiIO& io = ImGui::GetIO();

        switch (umsg) {
        case WM_LBUTTONDOWN:
            io.MouseDown[0] = true;
            break;
        }

        std::cerr << umsg << std::endl;

        if (io.WantCaptureMouse) {
            return true;
        }
    }

    return CallWindowProc(original_wnd_proc, hwnd, umsg, wparam, lparam);
}

typedef void(*UpdateSignature)(void);
UpdateSignature original_update;
void hookedUpdate() {
    if (GetAsyncKeyState(VK_F5)) {
        comlink::api::NewMenuSaga(1, 0);
    }

    /*if (!original_end_scene) {
        // hook WndProc
        original_wnd_proc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(*comlink::API::_g_hMainhWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(hookedWndProc)));

        // todo: error handling
        D3DPRESENT_PARAMETERS params = {};
        params.Windowed = false;
        params.SwapEffect = D3DSWAPEFFECT_DISCARD;
        params.hDeviceWindow = *comlink::API::_g_hMainhWnd;

        IDirect3DDevice9* device = nullptr;
        (*comlink::API::_d3dCore)->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, params.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &params, &device);

        uintptr_t* vtable = *(uintptr_t**)device;

        MH_CreateHook(reinterpret_cast<void*>(vtable[42]), reinterpret_cast<void*>(hookedEndScene), reinterpret_cast<void**>(&original_end_scene));
        MH_EnableHook(reinterpret_cast<void*>(vtable[42]));

        device->Release();

        ShowCursor(true);
    }*/

    return original_update();
}

comlink::api::_DrawMenuEntryExSignature original_DrawMenuEntryEx = nullptr;
void hooked_DrawMenuEntryEx(comlink::api::MENU_s* menu, char* entry, uint8_t alpha) {

    std::cerr << std::hex << menu << "; " << entry << "; " << alpha << std::endl;

    return original_DrawMenuEntryEx(menu, entry, alpha);
}

comlink::api::_UpdateMenuSignature original_UpdateMenu = nullptr;
uint64_t hooked_UpdateMenu(uint32_t param0, uint32_t param1, uint32_t param2, uint32_t param3, uint32_t param4, uint32_t param5, float param6, uint32_t param7, uint32_t param8, uint32_t param9, uint32_t param10) {

    uint64_t ret = original_UpdateMenu(param0, param1, param2, param3, param4, param5, param6, param7, param8, param9, param10);
    char test[] = "test";
    comlink::api::_DrawMenuEntryEx((comlink::api::MENU_s*)0x00921600, test, *comlink::api::_MenuA);

    return ret;
}

comlink::api::MenuDrawNewGameSignature original_MenuDrawNewGame = nullptr;
void hooked_MenuDrawNewGame(comlink::api::MENU_s* menu, uint32_t param1, uint32_t param2) {
    original_MenuDrawNewGame(menu, param1, param2);
    comlink::api::_DrawMenuEntryEx(menu, (char*)"Host Game", *comlink::api::_MenuA);
    comlink::api::_DrawMenuEntryEx(menu, (char*)"Join Game", *comlink::api::_MenuA);
}

comlink::api::_MenuPCVideoSettingsDrawSignature original__MenuPCVideoSettingsDraw = nullptr;
void hooked__MenuPCVideoSettingsDraw(comlink::api::MENU_s* menu) {
    std::string fps_text = std::format("Display FPS: {}", *comlink::api::_FPSDISPLAY ? "On" : "Off");
    comlink::api::_DrawMenuEntryEx(menu, fps_text.data(), *comlink::api::_MenuA);

    std::string fullscreen_text = std::format("Fullscreen: {}", comlink::api::_d3dCore->windowed ? "Off" : "On");
    comlink::api::_DrawMenuEntryEx(menu, fullscreen_text.data(), *comlink::api::_MenuA);

    original__MenuPCVideoSettingsDraw(menu);
}

comlink::api::_MenuPCVideoSettingsUpdateSignature original__MenuPCVideoSettingsUpdate = nullptr;
void hooked__MenuPCVideoSettingsUpdate(comlink::api::MENU_s* menu) {
    // we draw our custom options at the top so we handle them here starting at index 0
    if (menu->entry_selected == 0) {
        if (menu->unk_value0 != 0 || menu->unk_value1 != 0 || menu->unk_value2 != 0) {
            *comlink::api::_MenuSFX = *comlink::api::_MENUSFX_MENUMOVE;
            *comlink::api::_FPSDISPLAY = !*comlink::api::_FPSDISPLAY;
        }
    }
    else if (menu->entry_selected == 1) {
        if (menu->unk_value0 != 0 || menu->unk_value1 != 0 || menu->unk_value2 != 0) {
            *comlink::api::_MenuSFX = *comlink::api::_MENUSFX_MENUMOVE;
            comlink::api::_d3dCore->windowed = !comlink::api::_d3dCore->windowed;
        }
    }
    else {
        // then we decrement the index of the selected entry to ensure that the normal
        // logic isn't disrupted by our fuckery
        menu->entry_selected -= 2;
        original__MenuPCVideoSettingsUpdate(menu);
        menu->entry_selected += 2;
    }
}

comlink::api::BuildDeviceFromResolutionSignature original_BuildDeviceFromResolution = nullptr;
void __fastcall hooked_BuildDeviceFromResolution(comlink::api::CD3DCore* d3d_core) {
    D3DDISPLAYMODE res = comlink::api::_d3dCore->display_mode_table[comlink::api::_d3dCore->new_display_mode_index];
    //std::cerr << res.width << " x " << res.height << std::endl;
    SetWindowPos(*comlink::api::_g_hMainhWnd, NULL, 0, 0, res.Width, res.Height, NULL);
    original_BuildDeviceFromResolution(d3d_core);
}

void initializeHooks() {
    MH_CreateHook(reinterpret_cast<void*>(COMLINK_ADDRESS_UPDATE), reinterpret_cast<void*>(hookedUpdate), reinterpret_cast<void**>(&original_update));
    MH_EnableHook(reinterpret_cast<void*>(COMLINK_ADDRESS_UPDATE));

    MH_CreateHook(reinterpret_cast<void*>(comlink::api::MenuDrawNewGame), reinterpret_cast<void*>(hooked_MenuDrawNewGame), reinterpret_cast<void**>(&original_MenuDrawNewGame));
    MH_EnableHook(reinterpret_cast<void*>(comlink::api::MenuDrawNewGame));

    MH_CreateHook(reinterpret_cast<void*>(comlink::api::_MenuPCVideoSettingsDraw), reinterpret_cast<void*>(hooked__MenuPCVideoSettingsDraw), reinterpret_cast<void**>(&original__MenuPCVideoSettingsDraw));
    MH_EnableHook(reinterpret_cast<void*>(comlink::api::_MenuPCVideoSettingsDraw));

    MH_CreateHook(reinterpret_cast<void*>(comlink::api::_MenuPCVideoSettingsUpdate), reinterpret_cast<void*>(hooked__MenuPCVideoSettingsUpdate), reinterpret_cast<void**>(&original__MenuPCVideoSettingsUpdate));
    MH_EnableHook(reinterpret_cast<void*>(comlink::api::_MenuPCVideoSettingsUpdate));

    MH_CreateHook(reinterpret_cast<void*>(comlink::api::BuildDeviceFromResolution), reinterpret_cast<void*>(hooked_BuildDeviceFromResolution), reinterpret_cast<void**>(&original_BuildDeviceFromResolution));
    MH_EnableHook(reinterpret_cast<void*>(comlink::api::BuildDeviceFromResolution));

    comlink::Util::nopFill(COMLINK_ADDRESS_SET_WINDOW_FOCUSED, 6);
    comlink::Util::nopFill(COMLINK_ADDRESS_LIMIT_REFRESH_RATE, 5);
    //comlink::Util::nopFill(COMLINK_ADDRESS_SET_CURSOR_NULL0, 13);
    //comlink::Util::nopFill(COMLINK_ADDRESS_SET_CURSOR_NULL1, 8);
    //comlink::Util::nopFill(COMLINK_ADDRESS_SET_CURSOR_NULL2, 8);

    // REMOVE AFTER DEVELOPMENT IS DONE
    *comlink::api::_BGLOAD = false;

    // redo display mode table
    //delete comlink::api::_d3dCore->display_mode_table;
    //uint32_t mode_count = comlink::api::_d3dCore->d3d9->GetAdapterModeCount(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8);
    //std::vector<D3DDISPLAYMODE> modes;
    
    //for (size_t i = 0; i < mode_count; ++i) {
        //D3DDISPLAYMODE mode;
        //comlink::api::_d3dCore->d3d9->EnumAdapterModes(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8, i, &mode);

        //if (mode.Width < 800 || mode.Height < 600) continue;

        //modes.push_back(mode);
    //}

    //qsort(modes.data(), modes.size(), sizeof(D3DDISPLAYMODE), comlink::api::SortDisplayModes);

    //comlink::api::_d3dCore->display_mode_table = reinterpret_cast<D3DDISPLAYMODE*>(std::malloc(modes.size() * sizeof(D3DDISPLAYMODE)));
    //comlink::api::_d3dCore->display_mode_count = modes.size();
}

__declspec(naked) void jumpD3DPERF_BeginEvent() { _asm { jmp[d3d9.OrignalD3DPERF_BeginEvent] } }
__declspec(naked) void jumpD3DPERF_EndEvent() { _asm { jmp[d3d9.OrignalD3DPERF_EndEvent] } }
__declspec(naked) void jumpD3DPERF_GetStatus() { _asm { jmp[d3d9.OrignalD3DPERF_GetStatus] } }
__declspec(naked) void jumpD3DPERF_QueryRepeatFrame() { _asm { jmp[d3d9.OrignalD3DPERF_QueryRepeatFrame] } }
__declspec(naked) void jumpD3DPERF_SetMarker() { _asm { jmp[d3d9.OrignalD3DPERF_SetMarker] } }
__declspec(naked) void jumpD3DPERF_SetOptions() { _asm { jmp[d3d9.OrignalD3DPERF_SetOptions] } }
__declspec(naked) void jumpD3DPERF_SetRegion() { _asm { jmp[d3d9.OrignalD3DPERF_SetRegion] } }
__declspec(naked) void jumpDebugSetLevel() { _asm { jmp[d3d9.OrignalDebugSetLevel] } }
__declspec(naked) void jumpDebugSetMute() { _asm { jmp[d3d9.OrignalDebugSetMute] } }
__declspec(naked) void jumpDirect3D9EnableMaximizedWindowedModeShim() { _asm { jmp[d3d9.OrignalDirect3D9EnableMaximizedWindowedModeShim] } }
__declspec(naked) void jumpDirect3DCreate9() {
    std::cerr << "asdf" << std::endl;
    MH_Initialize();
    initializeHooks();
    _asm { jmp[d3d9.OrignalDirect3DCreate9] }
}
__declspec(naked) void jumpDirect3DCreate9Ex() { _asm { jmp[d3d9.OrignalDirect3DCreate9Ex] } }
__declspec(naked) void jumpDirect3DShaderValidatorCreate9() { _asm { jmp[d3d9.OrignalDirect3DShaderValidatorCreate9] } }
__declspec(naked) void jumpPSGPError() { _asm { jmp[d3d9.OrignalPSGPError] } }
__declspec(naked) void jumpPSGPSampleTexture() { _asm { jmp[d3d9.OrignalPSGPSampleTexture] } }

DWORD WINAPI hookThread(LPVOID lpparam) {
    // wait until the TTalesWindow class appears, this ensures that we don't attempt to 
    // hook too early in a SteamStub protected version of the game
    for (;;) {
        if (FindWindowA("TTalesWindow", NULL) == NULL) {
            //std::cerr << (char*)0x0075addc << std::endl;
            continue;
        }
        //if (strcmp((const char*)0x0075addc, "Episode_I.dat")) {
        //    std::cerr << "not loaded!!" << std::endl;
        //    continue;
        //}

        std::cerr << "LOADED!!!" << std::endl;

        MH_Initialize();
        initializeHooks();
        //comlink::api::_d3dCore->Initialize(); // we need to re-initialize this after the patches so we get updated refresh rates
        //comlink::api::Initialize(comlink::api::_d3dCore);
        return 0;
    }
}

BOOL WINAPI DllMain(HMODULE module, DWORD reason, LPVOID load_type) {
    switch (reason) {
        case DLL_PROCESS_ATTACH:{
            initializeConsole();
            initializeDLLProxy();
            //HANDLE thread = CreateThread(NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(hookThread), NULL, NULL, NULL);
            break;
        }
        case DLL_PROCESS_DETACH:
            FreeLibrary(d3d9.dll);
            break;
        default:
            break;
    }

    return TRUE;
}