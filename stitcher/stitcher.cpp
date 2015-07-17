#include "stitcher.h"
#include "mosaic/Mosaic.h"
//#include <iostream>

Stitcher::Stitcher(int width, int height, int maxFrames,
		   const StripType& stripType,
		   float stillCameraTranslationThreshold) :
  m_mosaic(new Mosaic),
  m_progress(0.0),
  m_cancel(false),
  m_rgb(0) {

  m_mosaic->initialize(Blend::BLEND_TYPE_HORZ, (int)stripType, width, height,
		       maxFrames, false, stillCameraTranslationThreshold);
}

Stitcher::~Stitcher() {
  if (m_mosaic) {
    delete m_mosaic;
    m_mosaic = 0;
  }

  if (m_rgb) {
    delete[] m_rgb;
  }
}

Stitcher::Return Stitcher::addFrame(unsigned char * data) {
  return (Stitcher::Return) m_mosaic->addFrame(data);
}

Stitcher::Return Stitcher::stitch() {
  return (Stitcher::Return) m_mosaic->createMosaic(m_progress, m_cancel);
}

void Stitcher::cancel() {
  m_cancel = true;
}

float Stitcher::progress() {
  return m_progress;
}

const unsigned char *Stitcher::image(int& width, int& height) {
  unsigned char *yuv = m_mosaic->getMosaic(width, height);
  if (!m_rgb) {
    m_rgb = new unsigned char[width * height * 3];
  }

  ImageUtils::yvu2rgb(m_rgb, yuv, width, height);

  return m_rgb;
}
