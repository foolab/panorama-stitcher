#include <iostream>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <vector>
#include <stdint.h>
#include <sys/time.h>
#include <numeric>
#include <png.h>
#include <sys/param.h>
#include "stitcher/tracker.h"
#include "stitcher/stitcher.h"
extern "C" {
#include <libswscale/swscale.h>
};

extern int opterr;

static const char *strips[] = {"Thin", "Wide"};

static bool write_png(const char *out, const unsigned char *rgb, int width, int height) {
  FILE *fp = NULL;
  png_structp png_ptr;
  png_infop info_ptr;
  png_bytep row;

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
    png_bytep row = const_cast<unsigned char *>(&rgb[width * x * 3]);
    png_write_row(png_ptr, row);
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

  int src_width = -1, src_height = -1;
  int tracker_width, tracker_height;
  int src_size, tracker_size;
  int max_frames = -1;

  const char *in = NULL;
  const char *out = NULL;
  bool time = false;
  int stripType = Stitcher::Thin;

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
      src_width = atoi(optarg);
      break;

    case 'h':
      src_height = atoi(optarg);
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
  if (src_width <= 0) {
    std::cerr << "invalid width " << src_width << std::endl;
    return 1;
  }

  if (src_height <= 0) {
    std::cerr << "invalid height " << src_height << std::endl;
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

  src_size = (src_width * src_height * 12) / 8;

  tracker_width = src_width > 720 ? src_width / 8 : src_width / 4;
  tracker_height = src_width > 720 ? src_height / 8 : src_height / 4;
  tracker_size = tracker_width * tracker_height;

  std::cout << "input width = " << src_width << ", height = " << src_height << std::endl;
  std::cout << "strip type: " << stripType << " (" << strips[stripType] << ")" << std::endl;
  std::cout << "tracker width = " << tracker_width << ", height = " << tracker_height << std::endl;
  int frames = count_frames(fd, src_size);

  if (frames == -1) {
    close(fd);
    return 1;
  }

  if (max_frames > 0) {
    frames = MIN(frames, max_frames);
  }

  // create tracker
  Tracker tracker(frames);
  if (!tracker.initialize(tracker_width, tracker_height)) {
    close(fd);
    return 1;
  }

#if LIBAVUTIL_VERSION_MAJOR >= 52
  enum AVPixelFormat fmt = AV_PIX_FMT_GRAY8;
#else
  enum PixelFormat fmt = PIX_FMT_GRAY8;
#endif
  // scalar
  struct SwsContext *scalar = sws_getContext(src_width, src_height, fmt,
					     tracker_width, tracker_height, fmt,
					     SWS_BICUBIC, NULL, NULL, NULL);

  if (!scalar) {
    std::cerr << "Failed to create scalar" << std::endl;
    return 1;
  }

  std::vector<unsigned char *> full_frames, scaled_frames;
  unsigned char *full_frame = 0;
  unsigned char *scaled_frame = 0;

  std::vector<int> tracker_times, full_times;

  for (int x = 0; x < frames; x++) {
    if (!full_frame) {
      full_frame = new unsigned char[src_size];
    }

    if (!scaled_frame) {
      scaled_frame = new unsigned char[tracker_size];
    }

    if (read(fd, full_frame, src_size) != src_size) {
      break;
    }

    // our input is I420 so we scale the Y frame only
    const uint8_t *const srcSlice[] = {full_frame, NULL, NULL};
    const int srcStride[] = {src_width, 0, 0};
    uint8_t *const dst[] = {scaled_frame, NULL, NULL};
    const int dstStride[] = {tracker_width, 0, 0};

    if (sws_scale(scalar, srcSlice, srcStride, 0, src_height, dst, dstStride) < 0) {
      abort();
    }

    uint64_t t = timeNow();
    Tracker::Return ret = tracker.addFrame(scaled_frame);
    t = timeNow() - t;
    tracker_times.push_back(t);

    if (ret >= 0) {
      scaled_frames.push_back(scaled_frame);
      scaled_frame = 0;

      full_frames.push_back(full_frame);
      full_frame = 0;
    }
  }

  // Now that we are done, let's try to stitch
  Stitcher stitcher(src_width, src_height, full_frames.size(), (Stitcher::StripType)stripType);
  for (int x = 0; x < full_frames.size(); x++) {
    uint64_t t = timeNow();
    Stitcher::Return ret = stitcher.addFrame(full_frames[x]);
    t = timeNow() - t;
    full_times.push_back(t);

    if (ret < Stitcher::Ok) {
      std::cerr << "Weird! Stitcher did not return Ok for frame " << x << std::endl;
    }
  }

  uint64_t stitchingTime = timeNow();
  Stitcher::Return ret = stitcher.stitch();
  stitchingTime = timeNow() - stitchingTime;

  if (ret != Stitcher::Ok) {
    std::cerr << "Failed to stitch" << std::endl;
    return 1;
  }

  int width, height;
  const unsigned char *rgb = stitcher.image(width, height);

  bool res = write_png(out, rgb, width, height);

  if (res) {
    std::cout << "Wrote mosaic image to " << out << std::endl;
    std::cout << "Width = " << width << " height = " << height << std::endl;
  }

  if (time) {
    std::cout << "Average tracker frame time = "
	      << std::accumulate(tracker_times.begin(), tracker_times.end(), 0) / tracker_times.size() << "ms" << std::endl;
    std::cout << "Average stitcher frame time = "
	      << std::accumulate(full_times.begin(), full_times.end(), 0) / full_times.size() << "ms" << std::endl;
    std::cout << "Final stitching time = " << stitchingTime << "ms" << std::endl;
  }

  return res ? 0 : 1;
}
