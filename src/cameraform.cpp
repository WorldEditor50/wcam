#include "cameraform.h"
#include "ui_cameraform.h"

CameraForm::CameraForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CameraForm),
    cameraManager(nullptr)
{
    ui->setupUi(this);
    connect(this, &CameraForm::sendImage,
            this, &CameraForm::updateImage, Qt::QueuedConnection);
    connect(ui->startBtn, &QPushButton::released, this, &CameraForm::startCapture);
    connect(ui->stopBtn, &QPushButton::released, this, &CameraForm::stopCapture);
    /* HOTPLUG */
    hotplug.registerDevice(UUID_CAMERA);
    hotplug.registerNotify([=](int notifyFlag, unsigned short vid, unsigned short pid){
        emit deviceAttached(notifyFlag == UsbHotplug::DEVICE_ATTACHED);
    });
    connect(this, &CameraForm::deviceAttached, this, [=](bool isAttached){
        if (isAttached) {
            onAddDevice();
        } else {
            onRemoveDevice();
        }
    });
    onAddDevice();
}

CameraForm::~CameraForm()
{
    if (cameraManager != nullptr) {
        delete cameraManager;
        cameraManager = nullptr;
    }
    delete ui;
}

void CameraForm::updateImage(const QImage &img)
{
    ui->imageLabel->setPixmap(QPixmap::fromImage(img.scaled(ui->imageLabel->size())));
    return;
}

void CameraForm::onAddDevice()
{
    disconnect(ui->resolutionComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
               this, &CameraForm::onResolutionChanged);
    disconnect(ui->devComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
               this, &CameraForm::onDeviceChanged);
    if (cameraManager != nullptr) {
        stopCapture();
        delete cameraManager;
    }

    cameraManager = new Camera::Manager([this](int h, int w, int c, unsigned char* data){
        if (c == 3) {
            QImage img(data, w, h, QImage::Format_RGB888);
            emit sendImage(img);
        } else if (c == 4){
            QImage img(data, w, h, QImage::Format_ARGB32);
            emit sendImage(img);
        }
    });
    std::vector<std::wstring> devList = cameraManager->getDeviceList();
    for (std::wstring& dev: devList) {
        ui->devComboBox->addItem(QString::fromWCharArray(dev.c_str()));
    }

    std::vector<std::string> resList = cameraManager->getResolutionList();
    for (std::string& res: resList) {
        ui->resolutionComboBox->addItem(QString::fromStdString(res));
    }

    connect(ui->resolutionComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CameraForm::onResolutionChanged);
    connect(ui->devComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CameraForm::onDeviceChanged);
    startCapture();
    return;
}

void CameraForm::onRemoveDevice()
{
    if (cameraManager != nullptr) {
        stopCapture();
        delete cameraManager;
        cameraManager = nullptr;
    }

    disconnect(ui->resolutionComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
               this, &CameraForm::onResolutionChanged);
    disconnect(ui->devComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
               this, &CameraForm::onDeviceChanged);

    ui->devComboBox->clear();
    ui->resolutionComboBox->clear();
    return;
}

void CameraForm::onResolutionChanged(int resolutionNum)
{
    bool wasCapturing = m_isCapturing;
    stopCapture();
    cameraManager->onResolutionChanged(resolutionNum);
    if (wasCapturing) {
        startCapture();
    }
}

void CameraForm::onDeviceChanged(int deviceNum)
{
    bool wasCapturing = m_isCapturing;
    stopCapture();

    cameraManager->onDeviceChanged(deviceNum);

    ui->resolutionComboBox->clear();
    std::vector<std::string> resList = cameraManager->getResolutionList();
    for (std::string& res : resList) {
        ui->resolutionComboBox->addItem(QString::fromStdString(res));
    }

    if (wasCapturing) {
        startCapture();
    }
    return;
}

void CameraForm::startCapture()
{
    ui->startBtn->setEnabled(false);
    ui->stopBtn->setEnabled(true);

    if (cameraManager->startCapture()) {
        m_isCapturing = true;
    }
    return;
}

void CameraForm::stopCapture()
{
    ui->startBtn->setEnabled(true);
    ui->stopBtn->setEnabled(false);
    if (cameraManager->stopCapture()) {
        m_isCapturing = false;
    }
    return;
}

void CameraForm::closeEvent(QCloseEvent *ev)
{
    if (cameraManager != nullptr) {
        cameraManager->stopCapture();
    }
    return;
}
