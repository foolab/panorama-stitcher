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
#include <png.h>
#include <sys/param.h>

extern int opterr;

int blendingType = Blend::BLEND_TYPE_HORZ;

static const char *strips[] = {"Thin", "Wide"};

static bool write_png(const char *out, ImageType rgb, int width, int height) {
  FILE *fp = NULL;
  png_structp png_ptr;
  png_infop info_ptr;

  fp = fopen(out, "w");
  if (!fp) {
    perror("fopen");
    return false;
  }

  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr) {
    std::cerr << "Could not allocate write struct" << std::endl;
    fclose(fp);
    return false;
  }

  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    std::cerr << "Could not allocate info struct" << std::endl;
    fclose(fp);
    return false;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    std::cerr << "Error during png creation" << std::endl;
    fclose(fp);
    return false;
  }

  png_init_io(png_ptr, fp);
  png_set_IHDR(png_ptr, info_ptr, width, height,
	       8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
	       PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  png_write_info(png_ptr, info_ptr);

  for (int x = 0; x < height; x++) {
    png_write_row(png_ptr, &rgb[width * x * 3]);
  }

  png_write_end(png_ptr, NULL);
  fclose(fp);

  png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
  png_destroy_write_struct(&png_ptr, NULL);

  return true;
}

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
	    << "  --strip, -s strip type" << std::endl
	    << "  --max, -m maximum frames to process" << std::endl
	    << "  --time, -t (Use to print times for operations)" << std::endl
	    << " Supported strip types:\n  0 is thin (default)\n  1 is wide " << std::endl;
}

int count_frames(int fd, int size) {
  struct stat buf;
  if (fstat(fd, &buf) != 0) {
    perror("fstat");
    return -1;
  }

  return buf.st_size / size;
}

int main(int argc, char *argv[]) {
  opterr = 0;

  int width = -1;
  int height = -1;
  int max_frames = -1;
  const char *in = NULL;
  const char *out = NULL;
  bool time = false;
  int stripType = Blend::STRIP_TYPE_THIN;

  const struct option long_options[] = {
    {"width",  required_argument, 0, 'w'},
    {"height", required_argument, 0, 'h'},
    {"in",     required_argument, 0, 'i'},
    {"out",    required_argument, 0, 'o'},
    {"strip",  required_argument, 0, 's'},
    {"max",    required_argument, 0, 'm'},
    {"time",   no_argument      , 0, 't'},
    {0,        0,                 0, 0}
  };

  int c;

  if (argc == 1) {
    usage();
    return 0;
  }

  while (1) {
    c = getopt_long(argc, argv, "w:h:i:o:s:m:t", long_options, NULL);
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

    case 't':
      time = true;
      break;

    case 's':
      stripType = atoi(optarg);
      break;

    case 'm':
      max_frames = atoi(optarg);
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

  if (stripType < 0 || stripType > 1) {
    std::cerr << "invalid strip type " << stripType << std::endl;
    return 1;
  }

  // input
  int fd = open(in, O_RDONLY);
  if (fd == -1) {
    perror("open");
    return 1;
  }


  int size = (width * height * 12) / 8;

  std::cout << "input width = " << width << ", height = " << height << std::endl;
  std::cout << "strip type: " << stripType << " (" << strips[stripType] << ")" << std::endl;

  int frames = count_frames(fd, size);

  if (frames == -1) {
    close(fd);
    return 1;
  }

  if (max_frames > 0) {
    frames = MIN(frames, max_frames);
  }

  // initialize our mosaicer
  Mosaic m;
  if (!m.initialize(blendingType, stripType, width, height, frames, true, 5.0f)) {
    std::cerr << "Failed to initialize mosaicer" << std::endl;
    close(fd);
    return 1;
  }

  std::vector<int> times;
  std::vector<unsigned char *> mosaic_frames;
  unsigned char *in_data = NULL;

  for (int x = 0; x < frames; x++) {
    // allocate:
    if (!in_data) {
      in_data = new unsigned char[size];
    }

    if (read(fd, in_data, size) == size) {
      // process
      int time = timeNow();
      int ret = m.addFrame(in_data);
      time = timeNow() - time;

      times.push_back(time);

      if (ret == Mosaic::MOSAIC_RET_OK || ret == Mosaic::MOSAIC_RET_FEW_INLIERS) {
	mosaic_frames.push_back(in_data);
	in_data = NULL;
      }
    } else {
      break;
    }
  }

  if (in_data) {
    delete[] in_data;
  }

  close(fd);

  std::cout << "Used " << mosaic_frames.size() << " frames" << std::endl;

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
  ImageType rgb = ImageUtils::allocateImage(width, height, 3);
  ImageUtils::yvu2rgb(rgb, yuv, width, height);

  for (unsigned int x = 0; x < mosaic_frames.size(); x++) {
    delete[] mosaic_frames[x];
  }

  mosaic_frames.clear();

  bool res = write_png(out, rgb, width, height);
  ImageUtils::freeImage(rgb);
  if (!res) {
    return 1;
  }

  std::cout << "Wrote mosaic image to " << out << std::endl;
  std::cout << "Width = " << width << " height = " << height << std::endl;

  if (time) {
    std::cout << "Average frame time = " << std::accumulate(times.begin(), times.end(), 0) / times.size() << "ms" << std::endl;
    std::cout << "Final stitching time = " << stitchingTime << "ms" << std::endl;
  }

  return 0;
}
