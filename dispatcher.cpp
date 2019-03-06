#include "dispatcher.h"

#include <stdexcept>
#include "emitter.h"

namespace gssc {

Dispatcher::Dispatcher(QObject* parent)
  : QObject{ parent }
{}

Dispatcher::~Dispatcher() = default;

void Dispatcher::on_notify_impl(QString name, QVariantList data)
{
  const auto it = _handlers.find(name);
  if (it == _handlers.end()) {
    throw std::logic_error{ QString("Handler for event \"%1\" was not found")
      .arg(name).toStdString() };
  }
  (*it)->process(std::move(data));
}

QMetaObject::Connection
connect(Emitter* emitter, Dispatcher* dispatcher, Qt::ConnectionType type)
{
  return QObject::connect(
    emitter, &Emitter::notify_impl,
    dispatcher, &Dispatcher::on_notify_impl,
    type
  );
}

} // namespace gssc
