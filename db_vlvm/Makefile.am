if GPROF
AM_CXXFLAGS = -pg -I$(top_builddir)/
AM_CFLAGS = -pg -I$(top_builddir)/
else
AM_CXXFLAGS = -I$(top_builddir)/
endif

noinst_LTLIBRARIES = libdb_vlvm.la

libdb_vlvm_la_SOURCES = \
	db_feature_detection.cpp \
	db_feature_matching.cpp \
	db_framestitching.cpp \
	db_image_homography.cpp \
	db_rob_image_homography.cpp \
	db_utilities_camera.cpp \
	db_utilities.cpp \
	db_utilities_indexing.cpp \
	db_utilities_linalg.cpp \
	db_utilities_poly.cpp

noinst_HEADERS = \
	db_bundle.h \
	db_feature_detection.h \
	db_feature_matching.h \
	db_framestitching.h \
	db_image_homography.h \
	db_metrics.h \
	db_rob_image_homography.h \
	db_robust.h \
	db_utilities_algebra.h \
	db_utilities_camera.h \
	db_utilities_constants.h \
	db_utilities_geometry.h \
	db_utilities.h \
	db_utilities_indexing.h \
	db_utilities_linalg.h \
	db_utilities_poly.h \
	db_utilities_random.h \
	db_utilities_rotation.h
