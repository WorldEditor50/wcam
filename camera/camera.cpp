#include "camera.hpp"

Camera::Graph Camera::Device::graph;

HRESULT Camera::Device::getPin(IBaseFilter* pFilter, PIN_DIRECTION PinDir, IPin** ppPin)
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

bool Camera::Device::checkMediaType(AM_MEDIA_TYPE* type)
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

bool Camera::Device::findDevice(const Property& property)
{
    HRESULT hr = S_FALSE;
    VARIANT name;
    std::wstring filterName;
    ICreateDevEnum* devEnum = nullptr;
    IEnumMoniker* enumMoniker = nullptr;
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
        enumMoniker->Release();
        return false;
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
        std::wstring displayName(wDisplayName);
        int pos = displayName.find(L"vid_");
        vendorID = displayName.substr(pos + 4, 4);
        pos = displayName.find(L"pid_");
        productID = displayName.substr(pos + 4, 4);
        free(wDisplayName);

        friendlyName = std::wstring(name.bstrVal, SysStringLen(name.bstrVal));
        filterName  = createUniqueName(friendlyName, devNum);
        if (property.friendlyName == friendlyName &&
                property.vendorID == vendorID &&
                property.productID == productID) {
            VariantClear(&name);
            pbag->Release();
            break;
        }
        VariantClear(&name);
        pbag->Release();
    }
    enumMoniker->Release();
    devEnum->Release();
    return true;
}

bool Camera::Device::initializeVideo(IFilterGraph2* filterGraph,
                     ICaptureGraphBuilder2* captureGraph,
                     const Property &property,
                     const FnProcessImage &callback)
{

    if (!findDevice(property)) {
        std::cout<<"can't find device."<<std::endl;
        return false;
    }
    HRESULT hr = S_FALSE;
    // add a filter for the device
    hr = filterGraph->AddSourceFilterForMoniker(moniker, nullptr, filterName.c_str(),
                                                &sourceFilter);
    if (hr != S_OK) {
        return false;
    }
    // create a samplegrabber filter for the device
    hr = CoCreateInstance(CLSID_SampleGrabber, nullptr, CLSCTX_INPROC_SERVER,
                          IID_IBaseFilter, reinterpret_cast<void**>(&sampleGrabberFilter));
    if (hr < 0) {
        return false;
    }
    // set mediatype on the samplegrabber
    hr = sampleGrabberFilter->QueryInterface(IID_ISampleGrabber,
                                                  reinterpret_cast<void**>(&sampleGrabber));
    if (hr != S_OK) {
        return false;
    }

    // set device capabilities
    updateDeviceCapabilities(captureGraph);

    std::wstring filterName_ = L"SG" + filterName;
    filterGraph->AddFilter(sampleGrabberFilter, filterName_.c_str());

    // add the callback to the samplegrabber, 0-->SampleCB, 1-->BufferCB
    hr = registerSampleCB(getCurrentFormat().pixelFormat, callback);
    if (hr != S_OK) {
        return false;
    }

    // set the null renderer
    hr = CoCreateInstance(CLSID_NullRenderer, nullptr, CLSCTX_INPROC_SERVER,
                          IID_IBaseFilter, reinterpret_cast<void**>(&nullRenderer));
    if (hr < 0) {
        return false;
    }

    filterName_ = L"NR" + filterName;
    filterGraph->AddFilter(nullRenderer, filterName_.c_str());

    hr = captureGraph->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video,
                                    sourceFilter,
                                    sampleGrabberFilter,
                                    nullRenderer);
    if (hr < 0) {
        return false;
    }

    // if the stream is started, start capturing immediatly
    LONGLONG start = 0, stop = MAXLONGLONG;
    hr = captureGraph->ControlStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
                                     sourceFilter, &start, &stop, 1, 2);
    if (hr < 0) {
        return false;
    }
    return true;
}

bool Camera::Device::updateDeviceCapabilities(ICaptureGraphBuilder2* captureGraph)
{
    HRESULT hr = S_FALSE;
    AM_MEDIA_TYPE* pmt = nullptr;
    VIDEO_STREAM_CONFIG_CAPS scc;
    IAMStreamConfig* pConfig = nullptr;

    hr = captureGraph->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
                                    sourceFilter, IID_IAMStreamConfig,
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

    if (streamConfig) {
        streamConfig->Release();
    }
    streamConfig = pConfig;

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
                                         sourceFilter, IID_IAMVideoControl,
                                         reinterpret_cast<void**>(&pVideoControl));
        if (hr < 0) {
            continue;
        }

        IPin* pPin = nullptr;
        hr = getPin(sourceFilter, PINDIR_OUTPUT, &pPin);
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

        formatList.push_back(format);

        pPin->Release();
        pVideoControl->Release();
    }

    if (!formatList.empty()) {
        currentFormat = *formatList.begin();
    }
    return true;
}

HRESULT Camera::Device::registerSampleCB(const GUID &pixelFormat, const FnProcessImage &callbak)
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
    // add the callback to the samplegrabber, 0-->SampleCB, 1-->BufferCB, sampleGrabberCB will be released by samplegrabber
    return sampleGrabber->SetCallback(sampleGrabberCB, 1);
}

void Camera::Device::disconnectFilters(IFilterGraph2* filterGraph)
{
    IPin* pPin = nullptr;
    HRESULT hr = getPin(sourceFilter, PINDIR_OUTPUT, &pPin);
    if (SUCCEEDED(hr)) {
        filterGraph->Disconnect(pPin);
        pPin->Release();
        pPin = nullptr;
    }

    hr = getPin(sampleGrabberFilter, PINDIR_INPUT, &pPin);
    if (SUCCEEDED(hr)) {
        filterGraph->Disconnect(pPin);
        pPin->Release();
        pPin = nullptr;
    }

    hr = getPin(sampleGrabberFilter, PINDIR_OUTPUT, &pPin);
    if (SUCCEEDED(hr)) {
        filterGraph->Disconnect(pPin);
        pPin->Release();
        pPin = nullptr;
    }

    hr = getPin(nullRenderer, PINDIR_INPUT, &pPin);
    if (SUCCEEDED(hr)) {
        filterGraph->Disconnect(pPin);
        pPin->Release();
        pPin = nullptr;
    }

    filterGraph->RemoveFilter(nullRenderer);
    filterGraph->RemoveFilter(sampleGrabberFilter);
    filterGraph->RemoveFilter(sourceFilter);
    return;
}

bool Camera::Device::runControl(IMediaControl* mediaControl)
{
    HRESULT hr = mediaControl->Run();
    if (hr < 0) {
        return false;
    }
    stop();
    return true;
}

bool Camera::Device::stopControl(IMediaControl* mediaControl)
{
    stop();
    HRESULT hr = mediaControl->Stop();
    if (hr < 0) {
        return false;
    }
    return true;
}

Camera::Device::Device():isActiveValue(false), moniker(nullptr), sourceFilter(nullptr),
    sampleGrabberFilter(nullptr), nullRenderer(nullptr),
    sampleGrabber(nullptr), sampleGrabberCB(nullptr),
    streamConfig(nullptr)
{

}

Camera::Device::Device(const Property &property, const FnProcessImage &func)
    :isActiveValue(false), moniker(nullptr), sourceFilter(nullptr),
    sampleGrabberFilter(nullptr), nullRenderer(nullptr),
    sampleGrabber(nullptr), sampleGrabberCB(new SampleGrabberCB),
    streamConfig(nullptr)
{
    initializeVideo(graph.filterGraph, graph.captureGraph, property, func);
    runControl(graph.mediaControl);
}

Camera::Device::~Device()
{

    stopControl(graph.mediaControl);
    disconnectFilters(graph.filterGraph);

    detail::objectRelease(&moniker);
    detail::objectRelease(&sourceFilter);
    detail::objectRelease(&sampleGrabberFilter);
    detail::objectRelease(&nullRenderer);
    detail::objectRelease(&streamConfig);
    detail::objectRelease(&sampleGrabber);
    //if (sampleGrabberCB) {
    //    delete sampleGrabberCB;
    //    sampleGrabberCB = nullptr;
    //}
}

int Camera::Device::enumerate(const std::wstring &vendorID, std::vector<Property> &devList)
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
        enumMoniker->Release();
        return -2;
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

        Property property;
        property.id = devNum;
        std::wstring wname(name.bstrVal, SysStringLen(name.bstrVal));
        property.friendlyName = wname;
        property.filterName = createUniqueName(wname, devNum);
        property.vendorID = vid;
        property.productID = pid;
        devList.push_back(property);

        VariantClear(&name);
        pbag->Release();
    }
    moniker->Release();
    enumMoniker->Release();
    devEnum->Release();
    return devNum;
}

bool Camera::Device::setCurrentFormat(const Device::Format& format)
{
    currentFormat = format;

    AM_MEDIA_TYPE mt = format.mediaType;
    HRESULT hr = streamConfig->SetFormat(&mt);
    if (hr < 0) {
        return false;
    }
    return true;
}

bool Camera::Device::start()
{
    if (isActiveValue) {
        return true;
    }
    if (formatList.empty()) {
        return false;
    }

    sampleGrabberCB->start(currentFormat.width,
                           currentFormat.height,
                           detail::parsePixelFormat(currentFormat.pixelFormat));
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

bool Camera::Device::stop()
{
    if (!isActiveValue) {
        return true;
    }
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
    if (sampleGrabberCB) {
        sampleGrabberCB->stop();
    }

    return true;
}

std::vector<std::string> Camera::Device::getResolutionList() const
{
    std::vector<std::string> resolutions;
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

bool Camera::Device::onResolutionChanged(unsigned int resolutionNum)
{
    if (!stopControl(graph.mediaControl)) {
        return false;
    }

    if (resolutionNum >= formatList.size()) {
        return false;
    }

    if (!setCurrentFormat(formatList[resolutionNum])) {
        std::cout<<"setCurrentFormat failed"<<std::endl;
        return false;
    }

    if (!runControl(graph.mediaControl)) {
        return false;
    }
    return true;
}

/* control parameter */
int Camera::Device::setControlParam(long property, long value, long flag)
{
    IAMCameraControl *pCameraControl = nullptr;
    HRESULT hr = sourceFilter->QueryInterface(IID_IAMCameraControl, (void**)&pCameraControl);
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

int Camera::Device::getControlParam(long property, long &value, long &flag)
{
    IAMCameraControl *pCameraControl = nullptr;
    HRESULT hr = sourceFilter->QueryInterface(IID_IAMCameraControl, (void**)&pCameraControl);
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

int Camera::Device::getControlParamRange(long property,
                         long &minValue, long &maxValue, long &step, long &defaultValue,
                         long &flag)
{
    IAMCameraControl *pCameraControl = nullptr;
    HRESULT hr = sourceFilter->QueryInterface(IID_IAMCameraControl, (void**)&pCameraControl);
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

int Camera::Device::setExposure(long value, long flag)
{
    return setControlParam(CameraControl_Exposure, value, flag);
}

int Camera::Device::getExposure(long &value, long &flag)
{
    return getControlParam(CameraControl_Exposure, value, flag);
}

int Camera::Device::setFocus(long value, long flag)
{
    return setControlParam(CameraControl_Focus, value, flag);
}

int Camera::Device::getFocus(long &value, long &flag)
{
    return getControlParam(CameraControl_Focus, value, flag);
}

/* video process parameter */
int Camera::Device::setProcParam(long property, long value, long flag)
{
    IAMVideoProcAmp *pVideoProcAmp = NULL;
    HRESULT hr = sourceFilter->QueryInterface(IID_IAMVideoProcAmp, (void**)&pVideoProcAmp);
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

int Camera::Device::getProcParam(long property, long &value, long &flag)
{
    IAMVideoProcAmp *pVideoProcAmp = NULL;
    HRESULT hr = sourceFilter->QueryInterface(IID_IAMVideoProcAmp, (void**)&pVideoProcAmp);
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

int Camera::Device::getProcParamRange(long property,
                      long &minValue, long &maxValue, long &step, long &defaultValue,
                      long &flag)
{
    IAMVideoProcAmp *pVideoProcAmp = NULL;
    HRESULT hr = sourceFilter->QueryInterface(IID_IAMVideoProcAmp, (void**)&pVideoProcAmp);
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

int Camera::Device::setBrightness(long value, long flag)
{
    return setProcParam(VideoProcAmp_Brightness, value, flag);
}

int Camera::Device::getBrightness(long &value, long &flag)
{
    return getProcParam(VideoProcAmp_Brightness, value, flag);
}

int Camera::Device::setContrast(long value, long flag)
{
    return setProcParam(VideoProcAmp_Contrast, value, flag);
}

int Camera::Device::getContrast(long &value, long &flag)
{
    return getProcParam(VideoProcAmp_Contrast, value, flag);
}

int Camera::Device::setHue(long value, long flag)
{
    return setProcParam(VideoProcAmp_Hue, value, flag);
}

int Camera::Device::getHue(long &value, long &flag)
{
    return getProcParam(VideoProcAmp_Hue, value, flag);
}

int Camera::Device::setSaturation(long value, long flag)
{
    return setProcParam(VideoProcAmp_Saturation, value, flag);
}

int Camera::Device::getSaturation(long &value, long &flag)
{
    return getProcParam(VideoProcAmp_Saturation, value, flag);
}

int Camera::Device::setSharpness(long value, long flag)
{
    return setProcParam(VideoProcAmp_Sharpness, value, flag);
}

int Camera::Device::getSharpness(long &value, long &flag)
{
    return getProcParam(VideoProcAmp_Sharpness, value, flag);
}

int Camera::Device::setGamma(long value, long flag)
{
    return setProcParam(VideoProcAmp_Gamma, value, flag);
}

int Camera::Device::getGamma(long &value, long &flag)
{
    return getProcParam(VideoProcAmp_Gamma, value, flag);
}

int Camera::Device::setWhiteBalance(long value, long flag)
{
    return setProcParam(VideoProcAmp_WhiteBalance, value, flag);
}

int Camera::Device::getWhiteBalance(long &value, long &flag)
{
    return getProcParam(VideoProcAmp_WhiteBalance, value, flag);
}

int Camera::Device::setBacklightCompensation(long value, long flag)
{
    return setProcParam(VideoProcAmp_BacklightCompensation, value, flag);
}

int Camera::Device::getBacklightCompensation(long &value, long &flag)
{
    return getProcParam(VideoProcAmp_BacklightCompensation, value, flag);
}

int Camera::Device::setGain(long value, long flag)
{
    return setProcParam(VideoProcAmp_Gain, value, flag);
}

int Camera::Device::getGain(long &value, long &flag)
{
    return getProcParam(VideoProcAmp_Gain, value, flag);
}

void Camera::Device::setParams(const Params &params)
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

void Camera::Device::getParams(Camera::Params &params)
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
