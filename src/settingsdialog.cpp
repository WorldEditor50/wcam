#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QFile>
#include <QTextStream>

SettingsDialog::SettingsDialog(Camera::Manager *cameraManager_, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog),
    cameraManager(cameraManager_)
{
    ui->setupUi(this);
    setWindowTitle("Camera Parameters");
    connect(ui->saveBtn, &QPushButton::clicked, this, [=](){
        saveParams("cameraparams.xml");
    });
    connect(ui->loadBtn, &QPushButton::clicked, this, [=](){

    });
    cameraManager->getParams(cameraParams);
    updateParams();
    /* brightness */
    connect(ui->brightnessSlider, &QSlider::valueChanged, this, [=](int value){
        ui->brightnessSpinBox->setValue(value);
    });
    connect(ui->brightnessSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int value){
         ui->brightnessSlider->setValue(value);
    });
    connect(ui->brightnessSpinBox, QOverload<const QString&>::of(&QSpinBox::valueChanged), this, [=](const QString &value){
        ui->brightnessSlider->setValue(value.toInt());
    });
    connect(ui->brightnessCheckBox, &QCheckBox::stateChanged, this, [=](int state){

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
    cameraManager->setParams(cameraParams);
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
    cameraManager->getParams(cameraParams);
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
    Camera::ParamValue param;
    cameraManager->getBrightness(param.value, param.flag);
    if (param.value == value && flag == Camera::Param_Manual) {
        return;
    }
    if (flag == Camera::Param_Manual) {
        cameraManager->setBrightness(value, Camera::Param_Manual);
    } else {
        cameraManager->setBrightness(param.value, Camera::Param_Auto);
    }
    return;
}

void SettingsDialog::onContrastChanged(int value, int isAuto)
{

}

void SettingsDialog::onHueChanged(int value, int isAuto)
{

}

void SettingsDialog::onSaturationChanged(int value, int isAuto)
{

}

void SettingsDialog::onSharpnessChanged(int value, int isAuto)
{

}

void SettingsDialog::onGammaChanged(int value, int isAuto)
{

}

void SettingsDialog::onWhiteBalanceChanged(int value, int isAuto)
{

}

void SettingsDialog::onBacklightCompensationChanged(int value, int isAuto)
{

}

void SettingsDialog::onGainChanged(int value, int isAuto)
{

}

void SettingsDialog::onExposureChanged(int value, int isAuto)
{

}

void SettingsDialog::onFocusChanged(int value, int isAuto)
{

}


