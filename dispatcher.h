#pragma once

#include <QObject>
#include <QHash>
#include <QSharedPointer>
#include <QString>
#include <QVariantList>

namespace gssc {

class Emitter;

class IHandlerWrapper
{
public:
  virtual IHandlerWrapper() = default;
  virtual void operator()(QVariantList data) = 0;
};

template <typename Handler, typename... Args>
class HandlerWrapper
{
public:
  HandlerWrapper(Handler handler)
    : _handler{ std::move(handler) }
  {}
  ~IHandlerWrapper() final = default;
  void operator()(QVariantList data) final {
    //TODO implement
    _handler();
  }
private:
   Handler _handler;
};

class Dispatcher
  : public QObject
{
  Q_OBJECT
  friend QMetaObject::Connection connect(Emitter*, Dispatcher*, Qt::ConnectionType);

public:
  Dispatcher(QObject* parent = nullptr);
  ~Dispatcher() override;

  template <typename Handler>
  void add_handler(QString name, Handler handler) {
    //TODO implement

    _handlers.insert(name, QSharedPointer<IHandlerWrapper>{nullptr});
  }

  void remove_handler(QString name) {
    _handlers.remove(name);
  }

private slots:
  void generic_slot_impl(QString name, QVariantList data);

private:
  QHash<QString, QSharedPointer<IHandlerWrapper>> _handlers;
};

QMetaObject::Connection
connect(Emitter* emitter, Dispatcher* dispatcher, Qt::ConnectionType type);

}
