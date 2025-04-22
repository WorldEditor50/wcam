#ifndef CAMERAFORM_H
#define CAMERAFORM_H

#include <QWidget>
#include <QCloseEvent>
#include <QMutex>
#include <camera/camera.hpp>
#include <camera/usbhotplug.h>
#include <opencv2/core.hpp>

namespace Ui {
class CameraForm;
}

class CameraForm : public QWidget
{
    Q_OBJECT

public:
    explicit CameraForm(QWidget *parent = nullptr);
    ~CameraForm();
signals:
    void sendImage(const QImage &img);
    void deviceAttached(unsigned short vid, unsigned short pid, bool isAttached);
public slots:
    void updateImage(const QImage &img);
    void onAddDevice(const Camera::Property &property);
    void onRemoveDevice();
    void onResolutionChanged(int resolutionNum);
    void onDeviceChanged(int deviceNum);
    void startCapture();
    void stopCapture();
private:
    bool isStreaming;
    bool isReadyForCapture;
    int processMethod;
    cv::Mat out;
    UsbHotplug hotplug;
    Ui::CameraForm *ui;
    std::vector<Camera::Property> devList;
    Camera::Device *dev;
};

#endif // CAMERAFORM_H
