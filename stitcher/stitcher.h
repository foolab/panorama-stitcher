#ifndef STITCHER_H
#define STITCHER_H

class Mosaic;

class Stitcher {
public:
  typedef enum {
    Thin = 0,
    Wide = 1,
  } StripType;

  typedef enum {
    LowTexture = -3,
    Cancelled = -2,
    Error = -1,
    Ok = 1,
    FewInliers = 2,
  } Return;

  Stitcher(int width, int height, int maxFrames,
	   const StripType& stripType = Thin,
	   float stillCameraTranslationThreshold = 0.0);
  virtual ~Stitcher();

  Return addFrame(unsigned char * data);

  Return stitch();
  void cancel();
  float progress();

  const unsigned char *image(int& width, int& height);

private:
  Mosaic *m_mosaic;
  float m_progress;
  bool m_cancel;
  unsigned char *m_rgb;
};

#endif /* STITCHER_H */
