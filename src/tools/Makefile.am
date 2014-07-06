AM_CXXFLAGS = -fpermissive
AM_CPPFLAGS = -I $(top_srcdir)/../WinUAE/include

noinst_PROGRAMS = build68k gencpu

build68k_SOURCES = build68k.cpp

gencpu_SOURCES = gencpu.cpp readcpu.cpp cpudefs.cpp


cpudefs.cpp: $(top_srcdir)/../WinUAE/table68k build68k$(EXEEXT)
	$(AM_V_GEN)./build68k$(EXEEXT) <$< >$@

cpuemu_0.cpp cpuemu_11.cpp cpuemu_13.cpp cpuemu_20.cpp cpuemu_21.cpp cpuemu_22.cpp cpuemu_31.cpp cpuemu_32.cpp cputbl.h cpustbl.cpp cpuemu_33.cpp: gencpu$(EXEEXT)
	$(AM_V_GEN)./gencpu$(EXEEXT)

cpuemu_13_stub.cpp: cpuemu_13.cpp
	$(AM_V_GEN)cat $< | grep -e 'sys\|CPUFUNC' | sed 's/^void.*)$$/\0 {}/g' > $@

CLEANFILES = cpudefs.cpp cpuemu_0.cpp cpuemu_11.cpp cpuemu_13.cpp cpuemu_20.cpp \
             cpuemu_21.cpp cpuemu_22.cpp cpuemu_31.cpp cpuemu_32.cpp cputbl.h \
             cpustbl.cpp cpuemu_33.cpp cpuemu_13_stub.cpp

