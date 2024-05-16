#ifndef USBHOTPLUG_H
#define USBHOTPLUG_H
#include <Windows.h>
#include <Dbt.h>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include "strings.h"

#define UUID_CAMERA {0x65E8773DL, 0x8F56, 0x11D0, {0xA3, 0xB9, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96}}
#define UUID_HID    {0x4D1E55B2L, 0xF16F, 0x11CF, {0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30}}
#define UUID_USB    {0xA5DCBF10L, 0x6530, 0x11D2, {0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED}}

class UsbHotplug
{
public:
    using FnNotify = std::function<void(int notifyFlag, unsigned short vid, unsigned short pid)>;
    enum NotifyFlag {
        DEVICE_ATTACHED = 0,
        DEVICE_DETACHED
    };

private:
    FnNotify notify;
    std::map<std::string, HDEVNOTIFY> devNotify;
    HWND hWnd;
private:
    static LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void onDeviceChanged(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    bool createWindow(const GUID &uuid);
    void destroyWindow();
public:
    UsbHotplug();
    ~UsbHotplug();
    void registerDevice(const GUID &uuid)
    {
        bool ret = createWindow(uuid);
        if (!ret) {
            destroyWindow();
        }
    }
    void registerNotify(const FnNotify &func) {notify = func;}

};
#endif // USBHOTPLUG_H
