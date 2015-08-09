#include "tracker.h"
#include "mosaic/AlignFeatures.h"
#include <iostream>

Tracker::Tracker(int maxFrames) :
  m_aligner(0) {

  m_frames.reserve(maxFrames);
}

Tracker::~Tracker() {
  if (m_aligner) {
    delete m_aligner;
    m_aligner = 0;
  }

  m_frames.clear();
}

bool Tracker::initialize(int width, int height, float stillCameraTranslationThreshold) {
  if (m_aligner) {
    return false;
  }

  m_aligner = new Align;

  if (m_aligner->initialize(width, height, false, stillCameraTranslationThreshold) ==
      Align::ALIGN_RET_ERROR) {
    std::cerr << "Tracker: Failed to initialize aligner" << std::endl;
    delete m_aligner;
    m_aligner = 0;

    return false;
  }

  return true;
}

Tracker::Return Tracker::addFrame(unsigned char *frame, float *xTranslation, float *yTranslation) {
  Tracker::Return ret = (Tracker::Return) m_aligner->addFrame(frame);
  if (ret >= 0) {
    m_frames.push_back(frame);
  }

  if (xTranslation || yTranslation) {
    float trs[3][3];

    m_aligner->getLastTRS(trs);

    if (xTranslation) {
      *xTranslation = trs[0][2];
    }

    if (yTranslation) {
      *yTranslation = trs[1][2];
    }
  }

  return ret;
}

bool Tracker::isInitialized() const {
  return m_aligner != 0;
}
