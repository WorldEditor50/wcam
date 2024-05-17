#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QtXml/QDomDocument>
#include <camera/camera.hpp>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(Camera::Manager *cameraManager_, QWidget *parent = nullptr);
    ~SettingsDialog();
    void updateParams();
    void setParams();
    void saveParams(const QString &fileName);
    bool loadParams(const QString &fileName);
public slots:
    /* brightness */
    void onBrightnessChanged(int value, int isAuto);
    /* contrast */
    void onContrastChanged(int value, int isAuto);
    /* hue */
    void onHueChanged(int value, int isAuto);
    /* saturation */
    void onSaturationChanged(int value, int isAuto);
    /* sharpness */
    void onSharpnessChanged(int value, int isAuto);
    /* gamma */
    void onGammaChanged(int value, int isAuto);
    /* whiteBalance */
    void onWhiteBalanceChanged(int value, int isAuto);
    /* backlightCompensation */
    void onBacklightCompensationChanged(int value, int isAuto);
    /* gain */
    void onGainChanged(int value, int isAuto);
    /* exposure */
    void onExposureChanged(int value, int isAuto);
    /* focus */
    void onFocusChanged(int value, int isAuto);
private:
    Ui::SettingsDialog *ui;
    Camera::Manager *cameraManager;
    Camera::Params cameraParams;
};

#endif // SETTINGSDIALOG_H
