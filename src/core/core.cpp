#include "core_api.h"
#include "core_internal.h"

#include "sysconfig.h"
#include "sysdeps.h"
#include "memory.h"
#include "newcpu.h"
#include "cpu_prefetch.h"

#include <cstdio>

using namespace uaecore11;


uaecore11_callbacks_t *uaecore11::callbacks = 0;
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
    if (callbacks->ticks) {
        callbacks->ticks(t);
    }
}

UAECORE11API void uaecore11_init(uaecore11_callbacks_t *callbacks) {
    ::uaecore11::callbacks = callbacks;
    init_m68k();
}

UAECORE11API void uaecore11_reset() {
    printf("%lx\n", callbacks);
    printf("%lx\n", callbacks->get_long);
    m68k_reset(true);
}

UAECORE11API void uaecore11_execute() {
    m68k_run_1();
}

UAECORE11API void uaecore11_raise_irq(int level) {
    if (level >= 0 && level <= 7) {
        interrupt_level |= (1 << level);
        doint();
    }
}

UAECORE11API void uaecore11_lower_irq(int level) {
    if (level >= 0 && level <= 7) {
        interrupt_level &= ~(1 << level);
        doint();
    }
}

UAECORE11API unsigned int uaecore11_get_register(uaecore11_register_t reg) {
    if (reg <= UAECORE11_REGISTER_A7) {
        return regs.regs[static_cast<int>(reg)]; 
    } else if (reg == UAECORE11_REGISTER_SSP) {
        return regs.isp;
    } else if (reg == UAECORE11_REGISTER_USP) {
        return regs.usp;
    } else if (reg == UAECORE11_REGISTER_PC) {
        return m68k_getpc(); 
    } else if (reg == UAECORE11_REGISTER_SR) {
        MakeSR();
        return static_cast<unsigned int>(regs.sr);
    } else {
        return 0xffffffff;
    }
}

UAECORE11API void uaecore11_set_register(uaecore11_register_t reg, unsigned int value) {
    if (reg <= UAECORE11_REGISTER_A7) {
        regs.regs[static_cast<int>(reg)] = value;
    } if (reg == UAECORE11_REGISTER_PC) {
        m68k_setpc(value);
        get_word_prefetch(0);
        regs.ir = regs.irc;
        get_word_prefetch(2);
    } if (reg == UAECORE11_REGISTER_SR) {
        regs.sr = static_cast<unsigned short>(value & 0xffff);
        MakeFromSR();
    }
}
