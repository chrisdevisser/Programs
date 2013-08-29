#ifndef DI8_PS3CONTROLLERSTATE_H
#define DI8_PS3CONTROLLERSTATE_H

#ifndef DIRECTINPUT_VERSION
    #define DIRECTINPUT_VERSION 0x0800
#endif

#include <algorithm>
#include <dinput.h>

namespace DI8 {

struct Ps3ControllerState {
    Ps3ControllerState(const DIJOYSTATE2 &js)
        : leftAnalogX_{js.lX}, leftAnalogY_{js.lY}, rightAnalogX_{js.lRz}, rightAnalogY_{js.lZ}, L2R2_{js.lRx},
          start_(js.rgbButtons[9] & 0x80), select_(js.rgbButtons[8] & 0x80), up_(js.rgdwPOV[0] == 0 || js.rgdwPOV[0] == 31500 || js.rgdwPOV[0] == 4500),
          left_(js.rgdwPOV[0] >= 22500 && js.rgdwPOV[0] <= 31500), down_(js.rgdwPOV[0] >= 13500 && js.rgdwPOV[0] <= 22500),
          right_(js.rgdwPOV[0] >= 4500 && js.rgdwPOV[0] <= 13500), triangle_(js.rgbButtons[0] & 0x80), square_(js.rgbButtons[3] & 0x80),
          cross_(js.rgbButtons[2] & 0x80), circle_(js.rgbButtons[1] & 0x80), L1_(js.rgbButtons[6] & 0x80),
          R1_(js.rgbButtons[7] & 0x80), L3_(js.rgbButtons[10] & 0x80), R3_(js.rgbButtons[11] & 0x80), ps3_(js.rgbButtons[12] & 0x80) {}

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

    inline LONG L2() const {
        return std::max(32511 - L2R2_, 0L) * 100 / 32511;
    }

    inline LONG R2() const {
        return std::max(L2R2_ - 32511, 0L) * 100 / 33024;
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

    inline bool triangle() const {
        return triangle_;
    }

    inline bool square() const {
        return square_;
    }

    inline bool cross() const {
        return cross_;
    }

    inline bool circle() const {
        return circle_;
    }

    inline bool L1() const {
        return L1_;
    }

    inline bool R1() const {
        return R1_;
    }

    inline bool L3() const {
        return L3_;
    }

    inline bool R3() const {
        return R3_;
    }

    inline bool ps3() const {
        return ps3_;
    }

private:
    LONG leftAnalogX_;
    LONG leftAnalogY_;
    LONG rightAnalogX_;
    LONG rightAnalogY_;
    LONG L2R2_;
    bool start_;
    bool select_;
    bool up_;
    bool left_;
    bool down_;
    bool right_;
    bool triangle_;
    bool square_;
    bool cross_;
    bool circle_;
    bool L1_;
    bool R1_;
    bool L3_;
    bool R3_;
    bool ps3_;
};

}

#endif // DI8_PS3CONTROLLERSTATE_H
