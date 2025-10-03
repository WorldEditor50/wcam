/****************************************************************************
** Meta object code from reading C++ file 'cameraform.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/cameraform.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cameraform.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN10CameraFormE_t {};
} // unnamed namespace

template <> constexpr inline auto CameraForm::qt_create_metaobjectdata<qt_meta_tag_ZN10CameraFormE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "CameraForm",
        "sendImage",
        "",
        "img",
        "deviceAttached",
        "vid",
        "pid",
        "isAttached",
        "updateImage",
        "onAddDevice",
        "Camera::Property",
        "property",
        "onRemoveDevice",
        "onResolutionChanged",
        "resolutionNum",
        "onDeviceChanged",
        "deviceNum",
        "startCapture",
        "stopCapture"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'sendImage'
        QtMocHelpers::SignalData<void(const QImage &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QImage, 3 },
        }}),
        // Signal 'deviceAttached'
        QtMocHelpers::SignalData<void(unsigned short, unsigned short, bool)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::UShort, 5 }, { QMetaType::UShort, 6 }, { QMetaType::Bool, 7 },
        }}),
        // Slot 'updateImage'
        QtMocHelpers::SlotData<void(const QImage &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QImage, 3 },
        }}),
        // Slot 'onAddDevice'
        QtMocHelpers::SlotData<void(const Camera::Property &)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 10, 11 },
        }}),
        // Slot 'onRemoveDevice'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onResolutionChanged'
        QtMocHelpers::SlotData<void(int)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 14 },
        }}),
        // Slot 'onDeviceChanged'
        QtMocHelpers::SlotData<void(int)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 16 },
        }}),
        // Slot 'startCapture'
        QtMocHelpers::SlotData<void()>(17, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'stopCapture'
        QtMocHelpers::SlotData<void()>(18, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<CameraForm, qt_meta_tag_ZN10CameraFormE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject CameraForm::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10CameraFormE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10CameraFormE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10CameraFormE_t>.metaTypes,
    nullptr
} };

void CameraForm::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<CameraForm *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->sendImage((*reinterpret_cast< std::add_pointer_t<QImage>>(_a[1]))); break;
        case 1: _t->deviceAttached((*reinterpret_cast< std::add_pointer_t<ushort>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<ushort>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[3]))); break;
        case 2: _t->updateImage((*reinterpret_cast< std::add_pointer_t<QImage>>(_a[1]))); break;
        case 3: _t->onAddDevice((*reinterpret_cast< std::add_pointer_t<Camera::Property>>(_a[1]))); break;
        case 4: _t->onRemoveDevice(); break;
        case 5: _t->onResolutionChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->onDeviceChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 7: _t->startCapture(); break;
        case 8: _t->stopCapture(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (CameraForm::*)(const QImage & )>(_a, &CameraForm::sendImage, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (CameraForm::*)(unsigned short , unsigned short , bool )>(_a, &CameraForm::deviceAttached, 1))
            return;
    }
}

const QMetaObject *CameraForm::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CameraForm::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10CameraFormE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int CameraForm::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void CameraForm::sendImage(const QImage & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void CameraForm::deviceAttached(unsigned short _t1, unsigned short _t2, bool _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2, _t3);
}
QT_WARNING_POP
