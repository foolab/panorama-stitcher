#ifndef TRACKER_H
#define TRACKER_H

#include <vector>

class Align;

class Tracker {
public:
  typedef enum {
    LowTexture = -2,
    Error = -1,
    Ok = 0,
    FewInliers = 1,
  } Return;

  Tracker(int width, int height, int maxFrames, float stillCameraTranslationThreshold = 0.5);
  virtual ~Tracker();

  bool isInitialized() const;

  Return addFrame(unsigned char *frame, float *xTranslation = 0, float *yTranslation = 0);

private:
  Align *m_aligner;
  std::vector<unsigned char *> m_frames;
};

#endif /* TRACKER_H */
