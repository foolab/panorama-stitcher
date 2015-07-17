#include "tracker.h"
#include "mosaic/AlignFeatures.h"
#include <iostream>

Tracker::Tracker(int width, int height, int maxFrames, float stillCameraTranslationThreshold) :
  m_aligner(new Align) {

  if (m_aligner->initialize(width, height, false, stillCameraTranslationThreshold) ==
      Align::ALIGN_RET_ERROR) {
    std::cerr << "Tracker: Failed to initialize aligner" << std::endl;
    delete m_aligner;
    m_aligner = 0;
  } else {
    m_frames.reserve(maxFrames);
  }
}

Tracker::~Tracker() {
  if (m_aligner) {
    delete m_aligner;
    m_aligner = 0;
  }

  m_frames.clear();
}

Tracker::Return Tracker::addFrame(unsigned char *frame) {
  Tracker::Return ret = (Tracker::Return) m_aligner->addFrame(frame);
  if (ret >= 0) {
    m_frames.push_back(frame);
  }

  return ret;
}

bool Tracker::isInitialized() const {
  return m_aligner != 0;
}
