#define _WIN32_WINNT 0x0601

#include <array>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "directinput.h"
#include "error.h"
#include "gamecontroller.h"
#include "window.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

int main() {
    HWND hwnd = DI8::CreateDefaultWindow(WndProc);
    DI8::DirectInput dinput;

    auto gameControllers = dinput.gameControllers();

    if (gameControllers.empty()) {
        std::wcout << L"No game controllers found.\n";
        return 0;
    }

    DI8::GameController ctlr{dinput, gameControllers[0], hwnd};
    ctlr.makeBackground();

    ctlr.acquire();

    DI8::MessageLoop([&]{
        Sleep(1000/30);

        auto state = ctlr.ps3State();
        if (state.L1() && state.R1() && state.start() && state.select()) {
            std::wcout << L"See ya!\n";
            SendMessage(hwnd, WM_CLOSE, 0, 0);
        }
    });
}
