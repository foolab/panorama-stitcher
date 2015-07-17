ACLOCAL_AMFLAGS = -I m4
AM_CXXFLAGS = -I./db_vlvm/

lib_LTLIBRARIES = libpanorama-stitcher.la
libpanorama_stitcher_la_SOURCES = \
	mosaic/AlignFeatures.cpp \
	mosaic/Delaunay.cpp \
	mosaic/Mosaic.cpp \
	mosaic/trsMatrix.cpp \
	mosaic/Blend.cpp \
	mosaic/ImageUtils.cpp \
	mosaic/Pyramid.cpp \
	mosaic/Log.cpp \
	dbreg/dbreg.cpp \
	dbreg/dbstabsmooth.cpp \
	dbreg/vp_motionmodel.c \
	db_vlvm/db_feature_detection.cpp \
	db_vlvm/db_utilities_camera.cpp \
	db_vlvm/db_feature_matching.cpp \
	db_vlvm/db_utilities.cpp \
	db_vlvm/db_framestitching.cpp \
	db_vlvm/db_utilities_indexing.cpp \
	db_vlvm/db_image_homography.cpp \
	db_vlvm/db_utilities_linalg.cpp \
	db_vlvm/db_rob_image_homography.cpp \
	db_vlvm/db_utilities_poly.cpp \
	stitcher/stitcher.cpp \
	stitcher/tracker.cpp

bin_PROGRAMS = stitch
stitch_SOURCES = stitch.cpp
stitch_CXXFLAGS = $(AM_CXXFLAGS) $(PNG_CFLAGS)
stitch_LDADD = libpanorama-stitcher.la $(PNG_LIBS)
