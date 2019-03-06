#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>

namespace gssc {

class Dispatcher;

class Emitter
  : public QObject
{
  Q_OBJECT
  friend QMetaObject::Connection connect(Emitter*, Dispatcher*, Qt::ConnectionType);

public:
  Emitter(QObject* parent = nullptr);
  ~Emitter() override;

  template <typename Args...>
  void notify(QString name, Args&&... args) const {
    QVariantList data;
    //TODO serialize "args..." to "data"
    emit generic_signal_impl(name, data);
  }

private signals:
  void generic_signal_impl(QString name, QVariantList data);
};

}
