/****************************************************************************
** Meta object code from reading C++ file 'ecganalyzer.h'
**
** Created: Sat 27. Oct 13:44:42 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ecganalyzer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ecganalyzer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ECGanalyzer[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x08,
      39,   12,   12,   12, 0x08,
      72,   12,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ECGanalyzer[] = {
    "ECGanalyzer\0\0on_actionO_Qt_triggered()\0"
    "on_actionO_Programie_triggered()\0"
    "on_actionWyjd_triggered()\0"
};

void ECGanalyzer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ECGanalyzer *_t = static_cast<ECGanalyzer *>(_o);
        switch (_id) {
        case 0: _t->on_actionO_Qt_triggered(); break;
        case 1: _t->on_actionO_Programie_triggered(); break;
        case 2: _t->on_actionWyjd_triggered(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData ECGanalyzer::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ECGanalyzer::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_ECGanalyzer,
      qt_meta_data_ECGanalyzer, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ECGanalyzer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ECGanalyzer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ECGanalyzer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ECGanalyzer))
        return static_cast<void*>(const_cast< ECGanalyzer*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int ECGanalyzer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
