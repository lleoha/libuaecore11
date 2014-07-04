UAECore11 Library
------------
Motorola MC68000 emulation core from WinUAE.

Introduction
------------
UAECore11 is Motorola/Freescale [MC68000](http://www.freescale.com/webapp/sps/site/prod_summary.jsp?code=MC68000) CPU emulation library extracted from the great Amiga emulator [WinUAE](http://www.winuae.net/) maintained by Toni Wilen.

The goal of this project is to extract emulation core of MC68000 from WinUAE and add some user-friendly interface to use it.
WinUAE provides different flavours of the MC68000 emulator (let's call it levels):
 - Level 0 - simple CPU,
 - Level 11 - prefetch emulation,
 - Level 13 - cycle exact CPU.

Only "level 11" is extracted to the library hence the name.
