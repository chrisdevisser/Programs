#ifndef DI8_WINDOW_H
#define DI8_WINDOW_H

#include <stdexcept>
#include <windows.h>

#include "error.h"

namespace DI8 {

HWND CreateDefaultWindow(WNDPROC f = DefWindowProcW) {
    WNDCLASSW wc;
    ZeroMemory(&wc, sizeof wc);
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpfnWndProc = f;
    wc.lpszClassName = L"DI8 Default Window Class";
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.hCursor = static_cast<HCURSOR>(LoadImageW(nullptr, MAKEINTRESOURCEW(OCR_NORMAL), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED));
    wc.hIcon = static_cast<HICON>(LoadImageW(nullptr, MAKEINTRESOURCEW(OIC_SAMPLE), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED));

    RegisterClassW(&wc);

    HWND hwnd;
    if (!(hwnd = CreateWindowW(L"DI8 Default Window Class", L"Direct Input Window", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, GetModuleHandle(nullptr), nullptr))) {
        throw Exception(-1, "Failed to create window, with error " + std::to_string(GetLastError()) + ".", DI8_ERROR_LOCATION);
    }

    return hwnd;
}

template<typename F>
void MessageLoop(F f) {
    MSG msg;
    while (true) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                return;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        f();
    }
}

}

#endif
