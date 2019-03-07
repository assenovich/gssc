#pragma once

#include <stdexcept>
#include <type_traits>
#include <QObject>
#include <QHash>
#include <QSharedPointer>
#include <QString>
#include <QVariantList>

namespace gssc {

class Emitter;

namespace impl {

template <typename... Args>
struct PackedArgs;

template <typename Handler, typename Deserialized, typename Serialized>
struct HandlerCaller;

template <typename Handler, typename... Deserialized, typename FirstSerialized, typename... RestSerialized>
struct HandlerCaller<Handler, PackedArgs<Deserialized...>, PackedArgs<FirstSerialized, RestSerialized...>>
{
  static void exec(const QString& name, const Handler& handler, Deserialized&&... deserialized, QVariantList&& serialized) {
    if (serialized.empty()) {
      throw std::logic_error{ "too little serialized arguments, expected more" };
    }
    QVariant& firstSerialized = serialized.front();
    if (!firstSerialized.canConvert<FirstSerialized>()) {
      throw std::logic_error{ QString("Handler \"%1\" expects argument#%2 to be of type \"%3\", not of \"%4\"")
        .arg(name).arg(sizeof...(deserialized) + 1)
        .arg(QVariant{FirstSerialized{}}.typeName()).arg(firstSerialized.typeName()).toStdString() };
    }
    auto nextDeserializedValue = std::move(firstSerialized).value<FirstSerialized>();
    serialized.pop_front();
    HandlerCaller<Handler, PackedArgs<Deserialized..., FirstSerialized>, PackedArgs<RestSerialized...>>::exec(
      name, handler, std::move(deserialized)..., std::move(nextDeserializedValue), std::move(serialized)
    );
  }
};

template <typename Handler, typename... Deserialized>
struct HandlerCaller<Handler, PackedArgs<Deserialized...>, PackedArgs<>>
{
  static void exec(const QString& name, const Handler& handler, Deserialized&&... deserialized, QVariantList&& serialized) {
    Q_UNUSED(name)
    if (!serialized.empty()) {
      throw std::logic_error{ "too many serialized arguments, expected less" };
    }
    handler(std::move(deserialized)...);
  }
};

class IHandlerWrapper
{
public:
  virtual ~IHandlerWrapper() = default;
  virtual void process(QVariantList data) = 0;
};

template <typename Handler, typename... Args>
class HandlerWrapper
  : public IHandlerWrapper
{
public:
  HandlerWrapper(QString name, Handler handler)
    : _name{ std::move(name) }
    , _handler{ std::move(handler) }
  {}
  ~HandlerWrapper() final = default;
  void process(QVariantList data) final {
    const int n_expected_arguments = sizeof...(Args);
    const int n_actual_argumants = data.size();
    if (n_actual_argumants != n_expected_arguments) {
      throw std::logic_error{ QString("Handler \"%1\" expects %2 arguments but got %3")
        .arg(_name).arg(n_expected_arguments).arg(n_actual_argumants).toStdString() };
    }
    HandlerCaller<Handler, PackedArgs<>, PackedArgs<typename std::decay<Args>::type...>>
      ::exec(_name, _handler, std::move(data));
  }
private:
  const QString _name;
  const Handler _handler;
};

template <typename Handler, typename ClassType, typename ReturnType, typename... Args>
IHandlerWrapper* create_wrapper(QString name, Handler handler, ReturnType (ClassType::*)(Args...) const)
{
  return new HandlerWrapper<Handler, Args...>{ std::move(name), std::move(handler) };
}

} // namespace impl

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
    _handlers[name].reset(impl::create_wrapper(name, std::move(handler), &Handler::operator()));
  }

  void remove_handler(QString name) {
    _handlers.remove(name);
  }

private slots:
  void on_notify_impl(QString name, QVariantList data);

private:
  QHash<QString, QSharedPointer<impl::IHandlerWrapper>> _handlers;
};

QMetaObject::Connection
connect(Emitter* emitter, Dispatcher* dispatcher, Qt::ConnectionType type);

} // namespace gssc
