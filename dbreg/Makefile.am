if GPROF
AM_CXXFLAGS = -pg -I$(top_srcdir)/
AM_CFLAGS = -pg -I$(top_srcdir)/
else
AM_CXXFLAGS = -I$(top_srcdir)/
endif

noinst_LTLIBRARIES = libdbreg.la

libdbreg_la_SOURCES = \
	dbreg.cpp \
	dbstabsmooth.cpp \
	vp_motionmodel.c

noinst_HEADERS = \
	dbreg.h \
	dbstabsmooth.h \
	vp_motionmodel.h
