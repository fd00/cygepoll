
nobase_include_HEADERS = sys/epoll.h

noinst_PROGRAMS = echo
echo_SOURCES = echo.c
echo_LDADD = libepoll.la

lib_LTLIBRARIES = libepoll.la
libepoll_la_SOURCES = epoll.c
libepoll_la_LIBADD = $(UPOLL_LIBS)
libepoll_la_LDFLAGS = -no-undefined

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = epoll.pc
