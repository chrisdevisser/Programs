#ifndef DI8_XBOXCONTROLLERSTATE_H
#define DI8_XBOXCONTROLLERSTATE_H

#ifndef DIRECTINPUT_VERSION
    #define DIRECTINPUT_VERSION 0x0800
#endif

#include <algorithm>
#include <dinput.h>

namespace DI8 {

struct XboxControllerState {
    XboxControllerState(const DIJOYSTATE2 &js)
        : leftAnalogX_{js.lX}, leftAnalogY_{js.lY}, rightAnalogX_{js.lRx}, rightAnalogY_{js.lRy}, LT_(js.lZ == 0xff80), RT_(js.lZ == 0x80),
          start_(js.rgbButtons[7] & 0x80), select_(js.rgbButtons[6] & 0x80), up_(js.rgdwPOV[0] == 0 || js.rgdwPOV[0] == 31500 || js.rgdwPOV[0] == 4500),
          left_(js.rgdwPOV[0] >= 22500 && js.rgdwPOV[0] <= 31500), down_(js.rgdwPOV[0] >= 13500 && js.rgdwPOV[0] <= 22500),
          right_(js.rgdwPOV[0] >= 4500 && js.rgdwPOV[0] <= 13500), Y_(js.rgbButtons[3] & 0x80), X_(js.rgbButtons[2] & 0x80),
          A_(js.rgbButtons[0] & 0x80), B_(js.rgbButtons[1] & 0x80), LB_(js.rgbButtons[4] & 0x80),
          RB_(js.rgbButtons[5] & 0x80), L3_(js.rgbButtons[8] & 0x80), R3_(js.rgbButtons[9] & 0x80) {}

    inline LONG leftAnalogLeft() const {
        return std::max(32511 - leftAnalogX_, 0L) * 100 / 32511;
    }

    inline LONG leftAnalogRight() const {
        return std::max(leftAnalogX_ - 32511, 0L) * 100 / 33024;
    }

    inline LONG leftAnalogUp() const {
        return std::max(32511 - leftAnalogY_, 0L) * 100 / 32511;
    }

    inline LONG leftAnalogDown() const {
        return std::max(leftAnalogY_ - 32511, 0L) * 100 / 33024;
    }

    inline LONG rightAnalogLeft() const {
        return std::max(32511 - rightAnalogX_, 0L) * 100 / 32511;
    }

    inline LONG rightAnalogRight() const {
        return std::max(rightAnalogX_ - 32511, 0L) * 100 / 33024;
    }

    inline LONG rightAnalogUp() const {
        return std::max(32511 - rightAnalogY_, 0L) * 100 / 32511;
    }

    inline LONG rightAnalogDown() const {
        return std::max(rightAnalogY_ - 32511, 0L) * 100 / 33024;
    }

    inline LONG LT() const {
        return LT_;
    }

    inline LONG RT() const {
        return RT_;
    }

    inline bool start() const {
        return start_;
    }

    inline bool select() const {
        return select_;
    }

    inline bool up() const {
        return up_;
    }

    inline bool left() const {
        return left_;
    }

    inline bool down() const {
        return down_;
    }

    inline bool right() const {
        return right_;
    }

    inline bool Y() const {
        return Y_;
    }

    inline bool X() const {
        return X_;
    }

    inline bool A() const {
        return A_;
    }

    inline bool B() const {
        return B_;
    }

    inline bool LB() const {
        return LB_;
    }

    inline bool RB() const {
        return RB_;
    }

    inline bool L3() const {
        return L3_;
    }

    inline bool R3() const {
        return R3_;
    }

private:
    LONG leftAnalogX_;
    LONG leftAnalogY_;
    LONG rightAnalogX_;
    LONG rightAnalogY_;
    bool LT_;
    bool RT_;
    bool start_;
    bool select_;
    bool up_;
    bool left_;
    bool down_;
    bool right_;
    bool Y_;
    bool X_;
    bool A_;
    bool B_;
    bool LB_;
    bool RB_;
    bool L3_;
    bool R3_;
};

}

#endif // DI8_XBOXCONTROLLERSTATE_H
