// ==WindhawkMod==
// @id              o0kam1-yalr
// @name            YALR
// @description     Yet Another Locale Remulator 
// @version         0.1
// @author          o0kam1
// @github          https://github.com/duzhaokun123
// @include         *
// @compilerOptions -lcomdlg32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# YALR
see https://github.com/duzhaokun123/YALR

本 Mod 未在(也不会在) Windhawk Mod 仓库发布
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- targetExe:
  - - target: ""
      $name: Target
      $name:zh-CN: 目标
    - lcid: 0
      $name: LCID
  $name: Target EXE
*/
// ==/WindhawkModSettings==

#include <libloaderapi.h>
#include <minwindef.h>
#include <windhawk_api.h>
#include <windows.h>
#include <winnls.h>
#include <winnt.h>
#include <winscard.h>
#include <cstring>

#define FUNC_HOOK(funcName, returnType, params) \
using funcName##_t = decltype(&funcName); \
funcName##_t funcName##_orig; \
returnType funcName##_hook params; \
boolean initHook_##funcName() { \
    return Wh_SetFunctionHook((void*)funcName, (void*)funcName##_hook, (void**)&funcName##_orig); \
}; \
returnType funcName##_hook params

#define RETURN_LCID { return settings.lcid; }

bool hasEndingW(const WCHAR* fullString, const WCHAR* ending) {
    auto fullStringLength = wcslen(fullString);
    auto endingLength = wcslen(ending);
    if (endingLength > fullStringLength) {
        return false;
    }
    if (wcscmp(fullString + (fullStringLength - endingLength), ending) == 0) {
        return true;
    }
    return false;
}

struct {
    LCID lcid;
} settings;

// using WinMain_t = decltype(&WinMain);
// WinMain_t WinMain_Orig;
// int WinMain_Hook(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
//     Wh_Log(L"before WinMain");
//     return WinMain_Orig(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
// }

FUNC_HOOK(GetSystemDefaultLangID, LANGID, (void)) RETURN_LCID

FUNC_HOOK(GetSystemDefaultLCID, LCID, (void)) RETURN_LCID

FUNC_HOOK(GetSystemDefaultUILanguage, LANGID, (void)) RETURN_LCID

FUNC_HOOK(GetSystemDefaultLocaleName, int, (LPWSTR lpLocaleName, int cchLocaleName)) {
    return GetLocaleInfoW(settings.lcid, LOCALE_SNAME, lpLocaleName, cchLocaleName);
}

FUNC_HOOK(GetUserDefaultLangID, LANGID, (void)) RETURN_LCID

FUNC_HOOK(GetUserDefaultLCID, LCID, (void)) RETURN_LCID

FUNC_HOOK(GetUserDefaultUILanguage, LANGID, (void)) RETURN_LCID

FUNC_HOOK(GetUserDefaultLocaleName, int, (LPWSTR lpLocaleName, int cchLocaleName)) {
    return GetLocaleInfoW(settings.lcid, LOCALE_SNAME, lpLocaleName, cchLocaleName);
}

FUNC_HOOK(GetThreadLocale, LCID, (void)) RETURN_LCID

FUNC_HOOK(GetThreadUILanguage, LCID, (void)) RETURN_LCID

FUNC_HOOK(GetLocaleInfoA, int, (LCID Locale, LCTYPE LCType, LPSTR lpLCData, int cchData)) {
    return GetLocaleInfoA_orig(settings.lcid, LCType, lpLCData, cchData);
}

FUNC_HOOK(GetLocaleInfoW, int, (LCID Locale, LCTYPE LCType, LPWSTR lpLCData, int cchData)) {
    return GetLocaleInfoW_orig(settings.lcid, LCType, lpLCData, cchData);
}

FUNC_HOOK(GetLocaleInfoEx, int, (LPCWSTR lpLocaleName, LCTYPE LCType, LPWSTR lpLCData, int cchData)) {
    auto localeName = new WCHAR[LOCALE_NAME_MAX_LENGTH]();
    GetSystemDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH);
    return GetLocaleInfoEx_orig(localeName, LCType, lpLCData, cchData);
}

void initHooks() {
    initHook_GetSystemDefaultLangID();
    initHook_GetSystemDefaultLCID();
    initHook_GetSystemDefaultUILanguage();
    initHook_GetSystemDefaultLocaleName();
    initHook_GetUserDefaultLangID();
    initHook_GetUserDefaultLCID();
    initHook_GetUserDefaultUILanguage();
    initHook_GetUserDefaultLocaleName();
    initHook_GetThreadLocale();
    initHook_GetThreadUILanguage();
    initHook_GetLocaleInfoA();
    initHook_GetLocaleInfoW();
    initHook_GetLocaleInfoEx();
}

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit() {
    auto exe = new WCHAR[MAX_PATH];
    GetModuleFileName(NULL, exe, MAX_PATH);
    
    for (int i = 0;; i++) {
        auto target = Wh_GetStringSetting(L"targetExe[%d].target", i);
        if (target[0] == '\0') {
            break;
        }
        if (hasEndingW(exe, target) == false) {
            continue;
        }
        Wh_Log(L"load for %s match %s", exe, target);
        
        settings.lcid = Wh_GetIntSetting(L"targetExe[%d].lcid", i);
        initHooks();

        return true;
    }

    return FALSE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
}

// The mod setting were changed, reload them.
void Wh_ModSettingsChanged() {
    // MessageBoxW(nullptr, L"settings changed", L"YALR", MB_OK);
}
