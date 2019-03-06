#include "dispatcher.h"

#include "emitter.h"
#include <QDebug>

namespace gssc {

Dispatcher::Dispatcher(QObject* parent)
  : QObject{ parent }
{}

Dispatcher::~Dispatcher() = default;

void Dispatcher::generic_slot_impl(QString name, QVariantList data)
{
  qDebug() << name << data;
}

QMetaObject::Connection
connect(Emitter* emitter, Dispatcher* dispatcher, Qt::ConnectionType type)
{
  return QObject::connect(
    emitter, SIGNAL(generic_signal_impl(QString, QVariantList)),
    dispatcher, SLOT(generic_slot_impl(QString, QVariantList)),
    type
  );
}

}
