#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QFileInfo>

SettingsDialog::SettingsDialog(Camera::Device *dev_, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog),
    dev(dev_)
{
    ui->setupUi(this);
    setWindowTitle("Camera Parameters");
    connect(ui->saveBtn, &QPushButton::clicked, this, [=](){
        saveParams("cameraparams.xml");
    });
    connect(ui->loadBtn, &QPushButton::clicked, this, [=](){
        QString filePath = QFileDialog::getOpenFileName(this, "select file", ".", "*.xml");
        if (filePath.isEmpty()) {
            return;
        }
        QFileInfo info(filePath);
        filePath = info.absoluteFilePath();
        loadParams(filePath);
        return;
    });
    dev->getParams(cameraParams);
    updateParams();
    /* brightness */
    connect(ui->brightnessSlider, &QSlider::valueChanged, this, [=](int value){
        ui->brightnessSpinBox->setValue(value);
        onBrightnessChanged(value, Camera::Param_Manual);
    });
    connect(ui->brightnessSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int value){
         ui->brightnessSlider->setValue(value);
         onBrightnessChanged(value, Camera::Param_Manual);
    });
    connect(ui->brightnessSpinBox, QOverload<const QString&>::of(&QSpinBox::valueChanged), this, [=](const QString &value){
        int valueInt = value.toInt();
        ui->brightnessSlider->setValue(valueInt);
        onBrightnessChanged(valueInt, Camera::Param_Manual);
    });
    connect(ui->brightnessCheckBox, &QCheckBox::stateChanged, this, [=](int state){
        ui->brightnessSlider->setEnabled(state == Qt::Unchecked);
        ui->brightnessSpinBox->setEnabled(state == Qt::Unchecked);
        int value = ui->brightnessSlider->value();
        onBrightnessChanged(value, state == Qt::Checked ? Camera::Param_Auto : Camera::Param_Manual);
    });
    /* contrast */
    connect(ui->contrastSlider, &QSlider::valueChanged, this, [=](int value){
        ui->contrastSpinBox->setValue(value);
        onContrastChanged(value, Camera::Param_Manual);
    });
    connect(ui->contrastSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int value){
        ui->contrastSlider->setValue(value);
        onContrastChanged(value, Camera::Param_Manual);
    });
    connect(ui->contrastSpinBox, QOverload<const QString&>::of(&QSpinBox::valueChanged), this, [=](const QString &value){
        int valueInt = value.toInt();
        ui->contrastSlider->setValue(valueInt);
        onContrastChanged(valueInt, Camera::Param_Manual);
    });
    connect(ui->contrastCheckBox, &QCheckBox::stateChanged, this, [=](int state){
        ui->contrastSlider->setEnabled(state == Qt::Unchecked);
        ui->contrastSpinBox->setEnabled(state == Qt::Unchecked);
        int value = ui->contrastSlider->value();
        onContrastChanged(value, state == Qt::Checked ? Camera::Param_Auto : Camera::Param_Manual);
    });
    /* hue */
    connect(ui->hueSlider, &QSlider::valueChanged, this, [=](int value){
        ui->hueSpinBox->setValue(value);
        onHueChanged(value, Camera::Param_Manual);
    });
    connect(ui->hueSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int value){
        ui->hueSlider->setValue(value);
        onHueChanged(value, Camera::Param_Manual);
    });
    connect(ui->hueSpinBox, QOverload<const QString&>::of(&QSpinBox::valueChanged), this, [=](const QString &value){
        int valueInt = value.toInt();
        ui->hueSlider->setValue(valueInt);
        onHueChanged(valueInt, Camera::Param_Manual);
    });
    connect(ui->hueCheckBox, &QCheckBox::stateChanged, this, [=](int state){
        ui->hueSlider->setEnabled(state == Qt::Unchecked);
        ui->hueSpinBox->setEnabled(state == Qt::Unchecked);
        int value = ui->hueSlider->value();
        onHueChanged(value, state == Qt::Checked ? Camera::Param_Auto : Camera::Param_Manual);
    });
    /* saturation */
    connect(ui->saturationSlider, &QSlider::valueChanged, this, [=](int value){
        ui->saturationSpinBox->setValue(value);
        onSaturationChanged(value, Camera::Param_Manual);
    });
    connect(ui->saturationSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int value){
        ui->saturationSlider->setValue(value);
        onSaturationChanged(value, Camera::Param_Manual);
    });
    connect(ui->saturationSpinBox, QOverload<const QString&>::of(&QSpinBox::valueChanged), this, [=](const QString &value){
        int valueInt = value.toInt();
        ui->saturationSlider->setValue(valueInt);
        onSaturationChanged(valueInt, Camera::Param_Manual);
    });
    connect(ui->saturationCheckBox, &QCheckBox::stateChanged, this, [=](int state){
        ui->saturationSlider->setEnabled(state == Qt::Unchecked);
        ui->saturationSpinBox->setEnabled(state == Qt::Unchecked);
        int value = ui->saturationSlider->value();
        onSaturationChanged(value, state == Qt::Checked ? Camera::Param_Auto : Camera::Param_Manual);
    });
    /* sharpness */
    connect(ui->sharpnessSlider, &QSlider::valueChanged, this, [=](int value){
        ui->sharpnessSpinBox->setValue(value);
        onSharpnessChanged(value, Camera::Param_Manual);
    });
    connect(ui->sharpnessSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int value){
        ui->sharpnessSlider->setValue(value);
        onSharpnessChanged(value, Camera::Param_Manual);
    });
    connect(ui->sharpnessSpinBox, QOverload<const QString&>::of(&QSpinBox::valueChanged), this, [=](const QString &value){
        int valueInt = value.toInt();
        ui->sharpnessSlider->setValue(valueInt);
        onSharpnessChanged(valueInt, Camera::Param_Manual);
    });
    connect(ui->sharpnessCheckBox, &QCheckBox::stateChanged, this, [=](int state){
        ui->sharpnessSlider->setEnabled(state == Qt::Unchecked);
        ui->sharpnessSpinBox->setEnabled(state == Qt::Unchecked);
        int value = ui->sharpnessSlider->value();
        onSharpnessChanged(value, state == Qt::Checked ? Camera::Param_Auto : Camera::Param_Manual);
    });
    /* gamma */
    connect(ui->gammaSlider, &QSlider::valueChanged, this, [=](int value){
        ui->gammaSpinBox->setValue(value);
        onGammaChanged(value, Camera::Param_Manual);
    });
    connect(ui->gammaSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int value){
        ui->gammaSlider->setValue(value);
        onGammaChanged(value, Camera::Param_Manual);
    });
    connect(ui->gammaSpinBox, QOverload<const QString&>::of(&QSpinBox::valueChanged), this, [=](const QString &value){
        int valueInt = value.toInt();
        ui->gammaSlider->setValue(valueInt);
        onGammaChanged(valueInt, Camera::Param_Manual);
    });
    connect(ui->gammaCheckBox, &QCheckBox::stateChanged, this, [=](int state){
        ui->gammaSlider->setEnabled(state == Qt::Unchecked);
        ui->gammaSpinBox->setEnabled(state == Qt::Unchecked);
        int value = ui->gammaSlider->value();
        onGammaChanged(value, state == Qt::Checked ? Camera::Param_Auto : Camera::Param_Manual);
    });
    /* whiteBalance */
    connect(ui->whiteBalanceSlider, &QSlider::valueChanged, this, [=](int value){
        ui->whiteBalanceSpinBox->setValue(value);
        onWhiteBalanceChanged(value, Camera::Param_Manual);
    });
    connect(ui->whiteBalanceSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int value){
        ui->whiteBalanceSlider->setValue(value);
        onWhiteBalanceChanged(value, Camera::Param_Manual);
    });
    connect(ui->whiteBalanceSpinBox, QOverload<const QString&>::of(&QSpinBox::valueChanged), this, [=](const QString &value){
        int valueInt = value.toInt();
        ui->whiteBalanceSlider->setValue(valueInt);
        onWhiteBalanceChanged(valueInt, Camera::Param_Manual);
    });
    connect(ui->whiteBalanceCheckBox, &QCheckBox::stateChanged, this, [=](int state){
        ui->whiteBalanceSlider->setEnabled(state == Qt::Unchecked);
        ui->whiteBalanceSpinBox->setEnabled(state == Qt::Unchecked);
        int value = ui->whiteBalanceSlider->value();
        onWhiteBalanceChanged(value, state == Qt::Checked ? Camera::Param_Auto : Camera::Param_Manual);
    });
    /* backlightCompensation */
    connect(ui->backlightCompensationSlider, &QSlider::valueChanged, this, [=](int value){
        ui->backlightCompensationSpinBox->setValue(value);
        onBacklightCompensationChanged(value, Camera::Param_Manual);
    });
    connect(ui->backlightCompensationSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int value){
        ui->backlightCompensationSlider->setValue(value);
        onBacklightCompensationChanged(value, Camera::Param_Manual);
    });
    connect(ui->backlightCompensationSpinBox, QOverload<const QString&>::of(&QSpinBox::valueChanged), this, [=](const QString &value){
        int valueInt = value.toInt();
        ui->backlightCompensationSlider->setValue(valueInt);
        onBacklightCompensationChanged(valueInt, Camera::Param_Manual);
    });
    connect(ui->backlightCompensationCheckBox, &QCheckBox::stateChanged, this, [=](int state){
        ui->backlightCompensationSlider->setEnabled(state == Qt::Unchecked);
        ui->backlightCompensationSpinBox->setEnabled(state == Qt::Unchecked);
        int value = ui->backlightCompensationSlider->value();
        onBacklightCompensationChanged(value, state == Qt::Checked ? Camera::Param_Auto : Camera::Param_Manual);
    });
    /* gain */
    connect(ui->gainSlider, &QSlider::valueChanged, this, [=](int value){
        ui->gainSpinBox->setValue(value);
        onGainChanged(value, Camera::Param_Manual);
    });
    connect(ui->gainSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int value){
        ui->gainSlider->setValue(value);
        onGainChanged(value, Camera::Param_Manual);
    });
    connect(ui->gainSpinBox, QOverload<const QString&>::of(&QSpinBox::valueChanged), this, [=](const QString &value){
        int valueInt = value.toInt();
        ui->gainSlider->setValue(valueInt);
        onGainChanged(valueInt, Camera::Param_Manual);
    });
    connect(ui->gainCheckBox, &QCheckBox::stateChanged, this, [=](int state){
        ui->gainSlider->setEnabled(state == Qt::Unchecked);
        ui->gainSpinBox->setEnabled(state == Qt::Unchecked);
        int value = ui->gainSlider->value();
        onGainChanged(value, state == Qt::Checked ? Camera::Param_Auto : Camera::Param_Manual);
    });
    /* exposure */
    connect(ui->exposureSlider, &QSlider::valueChanged, this, [=](int value){
        ui->exposureSpinBox->setValue(value);
        onExposureChanged(value, Camera::Param_Manual);
    });
    connect(ui->exposureSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int value){
        ui->exposureSlider->setValue(value);
        onExposureChanged(value, Camera::Param_Manual);
    });
    connect(ui->exposureSpinBox, QOverload<const QString&>::of(&QSpinBox::valueChanged), this, [=](const QString &value){
        int valueInt = value.toInt();
        ui->exposureSlider->setValue(valueInt);
        onExposureChanged(valueInt, Camera::Param_Manual);
    });
    connect(ui->exposureCheckBox, &QCheckBox::stateChanged, this, [=](int state){
        ui->exposureSlider->setEnabled(state == Qt::Unchecked);
        ui->exposureSpinBox->setEnabled(state == Qt::Unchecked);
        int value = ui->exposureSlider->value();
        onExposureChanged(value, state == Qt::Checked ? Camera::Param_Auto : Camera::Param_Manual);
    });
    /* focus */
    connect(ui->focusSlider, &QSlider::valueChanged, this, [=](int value){
        ui->focusSpinBox->setValue(value);
        onFocusChanged(value, Camera::Param_Manual);
    });
    connect(ui->focusSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int value){
        ui->focusSlider->setValue(value);
        onFocusChanged(value, Camera::Param_Manual);
    });
    connect(ui->focusSpinBox, QOverload<const QString&>::of(&QSpinBox::valueChanged), this, [=](const QString &value){
        int valueInt = value.toInt();
        ui->focusSlider->setValue(valueInt);
        onFocusChanged(valueInt, Camera::Param_Manual);
    });
    connect(ui->focusCheckBox, &QCheckBox::stateChanged, this, [=](int state){
        ui->focusSlider->setEnabled(state == Qt::Unchecked);
        ui->focusSpinBox->setEnabled(state == Qt::Unchecked);
        int value = ui->focusSlider->value();
        onFocusChanged(value, state == Qt::Checked ? Camera::Param_Auto : Camera::Param_Manual);
    });
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::updateParams()
{
    /* brightness */
    ui->brightnessSlider->setRange(cameraParams.brightness.minValue, cameraParams.brightness.maxValue);
    ui->brightnessSlider->setValue(cameraParams.brightness.value);
    ui->brightnessSpinBox->setValue(cameraParams.brightness.value);
    ui->brightnessSpinBox->setRange(cameraParams.brightness.minValue, cameraParams.brightness.maxValue);
    ui->brightnessCheckBox->setChecked(cameraParams.brightness.flag == Camera::Param_Auto);
    ui->brightnessSlider->setEnabled(cameraParams.brightness.flag == Camera::Param_Manual);
    ui->brightnessSpinBox->setEnabled(cameraParams.brightness.flag == Camera::Param_Manual);
    /* contrast */
    ui->contrastSlider->setRange(cameraParams.contrast.minValue, cameraParams.contrast.maxValue);
    ui->contrastSlider->setValue(cameraParams.contrast.value);
    ui->contrastSpinBox->setValue(cameraParams.contrast.value);
    ui->contrastSpinBox->setRange(cameraParams.contrast.minValue, cameraParams.contrast.maxValue);
    ui->contrastCheckBox->setChecked(cameraParams.contrast.flag == Camera::Param_Auto);
    ui->contrastSlider->setEnabled(cameraParams.contrast.flag == Camera::Param_Manual);
    ui->contrastSpinBox->setEnabled(cameraParams.contrast.flag == Camera::Param_Manual);
    /* hue */
    ui->hueSlider->setRange(cameraParams.hue.minValue, cameraParams.hue.maxValue);
    ui->hueSlider->setValue(cameraParams.hue.value);
    ui->hueSpinBox->setValue(cameraParams.hue.value);
    ui->hueSpinBox->setRange(cameraParams.hue.minValue, cameraParams.hue.maxValue);
    ui->hueCheckBox->setChecked(cameraParams.hue.flag == Camera::Param_Auto);
    ui->hueSlider->setEnabled(cameraParams.hue.flag == Camera::Param_Manual);
    ui->hueSpinBox->setEnabled(cameraParams.hue.flag == Camera::Param_Manual);
    /* saturation */
    ui->saturationSlider->setRange(cameraParams.saturation.minValue, cameraParams.saturation.maxValue);
    ui->saturationSlider->setValue(cameraParams.saturation.value);
    ui->saturationSpinBox->setValue(cameraParams.saturation.value);
    ui->saturationSpinBox->setRange(cameraParams.saturation.minValue, cameraParams.saturation.maxValue);
    ui->saturationCheckBox->setChecked(cameraParams.saturation.flag == Camera::Param_Auto);
    ui->saturationSlider->setEnabled(cameraParams.saturation.flag == Camera::Param_Manual);
    ui->saturationSpinBox->setEnabled(cameraParams.saturation.flag == Camera::Param_Manual);
    /* sharpness */
    ui->sharpnessSlider->setRange(cameraParams.sharpness.minValue, cameraParams.sharpness.maxValue);
    ui->sharpnessSlider->setValue(cameraParams.sharpness.value);
    ui->sharpnessSpinBox->setValue(cameraParams.sharpness.value);
    ui->sharpnessSpinBox->setRange(cameraParams.sharpness.minValue, cameraParams.sharpness.maxValue);
    ui->sharpnessCheckBox->setChecked(cameraParams.sharpness.flag == Camera::Param_Auto);
    ui->sharpnessSlider->setEnabled(cameraParams.sharpness.flag == Camera::Param_Manual);
    ui->sharpnessSpinBox->setEnabled(cameraParams.sharpness.flag == Camera::Param_Manual);
    /* gamma */
    ui->gammaSlider->setRange(cameraParams.gamma.minValue, cameraParams.gamma.maxValue);
    ui->gammaSlider->setValue(cameraParams.gamma.value);
    ui->gammaSpinBox->setValue(cameraParams.gamma.value);
    ui->gammaSpinBox->setRange(cameraParams.gamma.minValue, cameraParams.gamma.maxValue);
    ui->gammaCheckBox->setChecked(cameraParams.gamma.flag == Camera::Param_Auto);
    ui->gammaSlider->setEnabled(cameraParams.gamma.flag == Camera::Param_Manual);
    ui->gammaSpinBox->setEnabled(cameraParams.gamma.flag == Camera::Param_Manual);
    /* white balance */
    ui->whiteBalanceSlider->setRange(cameraParams.whiteBalance.minValue, cameraParams.whiteBalance.maxValue);
    ui->whiteBalanceSlider->setValue(cameraParams.whiteBalance.value);
    ui->whiteBalanceSpinBox->setValue(cameraParams.whiteBalance.value);
    ui->whiteBalanceSpinBox->setRange(cameraParams.whiteBalance.minValue, cameraParams.whiteBalance.maxValue);
    ui->whiteBalanceCheckBox->setChecked(cameraParams.whiteBalance.flag == Camera::Param_Auto);
    ui->whiteBalanceSlider->setEnabled(cameraParams.whiteBalance.flag == Camera::Param_Manual);
    ui->whiteBalanceSpinBox->setEnabled(cameraParams.whiteBalance.flag == Camera::Param_Manual);
    /* backlightCompensation */
    ui->backlightCompensationSlider->setRange(cameraParams.backlightCompensation.minValue, cameraParams.backlightCompensation.maxValue);
    ui->backlightCompensationSlider->setValue(cameraParams.backlightCompensation.value);
    ui->backlightCompensationSpinBox->setValue(cameraParams.backlightCompensation.value);
    ui->backlightCompensationSpinBox->setRange(cameraParams.backlightCompensation.minValue, cameraParams.backlightCompensation.maxValue);
    ui->backlightCompensationCheckBox->setChecked(cameraParams.backlightCompensation.flag == Camera::Param_Auto);
    ui->backlightCompensationSlider->setEnabled(cameraParams.backlightCompensation.flag == Camera::Param_Manual);
    ui->backlightCompensationSpinBox->setEnabled(cameraParams.backlightCompensation.flag == Camera::Param_Manual);
    /* gain */
    ui->gainSlider->setRange(cameraParams.gain.minValue, cameraParams.gain.maxValue);
    ui->gainSlider->setValue(cameraParams.gain.value);
    ui->gainSpinBox->setValue(cameraParams.gain.value);
    ui->gainSpinBox->setRange(cameraParams.gain.minValue, cameraParams.gain.maxValue);
    ui->gainCheckBox->setChecked(cameraParams.gain.flag == Camera::Param_Auto);
    ui->gainSlider->setEnabled(cameraParams.gain.flag == Camera::Param_Manual);
    ui->gainSpinBox->setEnabled(cameraParams.gain.flag == Camera::Param_Manual);
    /* exposure */
    ui->exposureSlider->setRange(cameraParams.exposure.minValue, cameraParams.exposure.maxValue);
    ui->exposureSlider->setValue(cameraParams.exposure.value);
    ui->exposureSpinBox->setValue(cameraParams.exposure.value);
    ui->exposureSpinBox->setRange(cameraParams.exposure.minValue, cameraParams.exposure.maxValue);
    ui->exposureCheckBox->setChecked(cameraParams.exposure.flag == Camera::Param_Auto);
    ui->exposureSlider->setEnabled(cameraParams.exposure.flag == Camera::Param_Manual);
    ui->exposureSpinBox->setEnabled(cameraParams.exposure.flag == Camera::Param_Manual);
    /* focus */
    ui->focusSlider->setRange(cameraParams.focus.minValue, cameraParams.focus.maxValue);
    ui->focusSlider->setValue(cameraParams.focus.value);
    ui->focusSpinBox->setValue(cameraParams.focus.value);
    ui->focusSpinBox->setRange(cameraParams.focus.minValue, cameraParams.focus.maxValue);
    ui->focusCheckBox->setChecked(cameraParams.focus.flag == Camera::Param_Auto);
    ui->focusSlider->setEnabled(cameraParams.focus.flag == Camera::Param_Manual);
    ui->focusSpinBox->setEnabled(cameraParams.focus.flag == Camera::Param_Manual);
    return;
}

void SettingsDialog::setParams()
{
    dev->setParams(cameraParams);
    updateParams();
    return;
}

void SettingsDialog::saveParams(const QString &fileName)
{
    /* open file */
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly|QFile::Truncate)) {
        return;
    }
    /* get camera params */
    dev->getParams(cameraParams);
    QDomDocument doc;
    /* instruction */
    QDomProcessingInstruction instruction;
    instruction=doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(instruction);
    /* root */
    QDomElement root = doc.createElement("Camera");
    doc.appendChild(root);
    auto insertElement = [&](const QString &key, const QString &value, QDomElement &parentElement) {
        QDomElement element = doc.createElement(key);
        element.appendChild(doc.createTextNode(value));
        parentElement.appendChild(element);
    };
    /* add node and element */
    QDomElement param = doc.createElement("Parameter");
    insertElement("AutoBrightness",
                  QString::number(cameraParams.brightness.flag), param);
    insertElement("Brightness",
                  QString::number(cameraParams.brightness.value), param);
    insertElement("AutoContrast",
                  QString::number(cameraParams.contrast.flag), param);
    insertElement("Contrast",
                  QString::number(cameraParams.contrast.value), param);
    insertElement("AutoHue",
                  QString::number(cameraParams.hue.flag), param);
    insertElement("Hue",
                  QString::number(cameraParams.hue.value), param);
    insertElement("AutoSaturation",
                  QString::number(cameraParams.saturation.flag), param);
    insertElement("Saturation",
                  QString::number(cameraParams.saturation.value), param);
    insertElement("AutoSharpness",
                  QString::number(cameraParams.sharpness.flag), param);
    insertElement("Sharpness",
                  QString::number(cameraParams.sharpness.value), param);
    insertElement("AutoGamma",
                  QString::number(cameraParams.gamma.flag), param);
    insertElement("Gamma",
                  QString::number(cameraParams.gamma.value), param);
    insertElement("AutoWhiteBalance",
                  QString::number(cameraParams.whiteBalance.flag), param);
    insertElement("WhiteBalance",
                  QString::number(cameraParams.whiteBalance.value), param);
    insertElement("AutoBacklightCompensation",
                  QString::number(cameraParams.backlightCompensation.flag), param);
    insertElement("BacklightCompensation",
                  QString::number(cameraParams.backlightCompensation.value), param);
    insertElement("AutoGain",
                  QString::number(cameraParams.gain.flag), param);
    insertElement("Gain",
                  QString::number(cameraParams.gain.value), param);
    insertElement("AutoExposure",
                  QString::number(cameraParams.exposure.flag), param);
    insertElement("Exposure",
                  QString::number(cameraParams.exposure.value), param);
    insertElement("AutoFocus",
                  QString::number(cameraParams.exposure.flag), param);
    insertElement("Focus",
                  QString::number(cameraParams.exposure.value), param);
    /* add node */
    root.appendChild(param);
    /* output */
    QTextStream out(&file);
    doc.save(out, 4);
    file.close();
    return;
}

bool SettingsDialog::loadParams(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        return false;
    }
    QDomDocument doc;
    if (!doc.setContent(&file))  {
        file.close();
        return false;
    }
    file.close();
    /* root */
    QDomElement root = doc.documentElement();
    QDomNode node = root.firstChild();
    while (!node.isNull()) {
        if (node.isElement()) {
            QDomElement e = node.toElement();
            if (e.tagName() == "Parameter") {
                QDomNodeList list=e.childNodes();
                for (int i = 0; i < list.count(); i++) {
                    QDomNode n = list.at(i);
                    if (n.isElement() == true) {
                        QString param = n.nodeName();
                        int value = n.toElement().text().toInt();
                        if (param == "AutoBrightness") {
                            cameraParams.brightness.flag = value;
                        } else if (param == "Brightness") {
                            cameraParams.brightness.value = value;
                        } else if (param == "AutoContrast") {
                            cameraParams.contrast.flag = value;
                        } else if (param == "Contrast") {
                            cameraParams.contrast.value = value;
                        } else if (param == "AutoHue") {
                            cameraParams.hue.flag = value;
                        } else if (param == "Hue") {
                            cameraParams.hue.value = value;
                        } else if (param == "AutoSaturation") {
                            cameraParams.saturation.flag = value;
                        } else if (param == "Saturation") {
                            cameraParams.saturation.value = value;
                        } else if (param == "AutoSharpness") {
                            cameraParams.sharpness.flag = value;
                        } else if (param == "Sharpness") {
                            cameraParams.sharpness.value = value;
                        } else if (param == "AutoGamma") {
                            cameraParams.gamma.flag = value;
                        } else if (param == "Gamma") {
                            cameraParams.gamma.value = value;
                        } else if (param == "AutoWhiteBalanceMode") {
                            cameraParams.whiteBalance.flag = value;
                        } else if (param == "WhiteBalance") {
                            cameraParams.whiteBalance.value = value;
                        } else if (param == "AutoBacklightCompensation") {
                            cameraParams.backlightCompensation.flag = value;
                        } else if (param == "BacklightCompensation") {
                            cameraParams.backlightCompensation.value = value;
                        } else if (param == "AutoGain") {
                            cameraParams.gain.flag = value;
                        } else if (param == "Gain") {
                            cameraParams.gain.value = value;
                        } else if (param == "AutoExposure") {
                            cameraParams.exposure.flag = value;
                        } else if (param == "Exposure") {
                            cameraParams.exposure.value = value;
                        } else if (param == "AutoFocus") {
                            cameraParams.focus.flag = value;
                        } else if (param == "Focus") {
                            cameraParams.focus.value = value;
                        }
                    }
                }
            }
        }
        node = node.nextSibling();
    }

    /* set params */
    setParams();
    return true;
}

void SettingsDialog::onBrightnessChanged(int value, int flag)
{
    if (!QFile::exists("cameraparams_default.xml")) {
        saveParams("cameraparams_default.xml");
    }
    Camera::Param param;
    dev->getBrightness(param.value, param.flag);
    if (param.value == value && flag == Camera::Param_Manual) {
        return;
    }
    if (flag == Camera::Param_Manual) {
        dev->setBrightness(value, Camera::Param_Manual);
    } else {
        dev->setBrightness(param.value, Camera::Param_Auto);
    }
    return;
}

void SettingsDialog::onContrastChanged(int value, int flag)
{
    if (!QFile::exists("cameraparams_default.xml")) {
        saveParams("cameraparams_default.xml");
    }
    Camera::Param param;
    dev->getContrast(param.value, param.flag);
    if (param.value == value && flag == Camera::Param_Manual) {
        return;
    }
    if (flag == Camera::Param_Manual) {
        dev->setContrast(value, Camera::Param_Manual);
    } else {
        dev->setContrast(param.value, Camera::Param_Auto);
    }
    return;
}

void SettingsDialog::onHueChanged(int value, int flag)
{
    if (!QFile::exists("cameraparams_default.xml")) {
        saveParams("cameraparams_default.xml");
    }
    Camera::Param param;
    dev->getHue(param.value, param.flag);
    if (param.value == value && flag == Camera::Param_Manual) {
        return;
    }
    if (flag == Camera::Param_Manual) {
        dev->setHue(value, Camera::Param_Manual);
    } else {
        dev->setHue(param.value, Camera::Param_Auto);
    }
    return;
}

void SettingsDialog::onSaturationChanged(int value, int flag)
{
    if (!QFile::exists("cameraparams_default.xml")) {
        saveParams("cameraparams_default.xml");
    }
    Camera::Param param;
    dev->getSaturation(param.value, param.flag);
    if (param.value == value && flag == Camera::Param_Manual) {
        return;
    }
    if (flag == Camera::Param_Manual) {
        dev->setSaturation(value, Camera::Param_Manual);
    } else {
        dev->setSaturation(param.value, Camera::Param_Auto);
    }
    return;
}

void SettingsDialog::onSharpnessChanged(int value, int flag)
{
    if (!QFile::exists("cameraparams_default.xml")) {
        saveParams("cameraparams_default.xml");
    }
    Camera::Param param;
    dev->getSharpness(param.value, param.flag);
    if (param.value == value && flag == Camera::Param_Manual) {
        return;
    }
    if (flag == Camera::Param_Manual) {
        dev->setSharpness(value, Camera::Param_Manual);
    } else {
        dev->setSharpness(param.value, Camera::Param_Auto);
    }
    return;
}

void SettingsDialog::onGammaChanged(int value, int flag)
{
    if (!QFile::exists("cameraparams_default.xml")) {
        saveParams("cameraparams_default.xml");
    }
    Camera::Param param;
    dev->getGamma(param.value, param.flag);
    if (param.value == value && flag == Camera::Param_Manual) {
        return;
    }
    if (flag == Camera::Param_Manual) {
        dev->setGamma(value, Camera::Param_Manual);
    } else {
        dev->setGamma(param.value, Camera::Param_Auto);
    }
    return;
}

void SettingsDialog::onWhiteBalanceChanged(int value, int flag)
{
    if (!QFile::exists("cameraparams_default.xml")) {
        saveParams("cameraparams_default.xml");
    }
    Camera::Param param;
    dev->getWhiteBalance(param.value, param.flag);
    if (param.value == value && flag == Camera::Param_Manual) {
        return;
    }
    if (flag == Camera::Param_Manual) {
        dev->setWhiteBalance(value, Camera::Param_Manual);
    } else {
        dev->setWhiteBalance(param.value, Camera::Param_Auto);
    }
    return;
}

void SettingsDialog::onBacklightCompensationChanged(int value, int flag)
{
    if (!QFile::exists("cameraparams_default.xml")) {
        saveParams("cameraparams_default.xml");
    }
    Camera::Param param;
    dev->getBacklightCompensation(param.value, param.flag);
    if (param.value == value && flag == Camera::Param_Manual) {
        return;
    }
    if (flag == Camera::Param_Manual) {
        dev->setBacklightCompensation(value, Camera::Param_Manual);
    } else {
        dev->setBacklightCompensation(param.value, Camera::Param_Auto);
    }
    return;
}

void SettingsDialog::onGainChanged(int value, int flag)
{
    if (!QFile::exists("cameraparams_default.xml")) {
        saveParams("cameraparams_default.xml");
    }
    Camera::Param param;
    dev->getGain(param.value, param.flag);
    if (param.value == value && flag == Camera::Param_Manual) {
        return;
    }
    if (flag == Camera::Param_Manual) {
        dev->setGain(value, Camera::Param_Manual);
    } else {
        dev->setGain(param.value, Camera::Param_Auto);
    }
    return;
}

void SettingsDialog::onExposureChanged(int value, int flag)
{
    if (!QFile::exists("cameraparams_default.xml")) {
        saveParams("cameraparams_default.xml");
    }
    Camera::Param param;
    dev->getExposure(param.value, param.flag);
    if (param.value == value && flag == Camera::Param_Manual) {
        return;
    }
    if (flag == Camera::Param_Manual) {
        dev->setExposure(value, Camera::Param_Manual);
    } else {
        dev->setExposure(param.value, Camera::Param_Auto);
    }
    return;
}

void SettingsDialog::onFocusChanged(int value, int flag)
{
    if (!QFile::exists("cameraparams_default.xml")) {
        saveParams("cameraparams_default.xml");
    }
    Camera::Param param;
    dev->getFocus(param.value, param.flag);
    if (param.value == value && flag == Camera::Param_Manual) {
        return;
    }
    if (flag == Camera::Param_Manual) {
        dev->setFocus(value, Camera::Param_Manual);
    } else {
        dev->setFocus(param.value, Camera::Param_Auto);
    }
    return;
}


