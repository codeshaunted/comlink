#ifndef COMLINK_API_HH
#define COMLINK_API_HH

namespace comlink {

#define COMLINK_ADDRESS_UPDATE 0x0057d230
#define COMLINK_ADDRESS_SET_WINDOW_FOCUSED 0x006de253
#define COMLINK_ADDRESS_SET_CURSOR_NULL0 0x006dc150
#define COMLINK_ADDRESS_SET_CURSOR_NULL1 0x006d33ea
#define COMLINK_ADDRESS_SET_CURSOR_NULL2 0x006de2d7
#define COMLINK_ADDRESS_LIMIT_REFRESH_RATE 0x006e4379

#define COMLINK_EXPAND(X) X
#define COMLINK_REGISTER_API_FUNCTION(Address, Return, Name, ...) typedef Return (*COMLINK_EXPAND(Name)Signature)(__VA_ARGS__);\
    static inline COMLINK_EXPAND(Name)Signature Name = (COMLINK_EXPAND(Name)Signature)Address
#define COMLINK_REGISTER_API_FUNCTION_WITH_CONVENTION(Address, Return, Convention, Name, ...) typedef Return (COMLINK_EXPAND(Convention)*COMLINK_EXPAND(Name)Signature)(__VA_ARGS__);\
    static inline COMLINK_EXPAND(Name)Signature Name = (COMLINK_EXPAND(Name)Signature)Address
#define COMLINK_REGISTER_API_GLOBAL(Address, Type, Name) static inline Type* Name = (Type*)Address

namespace api {

#pragma pack(1)
struct CD3DCore {
    uint8_t field_0[0x61c];
    D3DDISPLAYMODE* display_mode_table;
    size_t display_mode_index;
    uint8_t field_1[0x38];
    BOOL windowed; // 0x65c
    size_t new_display_mode_index;
    size_t test;

    //virtual void Initialize();
};

#pragma pack(1)
struct MENU_s {
    uint8_t field_0[0x0a];
    uint16_t entry_selected;
    uint8_t field_2[0x50];
    uint32_t unk_value0;
    uint32_t unk_value1;
    uint32_t unk_value2;
};

COMLINK_REGISTER_API_GLOBAL(0x029765c8, CD3DCore, _d3dCore);
COMLINK_REGISTER_API_GLOBAL(0x00924864, HWND, _g_hMainhWnd);
COMLINK_REGISTER_API_GLOBAL(0x007fac04, bool, _BGLOAD);
COMLINK_REGISTER_API_GLOBAL(0x00921440, MENU_s, _GameMenu);
COMLINK_REGISTER_API_GLOBAL(0x00921d60, char, _MenuA);
COMLINK_REGISTER_API_GLOBAL(0x00800944, uint32_t, _GameMenuLevel);
COMLINK_REGISTER_API_GLOBAL(0x00921454, void, unk_GameMenuLevelsTable);
COMLINK_REGISTER_API_GLOBAL(0x00800ab4, void, unk_MenuUpdateFnTable);
COMLINK_REGISTER_API_GLOBAL(0x00800ab0, void, unk_MenuDrawFnTable);
COMLINK_REGISTER_API_GLOBAL(0x00800940, uint32_t, _MenuSFX);
COMLINK_REGISTER_API_GLOBAL(0x00800950, uint32_t, _MENUSFX_MENUMOVE);
COMLINK_REGISTER_API_GLOBAL(0x009250b4, BOOL, _FPSDISPLAY);

COMLINK_REGISTER_API_FUNCTION(0x0050f740, void, _DrawMenuEntryEx, MENU_s* menu, char* entry, uint8_t alpha);
COMLINK_REGISTER_API_FUNCTION(0x00495110, void, MenuDrawNewGame, MENU_s* menu, uint32_t param1, uint32_t param2);
COMLINK_REGISTER_API_FUNCTION(0x00511ea0, void, _MenuPCVideoSettingsDraw, MENU_s* menu);
COMLINK_REGISTER_API_FUNCTION(0x0050ef80, void, _MenuPCVideoSettingsUpdate, MENU_s* menu);
COMLINK_REGISTER_API_FUNCTION(0x00509830, MENU_s*, NewMenuSaga, int menucode, int param1);
COMLINK_REGISTER_API_FUNCTION(0x006e1b10, int, SortDisplayModes, const void* x, const void* y);
COMLINK_REGISTER_API_FUNCTION_WITH_CONVENTION(0x006e3940, void, __fastcall, BuildDeviceFromResolution, CD3DCore* d3d_core); // this is actually part of the CD3Dcore vtable... too
COMLINK_REGISTER_API_FUNCTION_WITH_CONVENTION(0x006e42d0, void, __fastcall, Initialize, CD3DCore* d3d_core); // this is actually part of the CD3Dcore vtable... too

COMLINK_REGISTER_API_FUNCTION(0x00509d00, uint64_t, _UpdateMenu, uint32_t param0, uint32_t param1, uint32_t param2, uint32_t param3, uint32_t param4, uint32_t param5, float param6, uint32_t param7, uint32_t param8, uint32_t param9, uint32_t param10);

}; // namespace api

}; // namespace comlink

#endif // COMLINK_API_HH