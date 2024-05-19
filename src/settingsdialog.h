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
    void onBrightnessChanged(int value, int flag);
    /* contrast */
    void onContrastChanged(int value, int flag);
    /* hue */
    void onHueChanged(int value, int flag);
    /* saturation */
    void onSaturationChanged(int value, int flag);
    /* sharpness */
    void onSharpnessChanged(int value, int flag);
    /* gamma */
    void onGammaChanged(int value, int flag);
    /* whiteBalance */
    void onWhiteBalanceChanged(int value, int flag);
    /* backlightCompensation */
    void onBacklightCompensationChanged(int value, int flag);
    /* gain */
    void onGainChanged(int value, int flag);
    /* exposure */
    void onExposureChanged(int value, int flag);
    /* focus */
    void onFocusChanged(int value, int flag);
private:
    Ui::SettingsDialog *ui;
    Camera::Manager *cameraManager;
    Camera::Params cameraParams;
};

#endif // SETTINGSDIALOG_H
