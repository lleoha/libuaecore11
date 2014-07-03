#include "sysconfig.h"
#include "sysdeps.h"
#include "core_glue.h"


uaecore11_handlers_t *uaecore11::handlers = 0;
int uaecore11::interrupt_level = 0;

int uaecore11::intlev() {
    return interrupt_level;
}

void uaecore11::do_cycles_slow(unsigned long cycles) {
    if (handlers->cycles) {
        handlers->cycles(cycles / CYCLE_UNIT);
    }
}
