#ifndef CAMERA_H
#define CAMERA_H
#include <Windows.h>
#include <rpc.h>
#include <rpcndr.h>
#include <dshow.h>
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

}

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

private:
    int width;
    int height;
    int pixelFormat;
    int state;
    Buffer rawBuffer;
    Buffer image;
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
    SampleGrabberCB(const FnProcessImage &func)
        :processImage(func), width(0), height(0), pixelFormat(0), state(STATE_NONE){}
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
            if (rawBuffer.capacity < len) {
                rawBuffer.allocate(len);
            }
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

    void setProperty(int w, int h, int format)
    {
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

class Device
{
public:
    class Property
    {
    public:
        AM_MEDIA_TYPE mediaType;
        LONG width;
        LONG height;
        GUID pixelFormat;
        bool isFlippedHorizontal;
        bool isFlippedVertical;
    public:
        Property(): mediaType(), width(0), height(0), pixelFormat(),
            isFlippedHorizontal(false), isFlippedVertical(false) {}
    };

    friend class Manager;
private:
    int id;
    bool isActiveValue;
    std::wstring friendlyName;
    std::wstring filterName;
    IBaseFilter* sourceFilter;
    IBaseFilter* sampleGrabberFilter;
    IBaseFilter* nullRenderer;
    ISampleGrabber* sampleGrabber;
    IFilterGraph2* filterGraph;
    IAMStreamConfig* streamConfig;
    Device::Property currentProperty;
    std::vector<Device::Property> propertyList;
public:
    Device():
        id(-1), isActiveValue(false), sourceFilter(nullptr),
        sampleGrabberFilter(nullptr), nullRenderer(nullptr), sampleGrabber(nullptr),
        filterGraph(nullptr), streamConfig(nullptr){}

    ~Device()
    {
        detail::objectRelease(&sourceFilter);
        detail::objectRelease(&sampleGrabberFilter);
        detail::objectRelease(&sampleGrabber);
        detail::objectRelease(&nullRenderer);
        detail::objectRelease(&streamConfig);
    }

    int getId() const {return id;}

    std::wstring getFriendlyName() const {return friendlyName;}

    const std::vector<Device::Property> &getPropertyList() const { return propertyList;}

    const Device::Property &getCurrentProperty() const {return currentProperty;}

    bool setCurrentProperty(const Device::Property& properties)
    {
        currentProperty = properties;

        AM_MEDIA_TYPE mt = properties.mediaType;
        HRESULT hr = streamConfig->SetFormat(&mt);
        if (hr < 0) {
            return false;
        }
        return true;
    }

    bool isActive() const {return isActiveValue; }

    bool start()
    {
        if (propertyList.size() <= 0) {
            return false;
        }

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

class Manager
{
private:
    IFilterGraph2* filterGraph;
    ICaptureGraphBuilder2* captureGraph;
    IMediaControl* mediaControl;
    SampleGrabberCB* sampleGrabberCB;
    bool m_readyForCapture;
    int m_activeDeviceNum;
    std::vector<std::shared_ptr<Device>> deviceList;
private:
    std::wstring createUniqueName(const std::wstring& name, int id)
    {
        std::stringstream stream;
        stream << "id" << id;
        std::string uniquePostfix = stream.str();
        std::wstring newName = name + std::wstring(uniquePostfix.begin(), uniquePostfix.end());
        return newName;
    }
    bool initializeGraph()
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

    bool initializeVideo()
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
            return false;
        }

        hr = devEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,
                                            &enumMoniker, NULL);
        if (hr < 0 || !enumMoniker) {
            return false;
        }

        int devNum = 0;
        while (enumMoniker->Next(1, &moniker, 0) == S_OK) {
            ++devNum;
            hr = moniker->BindToStorage(nullptr, nullptr, IID_IPropertyBag,
                                        reinterpret_cast<void**>(&pbag));
            if (hr >= 0) {
                VariantInit(&name);

                hr = pbag->Read(L"Description", &name, 0);
                if (hr < 0) {
                    hr = pbag->Read(L"FriendlyName", &name, 0);
                    if (hr < 0) {
                        moniker->Release();
                        continue;
                    }
                }

                std::shared_ptr<Device> device(new Device);
                device->id = devNum;
                std::wstring wname(name.bstrVal, SysStringLen(name.bstrVal));
                device->friendlyName = device->filterName = wname;
                device->filterName = createUniqueName(device->filterName, device->id);

                // add a filter for the device
                hr = filterGraph->AddSourceFilterForMoniker(moniker, nullptr, device->filterName.c_str(),
                                                        &device->sourceFilter);
                if (hr != S_OK) {
                    pbag->Release();
                    moniker->Release();
                    continue;
                }

                // create a samplegrabber filter for the device
                hr = CoCreateInstance(CLSID_SampleGrabber, nullptr, CLSCTX_INPROC_SERVER,
                                      IID_IBaseFilter, reinterpret_cast<void**>(&device->sampleGrabberFilter));
                if (hr < 0) {
                    pbag->Release();
                    moniker->Release();
                    continue;
                }

                // set mediatype on the samplegrabber
                hr = device->sampleGrabberFilter->QueryInterface(IID_ISampleGrabber,
                                                                   reinterpret_cast<void**>(&device->sampleGrabber));
                if (hr != S_OK) {
                    pbag->Release();
                    moniker->Release();
                    continue;
                }

                // set device capabilities
                updateDeviceCapabilities(device.get());

                filterName = L"SG" + device->filterName;
                filterGraph->AddFilter(device->sampleGrabberFilter, filterName.c_str());

                // set the media type
                AM_MEDIA_TYPE mt;
                memset(&mt, 0, sizeof(AM_MEDIA_TYPE));

                mt.majortype = MEDIATYPE_Video;
                mt.subtype = device->getCurrentProperty().pixelFormat;

                hr = device->sampleGrabber->SetMediaType(&mt);
                if (hr != S_OK) {
                    pbag->Release();
                    moniker->Release();
                    continue;
                }

                // add the callback to the samplegrabber, 0-->SampleCB, 1-->BufferCB
                hr = device->sampleGrabber->SetCallback(sampleGrabberCB, 1);
                if (hr != S_OK) {
                    pbag->Release();
                    moniker->Release();
                    continue;
                }

                // set the null renderer
                hr = CoCreateInstance(CLSID_NullRenderer, nullptr, CLSCTX_INPROC_SERVER,
                                      IID_IBaseFilter, reinterpret_cast<void**>(&device->nullRenderer));
                if (hr < 0) {
                    pbag->Release();
                    moniker->Release();
                    continue;
                }

                filterName = L"NR" + device->filterName;
                filterGraph->AddFilter(device->nullRenderer, filterName.c_str());

                hr = captureGraph->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video,
                                             device->sourceFilter,
                                             device->sampleGrabberFilter,
                                             device->nullRenderer);
                if (hr < 0) {
                    pbag->Release();
                    moniker->Release();
                    continue;
                }

                // if the stream is started, start capturing immediatly
                LONGLONG start = 0, stop = MAXLONGLONG;
                hr = captureGraph->ControlStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
                                              device->sourceFilter, &start, &stop, 1, 2);
                if (hr < 0) {
                    pbag->Release();
                    moniker->Release();
                    continue;
                }

                // reference the graph
                device->filterGraph = filterGraph;
                deviceList.push_back(device);

                VariantClear(&name);
                pbag->Release();
            }
            moniker->Release();
        }
        enumMoniker->Release();
        devEnum->Release();
        return true;
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

    bool checkMediaType(AM_MEDIA_TYPE* type)
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

    bool updateDeviceCapabilities(Device* device)
    {
        if (!device) {
            return false;
        }

        HRESULT hr = S_FALSE;
        AM_MEDIA_TYPE* pmt = nullptr;
        VIDEO_STREAM_CONFIG_CAPS scc;
        IAMStreamConfig* pConfig = nullptr;

        hr = captureGraph->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
                                      device->sourceFilter, IID_IAMStreamConfig,
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

        if (device->streamConfig) {
            device->streamConfig->Release();
        }
        device->streamConfig = pConfig;

        for (int i = 0; i < iCount; ++i) {
            hr = pConfig->GetStreamCaps(i, &pmt, reinterpret_cast<BYTE*>(&scc));
            if (hr < 0) {
                continue;
            }

            if (!checkMediaType(pmt)) {
                continue;
            }

            Device::Property property;
            VIDEOINFOHEADER* pvi = reinterpret_cast<VIDEOINFOHEADER*>(pmt->pbFormat);
            property.mediaType = *pmt;
            property.width = pvi->bmiHeader.biWidth;
            property.height = pvi->bmiHeader.biHeight;
            property.pixelFormat = pmt->subtype;

            IAMVideoControl* pVideoControl = nullptr;
            hr = captureGraph->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
                                          device->sourceFilter, IID_IAMVideoControl,
                                          reinterpret_cast<void**>(&pVideoControl));
            if (hr < 0) {
                continue;
            }

            IPin* pPin = nullptr;
            hr = getPin(device->sourceFilter, PINDIR_OUTPUT, &pPin);
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

            property.isFlippedHorizontal = mode & VideoControlFlag_FlipHorizontal;
            property.isFlippedVertical = (mode & VideoControlFlag_FlipVertical) >> 1;

            device->propertyList.push_back(property);

            pPin->Release();
            pVideoControl->Release();
        }

        if (!device->propertyList.empty()) {
            device->currentProperty = *device->propertyList.begin();
        }
        return true;
    }

    bool runControl()
    {
        HRESULT hr = mediaControl->Run();
        if (hr < 0) {
            return false;
        }
        m_readyForCapture = true;

        for (auto& device : deviceList) {
            device->stop();
        }
        return true;
    }

    bool stopControl()
    {
        for (auto& device: deviceList) {
            device->stop();
        }
        m_readyForCapture = false;
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
    Manager(FnProcessImage callback):
        filterGraph(nullptr),
        captureGraph(nullptr),
        mediaControl(nullptr),
        sampleGrabberCB(nullptr),
        m_readyForCapture(false),
        m_activeDeviceNum(0),
        deviceList()
    {
        sampleGrabberCB = new SampleGrabberCB(callback);
        CoInitialize(NULL);
        initializeGraph();
        initializeVideo();
        runControl();
    }

    ~Manager()
    {
        sampleGrabberCB->stop();
        for (auto& device : deviceList) {
            device->stop();
            disconnectFilters(device.get());
        }
        deviceList.erase(deviceList.begin(), deviceList.end());
        stopControl();

        detail::objectRelease(&mediaControl);
        detail::objectRelease(&captureGraph);
        detail::objectRelease(&filterGraph);
        delete sampleGrabberCB;
        sampleGrabberCB = nullptr;
    }

    std::vector<std::wstring> getDeviceList() const
    {
        std::vector<std::wstring> names;
        for (auto& device: deviceList) {
            names.push_back(device->getFriendlyName());
        }
        return names;
    }

    std::vector<std::string> getResolutionList() const
    {
        std::vector<std::string> resolutions;
        if (m_activeDeviceNum >= deviceList.size()) {
            return resolutions;
        }

        const std::vector<Device::Property> &propertyList = deviceList[m_activeDeviceNum]->getPropertyList();
        for (const Device::Property& property: propertyList) {
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
        if (deviceNum >= deviceList.size()) {
            return false;
        }
        m_activeDeviceNum = deviceNum;
        return true;
    }

    bool onResolutionChanged(unsigned resolutionNum)
    {
        if (m_activeDeviceNum >= deviceList.size()) {
            return false;
        }

        stopCapture();
        if (!stopControl()) {
            return false;
        }

        auto propertyList = deviceList[m_activeDeviceNum]->getPropertyList();
        if (resolutionNum >= propertyList.size()) {
            return false;
        }

        if (!deviceList[m_activeDeviceNum]->setCurrentProperty(propertyList[resolutionNum])) {
            return false;
        }

        if (!runControl()) {
            return false;
        }
        return true;
    }

    bool startCapture()
    {
        if (m_activeDeviceNum >= deviceList.size()) {
            return false;
        }
        Device::Property &property = deviceList[m_activeDeviceNum]->currentProperty;
        sampleGrabberCB->setProperty(property.width, property.height, parsePixelFormat(property.pixelFormat));
        sampleGrabberCB->start();
        return deviceList[m_activeDeviceNum]->start();
    }

    bool stopCapture()
    {
        if (m_activeDeviceNum >= deviceList.size()) {
            return false;
        }
        return deviceList[m_activeDeviceNum]->stop();
    }

    /* control parameter */
    int setControlParam(long property, long value, long flag=CameraControl_Flags_Manual)
    {
        Device* device = deviceList[m_activeDeviceNum].get();
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
        Device* device = deviceList[m_activeDeviceNum].get();
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
        Device* device = deviceList[m_activeDeviceNum].get();
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
        Device* device = deviceList[m_activeDeviceNum].get();
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
        Device* device = deviceList[m_activeDeviceNum].get();
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
        Device* device = deviceList[m_activeDeviceNum].get();
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
};

}
#endif // CAMERA_H
