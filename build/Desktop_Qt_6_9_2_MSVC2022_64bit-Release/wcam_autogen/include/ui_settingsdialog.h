/********************************************************************************
** Form generated from reading UI file 'settingsdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.9.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGSDIALOG_H
#define UI_SETTINGSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SettingsDialog
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *brightnessLabel;
    QSlider *brightnessSlider;
    QSpinBox *brightnessSpinBox;
    QCheckBox *brightnessCheckBox;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_2;
    QLabel *contrastLabel;
    QSlider *contrastSlider;
    QSpinBox *contrastSpinBox;
    QCheckBox *contrastCheckBox;
    QSpacerItem *horizontalSpacer_2;
    QHBoxLayout *horizontalLayout_3;
    QLabel *hueLabel;
    QSlider *hueSlider;
    QSpinBox *hueSpinBox;
    QCheckBox *hueCheckBox;
    QSpacerItem *horizontalSpacer_3;
    QHBoxLayout *horizontalLayout_4;
    QLabel *saturationLabel;
    QSlider *saturationSlider;
    QSpinBox *saturationSpinBox;
    QCheckBox *saturationCheckBox;
    QSpacerItem *horizontalSpacer_4;
    QHBoxLayout *horizontalLayout_5;
    QLabel *sharpnessLabel;
    QSlider *sharpnessSlider;
    QSpinBox *sharpnessSpinBox;
    QCheckBox *sharpnessCheckBox;
    QSpacerItem *horizontalSpacer_5;
    QHBoxLayout *horizontalLayout_6;
    QLabel *gammaLabel;
    QSlider *gammaSlider;
    QSpinBox *gammaSpinBox;
    QCheckBox *gammaCheckBox;
    QSpacerItem *horizontalSpacer_6;
    QHBoxLayout *horizontalLayout_7;
    QLabel *whiteBalanceLabel;
    QSlider *whiteBalanceSlider;
    QSpinBox *whiteBalanceSpinBox;
    QCheckBox *whiteBalanceCheckBox;
    QSpacerItem *horizontalSpacer_7;
    QHBoxLayout *horizontalLayout_8;
    QLabel *backlightCompensationLabel;
    QSlider *backlightCompensationSlider;
    QSpinBox *backlightCompensationSpinBox;
    QCheckBox *backlightCompensationCheckBox;
    QSpacerItem *horizontalSpacer_8;
    QHBoxLayout *horizontalLayout_9;
    QLabel *gainLabel;
    QSlider *gainSlider;
    QSpinBox *gainSpinBox;
    QCheckBox *gainCheckBox;
    QSpacerItem *horizontalSpacer_9;
    QHBoxLayout *horizontalLayout_10;
    QLabel *exposureLabel;
    QSlider *exposureSlider;
    QSpinBox *exposureSpinBox;
    QCheckBox *exposureCheckBox;
    QSpacerItem *horizontalSpacer_10;
    QHBoxLayout *horizontalLayout_11;
    QLabel *focusLabel;
    QSlider *focusSlider;
    QSpinBox *focusSpinBox;
    QCheckBox *focusCheckBox;
    QSpacerItem *horizontalSpacer_11;
    QWidget *controlWidget;
    QHBoxLayout *horizontalLayout_12;
    QSpacerItem *horizontalSpacer_12;
    QPushButton *loadBtn;
    QPushButton *saveBtn;
    QSpacerItem *verticalSpacer;

    void setupUi(QDialog *SettingsDialog)
    {
        if (SettingsDialog->objectName().isEmpty())
            SettingsDialog->setObjectName("SettingsDialog");
        SettingsDialog->resize(550, 404);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(SettingsDialog->sizePolicy().hasHeightForWidth());
        SettingsDialog->setSizePolicy(sizePolicy);
        SettingsDialog->setMinimumSize(QSize(550, 400));
        SettingsDialog->setStyleSheet(QString::fromUtf8("QLabel {\n"
"	min-width:140px;\n"
"}\n"
"QSlider {\n"
"	min-width:200px;\n"
"}\n"
"QSpinBox {\n"
"	min-width:40px;\n"
"}"));
        verticalLayout = new QVBoxLayout(SettingsDialog);
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        brightnessLabel = new QLabel(SettingsDialog);
        brightnessLabel->setObjectName("brightnessLabel");

        horizontalLayout->addWidget(brightnessLabel);

        brightnessSlider = new QSlider(SettingsDialog);
        brightnessSlider->setObjectName("brightnessSlider");
        brightnessSlider->setOrientation(Qt::Horizontal);

        horizontalLayout->addWidget(brightnessSlider);

        brightnessSpinBox = new QSpinBox(SettingsDialog);
        brightnessSpinBox->setObjectName("brightnessSpinBox");

        horizontalLayout->addWidget(brightnessSpinBox);

        brightnessCheckBox = new QCheckBox(SettingsDialog);
        brightnessCheckBox->setObjectName("brightnessCheckBox");

        horizontalLayout->addWidget(brightnessCheckBox);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        contrastLabel = new QLabel(SettingsDialog);
        contrastLabel->setObjectName("contrastLabel");

        horizontalLayout_2->addWidget(contrastLabel);

        contrastSlider = new QSlider(SettingsDialog);
        contrastSlider->setObjectName("contrastSlider");
        contrastSlider->setOrientation(Qt::Horizontal);

        horizontalLayout_2->addWidget(contrastSlider);

        contrastSpinBox = new QSpinBox(SettingsDialog);
        contrastSpinBox->setObjectName("contrastSpinBox");

        horizontalLayout_2->addWidget(contrastSpinBox);

        contrastCheckBox = new QCheckBox(SettingsDialog);
        contrastCheckBox->setObjectName("contrastCheckBox");

        horizontalLayout_2->addWidget(contrastCheckBox);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        hueLabel = new QLabel(SettingsDialog);
        hueLabel->setObjectName("hueLabel");

        horizontalLayout_3->addWidget(hueLabel);

        hueSlider = new QSlider(SettingsDialog);
        hueSlider->setObjectName("hueSlider");
        hueSlider->setOrientation(Qt::Horizontal);

        horizontalLayout_3->addWidget(hueSlider);

        hueSpinBox = new QSpinBox(SettingsDialog);
        hueSpinBox->setObjectName("hueSpinBox");

        horizontalLayout_3->addWidget(hueSpinBox);

        hueCheckBox = new QCheckBox(SettingsDialog);
        hueCheckBox->setObjectName("hueCheckBox");

        horizontalLayout_3->addWidget(hueCheckBox);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        saturationLabel = new QLabel(SettingsDialog);
        saturationLabel->setObjectName("saturationLabel");

        horizontalLayout_4->addWidget(saturationLabel);

        saturationSlider = new QSlider(SettingsDialog);
        saturationSlider->setObjectName("saturationSlider");
        saturationSlider->setOrientation(Qt::Horizontal);

        horizontalLayout_4->addWidget(saturationSlider);

        saturationSpinBox = new QSpinBox(SettingsDialog);
        saturationSpinBox->setObjectName("saturationSpinBox");

        horizontalLayout_4->addWidget(saturationSpinBox);

        saturationCheckBox = new QCheckBox(SettingsDialog);
        saturationCheckBox->setObjectName("saturationCheckBox");

        horizontalLayout_4->addWidget(saturationCheckBox);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_4);


        verticalLayout->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        sharpnessLabel = new QLabel(SettingsDialog);
        sharpnessLabel->setObjectName("sharpnessLabel");

        horizontalLayout_5->addWidget(sharpnessLabel);

        sharpnessSlider = new QSlider(SettingsDialog);
        sharpnessSlider->setObjectName("sharpnessSlider");
        sharpnessSlider->setOrientation(Qt::Horizontal);

        horizontalLayout_5->addWidget(sharpnessSlider);

        sharpnessSpinBox = new QSpinBox(SettingsDialog);
        sharpnessSpinBox->setObjectName("sharpnessSpinBox");

        horizontalLayout_5->addWidget(sharpnessSpinBox);

        sharpnessCheckBox = new QCheckBox(SettingsDialog);
        sharpnessCheckBox->setObjectName("sharpnessCheckBox");

        horizontalLayout_5->addWidget(sharpnessCheckBox);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_5);


        verticalLayout->addLayout(horizontalLayout_5);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        gammaLabel = new QLabel(SettingsDialog);
        gammaLabel->setObjectName("gammaLabel");

        horizontalLayout_6->addWidget(gammaLabel);

        gammaSlider = new QSlider(SettingsDialog);
        gammaSlider->setObjectName("gammaSlider");
        gammaSlider->setOrientation(Qt::Horizontal);

        horizontalLayout_6->addWidget(gammaSlider);

        gammaSpinBox = new QSpinBox(SettingsDialog);
        gammaSpinBox->setObjectName("gammaSpinBox");

        horizontalLayout_6->addWidget(gammaSpinBox);

        gammaCheckBox = new QCheckBox(SettingsDialog);
        gammaCheckBox->setObjectName("gammaCheckBox");

        horizontalLayout_6->addWidget(gammaCheckBox);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_6);


        verticalLayout->addLayout(horizontalLayout_6);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName("horizontalLayout_7");
        whiteBalanceLabel = new QLabel(SettingsDialog);
        whiteBalanceLabel->setObjectName("whiteBalanceLabel");

        horizontalLayout_7->addWidget(whiteBalanceLabel);

        whiteBalanceSlider = new QSlider(SettingsDialog);
        whiteBalanceSlider->setObjectName("whiteBalanceSlider");
        whiteBalanceSlider->setOrientation(Qt::Horizontal);

        horizontalLayout_7->addWidget(whiteBalanceSlider);

        whiteBalanceSpinBox = new QSpinBox(SettingsDialog);
        whiteBalanceSpinBox->setObjectName("whiteBalanceSpinBox");

        horizontalLayout_7->addWidget(whiteBalanceSpinBox);

        whiteBalanceCheckBox = new QCheckBox(SettingsDialog);
        whiteBalanceCheckBox->setObjectName("whiteBalanceCheckBox");

        horizontalLayout_7->addWidget(whiteBalanceCheckBox);

        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer_7);


        verticalLayout->addLayout(horizontalLayout_7);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName("horizontalLayout_8");
        backlightCompensationLabel = new QLabel(SettingsDialog);
        backlightCompensationLabel->setObjectName("backlightCompensationLabel");

        horizontalLayout_8->addWidget(backlightCompensationLabel);

        backlightCompensationSlider = new QSlider(SettingsDialog);
        backlightCompensationSlider->setObjectName("backlightCompensationSlider");
        backlightCompensationSlider->setOrientation(Qt::Horizontal);

        horizontalLayout_8->addWidget(backlightCompensationSlider);

        backlightCompensationSpinBox = new QSpinBox(SettingsDialog);
        backlightCompensationSpinBox->setObjectName("backlightCompensationSpinBox");

        horizontalLayout_8->addWidget(backlightCompensationSpinBox);

        backlightCompensationCheckBox = new QCheckBox(SettingsDialog);
        backlightCompensationCheckBox->setObjectName("backlightCompensationCheckBox");

        horizontalLayout_8->addWidget(backlightCompensationCheckBox);

        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_8->addItem(horizontalSpacer_8);


        verticalLayout->addLayout(horizontalLayout_8);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName("horizontalLayout_9");
        gainLabel = new QLabel(SettingsDialog);
        gainLabel->setObjectName("gainLabel");

        horizontalLayout_9->addWidget(gainLabel);

        gainSlider = new QSlider(SettingsDialog);
        gainSlider->setObjectName("gainSlider");
        gainSlider->setOrientation(Qt::Horizontal);

        horizontalLayout_9->addWidget(gainSlider);

        gainSpinBox = new QSpinBox(SettingsDialog);
        gainSpinBox->setObjectName("gainSpinBox");

        horizontalLayout_9->addWidget(gainSpinBox);

        gainCheckBox = new QCheckBox(SettingsDialog);
        gainCheckBox->setObjectName("gainCheckBox");

        horizontalLayout_9->addWidget(gainCheckBox);

        horizontalSpacer_9 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_9->addItem(horizontalSpacer_9);


        verticalLayout->addLayout(horizontalLayout_9);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName("horizontalLayout_10");
        exposureLabel = new QLabel(SettingsDialog);
        exposureLabel->setObjectName("exposureLabel");

        horizontalLayout_10->addWidget(exposureLabel);

        exposureSlider = new QSlider(SettingsDialog);
        exposureSlider->setObjectName("exposureSlider");
        exposureSlider->setOrientation(Qt::Horizontal);

        horizontalLayout_10->addWidget(exposureSlider);

        exposureSpinBox = new QSpinBox(SettingsDialog);
        exposureSpinBox->setObjectName("exposureSpinBox");

        horizontalLayout_10->addWidget(exposureSpinBox);

        exposureCheckBox = new QCheckBox(SettingsDialog);
        exposureCheckBox->setObjectName("exposureCheckBox");

        horizontalLayout_10->addWidget(exposureCheckBox);

        horizontalSpacer_10 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_10->addItem(horizontalSpacer_10);


        verticalLayout->addLayout(horizontalLayout_10);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setObjectName("horizontalLayout_11");
        focusLabel = new QLabel(SettingsDialog);
        focusLabel->setObjectName("focusLabel");

        horizontalLayout_11->addWidget(focusLabel);

        focusSlider = new QSlider(SettingsDialog);
        focusSlider->setObjectName("focusSlider");
        focusSlider->setOrientation(Qt::Horizontal);

        horizontalLayout_11->addWidget(focusSlider);

        focusSpinBox = new QSpinBox(SettingsDialog);
        focusSpinBox->setObjectName("focusSpinBox");

        horizontalLayout_11->addWidget(focusSpinBox);

        focusCheckBox = new QCheckBox(SettingsDialog);
        focusCheckBox->setObjectName("focusCheckBox");

        horizontalLayout_11->addWidget(focusCheckBox);

        horizontalSpacer_11 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_11->addItem(horizontalSpacer_11);


        verticalLayout->addLayout(horizontalLayout_11);

        controlWidget = new QWidget(SettingsDialog);
        controlWidget->setObjectName("controlWidget");
        controlWidget->setMinimumSize(QSize(0, 50));
        horizontalLayout_12 = new QHBoxLayout(controlWidget);
        horizontalLayout_12->setObjectName("horizontalLayout_12");
        horizontalSpacer_12 = new QSpacerItem(289, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_12->addItem(horizontalSpacer_12);

        loadBtn = new QPushButton(controlWidget);
        loadBtn->setObjectName("loadBtn");

        horizontalLayout_12->addWidget(loadBtn);

        saveBtn = new QPushButton(controlWidget);
        saveBtn->setObjectName("saveBtn");

        horizontalLayout_12->addWidget(saveBtn);


        verticalLayout->addWidget(controlWidget);

        verticalSpacer = new QSpacerItem(20, 49, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        retranslateUi(SettingsDialog);

        QMetaObject::connectSlotsByName(SettingsDialog);
    } // setupUi

    void retranslateUi(QDialog *SettingsDialog)
    {
        SettingsDialog->setWindowTitle(QCoreApplication::translate("SettingsDialog", "Dialog", nullptr));
        brightnessLabel->setText(QCoreApplication::translate("SettingsDialog", "Brightness:", nullptr));
        brightnessCheckBox->setText(QCoreApplication::translate("SettingsDialog", "auto", nullptr));
        contrastLabel->setText(QCoreApplication::translate("SettingsDialog", "Contrast:", nullptr));
        contrastCheckBox->setText(QCoreApplication::translate("SettingsDialog", "auto", nullptr));
        hueLabel->setText(QCoreApplication::translate("SettingsDialog", "Hue:", nullptr));
        hueCheckBox->setText(QCoreApplication::translate("SettingsDialog", "auto", nullptr));
        saturationLabel->setText(QCoreApplication::translate("SettingsDialog", "Saturation:", nullptr));
        saturationCheckBox->setText(QCoreApplication::translate("SettingsDialog", "auto", nullptr));
        sharpnessLabel->setText(QCoreApplication::translate("SettingsDialog", "Sharpness:", nullptr));
        sharpnessCheckBox->setText(QCoreApplication::translate("SettingsDialog", "auto", nullptr));
        gammaLabel->setText(QCoreApplication::translate("SettingsDialog", "Gamma:", nullptr));
        gammaCheckBox->setText(QCoreApplication::translate("SettingsDialog", "auto", nullptr));
        whiteBalanceLabel->setText(QCoreApplication::translate("SettingsDialog", "WhiteBalance:", nullptr));
        whiteBalanceCheckBox->setText(QCoreApplication::translate("SettingsDialog", "auto", nullptr));
        backlightCompensationLabel->setText(QCoreApplication::translate("SettingsDialog", "BacklightCompensation:", nullptr));
        backlightCompensationCheckBox->setText(QCoreApplication::translate("SettingsDialog", "auto", nullptr));
        gainLabel->setText(QCoreApplication::translate("SettingsDialog", "Gain:", nullptr));
        gainCheckBox->setText(QCoreApplication::translate("SettingsDialog", "auto", nullptr));
        exposureLabel->setText(QCoreApplication::translate("SettingsDialog", "Exposure:", nullptr));
        exposureCheckBox->setText(QCoreApplication::translate("SettingsDialog", "auto", nullptr));
        focusLabel->setText(QCoreApplication::translate("SettingsDialog", "Focus:", nullptr));
        focusCheckBox->setText(QCoreApplication::translate("SettingsDialog", "auto", nullptr));
        loadBtn->setText(QCoreApplication::translate("SettingsDialog", "load", nullptr));
        saveBtn->setText(QCoreApplication::translate("SettingsDialog", "save", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SettingsDialog: public Ui_SettingsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGSDIALOG_H
