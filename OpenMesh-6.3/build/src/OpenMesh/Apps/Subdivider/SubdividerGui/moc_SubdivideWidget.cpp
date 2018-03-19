/****************************************************************************
** Meta object code from reading C++ file 'SubdivideWidget.hh'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../src/OpenMesh/Apps/Subdivider/SubdivideWidget.hh"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SubdivideWidget.hh' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SubdivideWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x08,
      34,   32,   16,   16, 0x08,
      55,   16,   16,   16, 0x08,
      69,   16,   16,   16, 0x08,
      82,   16,   16,   16, 0x08,
      94,   16,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_SubdivideWidget[] = {
    "SubdivideWidget\0\0animate_slot()\0i\0"
    "slot_select_sop(int)\0subdiv_slot()\0"
    "reset_slot()\0save_slot()\0load_slot()\0"
};

void SubdivideWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SubdivideWidget *_t = static_cast<SubdivideWidget *>(_o);
        switch (_id) {
        case 0: _t->animate_slot(); break;
        case 1: _t->slot_select_sop((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->subdiv_slot(); break;
        case 3: _t->reset_slot(); break;
        case 4: _t->save_slot(); break;
        case 5: _t->load_slot(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData SubdivideWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SubdivideWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_SubdivideWidget,
      qt_meta_data_SubdivideWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SubdivideWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SubdivideWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SubdivideWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SubdivideWidget))
        return static_cast<void*>(const_cast< SubdivideWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int SubdivideWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
