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
using FnProcessRaw = std::function<void(int h, int w, int type, unsigned char* data)>;

enum ParamFlag {
    Param_Auto = 0x01,
    Param_Manual = 0x02
};

struct Param {
    long value;
    long step;
    long defaultValue;
    long minValue;
    long maxValue;
    long flag;
};

struct Params {
    Param brightness;
    Param contrast;
    Param hue;
    Param saturation;
    Param sharpness;
    Param gamma;
    Param whiteBalance;
    Param backlightCompensation;
    Param gain;
    Param exposure;
    Param focus;
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

class Frame
{
public:
    unsigned char* data;
    unsigned long length;
    unsigned long capacity;
public:
    Frame():data(nullptr),length(0), capacity(0){}
    ~Frame()
    {
        if (data != nullptr) {
            delete [] data;
            data = nullptr;
        }
    }
    static unsigned long align(unsigned long s)
    {
        unsigned long size = s;
        if (size&0x3ff) {
            size = ((size >> 10) + 1)<<10;
        }
        return size;
    }

    inline operator unsigned char* () noexcept
    {
        return data;
    }

    void allocate(unsigned long length_)
    {
        if (data == nullptr) {
            capacity = align(length_);
            data = new unsigned char[capacity];
        } else {
            if (length_ > capacity) {
                delete [] data;
                capacity = align(length_);
                data = new unsigned char[capacity];
            }
        }
        length = length_;
        return;
    }

    void copy(unsigned char* data_, unsigned long length_)
    {
        allocate(length_);
        memcpy(data, data_, length_);
        return;
    }


};

class DecodeSampler : public ISampleGrabberCB
{
public:
    int width;
    int height;
    int pixelFormat;
    FnProcessImage processImage;
public:
    DecodeSampler():width(0), height(0), pixelFormat(0){}
    virtual ~DecodeSampler(){}

    void registerProcess(const FnProcessImage &func) { processImage = func; }

    void decode(const Frame& xi, Frame &xo)
    {
        switch (pixelFormat) {
        case PixelFormat_MJPG:
            Jpeg::decode(xo.data, width, height,
                         xi.data, xi.length, Jpeg::ALIGN_4);
            processImage(height, width, 3, xo.data);
            break;
        case PixelFormat_YUY2: {
            int alignedWidth = (width + 1) & ~1;
            libyuv::YUY2ToARGB(xi.data, alignedWidth * 2,
                    xo.data, width * 4,
                    width, height);
            processImage(height, width, 4, xo.data);
            break;
        }
        default:
            break;
        }
    }

};

class PingPongSampler : public DecodeSampler
{
public:
    enum State {
        STATE_NONE = 0,
        STATE_RUN,
        STATE_TERMINATE
    };
    constexpr static int max_buffer_len = 8;
private:
    int state;
    int in;
    int out;
    Frame encodeFrame[max_buffer_len];
    Frame decodeFrame[max_buffer_len];
    std::thread processThread;
protected:
    void run()
    {
        std::cout<<"enter process image"<<std::endl;
        while (state != STATE_TERMINATE) {
            int index = out;
            Frame &frame = encodeFrame[index];
            if (frame.data == nullptr) {
                continue;
            }
            Frame& image = decodeFrame[index];
            /* decode */
            decode(frame, image);
        }
        state = STATE_NONE;
        std::cout<<"leave process image"<<std::endl;
        return;
    }
public:
    PingPongSampler():state(STATE_NONE), in(0), out(0){}
    virtual ~PingPongSampler(){}

    virtual HRESULT STDMETHODCALLTYPE SampleCB(double time, IMediaSample* sample) override
    {
        return S_OK;
    }
    virtual HRESULT STDMETHODCALLTYPE BufferCB(double time, BYTE* buffer, long len) override
    {
        if (!buffer || len <= 0) {
            return S_OK;
        }
        encodeFrame[in].copy(buffer, len);
        out = in;
        in = (in + 1)%max_buffer_len;
        return S_OK;
    }
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID *ppv) override
    {
        if(iid == IID_ISampleGrabberCB || iid == IID_IUnknown) {
            *ppv = reinterpret_cast<LPVOID*>(this);
            return S_OK;
        }
        return E_NOINTERFACE;
    }
    virtual ULONG STDMETHODCALLTYPE AddRef() { return 1;}
    virtual ULONG STDMETHODCALLTYPE Release() { return 1;}

    void start(int w, int h, int format)
    {
        if (state != STATE_NONE) {
            return;
        }
        width = w;
        height = h;
        pixelFormat = format;
        /* allocate memory for image */
        unsigned long len = width * height * 4;
        if (pixelFormat == PixelFormat_MJPG) {
            len = Jpeg::align4(width, 3)*height;
        } else if (pixelFormat == PixelFormat_YUY2) {
            len = width * height * 4;
        }
        for (int i = 0; i < max_buffer_len; i++) {
            decodeFrame[i].allocate(len);
        }
        state = STATE_RUN;
        processThread = std::thread(&PingPongSampler::run, this);
        return;
    }

    void stop()
    {
        if (state != STATE_NONE) {
            state = STATE_TERMINATE;
            processThread.join();
        }
        return;
    }
};

class AsyncSampler : public DecodeSampler
{
public:
    enum State {
        STATE_NONE = 0,
        STATE_PREPEND,
        STATE_FRAME_READY,
        STATE_PROCESS,
        STATE_TERMINATE
    };
    static constexpr int max_buffer_len = 4;
private:
    int state;
    int index;
    Frame encodeFrame;
    Frame decodeFrame[max_buffer_len];
    std::condition_variable condit;
    std::mutex mutex;
    std::thread processThread;
protected:
    void run()
    {
        std::cout<<"enter process image"<<std::endl;
        while (1) {
            std::unique_lock<std::mutex> locker(mutex);
            condit.wait_for(locker, std::chrono::milliseconds(500), [this]()->bool{
                                    return state == STATE_TERMINATE || state == STATE_FRAME_READY;
                                });
            if (state == STATE_TERMINATE) {
                state = STATE_NONE;
                break;
            } else if (state == STATE_PREPEND) {
                continue;
            }
            Frame& image = decodeFrame[index];
            index = (index + 1)%max_buffer_len;
            /* decode */
            decode(encodeFrame, image);
            /* get next image */
            if (state != STATE_TERMINATE) {
                state = STATE_PREPEND;
            }
        }
        std::cout<<"leave process image"<<std::endl;
        return;
    }
public:
    AsyncSampler():state(STATE_NONE), index(0){}
    virtual ~AsyncSampler(){}

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
            encodeFrame.copy(buffer, len);
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

    void start(int w, int h, int format)
    {
        if (state != STATE_NONE) {
            return;
        }
        width = w;
        height = h;
        pixelFormat = format;
        /* allocate memory for image */
        unsigned long len = width * height * 4;
        if (pixelFormat == PixelFormat_MJPG) {
            len = Jpeg::align4(width, 3)*height;
        } else if (pixelFormat == PixelFormat_YUY2) {
            len = width * height * 4;
        }
        for (int i = 0; i < max_buffer_len; i++) {
            decodeFrame[i].allocate(len);
        }
        state = STATE_PREPEND;
        processThread = std::thread(&AsyncSampler::run, this);
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

using SampleGrabberCB = AsyncSampler;


class Graph
{
    friend class Device;
private:
    IFilterGraph2* filterGraph;
    ICaptureGraphBuilder2* captureGraph;
    IMediaControl* mediaControl;
public:
    Graph():filterGraph(nullptr),captureGraph(nullptr),mediaControl(nullptr){}

    ~Graph()
    {
        detail::objectRelease(&mediaControl);
        detail::objectRelease(&captureGraph);
        detail::objectRelease(&filterGraph);
    }

    bool init()
    {
        CoInitialize(NULL);
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

};

struct Property {
    int id;
    std::wstring vendorID;
    std::wstring productID;
    std::wstring friendlyName;
    std::wstring filterName;
};

inline std::wstring createUniqueName(const std::wstring& name, int id)
{
    std::stringstream stream;
    stream << "id" << id;
    std::string uniquePostfix = stream.str();
    std::wstring newName = name + std::wstring(uniquePostfix.begin(), uniquePostfix.end());
    return newName;
}

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

    using Ptr = std::shared_ptr<Device>;

    static Graph graph;
private:
    bool isActiveValue;
    IMoniker* moniker;
    IBaseFilter* sourceFilter;
    IBaseFilter* sampleGrabberFilter;
    IBaseFilter* nullRenderer;
    ISampleGrabber* sampleGrabber;
    SampleGrabberCB* sampleGrabberCB;
    IAMStreamConfig* streamConfig;
    Device::Format currentFormat;
    std::vector<Device::Format> formatList;
private:
    static HRESULT getPin(IBaseFilter* pFilter, PIN_DIRECTION PinDir, IPin** ppPin);

    static bool checkMediaType(AM_MEDIA_TYPE* type);

    bool findDevice(const Property& property);

    bool initializeVideo(IFilterGraph2* filterGraph,
                         ICaptureGraphBuilder2* captureGraph,
                         const Property &property,
                         const FnProcessImage &callback);

    bool updateDeviceCapabilities(ICaptureGraphBuilder2* captureGraph);

    HRESULT registerSampleCB(const GUID &pixelFormat, const FnProcessImage &callbak);

    void disconnectFilters(IFilterGraph2* filterGraph);

    bool runControl(IMediaControl* mediaControl);

    bool stopControl(IMediaControl* mediaControl);
public:
    Device();
    explicit Device(const Property &property, const FnProcessImage &func);
    ~Device();

    static int enumerate(const std::wstring &vendorID, std::vector<Property> &devList);

    int getId() const {return id;}

    std::wstring getFriendlyName() const {return friendlyName;}

    const std::vector<Device::Format> &getFormatList() const { return formatList;}

    const Device::Format &getCurrentFormat() const {return currentFormat;}

    bool setCurrentFormat(const Device::Format& format);

    bool isActive() const {return isActiveValue; }

    bool start();

    bool stop();

    std::vector<std::string> getResolutionList() const;

    bool onResolutionChanged(unsigned int resolutionNum);

    /* control parameter */
    int setControlParam(long property, long value, long flag=CameraControl_Flags_Manual);
    int getControlParam(long property, long &value, long &flag);
    int getControlParamRange(long property,
                             long &minValue, long &maxValue, long &step, long &defaultValue,
                             long &flag);

    int setExposure(long value, long flag=CameraControl_Flags_Manual);
    int getExposure(long &value, long &flag);

    int setFocus(long value, long flag=CameraControl_Flags_Manual);
    int getFocus(long &value, long &flag);

    /* video process parameter */
    int setProcParam(long property, long value, long flag=VideoProcAmp_Flags_Manual);
    int getProcParam(long property, long &value, long &flag);
    int getProcParamRange(long property,
                          long &minValue, long &maxValue, long &step, long &defaultValue,
                          long &flag);
    int setBrightness(long value, long flag=VideoProcAmp_Flags_Manual);
    int getBrightness(long &value, long &flag);

    int setContrast(long value, long flag=VideoProcAmp_Flags_Manual);
    int getContrast(long &value, long &flag);

    int setHue(long value, long flag=VideoProcAmp_Flags_Manual);
    int getHue(long &value, long &flag);

    int setSaturation(long value, long flag=VideoProcAmp_Flags_Manual);
    int getSaturation(long &value, long &flag);

    int setSharpness(long value, long flag=VideoProcAmp_Flags_Manual);
    int getSharpness(long &value, long &flag);

    int setGamma(long value, long flag=VideoProcAmp_Flags_Manual);
    int getGamma(long &value, long &flag);

    int setWhiteBalance(long value, long flag=VideoProcAmp_Flags_Manual);
    int getWhiteBalance(long &value, long &flag);

    int setBacklightCompensation(long value, long flag=VideoProcAmp_Flags_Manual);
    int getBacklightCompensation(long &value, long &flag);

    int setGain(long value, long flag=VideoProcAmp_Flags_Manual);
    int getGain(long &value, long &flag);

    void setParams(const Params &params);
    void getParams(Params &params);
};

}
#endif // CAMERA_H
