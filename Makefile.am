if GPROF
AM_CXXFLAGS = -pg
AM_CFLAGS = -pg
endif

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = log db_vlvm dbreg mosaic stitcher dbregtest

lib_LTLIBRARIES = libpanorama-stitcher.la
libpanorama_stitcher_la_LIBADD = \
	log/liblog.la \
	db_vlvm/libdb_vlvm.la \
	dbreg/libdbreg.la \
	mosaic/libmosaic.la \
	stitcher/libstitcher.la

# if I don't add this then autofoo blows up
libpanorama_stitcher_la_SOURCES = 

bin_PROGRAMS =

if STITCH
bin_PROGRAMS += stitch
stitch_SOURCES = stitch.cpp
stitch_CXXFLAGS = $(AM_CXXFLAGS) $(PNG_CFLAGS)
stitch_LDADD = libpanorama-stitcher.la $(PNG_LIBS)
endif

if STITCH2
bin_PROGRAMS += stitch2
stitch2_SOURCES = stitch2.cpp
stitch2_CXXFLAGS = $(AM_CXXFLAGS) $(PNG_CFLAGS) $(SWSCALE_CFLAGS)
stitch2_LDADD = libpanorama-stitcher.la $(PNG_LIBS)  $(SWSCALE_LIBS)
endif
