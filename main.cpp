#include <QCoreApplication>

#include <QDebug>
#include "emitter.h"
#include "dispatcher.h"

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);

  gssc::Dispatcher dispatcher;
  gssc::Emitter emitter;

  connect(&emitter, &dispatcher, Qt::QueuedConnection);

  dispatcher.add_handler("event3", [&](int i, char c, QString s){
    qDebug() << "event3: s =" << s;
    emitter.notify("event2", i, c);
  });
  dispatcher.add_handler("event2", [&](int i, char c){
    qDebug() << "event2: c =" << c;
    emitter.notify("event1", i);
  });
  dispatcher.add_handler("event1", [&](int i){
    qDebug() << "event1: i =" << i;
    emitter.notify("event0");
  });
  dispatcher.add_handler("event0", [&]{
    a.quit();
  });

  emitter.notify("event3", 42, 'x', "hello");
  qDebug() << "system prepared";

  return a.exec();
}
