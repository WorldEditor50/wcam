#ifndef CAMERA_H
#define CAMERA_H
#include <Windows.h>
#include <rpc.h>
#include <rpcndr.h>
#include <dshow.h>
#include <ObjIdl.h>
#include <strmif.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <functional>
#include "jpegwrap.h"
#include "libyuv.h"
#pragma comment(lib, "strmiids")
#pragma comment(lib, "Ole32")
#pragma comment(lib, "OleAut32")

namespace Camera {

class Manager;
class Device;

using FnProcessImage = std::function<void(int h, int w, int c, unsigned char* data)>;

enum ParamFlag {
    Param_Auto = 0x01,
    Param_Manual = 0x02
};

struct ParamValue {
    long value;
    long step;
    long defaultValue;
    long minValue;
    long maxValue;
    long flag;
};

struct Params {
    ParamValue brightness;
    ParamValue contrast;
    ParamValue hue;
    ParamValue saturation;
    ParamValue sharpness;
    ParamValue gamma;
    ParamValue whiteBalance;
    ParamValue backlightCompensation;
    ParamValue gain;
    ParamValue exposure;
    ParamValue focus;
};

struct ImageFormat {
    int pixelFormat;
    std::string name;
    GUID directshowFormat;
};

enum PixelFormat {
    PixelFormat_ARGB32 = 0,
    PixelFormat_RGB32,
    PixelFormat_RGB24,
    PixelFormat_RGB555,
    PixelFormat_RGB565,
    PixelFormat_MJPG,
    PixelFormat_JPG,
    PixelFormat_AYUV,
    PixelFormat_IYUV,
    PixelFormat_YV12,
    PixelFormat_UYVY,
    PixelFormat_YUYV,
    PixelFormat_YUY2,
    PixelFormat_NV12,
    PixelFormat_IMC1,
    PixelFormat_IMC2,
    PixelFormat_IMC3,
    PixelFormat_IMC4,
    PixelFormat_END
};

static const ImageFormat g_imageFormatTable[] = {
    {PixelFormat_ARGB32, "ARGB32", MEDIASUBTYPE_ARGB32},
    {PixelFormat_RGB32,  "RGB32",  MEDIASUBTYPE_RGB32},
    {PixelFormat_RGB24,  "RGB24",  MEDIASUBTYPE_RGB24},
    {PixelFormat_RGB555, "RGB555", MEDIASUBTYPE_RGB555},
    {PixelFormat_RGB565, "RGB565", MEDIASUBTYPE_RGB565},
    {PixelFormat_MJPG,   "MJPG",   MEDIASUBTYPE_MJPG},
    {PixelFormat_JPG,    "JPG",    MEDIASUBTYPE_IJPG},
    {PixelFormat_AYUV,   "AYUV",   MEDIASUBTYPE_AYUV},
    {PixelFormat_IYUV,   "IYUV",   MEDIASUBTYPE_IYUV},
    {PixelFormat_YV12,   "YV12",   MEDIASUBTYPE_YV12},
    {PixelFormat_UYVY,   "UYVY",   MEDIASUBTYPE_UYVY},
    {PixelFormat_YUYV,   "YUYV",   MEDIASUBTYPE_YUYV},
    {PixelFormat_YUY2,   "YUY2",   MEDIASUBTYPE_YUY2},
    {PixelFormat_NV12,   "NV12",   MEDIASUBTYPE_NV12},
    {PixelFormat_IMC1,   "IMC1",   MEDIASUBTYPE_IMC1},
    {PixelFormat_IMC2,   "IMC2",   MEDIASUBTYPE_IMC2},
    {PixelFormat_IMC3,   "IMC3",   MEDIASUBTYPE_IMC3},
    {PixelFormat_IMC4,   "IMC4",   MEDIASUBTYPE_IMC4}
};

namespace detail {

template <typename T>
inline void objectRelease(T **ppObj)
{
    if (*ppObj) {
        (*ppObj)->Release();
        *ppObj = nullptr;
    }
    return;
}

inline int parsePixelFormat(const GUID &guid)
{
    int pixelFormat = 0;
    for (int i = 0; i < PixelFormat_END; i++) {
        if (g_imageFormatTable[i].directshowFormat == guid) {
            pixelFormat = i;
            break;
        }
    }
    return pixelFormat;
}

}
// {C1F400A0-3F08-11D3-9F0B-006008039E37}
DEFINE_GUID(CLSID_SampleGrabber,
0xC1F400A0, 0x3F08, 0x11D3, 0x9F, 0x0B, 0x00, 0x60, 0x08, 0x03, 0x9E, 0x37); //qedit.dll

EXTERN_C const CLSID CLSID_NullRenderer;

// {0579154A-2B53-4994-B0D0-E773148EFF85}
DEFINE_GUID(IID_ISampleGrabberCB,
0x0579154A, 0x2B53, 0x4994, 0xB0, 0xD0, 0xE7, 0x73, 0x14, 0x8E, 0xFF, 0x85);

MIDL_INTERFACE("0579154A-2B53-4994-B0D0-E773148EFF85")
ISampleGrabberCB : public IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE SampleCB(
        double SampleTime,
        IMediaSample *pSample) = 0;

    virtual HRESULT STDMETHODCALLTYPE BufferCB(
        double SampleTime,
        BYTE *pBuffer,
        long BufferLen) = 0;

};

// {6B652FFF-11FE-4fce-92AD-0266B5D7C78F}
DEFINE_GUID(IID_ISampleGrabber,
0x6B652FFF, 0x11FE, 0x4fce, 0x92, 0xAD, 0x02, 0x66, 0xB5, 0xD7, 0xC7, 0x8F);

MIDL_INTERFACE("6B652FFF-11FE-4fce-92AD-0266B5D7C78F")
ISampleGrabber : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE SetOneShot(BOOL OneShot) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetMediaType(const AM_MEDIA_TYPE *pType) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetConnectedMediaType(AM_MEDIA_TYPE *pType) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetBufferSamples(BOOL BufferThem) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetCurrentBuffer(/* [out][in] */ long *pBufferSize,
                                                       /* [out] */ long *pBuffer) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentSample(/* [retval][out] */ IMediaSample **ppSample) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetCallback(ISampleGrabberCB *pCallback,
                                                  long WhichMethodToCallback) = 0;
};

class Buffer
{
public:
    unsigned char* data;
    std::size_t dataSize;
    std::size_t capacity;
public:
    Buffer():data(nullptr),dataSize(0), capacity(0){}
    ~Buffer()
    {
        if (data != nullptr) {
            delete [] data;
            data = nullptr;
        }
    }
    static std::size_t align(std::size_t s)
    {
        std::size_t size = s;
        if (size&0x3ff) {
            size = ((size >> 10) + 1)<<10;
        }
        return size;
    }

    void allocate(std::size_t size)
    {
        if (data == nullptr) {
            capacity = align(size);
            data = new unsigned char[capacity];
        } else {
            if (capacity < size) {
                delete [] data;
                capacity = align(size);
                data = new unsigned char[capacity];
            }
        }
        dataSize = size;
        return;
    }

};

class SampleGrabberCB : public ISampleGrabberCB
{
public:
    enum State {
        STATE_NONE = 0,
        STATE_PREPEND,
        STATE_FRAME_READY,
        STATE_PROCESS,
        STATE_TERMINATE
    };
    static constexpr int max_image_block = 4;
private:
    int width;
    int height;
    int pixelFormat;
    int state;
    int index;
    Buffer rawBuffer;
    Buffer imageBlocks[max_image_block];
    std::condition_variable condit;
    std::mutex mutex;
    std::thread processThread;
    FnProcessImage processImage;
protected:
    void run()
    {
        std::cout<<"enter process image"<<std::endl;
        while (1) {
            std::unique_lock<std::mutex> locker(mutex);
            condit.wait_for(locker, std::chrono::milliseconds(1000), [this]()->bool{
                                    return state == STATE_TERMINATE || state == STATE_FRAME_READY;
                                });
            if (state == STATE_TERMINATE) {
                state = STATE_NONE;
                break;
            } else if (state == STATE_PREPEND) {
                continue;
            }
            /* allocate memory for image */
            unsigned long len = width * height * 4;
            if (pixelFormat == PixelFormat_MJPG) {
                len = Jpeg::align4(width, 3)*height;
            } else if (pixelFormat == PixelFormat_YUY2) {
                len = width * height * 4;
            }
            Buffer& image = imageBlocks[index];
            index = (index + 1)%max_image_block;
            image.allocate(len);
            /* decode */
            if (pixelFormat == PixelFormat_MJPG) {
                Jpeg::decode(image.data, width, height, rawBuffer.data, rawBuffer.dataSize, Jpeg::ALIGN_4);
                /* process */
                processImage(height, width, 3, image.data);
            } else if(pixelFormat == PixelFormat_YUY2) {
                int alignedWidth = (width + 1) & ~1;
                libyuv::YUY2ToARGB(rawBuffer.data, alignedWidth * 2,
                        image.data, width * 4,
                        width, height);
                processImage(height, width, 4, image.data);
            } else {
                std::cout<<"unresolve format:"<<pixelFormat<<std::endl;
            }
            /* get next image */
            if (state != STATE_TERMINATE) {
                state = STATE_PREPEND;
            }
        }
        std::cout<<"leave process image"<<std::endl;
        return;
    }
public:
    SampleGrabberCB():width(0), height(0), pixelFormat(0), state(STATE_NONE), index(0){}
    virtual ~SampleGrabberCB(){}

    virtual HRESULT STDMETHODCALLTYPE SampleCB(double time, IMediaSample* sample)
    {
        return S_OK;
    }
    virtual HRESULT STDMETHODCALLTYPE BufferCB(double time, BYTE* buffer, long len)
    {
        if (!buffer || len <= 0) {
            return S_OK;
        }

        if (state == STATE_PREPEND) {
            std::unique_lock<std::mutex> locker(mutex);
            rawBuffer.allocate(len);
            memcpy(rawBuffer.data, buffer, len);
            rawBuffer.dataSize = len;
            state = STATE_FRAME_READY;
            condit.notify_all();
        }
        return S_OK;
    }
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID *ppv)
    {
        if(iid == IID_ISampleGrabberCB || iid == IID_IUnknown) {
            *ppv = reinterpret_cast<LPVOID*>(this);
            return S_OK;
        }
        return E_NOINTERFACE;
    }
    virtual ULONG STDMETHODCALLTYPE AddRef() { return 1;}
    virtual ULONG STDMETHODCALLTYPE Release() { return 1;}

    void registerProcess(const FnProcessImage &func) { processImage = func; }

    void setProperty(int w, int h, int format)
    {
        std::unique_lock<std::mutex> locker(mutex);
        width = w;
        height = h;
        pixelFormat = format;
        return;
    }

    void start()
    {
        if (state != STATE_NONE) {
            return;
        }
        state = STATE_PREPEND;
        processThread = std::thread(&SampleGrabberCB::run, this);
        return;
    }

    void stop()
    {
        if (state != STATE_NONE) {
            while (state != STATE_NONE) {
                std::unique_lock<std::mutex> locker(mutex);
                state = STATE_TERMINATE;
                condit.notify_all();
                condit.wait_for(locker, std::chrono::milliseconds(500), [=]()->bool{
                    return state == STATE_NONE;
                });
            }
            processThread.join();
        }
        return;
    }
};

struct Property {
    int id;
    std::wstring vendorID;
    std::wstring productID;
    std::wstring friendlyName;
    std::wstring filterName;
};

class Device : public Property
{
public:
    class Format
    {
    public:
        LONG width;
        LONG height;
        LONG isFlippedHorizontal;
        LONG isFlippedVertical;
        GUID pixelFormat;
        AM_MEDIA_TYPE mediaType;
    public:
        Format(): mediaType(), width(0), height(0), pixelFormat(),
            isFlippedHorizontal(false), isFlippedVertical(false) {}
    };

    friend class Manager;
    using Ptr = std::shared_ptr<Device>;
public:
    bool isActiveValue;
    IMoniker* moniker;
    IBaseFilter* sourceFilter;
    IBaseFilter* sampleGrabberFilter;
    IBaseFilter* nullRenderer;
    ISampleGrabber* sampleGrabber;
    SampleGrabberCB* sampleGrabberCB;
    IFilterGraph2* filterGraph;
    IAMStreamConfig* streamConfig;
    Device::Format currentFormat;
    std::vector<Device::Format> formatList;
public:
    Device():isActiveValue(false), moniker(nullptr), sourceFilter(nullptr),
        sampleGrabberFilter(nullptr), nullRenderer(nullptr),
        sampleGrabber(nullptr), sampleGrabberCB(new SampleGrabberCB),
        filterGraph(nullptr), streamConfig(nullptr){}

    ~Device()
    {

        detail::objectRelease(&moniker);
        detail::objectRelease(&sourceFilter);
        detail::objectRelease(&sampleGrabberFilter);
        detail::objectRelease(&sampleGrabber);
        detail::objectRelease(&nullRenderer);
        detail::objectRelease(&streamConfig);
        if (sampleGrabberCB) {
            delete sampleGrabberCB;
            sampleGrabberCB = nullptr;
        }
    }

    int getId() const {return id;}

    std::wstring getFriendlyName() const {return friendlyName;}

    const std::vector<Device::Format> &getFormatList() const { return formatList;}

    const Device::Format &getCurrentFormat() const {return currentFormat;}

    bool setCurrentFormat(const Device::Format& format)
    {
        currentFormat = format;

        AM_MEDIA_TYPE mt = format.mediaType;
        HRESULT hr = streamConfig->SetFormat(&mt);
        if (hr < 0) {
            return false;
        }
        return true;
    }

    HRESULT registerSampleCB(const GUID &pixelFormat, const FnProcessImage &callbak)
    {
        HRESULT hr = S_FALSE;
        // set the media type
        AM_MEDIA_TYPE mt;
        memset(&mt, 0, sizeof(AM_MEDIA_TYPE));
        mt.majortype = MEDIATYPE_Video;
        mt.subtype = pixelFormat;
        hr = sampleGrabber->SetMediaType(&mt);
        if (hr != S_OK) {
            return hr;
        }
        sampleGrabberCB->registerProcess(callbak);
        // add the callback to the samplegrabber, 0-->SampleCB, 1-->BufferCB
        return sampleGrabber->SetCallback(sampleGrabberCB, 1);
    }

    bool isActive() const {return isActiveValue; }

    bool start()
    {
        if (formatList.empty()) {
            return false;
        }
        sampleGrabberCB->setProperty(currentFormat.width,
                                     currentFormat.height,
                                     detail::parsePixelFormat(currentFormat.pixelFormat));
        sampleGrabberCB->start();

        HRESULT hr = S_FALSE;
        if (nullRenderer) {
            hr = nullRenderer->Run(0);
            if (hr < 0) {
                return false;
            }
        }

        if (sampleGrabberFilter) {
            hr = sampleGrabberFilter->Run(0);
            if (hr < 0) {
                return false;
            }
        }

        if (sourceFilter) {
            hr = sourceFilter->Run(0);
            if (hr < 0) {
                return false;
            }
        }
        isActiveValue = true;
        return true;
    }

    bool stop()
    {
        isActiveValue = false;
        HRESULT hr = S_FALSE;
        if (sampleGrabberCB) {
            sampleGrabberCB->stop();
        }

        if (sourceFilter) {
            hr = sourceFilter->Stop();
            if (hr < 0) {
                return false;
            }
        }

        if (sampleGrabberFilter) {
            hr = sampleGrabberFilter->Stop();
            if (hr < 0) {
                return false;
            }
        }

        if (nullRenderer) {
            hr = nullRenderer->Stop();
            if (hr < 0) {
                return false;
            }
        }
        return true;
    }

};

using DeviceList = std::vector<Device::Ptr>;

inline std::wstring createUniqueName(const std::wstring& name, int id)
{
    std::stringstream stream;
    stream << "id" << id;
    std::string uniquePostfix = stream.str();
    std::wstring newName = name + std::wstring(uniquePostfix.begin(), uniquePostfix.end());
    return newName;
}

inline int enumerate(const std::wstring &vendorID, DeviceList &devList)
{
    HRESULT hr = S_FALSE;
    VARIANT name;
    std::wstring filterName;
    ICreateDevEnum* devEnum = nullptr;
    IEnumMoniker* enumMoniker = nullptr;
    IMoniker* moniker = nullptr;
    IPropertyBag* pbag = nullptr;
    // create an enumerator for video input devices
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, nullptr,
                          CLSCTX_INPROC_SERVER, IID_ICreateDevEnum,
                          reinterpret_cast<void**>(&devEnum));
    if (hr < 0 || !devEnum) {
        return -1;
    }
    hr = devEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,
                                        &enumMoniker, NULL);
    if (hr < 0 || !enumMoniker) {
        return -1;
    }
    int devNum = 0;
    while (enumMoniker->Next(1, &moniker, 0) == S_OK) {
        ++devNum;
        hr = moniker->BindToStorage(nullptr, nullptr, IID_IPropertyBag,
                                    reinterpret_cast<void**>(&pbag));
        if (hr < 0) {
            moniker->Release();
            continue;
        }
        VariantInit(&name);
        hr = pbag->Read(L"Description", &name, 0);
        if (hr < 0) {
            hr = pbag->Read(L"FriendlyName", &name, 0);
            if (hr < 0) {
                VariantClear(&name);
                pbag->Release();
                moniker->Release();
                continue;
            }
        }
        WCHAR* wDisplayName = nullptr;
        hr = moniker->GetDisplayName(nullptr, nullptr, &wDisplayName);
        if (hr < 0) {
            VariantClear(&name);
            pbag->Release();
            moniker->Release();
            continue;
        }
        std::wcout<<L"Found Device:"<<wDisplayName<<std::endl;
        std::wstring displayName(wDisplayName);
        int pos = displayName.find(L"vid_");
        std::wstring vid = displayName.substr(pos + 4, 4);
        pos = displayName.find(L"pid_");
        std::wstring pid = displayName.substr(pos + 4, 4);
        std::wcout<<L"vid:"<<vid<<L", pid:"<<pid<<std::endl;
        free(wDisplayName);
        if (hr < 0) {
            VariantClear(&name);
            pbag->Release();
            moniker->Release();
            continue;
        }

        if (!vendorID.empty() && vendorID != vid) {
            VariantClear(&name);
            pbag->Release();
            moniker->Release();
            continue;
        }

        Device::Ptr dev(new Device);
        dev->id = devNum;
        std::wstring wname(name.bstrVal, SysStringLen(name.bstrVal));
        dev->friendlyName = wname;
        dev->filterName = createUniqueName(wname, devNum);
        dev->vendorID = vid;
        dev->productID = pid;
        dev->moniker = moniker;
        devList.push_back(dev);

        VariantClear(&name);
        pbag->Release();
    }
    enumMoniker->Release();
    devEnum->Release();
    return devNum;
}

class Manager
{
private:
    IFilterGraph2* filterGraph;
    ICaptureGraphBuilder2* captureGraph;
    IMediaControl* mediaControl;
    int m_activeDeviceNum;
    DeviceList devList;
private:

    static bool initializeGraph(IFilterGraph2* &filterGraph,
                                ICaptureGraphBuilder2* &captureGraph,
                                IMediaControl* &mediaControl)
    {
        HRESULT hr = S_FALSE;
        // create the FilterGraph
        hr = CoCreateInstance(CLSID_FilterGraph, nullptr,
                              CLSCTX_INPROC_SERVER, IID_IFilterGraph2,
                              reinterpret_cast<void**>(&filterGraph));
        if (hr < 0 || !filterGraph) {
            return false;
        }

        // create the CaptureGraphBuilder
        hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, nullptr,
                              CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2,
                              reinterpret_cast<void**>(&captureGraph));
        if (hr < 0 || !captureGraph) {
            return false;
        }

        // get the controller for the graph
        hr = filterGraph->QueryInterface(IID_IMediaControl, reinterpret_cast<void**>(&mediaControl));
        if (hr < 0 || !mediaControl) {
            return false;
        }
        captureGraph->SetFiltergraph(filterGraph);
        return true;
    }

    static void initializeVideo(IFilterGraph2* filterGraph,
                                ICaptureGraphBuilder2* captureGraph,
                                FnProcessImage callback,
                                DeviceList &devList)
    {
        HRESULT hr = S_FALSE;
        for (int i = 0; i < devList.size(); i++) {
            Device* dev = devList[i].get();
            // add a filter for the device
            hr = filterGraph->AddSourceFilterForMoniker(dev->moniker, nullptr, dev->filterName.c_str(),
                                                        &dev->sourceFilter);
            if (hr != S_OK) {
                continue;
            }
            // create a samplegrabber filter for the device
            hr = CoCreateInstance(CLSID_SampleGrabber, nullptr, CLSCTX_INPROC_SERVER,
                                  IID_IBaseFilter, reinterpret_cast<void**>(&dev->sampleGrabberFilter));
            if (hr < 0) {
                continue;
            }

            // set mediatype on the samplegrabber
            hr = dev->sampleGrabberFilter->QueryInterface(IID_ISampleGrabber,
                                                          reinterpret_cast<void**>(&dev->sampleGrabber));
            if (hr != S_OK) {
                continue;
            }

            // set device capabilities
            updateDeviceCapabilities(captureGraph, dev);

            std::wstring filterName = L"SG" + dev->filterName;
            filterGraph->AddFilter(dev->sampleGrabberFilter, filterName.c_str());

            // add the callback to the samplegrabber, 0-->SampleCB, 1-->BufferCB
            hr = dev->registerSampleCB(dev->getCurrentFormat().pixelFormat, callback);
            if (hr != S_OK) {
                continue;
            }

            // set the null renderer
            hr = CoCreateInstance(CLSID_NullRenderer, nullptr, CLSCTX_INPROC_SERVER,
                                  IID_IBaseFilter, reinterpret_cast<void**>(&dev->nullRenderer));
            if (hr < 0) {
                continue;
            }

            filterName = L"NR" + dev->filterName;
            filterGraph->AddFilter(dev->nullRenderer, filterName.c_str());

            hr = captureGraph->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video,
                                            dev->sourceFilter,
                                            dev->sampleGrabberFilter,
                                            dev->nullRenderer);
            if (hr < 0) {
                continue;
            }

            // if the stream is started, start capturing immediatly
            LONGLONG start = 0, stop = MAXLONGLONG;
            hr = captureGraph->ControlStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
                                             dev->sourceFilter, &start, &stop, 1, 2);
            if (hr < 0) {
                continue;
            }
            // reference the graph
            dev->filterGraph = filterGraph;
        }
        return;
    }

    void disconnectFilters(Device* device)
    {
        if (!device) {
            return;
        }

        IPin* pPin = nullptr;
        HRESULT hr = getPin(device->sourceFilter, PINDIR_OUTPUT, &pPin);
        if (SUCCEEDED(hr)) {
            filterGraph->Disconnect(pPin);
            pPin->Release();
            pPin = nullptr;
        }

        hr = getPin(device->sampleGrabberFilter, PINDIR_INPUT, &pPin);
        if (SUCCEEDED(hr)) {
            filterGraph->Disconnect(pPin);
            pPin->Release();
            pPin = nullptr;
        }

        hr = getPin(device->sampleGrabberFilter, PINDIR_OUTPUT, &pPin);
        if (SUCCEEDED(hr)) {
            filterGraph->Disconnect(pPin);
            pPin->Release();
            pPin = nullptr;
        }

        hr = getPin(device->nullRenderer, PINDIR_INPUT, &pPin);
        if (SUCCEEDED(hr)) {
            filterGraph->Disconnect(pPin);
            pPin->Release();
            pPin = nullptr;
        }

        filterGraph->RemoveFilter(device->nullRenderer);
        filterGraph->RemoveFilter(device->sampleGrabberFilter);
        filterGraph->RemoveFilter(device->sourceFilter);
    }

    static bool checkMediaType(AM_MEDIA_TYPE* type)
    {
        if (type->majortype != MEDIATYPE_Video ||
                type->formattype != FORMAT_VideoInfo) {
            return false;
        }

        VIDEOINFOHEADER* pvi = reinterpret_cast<VIDEOINFOHEADER*>(type->pbFormat);
        if (pvi->bmiHeader.biWidth <= 0 ||
                pvi->bmiHeader.biHeight <= 0) {
            return false;
        }

        bool isKnownFormat = false;
        for (auto& formatRow: g_imageFormatTable) {
            if (type->subtype == formatRow.directshowFormat) {
                isKnownFormat = true;
                break;
            }
        }
        if (!isKnownFormat) {
            return false;
        }
        return true;
    }

    static bool updateDeviceCapabilities(ICaptureGraphBuilder2* captureGraph, Device* dev)
    {
        HRESULT hr = S_FALSE;
        AM_MEDIA_TYPE* pmt = nullptr;
        VIDEO_STREAM_CONFIG_CAPS scc;
        IAMStreamConfig* pConfig = nullptr;

        hr = captureGraph->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
                                      dev->sourceFilter, IID_IAMStreamConfig,
                                      reinterpret_cast<void**>(&pConfig));
        if (hr < 0) {
            return false;
        }

        int iCount = 0;
        int iSize = 0;
        hr = pConfig->GetNumberOfCapabilities(&iCount, &iSize);
        if (hr < 0) {
            pConfig->Release();
            return false;
        }

        if (dev->streamConfig) {
            dev->streamConfig->Release();
        }
        dev->streamConfig = pConfig;

        for (int i = 0; i < iCount; ++i) {
            hr = pConfig->GetStreamCaps(i, &pmt, reinterpret_cast<BYTE*>(&scc));
            if (hr < 0) {
                continue;
            }

            if (!checkMediaType(pmt)) {
                continue;
            }

            Device::Format format;
            VIDEOINFOHEADER* pvi = reinterpret_cast<VIDEOINFOHEADER*>(pmt->pbFormat);
            format.mediaType = *pmt;
            format.width = pvi->bmiHeader.biWidth;
            format.height = pvi->bmiHeader.biHeight;
            format.pixelFormat = pmt->subtype;

            IAMVideoControl* pVideoControl = nullptr;
            hr = captureGraph->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
                                          dev->sourceFilter, IID_IAMVideoControl,
                                          reinterpret_cast<void**>(&pVideoControl));
            if (hr < 0) {
                continue;
            }

            IPin* pPin = nullptr;
            hr = getPin(dev->sourceFilter, PINDIR_OUTPUT, &pPin);
            if (hr < 0) {
                continue;
            }

            long supportedModes;
            hr = pVideoControl->GetCaps(pPin, &supportedModes);
            if (hr < 0) {
                pPin->Release();
                pVideoControl->Release();
                continue;
            }

            long mode;
            hr = pVideoControl->GetMode(pPin, &mode);
            if (hr < 0) {
                pPin->Release();
                pVideoControl->Release();
                continue;
            }

            format.isFlippedHorizontal = mode & VideoControlFlag_FlipHorizontal;
            format.isFlippedVertical = (mode & VideoControlFlag_FlipVertical) >> 1;

            dev->formatList.push_back(format);

            pPin->Release();
            pVideoControl->Release();
        }

        if (!dev->formatList.empty()) {
            dev->currentFormat = *dev->formatList.begin();
        }
        return true;
    }

    bool runControl()
    {
        HRESULT hr = mediaControl->Run();
        if (hr < 0) {
            return false;
        }
        for (auto& dev : devList) {
            dev->stop();
        }
        return true;
    }

    bool stopControl()
    {
        for (auto& dev: devList) {
            dev->stop();
        }
        HRESULT hr = mediaControl->Stop();
        if (hr < 0) {
            return false;
        }
        return true;
    }

    static HRESULT getPin(IBaseFilter* pFilter, PIN_DIRECTION PinDir, IPin** ppPin)
    {
        *ppPin = nullptr;
        IEnumPins *pEnum = nullptr;
        IPin *pPin = nullptr;

        HRESULT hr = pFilter->EnumPins(&pEnum);
        if (FAILED(hr)) {
            return hr;
        }

        pEnum->Reset();
        while (pEnum->Next(1, &pPin, NULL) == S_OK) {
            PIN_DIRECTION ThisPinDir;
            pPin->QueryDirection(&ThisPinDir);
            if (ThisPinDir == PinDir) {
                pEnum->Release();
                *ppPin = pPin;
                return S_OK;
            }
            pPin->Release();
        }
        pEnum->Release();
        return E_FAIL;
    }
public:
    Manager(const std::vector<Device::Ptr> &devList_, FnProcessImage callback):
        filterGraph(nullptr),
        captureGraph(nullptr),
        mediaControl(nullptr),
        m_activeDeviceNum(0),
        devList(devList_)
    {
        CoInitialize(NULL);
        if (initializeGraph(filterGraph, captureGraph, mediaControl)) {
            initializeVideo(filterGraph, captureGraph, callback, devList);
            runControl();
        }
    }

    ~Manager()
    {
        for (auto& dev : devList) {
            dev->stop();
            disconnectFilters(dev.get());
        }
        devList.erase(devList.begin(), devList.end());
        stopControl();

        detail::objectRelease(&mediaControl);
        detail::objectRelease(&captureGraph);
        detail::objectRelease(&filterGraph);
    }

    std::vector<std::wstring> getDeviceList() const
    {
        std::vector<std::wstring> names;
        for (auto& dev: devList) {
            names.push_back(dev->getFriendlyName());
        }
        return names;
    }

    std::vector<std::string> getResolutionList() const
    {
        std::vector<std::string> resolutions;
        if (m_activeDeviceNum >= devList.size()) {
            return resolutions;
        }

        const std::vector<Device::Format> &formatList = devList[m_activeDeviceNum]->getFormatList();
        for (const Device::Format& property: formatList) {
            std::string formatName = "unknown";
            for (auto& formatRow: g_imageFormatTable) {
                if (formatRow.directshowFormat == property.pixelFormat) {
                    formatName = formatRow.name;
                    break;
                }
            }

            std::stringstream stream;
            stream << property.width << "x" << property.height << "@" << formatName;
            std::string resolution;
            stream >> resolution;
            resolutions.push_back(resolution);
        }
        return resolutions;
    }

    bool onDeviceChanged(unsigned deviceNum)
    {
        if (!stopCapture()) {
            return false;
        }
        if (deviceNum >= devList.size()) {
            return false;
        }
        m_activeDeviceNum = deviceNum;
        return true;
    }

    bool onResolutionChanged(unsigned resolutionNum)
    {
        if (m_activeDeviceNum >= devList.size()) {
            return false;
        }

        stopCapture();
        if (!stopControl()) {
            return false;
        }

        auto formatList = devList[m_activeDeviceNum]->getFormatList();
        if (resolutionNum >= formatList.size()) {
            return false;
        }

        if (!devList[m_activeDeviceNum]->setCurrentFormat(formatList[resolutionNum])) {
            return false;
        }

        if (!runControl()) {
            return false;
        }
        return true;
    }

    bool startCapture()
    {
        if (m_activeDeviceNum >= devList.size()) {
            return false;
        }
        return devList[m_activeDeviceNum]->start();
    }

    bool stopCapture()
    {
        if (m_activeDeviceNum >= devList.size()) {
            return false;
        }
        return devList[m_activeDeviceNum]->stop();
    }

    /* control parameter */
    int setControlParam(long property, long value, long flag=CameraControl_Flags_Manual)
    {
        Device* device = devList[m_activeDeviceNum].get();
        IAMCameraControl *pCameraControl = nullptr;
        HRESULT hr = device->sourceFilter->QueryInterface(IID_IAMCameraControl, (void**)&pCameraControl);
        if (FAILED(hr)) {
            return -1;
        }
        hr = pCameraControl->Set(property, value, flag);
        if (FAILED(hr)) {
            pCameraControl->Release();
            return -2;
        }
        pCameraControl->Release();
        return 0;
    }

    int getControlParam(long property, long &value, long &flag)
    {
        Device* device = devList[m_activeDeviceNum].get();
        IAMCameraControl *pCameraControl = nullptr;
        HRESULT hr = device->sourceFilter->QueryInterface(IID_IAMCameraControl, (void**)&pCameraControl);
        if (FAILED(hr)) {
            return -1;
        }
        hr = pCameraControl->Get(property, &value, &flag);
        if (FAILED(hr)) {
            pCameraControl->Release();
            return -2;
        }
        pCameraControl->Release();
        return 0;
    }

    int getControlParamRange(long property,
                             long &minValue, long &maxValue, long &step, long &defaultValue,
                             long &flag)
    {
        Device* device = devList[m_activeDeviceNum].get();
        IAMCameraControl *pCameraControl = nullptr;
        HRESULT hr = device->sourceFilter->QueryInterface(IID_IAMCameraControl, (void**)&pCameraControl);
        if (FAILED(hr)) {
            return -1;
        }
        hr = pCameraControl->GetRange(property, &minValue, &maxValue, &step, &defaultValue, &flag);
        if (FAILED(hr)) {
            pCameraControl->Release();
            return -2;
        }
        pCameraControl->Release();
        return 0;
    }

    int setExposure(long value, long flag=CameraControl_Flags_Manual)
    {
        return setControlParam(CameraControl_Exposure, value, flag);
    }

    int getExposure(long &value, long &flag)
    {
        return getControlParam(CameraControl_Exposure, value, flag);
    }

    int setFocus(long value, long flag=CameraControl_Flags_Manual)
    {
        return setControlParam(CameraControl_Focus, value, flag);
    }

    int getFocus(long &value, long &flag)
    {
        return getControlParam(CameraControl_Focus, value, flag);
    }

    /* video process parameter */
    int setProcParam(long property, long value, long flag=VideoProcAmp_Flags_Manual)
    {
        Device* device = devList[m_activeDeviceNum].get();
        IAMVideoProcAmp *pVideoProcAmp = NULL;
        HRESULT hr = device->sourceFilter->QueryInterface(IID_IAMVideoProcAmp, (void**)&pVideoProcAmp);
        if (FAILED(hr)) {
            return -1;
        }
        hr = pVideoProcAmp->Set(property, value, flag);
        if (FAILED(hr)) {
            pVideoProcAmp->Release();
            return -2;
        }
        pVideoProcAmp->Release();
        return 0;
    }

    int getProcParam(long property, long &value, long &flag)
    {
        Device* device = devList[m_activeDeviceNum].get();
        IAMVideoProcAmp *pVideoProcAmp = NULL;
        HRESULT hr = device->sourceFilter->QueryInterface(IID_IAMVideoProcAmp, (void**)&pVideoProcAmp);
        if (FAILED(hr)) {
            return -1;
        }
        hr = pVideoProcAmp->Get(property, &value, &flag);
        if (FAILED(hr)) {
            pVideoProcAmp->Release();
            return -2;
        }
        pVideoProcAmp->Release();
        return 0;
    }

    int getProcParamRange(long property,
                          long &minValue, long &maxValue, long &step, long &defaultValue,
                          long &flag)
    {
        Device* device = devList[m_activeDeviceNum].get();
        IAMVideoProcAmp *pVideoProcAmp = NULL;
        HRESULT hr = device->sourceFilter->QueryInterface(IID_IAMVideoProcAmp, (void**)&pVideoProcAmp);
        if (FAILED(hr)) {
            return -1;
        }
        hr = pVideoProcAmp->GetRange(property, &minValue, &maxValue, &step, &defaultValue, &flag);
        if (FAILED(hr)) {
            pVideoProcAmp->Release();
            return -2;
        }
        pVideoProcAmp->Release();
        return 0;
    }

    int setBrightness(long value, long flag=VideoProcAmp_Flags_Manual)
    {
        return setProcParam(VideoProcAmp_Brightness, value, flag);
    }

    int getBrightness(long &value, long &flag)
    {
        return getProcParam(VideoProcAmp_Brightness, value, flag);
    }

    int setContrast(long value, long flag=VideoProcAmp_Flags_Manual)
    {
        return setProcParam(VideoProcAmp_Contrast, value, flag);
    }

    int getContrast(long &value, long &flag)
    {
        return getProcParam(VideoProcAmp_Contrast, value, flag);
    }

    int setHue(long value, long flag=VideoProcAmp_Flags_Manual)
    {
        return setProcParam(VideoProcAmp_Hue, value, flag);
    }

    int getHue(long &value, long &flag)
    {
        return getProcParam(VideoProcAmp_Hue, value, flag);
    }

    int setSaturation(long value, long flag=VideoProcAmp_Flags_Manual)
    {
        return setProcParam(VideoProcAmp_Saturation, value, flag);
    }

    int getSaturation(long &value, long &flag)
    {
        return getProcParam(VideoProcAmp_Saturation, value, flag);
    }

    int setSharpness(long value, long flag=VideoProcAmp_Flags_Manual)
    {
        return setProcParam(VideoProcAmp_Sharpness, value, flag);
    }

    int getSharpness(long &value, long &flag)
    {
        return getProcParam(VideoProcAmp_Sharpness, value, flag);
    }

    int setGamma(long value, long flag=VideoProcAmp_Flags_Manual)
    {
        return setProcParam(VideoProcAmp_Gamma, value, flag);
    }

    int getGamma(long &value, long &flag)
    {
        return getProcParam(VideoProcAmp_Gamma, value, flag);
    }

    int setWhiteBalance(long value, long flag=VideoProcAmp_Flags_Manual)
    {
        return setProcParam(VideoProcAmp_WhiteBalance, value, flag);
    }

    int getWhiteBalance(long &value, long &flag)
    {
        return getProcParam(VideoProcAmp_WhiteBalance, value, flag);
    }

    int setBacklightCompensation(long value, long flag=VideoProcAmp_Flags_Manual)
    {
        return setProcParam(VideoProcAmp_BacklightCompensation, value, flag);
    }

    int getBacklightCompensation(long &value, long &flag)
    {
        return getProcParam(VideoProcAmp_BacklightCompensation, value, flag);
    }

    int setGain(long value, long flag=VideoProcAmp_Flags_Manual)
    {
        return setProcParam(VideoProcAmp_Gain, value, flag);
    }

    int getGain(long &value, long &flag)
    {
        return getProcParam(VideoProcAmp_Gain, value, flag);
    }

    void setParams(const Params &params)
    {
        /* brightness */
        setBrightness(params.brightness.value, params.brightness.flag);
        /* contrast */
        setContrast(params.contrast.value, params.contrast.flag);
        /* hue */
        setHue(params.hue.value, params.hue.flag);
        /* saturation */
        setSaturation(params.saturation.value, params.saturation.flag);
        /* sharpness */
        setSharpness(params.sharpness.value, params.sharpness.flag);
        /* gamma */
        setGamma(params.gamma.value, params.gamma.flag);
        /* whiteBalance */
        setWhiteBalance(params.whiteBalance.value, params.whiteBalance.flag);
        /* backlightCompensation */
        setBacklightCompensation(params.backlightCompensation.value, params.backlightCompensation.flag);
        /* gain */
        setGain(params.gain.value, params.gain.flag);
        /* exposure */
        setExposure(params.exposure.value, params.exposure.flag);
        /* focus */
        setFocus(params.focus.value, params.focus.flag);
        return;
    }

    void getParams(Params &params)
    {
        /* brightness */
        getProcParamRange(VideoProcAmp_Brightness,
                          params.brightness.minValue,
                          params.brightness.maxValue,
                          params.brightness.step,
                          params.brightness.defaultValue,
                          params.brightness.flag);
        getBrightness(params.brightness.value, params.brightness.flag);
        /* contrast */
        getProcParamRange(VideoProcAmp_Contrast,
                          params.contrast.minValue,
                          params.contrast.maxValue,
                          params.contrast.step,
                          params.contrast.defaultValue,
                          params.contrast.flag);
        getContrast(params.contrast.value, params.contrast.flag);
        /* hue */
        getProcParamRange(VideoProcAmp_Hue,
                          params.hue.minValue,
                          params.hue.maxValue,
                          params.hue.step,
                          params.hue.defaultValue,
                          params.hue.flag);
        getHue(params.hue.value, params.hue.flag);
        /* saturation */
        getProcParamRange(VideoProcAmp_Saturation,
                          params.saturation.minValue,
                          params.saturation.maxValue,
                          params.saturation.step,
                          params.saturation.defaultValue,
                          params.saturation.flag);
        getSaturation(params.saturation.value, params.saturation.flag);
        /* sharpness */
        getProcParamRange(VideoProcAmp_Sharpness,
                          params.sharpness.minValue,
                          params.sharpness.maxValue,
                          params.sharpness.step,
                          params.sharpness.defaultValue,
                          params.sharpness.flag);
        getSharpness(params.sharpness.value, params.sharpness.flag);
        /* gamma */
        getProcParamRange(VideoProcAmp_Gamma,
                          params.gamma.minValue,
                          params.gamma.maxValue,
                          params.gamma.step,
                          params.gamma.defaultValue,
                          params.gamma.flag);
        getGamma(params.gamma.value, params.gamma.flag);
        /* whiteBalance */
        getProcParamRange(VideoProcAmp_WhiteBalance,
                          params.whiteBalance.minValue,
                          params.whiteBalance.maxValue,
                          params.whiteBalance.step,
                          params.whiteBalance.defaultValue,
                          params.whiteBalance.flag);
        getWhiteBalance(params.whiteBalance.value, params.whiteBalance.flag);
        /* backlightCompensation */
        getProcParamRange(VideoProcAmp_BacklightCompensation,
                          params.backlightCompensation.minValue,
                          params.backlightCompensation.maxValue,
                          params.backlightCompensation.step,
                          params.backlightCompensation.defaultValue,
                          params.backlightCompensation.flag);
        getBacklightCompensation(params.backlightCompensation.value, params.backlightCompensation.flag);
        /* gain */
        getProcParamRange(VideoProcAmp_Gain,
                          params.gain.minValue,
                          params.gain.maxValue,
                          params.gain.step,
                          params.gain.defaultValue,
                          params.gain.flag);
        getGain(params.gain.value, params.gain.flag);
        /* exposure */
        getControlParamRange(CameraControl_Exposure,
                          params.exposure.minValue,
                          params.exposure.maxValue,
                          params.exposure.step,
                          params.exposure.defaultValue,
                          params.exposure.flag);
        getExposure(params.exposure.value, params.exposure.flag);
        /* focus */
        getControlParamRange(CameraControl_Focus,
                          params.focus.minValue,
                          params.focus.maxValue,
                          params.focus.step,
                          params.focus.defaultValue,
                          params.focus.flag);
        getFocus(params.focus.value, params.focus.flag);
        return;
    }
};

}
#endif // CAMERA_H
