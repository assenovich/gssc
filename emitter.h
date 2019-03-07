#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>

namespace gssc {

namespace impl {

template <typename Result>
void serialize_to_qvariantlist(Result& result)
{
  Q_UNUSED(result)
}

template <typename Result, typename First, typename... Rest>
void serialize_to_qvariantlist(Result& result, First&& first, Rest&&... rest)
{
  result << QVariant{ std::forward<First>(first) };
  serialize_to_qvariantlist(result, std::forward<Rest>(rest)...);
}

} // namespace impl

class Emitter
  : public QObject
{
  Q_OBJECT

signals:
  void notify_impl(QString name, QVariantList data);

public:
  Emitter(QObject* parent = nullptr);
  ~Emitter() override;

  template <typename... Args>
  void notify(QString name, Args&&... args) {
    QVariantList data;
    impl::serialize_to_qvariantlist(data, std::forward<Args>(args)...);
    emit notify_impl(std::move(name), std::move(data));
  }
};

} // namespace gssc
