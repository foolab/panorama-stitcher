#include <iostream>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include "mosaic/Mosaic.h"
#include "mosaic/ImageUtils.h"
#include <vector>
#include <stdint.h>
#include <sys/time.h>
#include <numeric>

extern int opterr;

int blendingType = Blend::BLEND_TYPE_HORZ;
int stripType = Blend::STRIP_TYPE_THIN;

struct Format {
  const char *name;
  int bpp; /* bytes per pixel */
} Formats[] = {
  {"RGB", 3},
  {NULL, 0},
};

static uint64_t timeNow() {
  struct timeval tv;

  if (gettimeofday(&tv, NULL) == -1) {
    return -1;
  }

  uint64_t ret = tv.tv_usec;
  ret /= 1000;
  ret += (tv.tv_sec * 1000);

  return ret;
}

static void usage() {
  std::cout << "This application continuously reads frames from an input file, runs" << std::endl
	    << "them through mosaic and stitches the final result" << std::endl << std::endl
	    << "  --width, -w width" << std::endl
	    << "  --height, -h height" << std::endl
	    << "  --in, -i input" << std::endl
	    << "  --out, -o output" << std::endl
	    << "  --format, -f format" << std::endl
	    << "  --time, -t (Use to print times for operations)" << std::endl
	    << " Supported formats:" << std::endl;

  struct Format *f = Formats;
  while (f->name) {
    std::cout << "  " << (f - Formats) << " is " << f->name << std::endl;
    ++f;
  }
}

int main(int argc, char *argv[]) {
  opterr = 0;

  int width = -1;
  int height = -1;
  int format = -1;
  const char *in = NULL;
  const char *out = NULL;
  Format *fmt = NULL;
  bool time = false;

  const struct option long_options[] = {
    {"width",  required_argument, 0, 'w'},
    {"height", required_argument, 0, 'h'},
    {"in",     required_argument, 0, 'i'},
    {"out",    required_argument, 0, 'o'},
    {"format", required_argument, 0, 'f'},
    {"time",   no_argument      , 0, 't'},
    {0,        0,                 0, 0}
  };

  char c;

  if (argc == 1) {
    usage();
    return 0;
  }

  while (1) {
    c = getopt_long(argc, argv, "w:h:i:o:f:t", long_options, NULL);
    if (c == -1) {
      break;
    }

    switch (c) {
    case 'w':
      width = atoi(optarg);
      break;

    case 'h':
      height = atoi(optarg);
      break;

    case 'i':
      in = optarg;
      break;

    case 'o':
      out = optarg;
      break;

    case 'f':
      format = atoi(optarg);
      break;

    case 't':
      time = true;
      break;

    case '?':
      usage();
      return 0;
    }
  }

  // validate
  if (width <= 0) {
    std::cerr << "invalid width " << width << std::endl;
    return 1;
  }

  if (height <= 0) {
    std::cerr << "invalid height " << height << std::endl;
    return 1;
  }

  if (!in) {
    std::cerr << "input file not provided" << std::endl;
    return 1;
  }

  if (!out) {
    std::cerr << "output file not provided" << std::endl;
    return 1;
  }

  if (format < 0 || format >= sizeof (Formats) / sizeof (struct Format)) {
    std::cerr << "invalid format " << format << std::endl;
    return 1;
  }

  fmt = &Formats[format];

  // input
  int fd = open(in, O_RDONLY);
  if (fd == -1) {
    perror("open");
    return 1;
  }


  int size = width * height * fmt->bpp;

  // initialize our mosaicer
  Mosaic m;
  if (!m.initialize(blendingType, stripType, width, height, -1, true, 5.0f)) {
    std::cerr << "Failed to initialize mosaicer" << std::endl;
    close(fd);
    return 1;
  }

  unsigned char *in_data = new unsigned char[size];
  int added_frames = 0;

  std::vector<int> times;

  while (read(fd, in_data, size) == size) {
    // process
    uint64_t t = timeNow();

    int ret = m.addFrameRGB(in_data);

    times.push_back(timeNow() - t);

    if (ret == Mosaic::MOSAIC_RET_OK || ret == Mosaic::MOSAIC_RET_FEW_INLIERS) {
      ++added_frames;
    }

    // TODO: seems 200 is a hardcoded maximum
    if (added_frames == 200) {
      break;
    }
  }

  close(fd);
  delete[] in_data;

  std::cout << "Used " << added_frames << " frames" << std::endl;

  // output
  // TODO: what are those?
  float progress = 0;
  bool cancel = false;

  int64_t stitchingTime = timeNow();

  if (m.createMosaic(progress, cancel) != Mosaic::MOSAIC_RET_OK) {
    std::cerr << "Failed to stitch" << std::endl;
    return 1;
  }

  stitchingTime = timeNow() - stitchingTime;

  ImageType yuv = m.getMosaic(width, height);
  ImageType rgb = ImageUtils::allocateImage(width, height, 3, 0);
  ImageUtils::yvu2rgb(rgb, yuv, width, height);

  fd = open(out, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if (fd == -1) {
    perror("open");
    ImageUtils::freeImage(rgb);
    return 1;
  }

  size = width * height * 3;
  if (write(fd, rgb, size) != size) {
    perror("write");
    ImageUtils::freeImage(rgb);
    close(fd);
    return 1;
  }

  ImageUtils::freeImage(rgb);
  close(fd);
  std::cout << "Wrote mosaic image to " << out << std::endl;
  std::cout << "Width = " << width << " height = " << height << std::endl;

  if (time) {
    std::cout << "Average frame time = " << std::accumulate(times.begin(), times.end(), 0) / times.size() << "ms" << std::endl;
    std::cout << "Final stitching time = " << stitchingTime << "ms" << std::endl;
  }

  return 0;
}
