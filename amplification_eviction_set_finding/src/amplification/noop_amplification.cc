#include "amplification/noop_amplification.h"

namespace amplification {

auto _noop_amplification = NoopAmplification();
Amplification &noop_amplification = _noop_amplification;

}