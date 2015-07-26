if GPROF
AM_CXXFLAGS = -pg -I$(top_builddir)/
AM_CFLAGS = -pg -I$(top_builddir)/
else
AM_CXXFLAGS = -I$(top_builddir)/
endif

noinst_LTLIBRARIES = libmosaic.la

libmosaic_la_SOURCES = \
	AlignFeatures.cpp \
	Blend.cpp \
	Delaunay.cpp \
	ImageUtils.cpp \
	Mosaic.cpp \
	Pyramid.cpp \
	trsMatrix.cpp

noinst_HEADERS = \
	AlignFeatures.h \
	Blend.h \
	CSite.h \
	Delaunay.h \
	EdgePointerUtil.h \
	Geometry.h \
	ImageUtils.h \
	Interp.h \
	MatrixUtils.h \
	Mosaic.h \
	MosaicTypes.h \
	Pyramid.h \
	trsMatrix.h
