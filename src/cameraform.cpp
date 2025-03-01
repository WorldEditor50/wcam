#include "cameraform.h"
#include "ui_cameraform.h"
#include <QApplication>
#include <QDateTime>
#include <QDesktopServices>
#include "improcess/improcess.h"
#include "settingsdialog.h"
#include "emitter.h"

CameraForm::CameraForm(QWidget *parent) :
    QWidget(parent),
    isReadyForCapture(false),
    processMethod(imp::Method_None),
    ui(new Ui::CameraForm),
    cameraManager(nullptr)
{
    ui->setupUi(this);

    connect(this, &CameraForm::sendImage,
            this, &CameraForm::updateImage, Qt::QueuedConnection);
    connect(ui->startBtn, &QPushButton::clicked, this, &CameraForm::startCapture);
    connect(ui->stopBtn, &QPushButton::clicked, this, &CameraForm::stopCapture);
    connect(ui->captureBtn, &QPushButton::clicked, this, [=](){
        if (!isReadyForCapture) {
            isReadyForCapture = true;
        }
    });
    connect(ui->settingsBtn, &QPushButton::clicked, this, [=](){
        SettingsDialog settings(cameraManager, this);
        settings.exec();
    });
    ui->processComboBox->addItems(QStringList{"none", "filter"});
    connect(ui->processComboBox, &QComboBox::currentTextChanged, this, [=](const QString &method){
        if (method == "none") {
            processMethod = imp::Method_None;
        } else if (method == "filter") {
            processMethod = imp::Method_Filter;
        }
    });

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
    /* terminate */
    connect(Emitter::ptr(), &Emitter::terminate, this, [=](){
        if (cameraManager != nullptr) {
            cameraManager->stopCapture();
        }
    });
    std::wstring vendor;
    int count = Camera::enumerate(vendor, devList);
    if (count > 0) {
        onAddDevice();
    }
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
    if (isReadyForCapture) {
        isReadyForCapture = false;
        QString fileName = QString("%1/%2.jpg")
                .arg(qApp->applicationDirPath()).arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"));
        img.save(fileName);
        QDesktopServices::openUrl(QUrl(fileName));
    }
    ui->imageLabel->setPixmap(QPixmap::fromImage(img.scaled(ui->imageLabel->size())));
    return;
}

void CameraForm::onAddDevice()
{
    if (devList.empty()) {
        return;
    }
    if (cameraManager != nullptr) {
        return;
    }

    disconnect(ui->resolutionComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
               this, &CameraForm::onResolutionChanged);
    disconnect(ui->devComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
               this, &CameraForm::onDeviceChanged);


    cameraManager = new Camera::Manager(devList, [this](int h, int w, int c, unsigned char* data){
        if (c == 3) {
            if (processMethod == imp::Method_None) {
                emit sendImage(QImage(data, w, h, QImage::Format_RGB888));
            } else if (processMethod == imp::Method_Filter) {
                cv::Mat img(h, w, CV_8UC3, data);
                imp::filter(img, out);
                emit sendImage(QImage(out.data, out.cols, out.rows, QImage::Format_RGB888));
            }
        } else if (c == 4){
            emit sendImage(QImage(data, w, h, QImage::Format_ARGB32));
        }
    });
    std::vector<std::wstring> devNameList = cameraManager->getDeviceList();
    for (std::wstring& dev: devNameList) {
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

    cameraManager->startCapture();
    isStreaming = true;
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
    bool wasStreaming = isStreaming;
    stopCapture();
    cameraManager->onResolutionChanged(resolutionNum);
    if (wasStreaming) {
        startCapture();
    }
}

void CameraForm::onDeviceChanged(int deviceNum)
{
    bool wasStreaming = isStreaming;
    stopCapture();

    cameraManager->onDeviceChanged(deviceNum);

    ui->resolutionComboBox->clear();
    std::vector<std::string> resList = cameraManager->getResolutionList();
    for (std::string& res : resList) {
        ui->resolutionComboBox->addItem(QString::fromStdString(res));
    }

    if (wasStreaming) {
        startCapture();
    }
    return;
}

void CameraForm::startCapture()
{
    ui->startBtn->setEnabled(false);
    ui->stopBtn->setEnabled(true);

    if (cameraManager->startCapture()) {
        isStreaming = true;
    }
    return;
}

void CameraForm::stopCapture()
{
    ui->startBtn->setEnabled(true);
    ui->stopBtn->setEnabled(false);
    if (cameraManager->stopCapture()) {
        isStreaming = false;
    }
    return;
}

