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

static void usage() {
  std::cout << "This application continuously reads frames from an input file, runs" << std::endl
	    << "them through mosaic and stitches the final result" << std::endl << std::endl
	    << "  --width, -w width" << std::endl
	    << "  --height, -h height" << std::endl
	    << "  --in, -i input" << std::endl
	    << "  --out, -o output" << std::endl
	    << "  --format, -f format" << std::endl
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

  const struct option long_options[] = {
    {"width",  required_argument, 0, 'w'},
    {"height", required_argument, 0, 'h'},
    {"in",     required_argument, 0, 'i'},
    {"out",    required_argument, 0, 'o'},
    {"format", required_argument, 0, 'f'},
    {0,        0,                 0, 0}
  };

  char c;

  if (argc == 1) {
    usage();
    return 0;
  }

  while (1) {
    c = getopt_long(argc, argv, "w:h:i:o:f:", long_options, NULL);
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

  while (read(fd, in_data, size) == size) {
    // process
    int ret = m.addFrameRGB(in_data);

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

  if (m.createMosaic(progress, cancel) != Mosaic::MOSAIC_RET_OK) {
    std::cerr << "Failed to stitch" << std::endl;
    return 1;
  }

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
  return 0;
}
