#define _WIN32_WINNT 0x0601

#include <random>

#include <windowsx.h>

#include "window.h"
#include "directinput.h"
#include "gamecontroller.h"

struct Ball {
    int x{};
    int y{};
    int w{10};
    int h{10};
    int speed = 1;
    COLORREF colour{};
} ball;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            HBRUSH brush = CreateSolidBrush(ball.colour);
            HBRUSH old = SelectBrush(hdc, brush);

            SelectObject(hdc, GetStockObject(NULL_PEN));

            Ellipse(hdc, ball.x, ball.y, ball.x + ball.w, ball.y + ball.h);

            SelectObject(hdc, old);
            DeleteObject(brush);
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int main() {
    using namespace DI8;

    HWND hwnd = CreateGameWindow(WndProc);

    DirectInput di8;
    auto ctlrs = di8.gameControllers();

    if (ctlrs.empty()) {
        return 0;
    }

    GameController ps3{di8, ctlrs[0], hwnd};
    ps3.disableWindowsKey();
    ps3.acquire();

    bool xDown{};
    bool cDown{};
    bool sDown{};
    MessageLoop([&] {
        Sleep(1000/30);

        auto state = ps3.ps3State();

        ball.x -= state.leftAnalogLeft() / 100 * ball.speed;
        ball.x += state.leftAnalogRight() / 100 * ball.speed;
        ball.y -= state.leftAnalogUp() / 100 * ball.speed;
        ball.y += state.leftAnalogDown() / 100 * ball.speed;

        ball.w -= state.rightAnalogLeft() / 100;
        ball.w += state.rightAnalogRight() / 100;
        ball.h -= state.rightAnalogUp() / 100;
        ball.h += state.rightAnalogDown() / 100;

        if (cDown && !state.circle()) {
            cDown = false;
        }

        if (!cDown && state.circle()) {
            ++ball.speed;
            cDown = true;
        }

        if (sDown && !state.square()) {
            sDown = false;
        }

        if (!sDown && state.square()) {
            ball.speed = std::max(ball.speed - 1, 1);
            sDown = true;
        }

        if (xDown && !state.cross()) {
            xDown = false;
        }

        if (!xDown && state.cross()) {
            std::mt19937 gen{GetTickCount()};
            std::uniform_int_distribution<> dist{0, 255};
            ball.colour = RGB(dist(gen), dist(gen), dist(gen));
            xDown = true;
        }

        InvalidateRect(hwnd, nullptr, true);
    });
}
