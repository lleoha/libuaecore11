#ifndef UAECORE11_CORE_INTERNAL_H
#define UAECORE11_CORE_INTERNAL_H

#include "core_api.h"

namespace uaecore11 {

extern uaecore11_callbacks_t *callbacks;
extern int interrupt_level;
extern int intlev();
extern void do_cycles_slow(unsigned long cycles_to_add);

}

using uaecore11::intlev;
using uaecore11::do_cycles_slow;

#endif
