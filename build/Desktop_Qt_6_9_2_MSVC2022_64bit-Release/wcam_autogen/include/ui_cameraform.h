/********************************************************************************
** Form generated from reading UI file 'cameraform.ui'
**
** Created by: Qt User Interface Compiler version 6.9.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CAMERAFORM_H
#define UI_CAMERAFORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CameraForm
{
public:
    QHBoxLayout *horizontalLayout;
    QLabel *imageLabel;
    QWidget *controlWidget;
    QVBoxLayout *verticalLayout;
    QComboBox *devComboBox;
    QComboBox *resolutionComboBox;
    QComboBox *processComboBox;
    QPushButton *startBtn;
    QPushButton *stopBtn;
    QPushButton *captureBtn;
    QPushButton *settingsBtn;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *CameraForm)
    {
        if (CameraForm->objectName().isEmpty())
            CameraForm->setObjectName("CameraForm");
        CameraForm->resize(711, 481);
        horizontalLayout = new QHBoxLayout(CameraForm);
        horizontalLayout->setObjectName("horizontalLayout");
        imageLabel = new QLabel(CameraForm);
        imageLabel->setObjectName("imageLabel");

        horizontalLayout->addWidget(imageLabel);

        controlWidget = new QWidget(CameraForm);
        controlWidget->setObjectName("controlWidget");
        controlWidget->setMaximumSize(QSize(150, 16777215));
        verticalLayout = new QVBoxLayout(controlWidget);
        verticalLayout->setObjectName("verticalLayout");
        devComboBox = new QComboBox(controlWidget);
        devComboBox->setObjectName("devComboBox");

        verticalLayout->addWidget(devComboBox);

        resolutionComboBox = new QComboBox(controlWidget);
        resolutionComboBox->setObjectName("resolutionComboBox");

        verticalLayout->addWidget(resolutionComboBox);

        processComboBox = new QComboBox(controlWidget);
        processComboBox->setObjectName("processComboBox");

        verticalLayout->addWidget(processComboBox);

        startBtn = new QPushButton(controlWidget);
        startBtn->setObjectName("startBtn");

        verticalLayout->addWidget(startBtn);

        stopBtn = new QPushButton(controlWidget);
        stopBtn->setObjectName("stopBtn");

        verticalLayout->addWidget(stopBtn);

        captureBtn = new QPushButton(controlWidget);
        captureBtn->setObjectName("captureBtn");

        verticalLayout->addWidget(captureBtn);

        settingsBtn = new QPushButton(controlWidget);
        settingsBtn->setObjectName("settingsBtn");

        verticalLayout->addWidget(settingsBtn);

        verticalSpacer = new QSpacerItem(20, 332, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        horizontalLayout->addWidget(controlWidget);


        retranslateUi(CameraForm);

        QMetaObject::connectSlotsByName(CameraForm);
    } // setupUi

    void retranslateUi(QWidget *CameraForm)
    {
        CameraForm->setWindowTitle(QCoreApplication::translate("CameraForm", "Form", nullptr));
        imageLabel->setText(QString());
        startBtn->setText(QCoreApplication::translate("CameraForm", "start", nullptr));
        stopBtn->setText(QCoreApplication::translate("CameraForm", "stop", nullptr));
        captureBtn->setText(QCoreApplication::translate("CameraForm", "capture", nullptr));
        settingsBtn->setText(QCoreApplication::translate("CameraForm", "settings", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CameraForm: public Ui_CameraForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CAMERAFORM_H
