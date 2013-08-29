#ifndef DI8_GAMECONTROLLER_H
#define DI8_GAMECONTROLLER_H

#include <stdexcept>

#include <dxutil/comptr.hh>

#include "directinput.h"
#include "ps3controllerstate.h"

namespace DI8 {

struct GameController {
    GameController(const DirectInput &di8, DeviceInstance device, HWND hwnd)
        : hwnd_{hwnd} {
        IDirectInputDevice8W *temp;
        HRESULT hr;

        if (FAILED(hr = di8.get()->CreateDevice(device.guidInstance, &temp, nullptr))) {
            throw Exception(hr, "Failed to create IDirectInputDevice8.", DI8_ERROR_LOCATION);
        }

        device_ = temp;

        if (FAILED(hr = device_->SetDataFormat(&c_dfDIJoystick2))) {
            throw Exception(hr, "Failed to set data format.", DI8_ERROR_LOCATION);
        }

//        if (FAILED(device_->SetProperty())) {
//            throw std::runtime_error("Failed to set buffer size for game controller.");
//        }

        setCooperativeLevel();
    }

    inline IDirectInputDevice8W *get() const {
        return device_.get();
    }

    void makeExclusive() {
        applyCooperativeLevelFlag(DISCL_NONEXCLUSIVE, false);
        applyCooperativeLevelFlag(DISCL_EXCLUSIVE, true);
        setCooperativeLevel();
    }

    void makeNonExclusive() {
        applyCooperativeLevelFlag(DISCL_EXCLUSIVE, false);
        applyCooperativeLevelFlag(DISCL_NONEXCLUSIVE, true);
        setCooperativeLevel();
    }

    void makeForeground() {
        applyCooperativeLevelFlag(DISCL_BACKGROUND, false);
        applyCooperativeLevelFlag(DISCL_FOREGROUND, true);
        setCooperativeLevel();
    }

    void makeBackground() {
        applyCooperativeLevelFlag(DISCL_FOREGROUND, false);
        applyCooperativeLevelFlag(DISCL_BACKGROUND, true);
        setCooperativeLevel();
    }

    void enableWindowsKey() {
        applyCooperativeLevelFlag(DISCL_NOWINKEY, false);
        setCooperativeLevel();
    }

    void disableWindowsKey() {
        applyCooperativeLevelFlag(DISCL_NOWINKEY, true);
        setCooperativeLevel();
    }

    void acquire() {
        HRESULT hr;
        if (FAILED(hr = device_->Acquire())) {
            throw Exception(hr, DI8_ERROR_LOCATION);
        }
    }

    void unacquire() {
        HRESULT hr;
        if (FAILED(hr = device_->Unacquire())) {
            throw Exception(hr, DI8_ERROR_LOCATION);
        }
    }

    void reacquire() {
        try {
            unacquire();
            acquire();
        } catch (...){}
    }

    template<typename T>
    T state() {
        DIJOYSTATE2 js;
        HRESULT hr;

        if (FAILED(hr = device_->Poll())) {
            throw Exception(hr, "Failed to poll device.", DI8_ERROR_LOCATION);
        }

        if (FAILED(hr = device_->GetDeviceState(sizeof js, &js))) {
            reacquire();
            throw Exception(hr, DI8_ERROR_LOCATION);
        }

        return js;
    }

    inline Ps3ControllerState ps3State() {
        return state<Ps3ControllerState>();
    }

private:
    DxUtil::ComPtr<IDirectInputDevice8W> device_;
    HWND hwnd_;
    DWORD cooperativeLevelFlags_{DISCL_EXCLUSIVE | DISCL_FOREGROUND};

    void applyCooperativeLevelFlag(DWORD flag, bool enabled) {
        if (enabled) {
            cooperativeLevelFlags_ |= flag;
        } else {
            cooperativeLevelFlags_ &= ~flag;
        }
    }

//    DWORD setBufferSize() {
//        DWORD items{INFINITE};
//        HRESULT hr;
//
//        if (FAILED(hr = device_->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), nullptr, &items, DIGDD_PEEK))) {
//            throw std::runtime_error("Failed to get buffer size for game controller.");
//        }
//
//        if ()
//    }

    void setCooperativeLevel() {
        if (FAILED(device_->SetCooperativeLevel(hwnd_, cooperativeLevelFlags_))) {
            throw std::runtime_error("Failed to set cooperative level for game controller.");
        }
    }
};

}

#endif // DI8_GAMECONTROLLER_H
