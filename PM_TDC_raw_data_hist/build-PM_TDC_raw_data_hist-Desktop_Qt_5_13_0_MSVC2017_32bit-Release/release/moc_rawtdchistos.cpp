/****************************************************************************
** Meta object code from reading C++ file 'rawtdchistos.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.13.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../PM_TDC_raw_data_hist/rawtdchistos.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'rawtdchistos.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.13.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_RawTDChistos_t {
    QByteArrayData data[9];
    char stringdata0[113];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RawTDChistos_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RawTDChistos_t qt_meta_stringdata_RawTDChistos = {
    {
QT_MOC_LITERAL(0, 0, 12), // "RawTDChistos"
QT_MOC_LITERAL(1, 13, 13), // "samplingReady"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 12), // "storingReady"
QT_MOC_LITERAL(4, 41, 16), // "calculatingReady"
QT_MOC_LITERAL(5, 58, 26), // "on_spinBox_editingFinished"
QT_MOC_LITERAL(6, 85, 8), // "changeIP"
QT_MOC_LITERAL(7, 94, 7), // "recheck"
QT_MOC_LITERAL(8, 102, 10) // "read_files"

    },
    "RawTDChistos\0samplingReady\0\0storingReady\0"
    "calculatingReady\0on_spinBox_editingFinished\0"
    "changeIP\0recheck\0read_files"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RawTDChistos[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x06 /* Public */,
       3,    0,   50,    2, 0x06 /* Public */,
       4,    0,   51,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   52,    2, 0x08 /* Private */,
       6,    0,   53,    2, 0x08 /* Private */,
       7,    0,   54,    2, 0x08 /* Private */,
       8,    0,   55,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void RawTDChistos::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<RawTDChistos *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->samplingReady(); break;
        case 1: _t->storingReady(); break;
        case 2: _t->calculatingReady(); break;
        case 3: _t->on_spinBox_editingFinished(); break;
        case 4: _t->changeIP(); break;
        case 5: _t->recheck(); break;
        case 6: _t->read_files(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (RawTDChistos::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RawTDChistos::samplingReady)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (RawTDChistos::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RawTDChistos::storingReady)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (RawTDChistos::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RawTDChistos::calculatingReady)) {
                *result = 2;
                return;
            }
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject RawTDChistos::staticMetaObject = { {
    &QMainWindow::staticMetaObject,
    qt_meta_stringdata_RawTDChistos.data,
    qt_meta_data_RawTDChistos,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *RawTDChistos::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RawTDChistos::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RawTDChistos.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int RawTDChistos::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void RawTDChistos::samplingReady()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void RawTDChistos::storingReady()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void RawTDChistos::calculatingReady()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
