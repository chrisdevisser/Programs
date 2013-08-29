#ifndef DI8_ERROR_H
#define DI8_ERROR_H

#ifndef DIRECTINPUT_VERSION
    #define DIRECTINPUT_VERSION 0x0800
#endif

#include <exception>
#include <map>
#include <string>
#include <utility>
#include <dinput.h>

#define DI8_ERROR_LOCATION __FILE__, __func__, __LINE__

namespace DI8 {

std::string ErrorCodeToString(LONG);
std::string ErrorCodeToDescription(LONG);

struct Exception : std::exception {
    Exception(LONG c, const std::string i = {})
        : Exception(c, i, {}, {}, {}) {}

    Exception(LONG c, const std::string &f, const std::string &func, int l)
        : Exception(c, {}, f, func, l) {}

    Exception(LONG c, const std::string &i, const std::string &f, const std::string &func, int l)
        : code_{c}, info_{i}, file_{f}, function_{func}, line_{l} {}

    virtual const char *what() const noexcept(true) override {
        return (
            "DI8:\n" + ((code() == -1) ? "" : (ErrorCodeToString(code()) + " - " + ErrorCodeToDescription(code()) + '\n'))
            + (file().empty() ? "" : ("File: " + file() + '\n'))
            + (function().empty() ? "" : ("Function: " + function() + '\n'))
            + (!line() ? "" : ("Line: " + std::to_string(line()) + '\n'))
            + (info().empty() ? "" : ("Extra info: " + info() + '\n'))
        ).c_str();
    }

    inline LONG code() const {
        return code_;
    }

    inline std::string info() const {
        return info_;
    }

    inline std::string file() const {
        return file_;
    }

    inline std::string function() const {
        return function_;
    }

    inline int line() const {
        return line_;
    }

private:
    LONG code_;
    std::string info_;
    std::string file_;
    std::string function_;
    int line_;
};

//NOTE: Missing DI_WRITEPROTECT
static const std::map<LONG, std::pair<std::string, std::string>> Code_StringAndDescription = {
    {DI_BUFFEROVERFLOW, {"DI_BUFFEROVERFLOW", "The device buffer overflowed and some input was lost. This value is equal to the S_FALSE standard COM return value."}},
    {DI_DOWNLOADSKIPPED, {"DI_DOWNLOADSKIPPED", "The parameters of the effect were successfully updated, but the effect could not be downloaded because the associated device was not acquired in exclusive mode."}},
    {DI_EFFECTRESTARTED, {"DI_EFFECTRESTARTED", "The effect was stopped, the parameters were updated, and the effect was restarted."}},
    {DI_NOEFFECT, {"DI_NOEFFECT", "The operation had no effect. This value is equal to the S_FALSE standard COM return value."}},
    {DI_NOTATTACHED, {"DI_NOTATTACHED", "The device exists but is not currently attached to the user's computer. This value is equal to the S_FALSE standard COM return value."}},
    {DI_OK, {"DI_OK", "The operation completed successfully. This value is equal to the S_OK standard COM return value."}},
    {DI_POLLEDDEVICE, {"DI_POLLEDDEVICE", "The device is a polled device. As a result, device buffering does not collect any data and event notifications is not signaled until the IDirectInputDevice8 Interface method is called."}},
    {DI_PROPNOEFFECT, {"DI_PROPNOEFFECT", "The change in device properties had no effect. This value is equal to the S_FALSE standard COM return value."}},
    {DI_SETTINGSNOTSAVED, {"DI_SETTINGSNOTSAVED", "The action map was applied to the device, but the settings could not be saved."}},
    {DI_TRUNCATED, {"DI_TRUNCATED", "The parameters of the effect were successfully updated, but some of them were beyond the capabilities of the device and were truncated to the nearest supported value."}},
    {DI_TRUNCATEDANDRESTARTED, {"DI_TRUNCATEDANDRESTARTED", "Equal to DI_EFFECTRESTARTED | DI_TRUNCATED."}},
    {DIERR_ACQUIRED, {"DIERR_ACQUIRED", "The operation cannot be performed while the device is acquired."}},
    {DIERR_ALREADYINITIALIZED, {"DIERR_ALREADYINITIALIZED", "This object is already initialized"}},
    {DIERR_BADDRIVERVER, {"DIERR_BADDRIVERVER", "The object could not be created due to an incompatible driver version or mismatched or incomplete driver components."}},
    {DIERR_BETADIRECTINPUTVERSION, {"DIERR_BETADIRECTINPUTVERSION", "The application was written for an unsupported prerelease version of DirectInput."}},
    {DIERR_DEVICEFULL, {"DIERR_DEVICEFULL", "The device is full."}},
    {DIERR_DEVICENOTREG, {"DIERR_DEVICENOTREG", "The device or device instance is not registered with DirectInput. This value is equal to the REGDB_E_CLASSNOTREG standard COM return value."}},
    {DIERR_EFFECTPLAYING, {"DIERR_EFFECTPLAYING", "The parameters were updated in memory but were not downloaded to the device because the device does not support updating an effect while it is still playing."}},
    {DIERR_GENERIC, {"DIERR_GENERIC", "An undetermined error occurred inside the DirectInput subsystem. This value is equal to the E_FAIL standard COM return value."}},
    {DIERR_HANDLEEXISTS, {"DIERR_HANDLEEXISTS", "The device already has an event notification associated with it. This value is equal to the E_ACCESSDENIED standard COM return value."}},
    {DIERR_HASEFFECTS, {"DIERR_HASEFFECTS", "The device cannot be reinitialized because effects are attached to it."}},
    {DIERR_INCOMPLETEEFFECT, {"DIERR_INCOMPLETEEFFECT", "The effect could not be downloaded because essential information is missing. For example, no axes have been associated with the effect, or no type-specific information has been supplied."}},
    {DIERR_INPUTLOST, {"DIERR_INPUTLOST", "Access to the input device has been lost. It must be reacquired."}},
    {DIERR_INVALIDPARAM, {"DIERR_INVALIDPARAM", "An invalid parameter was passed to the returning function, or the object was not in a state that permitted the function to be called. This value is equal to the E_INVALIDARG standard COM return value."}},
    {DIERR_MAPFILEFAIL, {"DIERR_MAPFILEFAIL", "An error has occurred either reading the vendor-supplied action-mapping file for the device or reading or writing the user configuration mapping file for the device."}},
    {DIERR_MOREDATA, {"DIERR_MOREDATA", "Not all the requested information fit into the buffer."}},
    {DIERR_NOAGGREGATION, {"DIERR_NOAGGREGATION", "This object does not support aggregation."}},
    {DIERR_NOINTERFACE, {"DIERR_NOINTERFACE", "The object does not support the specified interface. This value is equal to the E_NOINTERFACE standard COM return value."}},
    {DIERR_NOTACQUIRED, {"DIERR_NOTACQUIRED", "The operation cannot be performed unless the device is acquired."}},
    {DIERR_NOTBUFFERED, {"DIERR_NOTBUFFERED", "The device is not buffered. Set the DIPROP_BUFFERSIZE property to enable buffering."}},
    {DIERR_NOTDOWNLOADED, {"DIERR_NOTDOWNLOADED", "The effect is not downloaded."}},
    {DIERR_NOTEXCLUSIVEACQUIRED, {"DIERR_NOTEXCLUSIVEACQUIRED", "The operation cannot be performed unless the device is acquired in DISCL_EXCLUSIVE mode."}},
    {DIERR_NOTFOUND, {"DIERR_NOTFOUND", "The requested object does not exist."}},
    {DIERR_NOTINITIALIZED, {"DIERR_NOTINITIALIZED", "This object has not been initialized."}},
    {DIERR_OBJECTNOTFOUND, {"DIERR_OBJECTNOTFOUND", "The requested object does not exist."}},
    {DIERR_OLDDIRECTINPUTVERSION, {"DIERR_OLDDIRECTINPUTVERSION", "The application requires a newer version of DirectInput."}},
    {DIERR_OTHERAPPHASPRIO, {"DIERR_OTHERAPPHASPRIO", "Another application has a higher priority level, preventing this call from succeeding. This value is equal to the E_ACCESSDENIED standard DirectInput return value. This error can be returned when an application has only foreground access to a device but is attempting to acquire the device while in the background."}},
    {DIERR_OUTOFMEMORY, {"DIERR_OUTOFMEMORY", "The DirectInput subsystem could not allocate sufficient memory to complete the call. This value is equal to the E_OUTOFMEMORY standard COM return value."}},
    {DIERR_READONLY, {"DIERR_READONLY", "The specified property cannot be changed. This value is equal to the E_ACCESSDENIED standard COM return value."}},
    {DIERR_REPORTFULL, {"DIERR_REPORTFULL", "More information was requested to be sent than can be sent to the device."}},
    {DIERR_UNPLUGGED, {"DIERR_UNPLUGGED", "The operation could not be completed because the device is not plugged in."}},
    {DIERR_UNSUPPORTED, {"DIERR_UNSUPPORTED", "The function called is not supported at this time. This value is equal to the E_NOTIMPL standard COM return value."}},
    {E_HANDLE, {"E_HANDLE", "The HWND parameter is not a valid top-level window that belongs to the process."}},
    {E_PENDING, {"E_PENDING", "Data is not yet available."}},
    {E_POINTER, {"E_POINTER", "An invalid pointer, usually NULL, was passed as a parameter."}},
};

std::string ErrorCodeToString(LONG code) {
    return Code_StringAndDescription.at(code).first;
}

std::string ErrorCodeToDescription(LONG code) {
    return Code_StringAndDescription.at(code).second;
}

}

#endif
