#ifndef DI8_DIRECTINPUT_H
#define DI8_DIRECTINPUT_H

#ifndef DIRECTINPUT_VERSION
    #define DIRECTINPUT_VERSION 0x0800
#endif

#include <memory>
#include <stdexcept>
#include <vector>
#include <dinput.h>

#include "deviceinstance.h"
#include "error.h"

namespace DI8 {

struct DirectInput {
    DirectInput() {
        IDirectInput8W *temp;
        HRESULT hr;
        if (FAILED(hr = DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&temp, nullptr))) {
            throw Exception(hr, "Failed to create IDirectInput8 interface.", DI8_ERROR_LOCATION);
        }

        di8_.reset(temp);
    }

    inline IDirectInput8W *get() const {
        return di8_.get();
    }

    std::vector<DeviceInstance> gameControllers(bool attachedOnly = true) {
        std::vector<DeviceInstance> ret;
        HRESULT hr;
        if (FAILED(hr = di8_->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumDevicesCallback, &ret, attachedOnly ? DIEDFL_ATTACHEDONLY : DIEDFL_ALLDEVICES))) {
            throw Exception(hr, "Failed to enumerate game controllers.", DI8_ERROR_LOCATION);
        }

        return ret;
    }

private:
    std::unique_ptr<IDirectInput8W, void(*)(IDirectInput8W *)> di8_{nullptr, del};

    static BOOL CALLBACK EnumDevicesCallback(LPCDIDEVICEINSTANCEW di, LPVOID tempVec) {
        auto &vec = *static_cast<std::vector<DeviceInstance> *>(tempVec);
        vec.emplace_back(*di);
        return DIENUM_CONTINUE;
    }

    static void del(IDirectInput8W *p) {
        if (p) {
            p->Release();
        }
    }
};

}

#endif
