#ifndef DI8_DEVICEINSTANCE_H
#define DI8_DEVICEINSTANCE_H

#ifndef DIRECTINPUT_VERSION
    #define DIRECTINPUT_VERSION 0x0800
#endif

#include <string>
#include <dinput.h>

namespace DI8 {

struct DeviceInstance {
    DeviceInstance(const DIDEVICEINSTANCEW &other)
        : dwSize{other.dwSize}, guidInstance(other.guidInstance), guidProduct(other.guidProduct),
          dwDevType{other.dwDevType}, tszInstanceName{other.tszInstanceName}, tszProductName{other.tszProductName},
          guidFFDriver(other.guidFFDriver), wUsagePage{other.wUsagePage}, wUsage{other.wUsage} {}

    DIDEVICEINSTANCEW get() {
        DIDEVICEINSTANCEW ret;
        ret.dwSize = dwSize;
        ret.guidInstance = guidInstance;
        ret.guidProduct = guidProduct;
        ret.dwDevType = dwDevType;
        std::copy(std::begin(tszInstanceName), std::end(tszInstanceName), ret.tszInstanceName);
        std::copy(std::begin(tszProductName), std::end(tszProductName), ret.tszProductName);
        ret.guidFFDriver = guidFFDriver;
        ret.wUsagePage = wUsagePage;
        ret.wUsage = wUsage;

        return ret;
    }

    DWORD dwSize;
    GUID guidInstance;
    GUID guidProduct;
    DWORD dwDevType;
    std::wstring tszInstanceName;
    std::wstring tszProductName;
    GUID guidFFDriver;
    WORD wUsagePage;
    WORD wUsage;
};

}

#endif // DI8_DEVICEINSTANCE_H
