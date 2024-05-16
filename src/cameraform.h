#ifndef CAMERAFORM_H
#define CAMERAFORM_H

#include <QWidget>
#include <QCloseEvent>
#include <QMutex>
#include <camera/camera.hpp>
#include <camera/usbhotplug.h>

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
    void deviceAttached(bool isAttached);
public slots:
    void updateImage(const QImage &img);
    void onAddDevice();
    void onRemoveDevice();
    void onResolutionChanged(int resolutionNum);
    void onDeviceChanged(int deviceNum);
    void startCapture();
    void stopCapture();
protected:
    void closeEvent(QCloseEvent *ev) override;
private:
    bool m_isCapturing;
    UsbHotplug hotplug;
    Ui::CameraForm *ui;
    Camera::Manager *cameraManager;
};

#endif // CAMERAFORM_H
