
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

typedef int callback(int id, int i, int num, int den, int start, int dur, int pts, int dts, char* lines, int size, int width, int height);
int getFrames(int id, char *fname, callback frameCallback);

typedef struct {
  uint8_t           *buffer;
  AVCodec           *pCodec;
  AVFrame           *pFrame;
  struct SwsContext *sws_ctx;
  AVCodecContext    *pCodecCtx;
  AVFrame           *pFrameRGB;
  AVFormatContext   *pFormatCtx;
  AVDictionary      *optionsDict;

  int               videoStream;
  char*             error;
  char*             fname;
  int               id;
} state;

// setup functions
int initState(char*, state*);
void cleanupState(state*);

typedef struct {
  int index;
  int numerator;
  int denominator;
  int start_time;
  int duration;
  int play_time;
  int decode_time;
  char* data;
  int line_size;
  int width;
  int height;
} status;

typedef callback(int id, status s);

