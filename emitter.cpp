#include "emitter.h"

namespace gssc {

Emitter::Emitter(QObject* parent)
  : QObject{ parent }
{}

Emitter::~Emitter() = default;

} // namespace gssc
