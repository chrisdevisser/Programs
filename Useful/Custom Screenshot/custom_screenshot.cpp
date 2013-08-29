#include <algorithm>
#include <atomic>
#include <cmath>
#include <functional>
#include <thread>
#include <utility>
#include <windows.h>

#include "w32/handle.h"

void Capture(const POINT p1, const POINT p2) noexcept {
    HDC screen = GetDC(nullptr);
    auto memDC = CreateCompatibleDC(screen);

    if (memDC) {
        if (OpenClipboard(nullptr)) {
            EmptyClipboard();

            auto w = std::abs(p2.x - p1.x);
            auto h = std::abs(p2.y - p1.y);

            auto bm = CreateCompatibleBitmap(screen, w, h);
            auto old = static_cast<HBITMAP>(SelectObject(memDC, bm));

            BitBlt(memDC, 0, 0, w, h, screen, std::min(p1.x, p2.x), std::min(p1.y, p2.y), SRCCOPY);
            SetClipboardData(CF_BITMAP, bm);

            SelectObject(memDC, old);
            DeleteObject(bm);
            CloseClipboard();
        }

        DeleteDC(memDC);
    }

    ReleaseDC(nullptr, screen);
}

void MessageLoop() noexcept {
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

std::atomic_bool MouseHookActive;
std::atomic_bool ResetKeyboardHookState;

extern "C" LRESULT CALLBACK MouseProc(int code, WPARAM wParam, LPARAM lParam) {
    enum class ActOn {
        MouseDown,
        MouseUp
    } static state = ActOn::MouseDown;

    static POINT beginDrag{};

    auto ret = [&] {return CallNextHookEx(nullptr, code, wParam, lParam);};

    if (code < 0) {return ret();}
    if (!MouseHookActive.load()) {state = ActOn::MouseDown; return ret();}

    auto pt = reinterpret_cast<MSLLHOOKSTRUCT *>(lParam)->pt;

    if (wParam == WM_LBUTTONDOWN) {
        if (state == ActOn::MouseDown) {
            beginDrag = pt;
            state = ActOn::MouseUp;
            return 1;
        }
    } else if (wParam == WM_LBUTTONUP) {
        if (state == ActOn::MouseUp) {
            state = ActOn::MouseDown;
            MouseHookActive.store(false);
            ResetKeyboardHookState.store(true);
            Capture(beginDrag, pt);
            return 1;
        }
    }

    return ret();
}

extern "C" LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam) {
    enum class ActOn {
        PrintDown,
        Any,
    } static state = ActOn::PrintDown;

    auto ret = [&] {return CallNextHookEx(nullptr, code, wParam, lParam);};

    if (code < 0) {return ret();}
    if (ResetKeyboardHookState.load()) {state = ActOn::PrintDown; ResetKeyboardHookState.store(false);}

    if (wParam == WM_KEYDOWN) {
        if (state == ActOn::PrintDown) {
            auto vk = reinterpret_cast<KBDLLHOOKSTRUCT *>(lParam)->vkCode;

            if (vk == VK_SNAPSHOT && GetAsyncKeyState(VK_SHIFT) & 0x8000) {
                state = ActOn::Any;
                MouseHookActive.store(true);
                return 1;
            }
        } else if (state == ActOn::Any) {
            state = ActOn::PrintDown;
            MouseHookActive.store(false);
        }
    }

    return ret();
}

int main() {
    std::thread mouse{[] {
        w32::Handle<HHOOK> hook{SetWindowsHookEx(WH_MOUSE_LL, MouseProc, GetModuleHandle(nullptr), 0)};
        MessageLoop();
    }};
    mouse.detach();

    w32::Handle<HHOOK> hook {SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(nullptr), 0)};
    MessageLoop();
}
