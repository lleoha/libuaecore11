#include "sysconfig.h"
#include "sysdeps.h"
#include "core_glue.h"

uaecore11_handlers_t *uaecore11::handlers = 0;
int uaecore11::interrupt_level = -1;

int uaecore11::intlev() {
    if (interrupt_level < 0 || interrupt_level > 7) {
        return -1;
    }

    for (int i = 7; i >= 0; --i) {
        if ((interrupt_level & (1 << i)) != 0) return i;
    }

    return -1;
}

void uaecore11::do_cycles_slow(unsigned long t) {
    if (handlers->ticks) {
        handlers->ticks(t / CYCLE_UNIT);
    }
}
