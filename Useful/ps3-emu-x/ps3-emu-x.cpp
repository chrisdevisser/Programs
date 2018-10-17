#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <boost/optional.hpp>

#include <windows.h>

#include "directinput/directinput.h"
#include "directinput/error.h"
#include "directinput/gamecontroller.h"
#include "directinput/genericcontrollerstate.h"
#include "directinput/window.h"

HWND Create();
HWND FindGameWindow();
void SendKeyDown(int vk, int sc);
void SendKeyUp(int vk, int sc);
void SetOutput(std::wstring out);
std::wstring GetTitle(HWND hwnd);

#ifdef DOLPHIN
int VkFromState(int state);
int ScFromState(int state);
#else
#ifdef PCSX2
bool PCSX2FrameLimitOn();
void AdjustPCSX2State(int newState);
int GetPCSX2State();
#endif
#endif

std::wstring output;
bool clearStringTimerRunning;
DWORD pid;

#ifdef DOLPHIN
int saveState = 1;
#endif

int main() {
    auto hwnd = Create();

    DI8::DirectInput di;

    boost::optional<DI8::GameController> ctlr;
    std::vector<DI8::DeviceInstance> ctlrs;

    auto resetCtlrs = [&] {
        ctlrs = di.gameControllers();
        if (!ctlrs.empty()) {
            ctlr = {di, ctlrs[0], hwnd};

            ctlr->makeNonExclusive();
            ctlr->makeBackground();
        } else {
            ctlr = boost::none;
        }
    };

    resetCtlrs();

    // struct TestState {
    //     TestState(DIJOYSTATE2 state) : state(state) {}
    //     DIJOYSTATE2 state;
    // };

    // DI8::MessageLoop([&] {
    //     try {
    //         Sleep(100);
    //         ctlr->acquire();
    //         auto state = ctlr->state<TestState>().state;

    //         system("cls");
    //         std::cout << 
    //             "lX: " << state.lX << "\n" <<
    //             "lY: " << state.lY << "\n" <<
    //             "lZ: " << state.lZ << "\n" <<
    //             "lRx: " << state.lRx << "\n" <<
    //             "lRy: " << state.lRy << "\n" <<
    //             "lRz: " << state.lRz << "\n" <<
    //             "rgbButtons: [" << std::hex << std::showbase;
    //         for (int i = 0; i <= 15; ++i) std::cout << std::dec << i << "=" << std::hex << +state.rgbButtons[i] << ",";
    //         std::cout << +state.rgbButtons[16] << "]\n" <<
    //             "rgdwPOV: [" << state.rgdwPOV[0] << "," << state.rgdwPOV[1] << "," << state.rgdwPOV[2] << "," << state.rgdwPOV[3] << "]\n";
    //     } catch (DI8::Exception &ex) {
    //         resetCtlrs();
    //     }
    // });

    DI8::MessageLoop([&] {
        try {
            Sleep(100);

            if (pid) {
                auto proc = OpenProcess(SYNCHRONIZE, FALSE, pid);
                if (WaitForSingleObject(proc, 0) != WAIT_TIMEOUT) std::exit(0);
            }

            #ifdef DOLPHIN
            static bool frameLimitOff;
            #endif

            static bool l3Down;
            static bool l3JustDown;
            static bool leftDown;
            static bool rightDown;
            static bool startDown;
            static bool selectDown;

            static bool switched;

            if (!ctlr) {
                resetCtlrs();
                return;
            }

            ctlr->acquire();

            if (GetForegroundWindow() != FindGameWindow()) return;

            #ifdef PS3
                auto tempState = ctlr->ps3State();
            #elif defined(PS4)
                auto tempState = ctlr->ps4State();
            #elif defined(XBOX)
                auto tempState = ctlr->xboxState();
            #endif

            auto state = DI8::GenericControllerState(tempState);

            if (!state.left()) leftDown = false;
            if (!state.right()) rightDown = false;
            if (switched? !state.select() : !state.start()) startDown = false;
            if (switched ? !state.start() : !state.select()) selectDown = false;

            if (state.L3()) {
                if (l3Down) {
                    l3JustDown = false;
                    return;
                }

                l3Down = true;
                l3JustDown = true;

                if (state.R3() && l3JustDown) {
                    switched = !switched;
                    SetOutput(std::wstring(L"Save and load buttons switched to ") + (switched ? L"reversed" : L"normal"));
                    return;
                }

                #ifdef DOLPHIN
                    if (frameLimitOff) {
                        SendKeyUp(VK_TAB, 0x0f);
                        frameLimitOff = false;

                        SetOutput(L"Frame Limit On");
                    } else {
                        SendKeyDown(VK_TAB, 0x0f);
                        frameLimitOff = true;

                        SetOutput(L"Frame Limit Off");
                    }
                #elif defined(PCSX2)
                    if (!PCSX2FrameLimitOn()) {
                        SendKeyDown(VK_F4, 0x3e);
                        SendKeyUp(VK_F4, 0x3e);

                        SetOutput(L"Frame Limit On");
                    } else {
                        SendKeyDown(VK_F4, 0x3e);
                        SendKeyUp(VK_F4, 0x3e);

                        SetOutput(L"Frame Limit On");
                    }
                #endif
            } else {
                l3Down = false;
                l3JustDown = false;
            }

            if (state.R3()) {
                if (state.left()) {
                    if (leftDown) return;
                    leftDown = true;

                    #ifdef DOLPHIN
                    --saveState;
                    if (saveState < 0) saveState = 9;
                    #else
                    #ifdef PCSX2
                    auto saveState = GetPCSX2State() - 1;
                    if (saveState < 0) saveState = 9;
                    AdjustPCSX2State(saveState);
                    #endif
                    #endif
                    SetOutput(L"Change to state " + std::to_wstring(saveState));
                }

                if (state.right()) {
                    if (rightDown) return;
                    rightDown = true;

                    #ifdef DOLPHIN
                    ++saveState;
                    if (saveState > 9) saveState = 0;
                    #else
                    #ifdef PCSX2
                    auto saveState = GetPCSX2State() + 1;
                    if (saveState > 9) saveState = 0;
                    AdjustPCSX2State(saveState);
                    #endif
                    #endif
                    SetOutput(L"Change to state " + std::to_wstring(saveState));
                }

                if (switched ? state.select() : state.start()) {
                    if (startDown) return;
                    startDown = true;

                    #ifdef DOLPHIN
                    SendKeyDown(VkFromState(saveState), ScFromState(saveState));
                    SendKeyUp(VkFromState(saveState), ScFromState(saveState));
                    #else
                    #ifdef PCSX2
                    SendKeyDown(VK_F3, 0x3d);
                    SendKeyUp(VK_F3, 0x3d);
                    auto saveState = GetPCSX2State();
                    #endif
                    #endif

                    SetOutput(L"Load from state " + std::to_wstring(saveState));
                }

                if (switched ? state.start() : state.select()) {
                    if (selectDown) return;
                    selectDown = true;

                    #ifdef DOLPHIN
                    SendKeyDown(VK_LSHIFT, 0x2a);
                    SendKeyDown(VkFromState(saveState), ScFromState(saveState));
                    SendKeyUp(VkFromState(saveState), ScFromState(saveState));
                    SendKeyUp(VK_LSHIFT, 0x2a);
                    #else
                    #ifdef PCSX2
                    SendKeyDown(VK_F1, 0x3b);
                    SendKeyUp(VK_F1, 0x3b);
                    auto saveState = GetPCSX2State();
                    #endif
                    #endif

                    SetOutput(L"Save to state " + std::to_wstring(saveState));
                }
            }
        } catch (DI8::Exception &ex) {
            resetCtlrs();
        }
    });
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static constexpr UINT_PTR TIMER_CLEARSTRING = 1;
    static constexpr UINT_PTR TIMER_REPAINT = 2;

    static HFONT font;

	switch (msg) {
	    case WM_CREATE: {
	        auto screenDC = GetDC(nullptr);
	        auto height = -MulDiv(12, GetDeviceCaps(screenDC, LOGPIXELSY), 48);
	        ReleaseDC(nullptr, screenDC);

	        font = CreateFont(height, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");

            return 0;
	    }

		case WM_DESTROY: {
			PostQuitMessage(0);
			return 0;
		}

		case WM_PAINT: {
		    PAINTSTRUCT ps;
            auto hdc = BeginPaint(hwnd, &ps);
            auto oldFont = SelectObject(hdc, font);

            SetTextColor(hdc, RGB(255, 255, 255));
            SetBkMode(hdc, TRANSPARENT);

            RECT bounds{0, 0, 50, 500};

            auto gameWindow = FindGameWindow();
            if (gameWindow && GetForegroundWindow() == gameWindow && !output.empty()) {
                RECT client;
                GetClientRect(gameWindow, &client);

                DrawText(hdc, output.c_str(), -1, &bounds, DT_CALCRECT);

                POINT topLeft{client.right - (bounds.right - bounds.left), client.bottom - (bounds.bottom - bounds.top)};
                ClientToScreen(gameWindow, &topLeft);

                bounds.right += topLeft.x;
                bounds.bottom += topLeft.y;
                bounds.left = topLeft.x;
                bounds.top = topLeft.y;

                DrawText(hdc, output.c_str(), -1, &bounds, 0);

                if (!clearStringTimerRunning) {
                    SetTimer(hwnd, TIMER_CLEARSTRING, 3000, nullptr);
                    clearStringTimerRunning = true;
                }
            }

            SetTimer(hwnd, TIMER_REPAINT, 200, nullptr);

            SelectObject(hdc, oldFont);
            EndPaint(hwnd, &ps);
            return 0;
		}

		case WM_TIMER: {
            if (wParam == TIMER_CLEARSTRING) {
                output.clear();
                clearStringTimerRunning = false;
            } else if (wParam == TIMER_REPAINT) {
                InvalidateRect(hwnd, nullptr, TRUE);
            }

            return 0;
		}
	}

	return DefWindowProcW(hwnd, msg, wParam, lParam);
}

HWND Create() {
    WNDCLASSEXW wce{};

    wce.cbSize = sizeof wce;
    wce.lpfnWndProc = WndProc;
    wce.hInstance = GetModuleHandleW(nullptr);
    wce.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    wce.lpszClassName = L"ps3-emu-x";

    if (!RegisterClassExW(&wce)) {
        return nullptr;
    }

    auto hwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT, wce.lpszClassName, L"ps3-emu-x", WS_VISIBLE | WS_POPUP,
        0, 0, GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN),
        nullptr, nullptr, nullptr, nullptr
    );

    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
    return hwnd;
}

BOOL CALLBACK FindGameWindowEnumProc(HWND hwnd, LPARAM gameHwnd) {
    wchar_t className[256];
    GetClassName(hwnd, className, 256);

    auto text = GetTitle(hwnd);

    auto found = [&] {
        *reinterpret_cast<HWND *>(gameHwnd) = hwnd;
        return FALSE;
    };

    #ifdef DOLPHIN
        if (std::wstring(className) != L"Qt5QWindowIcon") return TRUE;

        if (text.find(L"FPS") != std::wstring::npos && text.find(L"VPS") != std::wstring::npos) {
            found();
        }
    #else
    #ifdef PCSX2
        if (std::wstring(className) != L"wxWindowNR") return TRUE;

        if (text.find(L"GSdx") != std::wstring::npos && text.find(L"Limiter") != std::wstring::npos && text.find(L"EE") != std::wstring::npos) {
            found();
        }
    #endif
    #endif

    return TRUE;
}

HWND FindGameWindow() {
    HWND hwnd{};
    EnumWindows(FindGameWindowEnumProc, reinterpret_cast<LPARAM>(&hwnd));

    if (hwnd) GetWindowThreadProcessId(hwnd, &pid);

    return hwnd;
}

void SendKeyDown(int vk, int sc) {
    INPUT in{};
    in.type = INPUT_KEYBOARD;
    in.ki.wVk = vk;
    in.ki.wScan = sc;
    SendInput(1, &in, sizeof in);
}

void SendKeyUp(int vk, int sc) {
    INPUT in{};
    in.type = INPUT_KEYBOARD;
    in.ki.wVk = vk;
    in.ki.wScan = sc;
    in.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &in, sizeof in);
}

void SetOutput(std::wstring out) {
    output = std::move(out);
    clearStringTimerRunning = false;
}

std::wstring GetTitle(HWND hwnd) {
    std::wstring ret(GetWindowTextLength(hwnd) + 1, 0);
    GetWindowText(hwnd, &ret[0], ret.size());
    return ret.c_str();
}

#ifdef DOLPHIN
int VkFromState(int state) {
    return state == 0 ? VK_F10 : VK_F1 - 1 + state;
}

int ScFromState(int state) {
    return state == 0 ? 0x44 : 0x3a + state;
}
#else
#ifdef PCSX2
bool PCSX2FrameLimitOn() {
    if (!FindGameWindow()) return false;
    return GetTitle(FindGameWindow()).find(L"Limiter: None") == std::wstring::npos;
}

void AdjustPCSX2State(int newState) {
    auto state = GetPCSX2State();
    if (state <= newState) {
        for (; state < newState; ++state) {
            SendKeyDown(VK_F2, 0x3c);
            SendKeyUp(VK_F2, 0x3c);
            Sleep(20);
        }
    } else {
        for (; state > newState; --state) {
            SendKeyDown(VK_LSHIFT, 0x2a);
            SendKeyDown(VK_F2, 0x3c);
            SendKeyUp(VK_F2, 0x3c);
            SendKeyUp(VK_LSHIFT, 0x2a);
            Sleep(20);
        }
    }
}

int GetPCSX2State() {
    if (!FindGameWindow()) return 0;
    auto title = GetTitle(FindGameWindow());
    auto pos = title.find(L"Slot ") + 5;
    auto rest = title.substr(pos);

    int state = 0;
    std::wistringstream iss(rest);
    iss >> state;
    return state;
}
#endif
#endif
