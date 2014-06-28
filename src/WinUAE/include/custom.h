/*
* UAE - The Un*x Amiga Emulator
*
* custom chip support
*
* (c) 1995 Bernd Schmidt
*/

#ifndef CUSTOM_H
#define CUSTOM_H

#define SPCFLAG_STOP 2
#define SPCFLAG_COPPER 4
#define SPCFLAG_INT 8
#define SPCFLAG_BRK 16
#define SPCFLAG_TRACE 64
#define SPCFLAG_DOTRACE 128
#define SPCFLAG_DOINT 256 /* arg, JIT fails without this.. */
#define SPCFLAG_BLTNASTY 512
#define SPCFLAG_EXEC 1024
#define SPCFLAG_ACTION_REPLAY 2048
#define SPCFLAG_TRAP 4096 /* enforcer-hack */
#define SPCFLAG_MODE_CHANGE 8192

#endif /* CUSTOM_H */
