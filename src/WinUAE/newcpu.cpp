/*
* UAE - The Un*x Amiga Emulator
*
* MC68000 emulation
*
* (c) 1995 Bernd Schmidt
*/

#include "sysconfig.h"
#include "sysdeps.h"

#include "events.h"
#include "memory.h"
#include "custom.h"
#include "newcpu.h"
#include "cpu_prefetch.h"
#include "core_internal.h"


/* Opcode of faulting instruction */
static uae_u16 last_op_for_exception_3;
/* PC at fault time */
static uaecptr last_addr_for_exception_3;
/* Address that generated the exception */
static uaecptr last_fault_for_exception_3;
/* read (0) or write (1) access */
static int last_writeaccess_for_exception_3;
/* instruction (1) or data (0) access */
static int last_instructionaccess_for_exception_3;

int cpu_cycles;
bool m68k_pc_indirect;

int cpucycleunit;
int cpu_tracer;

const int areg_byteinc[] = { 1, 1, 1, 1, 1, 1, 1, 2 };
const int imm8_table[] = { 8, 1, 2, 3, 4, 5, 6, 7 };

int movem_index1[256];
int movem_index2[256];
int movem_next[256];

cpuop_func *cpufunctbl[65536];

uae_u32 (*x_prefetch)(int);
uae_u32 (*x_next_iword)(void);
uae_u32 (*x_next_ilong)(void);
uae_u32 (*x_get_ilong)(int);
uae_u32 (*x_get_iword)(int);
uae_u32 (*x_get_ibyte)(int);
uae_u32 (*x_get_long)(uaecptr);
uae_u32 (*x_get_word)(uaecptr);
uae_u32 (*x_get_byte)(uaecptr);
void (*x_put_long)(uaecptr,uae_u32);
void (*x_put_word)(uaecptr,uae_u32);
void (*x_put_byte)(uaecptr,uae_u32);

void (*x_do_cycles)(unsigned long);
void (*x_do_cycles_pre)(unsigned long);
void (*x_do_cycles_post)(unsigned long, uae_u32);

static void do_cycles_post (unsigned long cycles, uae_u32 v)
{
	do_cycles (cycles);
}

// indirect memory access functions
static void set_x_funcs (void)
{
			x_prefetch = get_word_prefetch;
			x_get_ilong = NULL;
			x_get_iword = get_iiword;
			x_get_ibyte = get_iibyte;
			x_next_iword = NULL;
			x_next_ilong = NULL;
			x_put_long = put_long;
			x_put_word = put_word;
			x_put_byte = put_byte;
			x_get_long = get_long;
			x_get_word = get_word;
			x_get_byte = get_byte;
			x_do_cycles = do_cycles;
			x_do_cycles_pre = do_cycles;
			x_do_cycles_post = do_cycles_post;
}

static uae_u32 REGPARAM2 op_illg_1 (uae_u32 opcode)
{
	op_illg (opcode);
	return 4;
}

static void build_cpufunctbl (void)
{
	int i, opcnt;
	unsigned long opcode;
	const struct cputbl *tbl = 0;
	int lvl;

	lvl = 0;
	tbl = op_smalltbl_12_ff; /* prefetch */

	if (tbl == 0) {
		abort ();
	}

	for (opcode = 0; opcode < 65536; opcode++)
		cpufunctbl[opcode] = op_illg_1;
	for (i = 0; tbl[i].handler != NULL; i++) {
		opcode = tbl[i].opcode;
		cpufunctbl[opcode] = tbl[i].handler;
	}

	opcnt = 0;
	for (opcode = 0; opcode < 65536; opcode++) {
		cpuop_func *f;
		instr *table = &table68k[opcode];

		if (table->mnemo == i_ILLG)
			continue;		

		/* unimplemented opcode? */
		if (table->unimpclev > 0 && lvl >= table->unimpclev) {
			cpufunctbl[opcode] = op_illg_1;
			continue;
		}

		if (table->clev > lvl) {
			continue;
		}

		if (table->handler != -1) {
			int idx = table->handler;
			f = cpufunctbl[idx];
			if (f == op_illg_1)
				abort ();
			cpufunctbl[opcode] = f;
			opcnt++;
		}
	}

	regs.address_space_mask = 0x00ffffff;
	m68k_pc_indirect = true;
}

void init_m68k (void)
{
	for (int i = 0 ; i < 256 ; i++) {
		int j;
		for (j = 0 ; j < 8 ; j++) {
			if (i & (1 << j)) break;
		}
		movem_index1[i] = j;
		movem_index2[i] = 7 - j;
		movem_next[i] = i & (~(1 << j));
	}

	read_table68k ();
	do_merges ();

	build_cpufunctbl ();
	set_x_funcs ();
}

struct regstruct regs, mmu_backup_regs;
struct flag_struct regflags;
static long int m68kpc_offset;

void REGPARAM2 MakeSR (void)
{
	regs.sr = ((regs.t1 << 15) | (regs.t0 << 14)
		| (regs.s << 13) | (regs.m << 12) | (regs.intmask << 8)
		| (GET_XFLG () << 4) | (GET_NFLG () << 3)
		| (GET_ZFLG () << 2) | (GET_VFLG () << 1)
		|  GET_CFLG ());
}

void SetSR (uae_u16 sr)
{
	regs.sr &= 0xff00;
	regs.sr |= sr;

	SET_XFLG ((regs.sr >> 4) & 1);
	SET_NFLG ((regs.sr >> 3) & 1);
	SET_ZFLG ((regs.sr >> 2) & 1);
	SET_VFLG ((regs.sr >> 1) & 1);
	SET_CFLG (regs.sr & 1);
}

void REGPARAM2 MakeFromSR (void)
{
	int oldm = regs.m;
	int olds = regs.s;

	SET_XFLG ((regs.sr >> 4) & 1);
	SET_NFLG ((regs.sr >> 3) & 1);
	SET_ZFLG ((regs.sr >> 2) & 1);
	SET_VFLG ((regs.sr >> 1) & 1);
	SET_CFLG (regs.sr & 1);
	if (regs.t1 == ((regs.sr >> 15) & 1) &&
		regs.t0 == ((regs.sr >> 14) & 1) &&
		regs.s  == ((regs.sr >> 13) & 1) &&
		regs.m  == ((regs.sr >> 12) & 1) &&
		regs.intmask == ((regs.sr >> 8) & 7))
		return;
	regs.t1 = (regs.sr >> 15) & 1;
	regs.t0 = (regs.sr >> 14) & 1;
	regs.s  = (regs.sr >> 13) & 1;
	regs.m  = (regs.sr >> 12) & 1;
	regs.intmask = (regs.sr >> 8) & 7;
	regs.t0 = regs.m = 0;
	if (olds != regs.s) {
		if (olds) {
			regs.isp = m68k_areg (regs, 7);
			m68k_areg (regs, 7) = regs.usp;
		} else {
			regs.usp = m68k_areg (regs, 7);
			m68k_areg (regs, 7) = regs.isp;
		}
	}

	doint ();
	if (regs.t1 || regs.t0)
		set_special (SPCFLAG_TRACE);
	else
		/* Keep SPCFLAG_DOTRACE, we still want a trace exception for
		SR-modifying instructions (including STOP).  */
		unset_special (SPCFLAG_TRACE);
}

static void exception_trace (int nr)
{
	unset_special (SPCFLAG_TRACE | SPCFLAG_DOTRACE);
	if (regs.t1 && !regs.t0) {
		/* trace stays pending if exception is div by zero, chk,
		* trapv or trap #x
		*/
		if (nr == 5 || nr == 6 || nr == 7 || (nr >= 32 && nr <= 47))
			set_special (SPCFLAG_DOTRACE);
	}
	regs.t1 = regs.t0 = regs.m = 0;
}



static uae_u32 exception_pc (int nr)
{
	// zero divide, chk, trapcc/trapv, trace, trap#
	if (nr == 5 || nr == 6 || nr == 7 || nr == 9 || (nr >= 32 && nr <= 47))
		return m68k_getpc ();
	return regs.instruction_pc;
}

static void Exception_normal (int nr)
{
	uae_u32 currpc, newpc;
	int sv = regs.s;

//	if (nr >= 24 && nr < 24 + 8)
//		nr = x_get_byte (0x00fffff1 | (nr << 1));

	MakeSR ();

	if (!regs.s) {
		regs.usp = m68k_areg (regs, 7);
		m68k_areg (regs, 7) = regs.isp;
		regs.s = 1;
	}
	currpc = m68k_getpc ();
	if (nr == 2 || nr == 3) {
		// 68000 address error
		uae_u16 mode = (sv ? 4 : 0) | (last_instructionaccess_for_exception_3 ? 2 : 1);
		mode |= last_writeaccess_for_exception_3 ? 0 : 16;
		m68k_areg (regs, 7) -= 14;
		/* fixme: bit3=I/N */
		x_put_word (m68k_areg (regs, 7) + 0, mode);
		x_put_long (m68k_areg (regs, 7) + 2, last_fault_for_exception_3);
		x_put_word (m68k_areg (regs, 7) + 6, last_op_for_exception_3);
		x_put_word (m68k_areg (regs, 7) + 8, regs.sr);
		x_put_long (m68k_areg (regs, 7) + 10, last_addr_for_exception_3);
		goto kludge_me_do;
	}
	m68k_areg (regs, 7) -= 4;
	x_put_long (m68k_areg (regs, 7), currpc);
	m68k_areg (regs, 7) -= 2;
	x_put_word (m68k_areg (regs, 7), regs.sr);
kludge_me_do:
	newpc = x_get_long (regs.vbr + 4 * nr);
	if (newpc & 1) {
		if (nr == 2 || nr == 3)
			cpu_halt (2);
		else
			exception3 (regs.ir, newpc);
		return;
	}
	m68k_setpc (newpc);
	fill_prefetch ();
	exception_trace (nr);
}

// address = format $2 stack frame address field
static void ExceptionX (int nr, uaecptr address)
{
	regs.exception = nr;

	Exception_normal (nr);

	regs.exception = 0;
}

void REGPARAM2 Exception (int nr)
{
	ExceptionX (nr, -1);
}
void REGPARAM2 ExceptionL (int nr, uaecptr address)
{
	ExceptionX (nr, address);
}

static void do_interrupt (int nr)
{
	regs.stopped = 0;
	unset_special (SPCFLAG_STOP);
	assert (nr < 8 && nr >= 0);

	Exception (nr + 24);
	uaecore11::interrupt_level &= ~(1 << nr);

	regs.intmask = nr;
	doint ();
}

void NMI (void)
{
	do_interrupt (7);
}


void m68k_reset (bool hardreset)
{
	uae_u32 v;

	regs.spcflags = 0;
	regs.ipl = regs.ipl_pin = 0;
	v = get_long (4);
	m68k_areg (regs, 7) = get_long (0);
	m68k_setpc_normal(v);
	regs.s = 1;
	regs.m = 0;
	regs.stopped = 0;
	regs.t1 = 0;
	regs.t0 = 0;
	SET_ZFLG (0);
	SET_XFLG (0);
	SET_CFLG (0);
	SET_VFLG (0);
	SET_NFLG (0);
	regs.intmask = 7;
	regs.vbr = regs.sfc = regs.dfc = 0;
	regs.irc = 0xffff;

	regs.halted = 0;

	regs.pcr = 0;
	fill_prefetch ();
}

uae_u32 REGPARAM2 op_illg (uae_u32 opcode)
{
	uaecptr pc = m68k_getpc ();

	if ((opcode & 0xF000) == 0xF000) {
		Exception (0xB);
		//activate_debugger ();
		return 4;
	}
	if ((opcode & 0xF000) == 0xA000) {
		Exception (0xA);
		//activate_debugger();
		return 4;
	}

	Exception (4);
	return 4;
}


static uaecptr last_trace_ad = 0;

static void do_trace (void)
{
	if (regs.t1) {
		last_trace_ad = m68k_getpc ();
		unset_special (SPCFLAG_TRACE);
		set_special (SPCFLAG_DOTRACE);
	}
}


// handle interrupt delay (few cycles)
STATIC_INLINE bool time_for_interrupt (void)
{
	return regs.ipl > regs.intmask || regs.ipl == 7;
}

void doint (void)
{
	set_special (SPCFLAG_INT);
}

static int do_specialties (int cycles)
{
	regs.instruction_pc = m68k_getpc();

	if (regs.spcflags & SPCFLAG_DOTRACE)
		Exception (9);

	if (regs.spcflags & SPCFLAG_TRAP) {
		unset_special (SPCFLAG_TRAP);
		Exception (3);
	}

	while (regs.spcflags & SPCFLAG_STOP) {
		x_do_cycles (4 * CYCLE_UNIT);

		if (regs.spcflags & (SPCFLAG_INT | SPCFLAG_DOINT)) {
			int intr = intlev ();
			unset_special (SPCFLAG_INT | SPCFLAG_DOINT);
			if (intr > 0 && intr > regs.intmask)
				do_interrupt (intr);
		}

	}

	if (regs.spcflags & SPCFLAG_TRACE)
		do_trace ();

	if (regs.spcflags & SPCFLAG_INT) {
		int intr = intlev ();
		unset_special (SPCFLAG_INT | SPCFLAG_DOINT);
		if (intr > 0 && (intr > regs.intmask || intr == 7))
			do_interrupt (intr);
	}

	if (regs.spcflags & SPCFLAG_DOINT) {
		unset_special (SPCFLAG_DOINT);
		set_special (SPCFLAG_INT);
	}

	return 0;
}

void cpu_halt (int id)
{
	if (!regs.halted) {
		regs.halted = id;
		regs.intmask = 7;
		MakeSR ();
	}
	while (regs.halted) {
		x_do_cycles (100 * CYCLE_UNIT);
	}
}

/* It's really sad to have two almost identical functions for this, but we
do it all for performance... :(
This version emulates 68000's prefetch "cache" */
void m68k_run_1 (void)
{
	struct regstruct *r = &regs;

	uae_u16 opcode = r->ir;

	do_cycles (cpu_cycles);
	r->instruction_pc = m68k_getpc ();
	cpu_cycles = (*cpufunctbl[opcode])(opcode);
	if (r->spcflags) {
		if (do_specialties (cpu_cycles)) {
			regs.ipl = regs.ipl_pin;
			return;
		}
	}
	regs.ipl = regs.ipl_pin;
}

static void exception3f (uae_u32 opcode, uaecptr addr, int writeaccess, int instructionaccess, uaecptr pc)
{
	if (pc == 0xffffffff) {
		last_addr_for_exception_3 = m68k_getpc () + 2;
	} else {
		last_addr_for_exception_3 = pc;
	}
	last_fault_for_exception_3 = addr;
	last_op_for_exception_3 = opcode;
	last_writeaccess_for_exception_3 = writeaccess;
	last_instructionaccess_for_exception_3 = instructionaccess;
	Exception (3);
}

void exception3 (uae_u32 opcode, uaecptr addr)
{
	exception3f (opcode, addr, 0, 0, 0xffffffff);
}
void exception3i (uae_u32 opcode, uaecptr addr)
{
	exception3f (opcode, addr, 0, 1, 0xffffffff);
}
void exception3b (uae_u32 opcode, uaecptr addr, bool w, bool i, uaecptr pc)
{
	exception3f (opcode, addr, w, i, pc);
}

void cpureset (void)
{
    /* RESET hasn't increased PC yet, 1 word offset */
	uaecptr pc;
	uaecptr ksboot = 0xf80002 - 2;
	m68k_setpc_normal (ksboot);
}


void m68k_setstopped (void)
{
	regs.stopped = 1;
	/* A traced STOP instruction drops through immediately without
	actually stopping.  */
	if ((regs.spcflags & SPCFLAG_DOTRACE) == 0)
		set_special (SPCFLAG_STOP);
	else
		m68k_resumestopped ();
}

void m68k_resumestopped (void)
{
	if (!regs.stopped)
		return;
	regs.stopped = 0;
	fill_prefetch ();
	unset_special (SPCFLAG_STOP);
}


void fill_prefetch (void)
{
	uaecptr pc = m68k_getpc ();
	regs.ir = x_get_word (pc);
	regs.irc = x_get_word (pc + 2);
}
