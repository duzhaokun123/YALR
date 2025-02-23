// ==WindhawkMod==
// @id              o0kam1-yalr
// @name            YALR
// @description     Yet Another Locale Remulator
// @version         0.1.1
// @author          o0kam1
// @github          https://github.com/duzhaokun123
// @include         *
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
    - codePage: 0
      $name: Code Page
  $name: Target EXE
*/
// ==/WindhawkModSettings==

#include <libloaderapi.h>
#include <minwindef.h>
#include <stringapiset.h>
#include <wincon.h>
#include <windhawk_api.h>
#include <windows.h>
#include <winerror.h>
#include <winnls.h>
#include <winnt.h>
#include <winscard.h>
#include <cstring>
#include <cwchar>

#define FUNC_HOOK(funcName, returnType, params, code)                                               \
using funcName##_t = decltype(&funcName);                                                           \
funcName##_t funcName##_orig;                                                                       \
returnType funcName##_hook params code;                                                             \
boolean initHook_##funcName() {                                                                     \
    return Wh_SetFunctionHook((void*)funcName, (void*)funcName##_hook, (void**)&funcName##_orig);   \
}

#define RETURN_LCID { return settings.lcid; }
#define RETRUN_CODEPAGE { return settings.codePage; }

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

LPCWSTR LPCSTRtoLPCWSTR(LPCSTR cstr) {
    if (cstr == nullptr) {
        return nullptr;
    }
    auto cstrLen = strlen(cstr);
    auto wstrLen = cstrLen;
    auto wstr = new WCHAR[wstrLen + 1];
    auto n = MultiByteToWideChar(CP_ACP, 0, cstr, cstrLen, wstr, wstrLen);
    wstr[n] = L'\0';
    return wstr;
}

LPCSTR LPCWSTRtoLPCSTR(LPCWSTR wstr) {
    if (wstr == nullptr) {
        return nullptr;
    }
    auto wstrLen = wcslen(wstr);
    auto cstrLen = wstrLen * 2;
    auto cstr = new CHAR[cstrLen + 1];
    auto n = WideCharToMultiByte(CP_ACP, 0, wstr, wstrLen, cstr, cstrLen, nullptr, nullptr);
    cstr[n] = '\0';
    return cstr;
}

// begin of generated code

const MSGBOXPARAMSW* MSGBOXPARAMSAtoMSGBOXPARAMSW(const MSGBOXPARAMSA* in) {
    auto out = new MSGBOXPARAMSW;
    out->cbSize = in->cbSize;
    out->hwndOwner = in->hwndOwner;
    out->hInstance = in->hInstance;
    out->lpszText = LPCSTRtoLPCWSTR(in->lpszText);
    out->lpszCaption = LPCSTRtoLPCWSTR(in->lpszCaption);
    out->dwStyle = in->dwStyle;
    out->lpszIcon = LPCSTRtoLPCWSTR(in->lpszIcon);
    out->dwContextHelpId = in->dwContextHelpId;
    out->lpfnMsgBoxCallback = in->lpfnMsgBoxCallback;
    out->dwLanguageId = in->dwLanguageId;
    return out;
}

const WNDCLASSW* WNDCLASSAtoWNDCLASSW(const WNDCLASSA* in) {
    auto out = new WNDCLASSW;
    out->style = in->style;
    out->lpfnWndProc = in->lpfnWndProc;
    out->cbClsExtra = in->cbClsExtra;
    out->cbWndExtra = in->cbWndExtra;
    out->hInstance = in->hInstance;
    out->hIcon = in->hIcon;
    out->hCursor = in->hCursor;
    out->hbrBackground = in->hbrBackground;
    out->lpszMenuName = LPCSTRtoLPCWSTR(in->lpszMenuName);
    out->lpszClassName = LPCSTRtoLPCWSTR(in->lpszClassName);
    return out;
}

const WNDCLASSEXW* WNDCLASSEXAtoWNDCLASSEXW(const WNDCLASSEXA* in) {
    auto out = new WNDCLASSEXW;
    out->cbSize = in->cbSize;
    out->style = in->style;
    out->lpfnWndProc = in->lpfnWndProc;
    out->cbClsExtra = in->cbClsExtra;
    out->cbWndExtra = in->cbWndExtra;
    out->hInstance = in->hInstance;
    out->hIcon = in->hIcon;
    out->hCursor = in->hCursor;
    out->hbrBackground = in->hbrBackground;
    out->lpszMenuName = LPCSTRtoLPCWSTR(in->lpszMenuName);
    out->lpszClassName = LPCSTRtoLPCWSTR(in->lpszClassName);
    out->hIconSm = in->hIconSm;
    return out;
}

// end of generated code

struct {
    LCID lcid;
    UINT codePage;
} settings;

// using WinMain_t = decltype(&WinMain);
// WinMain_t WinMain_Orig;
// int WinMain_Hook(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
//     Wh_Log(L"before WinMain");
//     return WinMain_Orig(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
// }

FUNC_HOOK(GetSystemDefaultLangID, LANGID, (void), RETURN_LCID);

FUNC_HOOK(GetSystemDefaultLCID, LCID, (void), RETURN_LCID);

FUNC_HOOK(GetSystemDefaultUILanguage, LANGID, (void), RETURN_LCID);

FUNC_HOOK(GetSystemDefaultLocaleName, int, (LPWSTR lpLocaleName, int cchLocaleName), {
    return GetLocaleInfoW(settings.lcid, LOCALE_SNAME, lpLocaleName, cchLocaleName);
});

FUNC_HOOK(GetUserDefaultLangID, LANGID, (void), RETURN_LCID);

FUNC_HOOK(GetUserDefaultLCID, LCID, (void), RETURN_LCID);

FUNC_HOOK(GetUserDefaultUILanguage, LANGID, (void), RETURN_LCID);

FUNC_HOOK(GetUserDefaultLocaleName, int, (LPWSTR lpLocaleName, int cchLocaleName), {
    return GetLocaleInfoW(settings.lcid, LOCALE_SNAME, lpLocaleName, cchLocaleName);
});

FUNC_HOOK(GetThreadLocale, LCID, (void), RETURN_LCID);

FUNC_HOOK(GetThreadUILanguage, LCID, (void), RETURN_LCID);

FUNC_HOOK(GetLocaleInfoA, int, (LCID Locale, LCTYPE LCType, LPSTR lpLCData, int cchData), {
    return GetLocaleInfoA_orig(settings.lcid, LCType, lpLCData, cchData);
});

FUNC_HOOK(GetLocaleInfoW, int, (LCID Locale, LCTYPE LCType, LPWSTR lpLCData, int cchData), {
    return GetLocaleInfoW_orig(settings.lcid, LCType, lpLCData, cchData);
});

FUNC_HOOK(GetLocaleInfoEx, int, (LPCWSTR lpLocaleName, LCTYPE LCType, LPWSTR lpLCData, int cchData), {
    auto localeName = new WCHAR[LOCALE_NAME_MAX_LENGTH]();
    GetSystemDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH);
    return GetLocaleInfoEx_orig(localeName, LCType, lpLCData, cchData);
});

FUNC_HOOK(GetACP, UINT, (void), RETRUN_CODEPAGE);

FUNC_HOOK(GetOEMCP, UINT, (void), RETRUN_CODEPAGE);

FUNC_HOOK(WideCharToMultiByte, int, (UINT CodePage, DWORD dwFlags, LPCWCH lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCCH lpDefaultChar, LPBOOL lpUsedDefaultChar), {
    auto newCodePage = CodePage;
    switch (CodePage) {
        case CP_ACP:
        case CP_OEMCP:
        case CP_THREAD_ACP:
            newCodePage = settings.codePage;
    }
    return WideCharToMultiByte_orig(newCodePage, dwFlags, lpWideCharStr, cchWideChar, lpMultiByteStr, cbMultiByte, lpDefaultChar, lpUsedDefaultChar);
});

FUNC_HOOK(MultiByteToWideChar, int, (UINT CodePage, DWORD dwFlags, LPCCH lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar), {
    auto newCodePage = CodePage;
    switch (CodePage) {
        case CP_ACP:
        case CP_OEMCP:
        case CP_THREAD_ACP:
            newCodePage = settings.codePage;
    }
    return MultiByteToWideChar_orig(newCodePage, dwFlags, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar);
});

// begin of generated code

FUNC_HOOK(MessageBoxA, int, (HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType), {
    auto p0 = hWnd;
    auto p1 = LPCSTRtoLPCWSTR(lpText);
    auto p2 = LPCSTRtoLPCWSTR(lpCaption);
    auto p3 = uType;
    auto ret = MessageBoxW(p0, p1, p2, p3);
    return ret;
});

FUNC_HOOK(MessageBoxExA, int, (HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType, WORD wLanguageId), {
    auto p0 = hWnd;
    auto p1 = LPCSTRtoLPCWSTR(lpText);
    auto p2 = LPCSTRtoLPCWSTR(lpCaption);
    auto p3 = uType;
    auto p4 = wLanguageId;
    auto ret = MessageBoxExW(p0, p1, p2, p3, p4);
    return ret;
});


FUNC_HOOK(MessageBoxIndirectA, int, (const MSGBOXPARAMSA* lpmbp), {
    auto p0 = MSGBOXPARAMSAtoMSGBOXPARAMSW(lpmbp);
    auto ret = MessageBoxIndirectW(p0);
    return ret;
});


FUNC_HOOK(RegisterClassA, ATOM, (const WNDCLASSA* lpWndClass), {
    auto p0 = WNDCLASSAtoWNDCLASSW(lpWndClass);
    auto ret = RegisterClassW(p0);
    return ret;
});


FUNC_HOOK(RegisterClassExA, ATOM, (const WNDCLASSEXA* unnamedParam1), {
    auto p0 = WNDCLASSEXAtoWNDCLASSEXW(unnamedParam1);
    auto ret = RegisterClassExW(p0);
    return ret;
});

// end of generated code

void initHooks() {
    if (settings.lcid) {
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

    if (settings.codePage) {
        initHook_GetACP();
        initHook_GetOEMCP();
        initHook_WideCharToMultiByte();
        initHook_MultiByteToWideChar();
    }

// begin of generated code
    initHook_MessageBoxA();
    initHook_MessageBoxExA();
    initHook_MessageBoxIndirectA();
    initHook_RegisterClassA();
    initHook_RegisterClassExA();
// end of generated code
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
        settings.codePage = Wh_GetIntSetting(L"targetExe[%d].codePage", i);
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
