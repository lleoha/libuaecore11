AM_CPPFLAGS = -DLIBUAECORE11_BUILD -I $(top_srcdir)/src/WinUAE/include -I $(top_srcdir)/src/core -I tools
AM_CXXFLAGS = -fpermissive

lib_LTLIBRARIES = libuaecore11.la

noinst_HEADERS = WinUAE/include/sysconfig.h WinUAE/include/events.h WinUAE/include/machdep/m68k.h WinUAE/include/tchar.h \
                 WinUAE/include/newcpu.h WinUAE/include/options.h WinUAE/include/sysdeps.h WinUAE/include/cpu_prefetch.h \
                 WinUAE/include/memory.h WinUAE/include/custom.h WinUAE/include/readcpu.h core/core_glue.h

uaecore11includedir=$(includedir)/uaecore11
uaecore11include_HEADERS = core/core_api.h

dist_noinst_DATA = WinUAE/table68k

# libuaecore11
libuaecore11_la_SOURCES = WinUAE/newcpu.cpp WinUAE/readcpu.cpp core/core.cpp core/core_glue.cpp
nodist_libuaecore11_la_SOURCES = cpuemu_11.cpp cpuemu_13_stub.cpp cpustbl.cpp cpudefs.cpp
libuaecore11_la_CXXFLAGS = -fvisibility=hidden $(AM_CXXFLAGS)
libuaecore11_la_LDFLAGS = -no-undefined $(AM_LDFLAGS)

cpuemu_11.cpp: tools/cpuemu_11.cpp
	cp $< $@

cpuemu_13_stub.cpp: tools/cpuemu_13_stub.cpp
	cp $< $@

cpustbl.cpp: tools/cpustbl.cpp
	cp $< $@

cpudefs.cpp: tools/cpudefs.cpp
	cp $< $@
	

tools/cpuemu_11.cpp tools/cpuemu_13_stub.cpp tools/cpustbl.cpp tools/cputbl.h tools/cpudefs.cpp: tools/Makefile
	$(MAKE) -C tools $(notdir $@)

SUBDIRS = tools
CLEANFILES = cpuemu_11.cpp cpuemu_13_stub.cpp cpustbl.cpp cpudefs.cpp
