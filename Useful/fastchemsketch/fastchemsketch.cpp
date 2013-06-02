//Copyright Chris DeVisser 2013
//Code may be used freely in personal and commercial environments.
//Compiled with GCC 4.8.0 and MinGW - nuwen.net/mingw.html Distro 10.0.
//*winuser.h manually modified to make the calling convention of WINEVENTPROC stdcall.

//Chem Sketch is in no way mine. The code makes assumptions about its behaviour.

//Presses the OK button on the dialog that is shown when starting the Freeware version.

#define WINVER 0x0501

#include <string>
#include <utility>
#include <windows.h>

#include <winuser.h>

namespace {
    enum {
        MAX_CLASS = 256,
    };
}

void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD, HWND hwnd, LONG idObject, LONG idChild, DWORD, DWORD) {
    if (!hwnd || idObject != OBJID_WINDOW) {
        return;
    }

    std::wstring className(MAX_CLASS, L'\0');
    std::wstring windowText;

    if (idChild != CHILDID_SELF) {
        return;
    }

    windowText.resize(GetWindowTextLengthW(hwnd) + 1);
    GetWindowTextW(hwnd, &windowText[0], windowText.size());
    windowText = windowText.c_str();

    GetClassNameW(hwnd, &className[0], className.size());
    className = className.c_str();

    if (className != L"TBannerDialog" || windowText != L"ACD/Labs Products") {
        return;
    }

    HWND button = FindWindowExW(hwnd, nullptr, L"TOWLButton", L"OK");
    if (!button) {
        return;
    }

    SendMessageW(button, BM_CLICK, 0, 0);
}

int main() {
    HWINEVENTHOOK hWinEventHook = SetWinEventHook(EVENT_OBJECT_SHOW, EVENT_OBJECT_SHOW, nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (hWinEventHook) {
        UnhookWinEvent(hWinEventHook);
    }
}
