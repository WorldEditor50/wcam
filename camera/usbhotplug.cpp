#include "usbhotplug.h"
#include <iostream>
#include <combaseapi.h>
#pragma comment(lib, "user32.lib")

UsbHotplug::UsbHotplug()
{

}

UsbHotplug::~UsbHotplug()
{
    destroyWindow();
}

LRESULT UsbHotplug::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    UsbHotplug *usbHotplug = reinterpret_cast<UsbHotplug *>(::GetWindowLongPtrW(hWnd, GWLP_USERDATA));
    if (usbHotplug != nullptr) {
        if (message == WM_DEVICECHANGE) {
            usbHotplug->onDeviceChanged(hWnd, message, wParam, lParam);
        }
    }
    return ::DefWindowProcW(hWnd, message, wParam, lParam);
}

void UsbHotplug::onDeviceChanged(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // 设备可用事件
    const bool isAttached = wParam == DBT_DEVICEARRIVAL;
    // 设备移除事件
    const bool isDettached = wParam == DBT_DEVICEREMOVECOMPLETE;
    if (!isAttached && !isDettached) {
        return;
    }
    // 过滤 device interface class 以外类型的消息
    DEV_BROADCAST_HDR *broadcast = reinterpret_cast<DEV_BROADCAST_HDR *>(lParam);
    if (!broadcast || broadcast->dbch_devicetype != DBT_DEVTYP_DEVICEINTERFACE) {
        return;
    }
    // 过滤不监听的设备类型
    DEV_BROADCAST_DEVICEINTERFACE *deviceInterface = reinterpret_cast<DEV_BROADCAST_DEVICEINTERFACE *>(broadcast);
    std::string devType = Strings::guidToString(deviceInterface->dbcc_classguid);
    //std::cout<<"device changed, type="<<devType<<std::endl;
    if (devNotify.find(devType) == devNotify.end()) {
        return;
    }
    std::string deviceName;
    deviceName = Strings::toString(deviceInterface->dbcc_name);
    //std::cout<<"device name = "<<deviceName<<std::endl;
    // 从设备描述中获取 vid 和 pid
    unsigned short vid = 0;
    unsigned short pid = 0;
    if (!Strings::parseVidPid(deviceName, vid, pid)) {
        return;
    }
    if (isAttached) {
        fprintf(stderr, "device attached: vid 0x%04x, pid 0x%04x.\n", vid, pid);
        if (notify) {
            notify(DEVICE_ATTACHED, vid, pid);
        }
    } else if (isDettached) {
        fprintf(stderr, "device detached: vid 0x%04x, pid 0x%04x.\n", vid, pid);
        if (notify) {
            notify(DEVICE_DETACHED, vid, pid);
        }
    }
    return;
}

bool UsbHotplug::createWindow(const GUID &uuid)
{
    HINSTANCE hi = ::GetModuleHandleW(nullptr);
    WNDCLASSW wc;
    memset(&wc, 0, sizeof(WNDCLASSW));
    wc.lpfnWndProc = wndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hi;
    wc.lpszClassName = L"USB_HOTPLUG_WINDOW";
    ::RegisterClassW(&wc);

    hWnd = ::CreateWindowW(wc.lpszClassName, // classname
                           wc.lpszClassName, // window name
                           0,  // style
                           0,  // x
                           0,  // y
                           0,  // width
                           0,  // height
                           0,  // parent
                           0,  // menu handle
                           hi, // application
                           0); // windows creation data.
    if (!hWnd) {
        std::cout<<"createMessageWindow error"<<(int)GetLastError()<<std::endl;
    } else {
        // 初始化 DEV_BROADCAST_DEVICEINTERFACE 数据结构
        DEV_BROADCAST_DEVICEINTERFACE_W filterData;
        memset(&filterData, 0, sizeof(DEV_BROADCAST_DEVICEINTERFACE_W));
        filterData.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE_W);
        filterData.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
        filterData.dbcc_classguid = uuid;
        HDEVNOTIFY handle = ::RegisterDeviceNotificationW(hWnd, &filterData, DEVICE_NOTIFY_WINDOW_HANDLE);
        if (!handle) {
            std::cout<<"RegisterDeviceNotification error"<<std::endl;;
        } else {
            devNotify.insert(std::pair<std::string, HDEVNOTIFY>(Strings::guidToString(uuid), handle));
        }
        ::SetWindowLongPtrW(hWnd, GWLP_USERDATA, (LONG_PTR)this);
    }
    return !!hWnd;
}

void UsbHotplug::destroyWindow()
{
    if (hWnd) {
        ::DestroyWindow(hWnd);
        hWnd = nullptr;
        for (auto &x  : devNotify) {
            ::UnregisterDeviceNotification(x.second);
        }
        devNotify.clear();
    }
    ::UnregisterClassW(L"USB_HOTPLUG_WINDOW", ::GetModuleHandleW(nullptr));
    return;
}

