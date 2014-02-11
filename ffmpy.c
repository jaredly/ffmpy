
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#include <stdio.h>

#include "ffmpy.h"


int cb(int id, int i, int num, int den, int start, int dur, int pts, int dts, char* lines, int size, int width, int height) {
  printf("Hi %d\n", time);
  return 0;
}

int other(int one) {
  return one * 2;
}

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

int initState(char* fname, state *s) {
  int             i, videoStream;

  uint8_t         *buffer = NULL;
  AVCodec         *pCodec = NULL;
  AVFrame         *pFrame = NULL; 
  struct SwsContext *sws_ctx = NULL;
  AVCodecContext  *pCodecCtx = NULL;
  AVFrame         *pFrameRGB = NULL;
  AVFormatContext *pFormatCtx = NULL;
  AVDictionary    *optionsDict = NULL;

  AVPacket        packet;
  int             quit = 0;
  int             numBytes;
  int             frameFinished;

  // Register all formats and codecs
  av_register_all();

  // Open video file
  if(avformat_open_input(&pFormatCtx, fname, NULL, NULL)!=0) {
    fprintf(stderr, "File not found\n");
    return -1; // Couldn't open file
  }

  // Retrieve stream information
  if(avformat_find_stream_info(pFormatCtx, NULL)<0) {
    fprintf(stderr, "No stream information\n");
    return -1; // Couldn't find stream information
  }

  // Dump information about file onto standard error
  av_dump_format(pFormatCtx, 0, fname, 0);

  // Find the first video stream
  videoStream=-1;
  for(i=0; i<pFormatCtx->nb_streams; i++) {
    if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
      videoStream=i;
      break;
    }
  }
  if(videoStream==-1) {
    fprintf(stderr, "No video stream found\n");
    return -1; // Didn't find a video stream
  }
  // Get a pointer to the codec context for the video stream
  pCodecCtx=pFormatCtx->streams[videoStream]->codec;

  // Find the decoder for the video stream
  pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
  if(pCodec==NULL) {
    fprintf(stderr, "Unsupported codec!\n");
    return -1; // Codec not found
  }
  // Open codec
  if(avcodec_open2(pCodecCtx, pCodec, &optionsDict)<0) {
    fprintf(stderr, "Coundn't open codec\n");
    return -1; // Could not open codec
  }

  // Allocate video frame
  pFrame=avcodec_alloc_frame();

  // Allocate an AVFrame structure
  pFrameRGB=avcodec_alloc_frame();
  if(pFrameRGB==NULL) {
    fprintf(stderr, "Allocation error (rgb buffer)\n");
    return -1;
  }

  // Determine required buffer size and allocate buffer
  numBytes=avpicture_get_size(PIX_FMT_RGB24, pCodecCtx->width,
      pCodecCtx->height);
  buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

  sws_ctx =
    sws_getContext
    (
     pCodecCtx->width,
     pCodecCtx->height,
     pCodecCtx->pix_fmt,
     pCodecCtx->width,
     pCodecCtx->height,
     PIX_FMT_RGB24,
     SWS_BILINEAR,
     NULL,
     NULL,
     NULL
    );

  // Assign appropriate parts of buffer to image planes in pFrameRGB
  // Note that pFrameRGB is an AVFrame, but AVFrame is a superset
  // of AVPicture
  avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_RGB24,
      pCodecCtx->width, pCodecCtx->height);

  s->videoStream = videoStream;
  s->buffer  = buffer;
  s->pCodec  = pCodec;
  s->pFrame  = pFrame;
  s->sws_ctx  = sws_ctx;
  s->pCodecCtx  = pCodecCtx;
  s->pFrameRGB  = pFrameRGB;
  s->pFormatCtx  = pFormatCtx;
  s->optionsDict  = optionsDict;
}

void processFrames(state *s, callback cb) {
  AVPacket        packet;
  int             quit = 0;
  int i=0;
  int             numBytes;
  int             frameFinished;
  int id = s->id;

  // Read frames and save first five frames to disk
  i=0;
  while(av_read_frame(s->pFormatCtx, &packet)>=0) {
    // Is this a packet from the video stream?
    if(packet.stream_index==s->videoStream) {
      // Decode video frame
      avcodec_decode_video2(s->pCodecCtx, s->pFrame, &frameFinished, 
          &packet);

      // Did we get a video frame?
      if(frameFinished) {
        // printf("Time %" PRId64 ", %" PRId64 "\n", pFrame->pkt_pts, pFrame->pkt_dts);
        // Convert the image from its native format to RGB
        sws_scale
          (
           s->sws_ctx,
           (uint8_t const * const *)s->pFrame->data,
           s->pFrame->linesize,
           0,
           s->pCodecCtx->height,
           s->pFrameRGB->data,
           s->pFrameRGB->linesize
          );
        quit = cb(
            id,
            i++,
            (int)s->pFormatCtx->streams[s->videoStream]->time_base.num,
            (int)s->pFormatCtx->streams[s->videoStream]->time_base.den,
            (int)s->pFormatCtx->streams[s->videoStream]->start_time,
            (int)s->pFormatCtx->streams[s->videoStream]->duration,
            (int)s->pFrame->pkt_pts,
            (int)s->pFrame->pkt_dts,
            (char*)s->pFrameRGB->data[0],
            s->pFrameRGB->linesize[0],
            s->pCodecCtx->width,
            s->pCodecCtx->height
            );
        if (quit == 1) {
          break;
        }
        // if (i > 5000) break;
      }
    }

    // Free the packet that was allocated by av_read_frame
    av_free_packet(&packet);
  }
}

int getFrames(int id, char* fname, callback cb) {

  state s;

  s.id = id;
  s.buffer = NULL;
  s.pCodec = NULL;
  s.pFrame = NULL; 
  s.sws_ctx = NULL;
  s.pCodecCtx = NULL;
  s.pFrameRGB = NULL;
  s.pFormatCtx = NULL;
  s.optionsDict = NULL;

  if (-1 == initState(fname, &s)) {
    return -1;
  }

  processFrames(&s, cb);

  cleanupState(&s);
  return 7;
}

void cleanupState(state *s) {

  // Free the RGB image
  av_free(s->buffer);
  av_free(s->pFrameRGB);

  // Free the YUV frame
  av_free(s->pFrame);

  // Close the codec
  avcodec_close(s->pCodecCtx);

  // Close the video file
  avformat_close_input(&(s->pFormatCtx));
}

