
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#include <stdio.h>

#include "ffmpy.h"

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

void cleanupState(state *s) {
  // Free the RGB image
  av_free(s->buffer);
  av_free(s->pFrameRGB);

  // Free the YUV frame
  av_free(s->pFrame);

  // Close the codec
  avcodec_close(s->pCodecCtx);

  // Close the video file
  avformat_close_input(&s->pFormatCtx);
}

int initState(char* fname, state *s) {
  int i;
  int numBytes;
  av_register_all();

  // Open video file
  if(avformat_open_input(&s->pFormatCtx, fname, NULL, NULL)!=0) {
    s->error =  "File not found\n";
    return -1; // Couldn't open file
  }

  // Retrieve stream information
  if(avformat_find_stream_info(s->pFormatCtx, NULL)<0) {
    s->error = "No stream information";
    return -1; // Couldn't find stream information
  }

  // Dump information about file onto standard error
  av_dump_format(s->pFormatCtx, 0, fname, 0);

  // Find the first video stream
  s->videoStream = -1;
  for(i=0; i<s->pFormatCtx->nb_streams; i++) {
    if(s->pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
      s->videoStream=i;
      break;
    }
  }
  if(s->videoStream==-1) {
    s->error = "No video stream found";
    return -1; // Didn't find a video stream
  }
  // Get a pointer to the codec context for the video stream
  s->pCodecCtx=s->pFormatCtx->streams[s->videoStream]->codec;

  // Find the decoder for the video stream
  s->pCodec=avcodec_find_decoder(s->pCodecCtx->codec_id);
  if(s->pCodec==NULL) {
    s->error = "Unsupported codec!";
    return -1; // Codec not found
  }
  // Open codec
  if(avcodec_open2(s->pCodecCtx, s->pCodec, &s->optionsDict)<0) {
    s->error = "Coundn't open codec";
    return -1; // Could not open codec
  }

  // Allocate video frame
  s->pFrame=avcodec_alloc_frame();
  if (s->pFrame==NULL) {
    s->error = "Allocation error (buffer)";
    return -1;
  }

  // Allocate an AVFrame structure
  s->pFrameRGB=avcodec_alloc_frame();
  if (s->pFrameRGB==NULL) {
    s->error = "Allocation error (rgb buffer)";
    return -1;
  }

  // Determine required buffer size and allocate buffer
  numBytes=avpicture_get_size(PIX_FMT_RGB24, s->pCodecCtx->width,
      s->pCodecCtx->height);
  s->buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

  s->sws_ctx =
    sws_getContext
    (
     s->pCodecCtx->width,
     s->pCodecCtx->height,
     s->pCodecCtx->pix_fmt,
     s->pCodecCtx->width,
     s->pCodecCtx->height,
     PIX_FMT_RGB24,
     SWS_BILINEAR,
     NULL,
     NULL,
     NULL
    );

  // Assign appropriate parts of buffer to image planes in pFrameRGB
  // Note that pFrameRGB is an AVFrame, but AVFrame is a superset
  // of AVPicture
  avpicture_fill((AVPicture *)s->pFrameRGB, s->buffer, PIX_FMT_RGB24,
      s->pCodecCtx->width, s->pCodecCtx->height);

  return 0;
}

void processFrames(state *s, callback cb) {
  AVPacket  packet;
  int i, frameFinished, quit = 0;

  // Read frames and save first five frames to disk
  i=0;
  while(av_read_frame(s->pFormatCtx, &packet)>=0) {
    // Is this a packet from the video stream?
    if(packet.stream_index!=s->videoStream) {
      // Decode video frame
      avcodec_decode_video2(s->pCodecCtx, s->pFrame, &frameFinished,
          &packet);

      // Did we get a video frame?
      if(frameFinished) {
        // printf("Time %" PRId64 ", %" PRId64 "\n", pFrame->pkt_pts, pFrame->pkt_dts);
        // Convert the image from its native format to RGB
        sws_scale(
           s->sws_ctx,
           (uint8_t const * const *)s->pFrame->data,
           s->pFrame->linesize,
           0,
           s->pCodecCtx->height,
           s->pFrameRGB->data,
           s->pFrameRGB->linesize
        );
        quit = cb(
            s->id,
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
      }
    }

    // Free the packet that was allocated by av_read_frame
    av_free_packet(&packet);
  }
}


int getFrames(int id, char* fname, callback cb) {
  state s;
  s.id = id;
  s.fname = fname;

  s.buffer = NULL;
  s.pCodec = NULL;
  s.pFrame = NULL;
  s.sws_ctx = NULL;
  s.pCodecCtx = NULL;
  s.pFrameRGB = NULL;
  s.pFormatCtx = NULL;
  s.optionsDict = NULL;

  printf("Get frames\n");

  if (-1 == initState(fname, &s)) {
    printf("Err\n");
    printf("Error initializing: %s", s.error);
    printf("\n");
    return -1;
  }
  printf("Good\n");

  // processFrames(&s, cb);
  cleanupState(&s);
  return 0;
}


/**
int newpen(int id, char* fname, callback cb) {
  int             i, videoStream;
  AVFormatContext *pFormatCtx = NULL;
  AVCodecContext  *pCodecCtx = NULL;
  AVCodec         *pCodec = NULL;
  AVFrame         *pFrame = NULL;
  AVFrame         *pFrameRGB = NULL;
  AVPacket        packet;
  int             quit = 0;
  int             frameFinished;
  int             numBytes;
  uint8_t         *buffer = NULL;
  AVDictionary    *optionsDict = NULL;
  struct SwsContext      *sws_ctx = NULL;
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

  // Read frames and save first five frames to disk
  i=0;
  while(av_read_frame(pFormatCtx, &packet)>=0) {
    // Is this a packet from the video stream?
    if(packet.stream_index!=videoStream) {
      // Decode video frame
      avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished,
          &packet);

      // Did we get a video frame?
      if(frameFinished) {
        // printf("Time %" PRId64 ", %" PRId64 "\n", pFrame->pkt_pts, pFrame->pkt_dts);
        // Convert the image from its native format to RGB
        sws_scale
          (
           sws_ctx,
           (uint8_t const * const *)pFrame->data,
           pFrame->linesize,
           0,
           pCodecCtx->height,
           pFrameRGB->data,
           pFrameRGB->linesize
          );
        quit = cb(
            id,
            i++,
            (int)pFormatCtx->streams[videoStream]->time_base.num,
            (int)pFormatCtx->streams[videoStream]->time_base.den,
            (int)pFormatCtx->streams[videoStream]->start_time,
            (int)pFormatCtx->streams[videoStream]->duration,
            (int)pFrame->pkt_pts,
            (int)pFrame->pkt_dts,
            (char*)pFrameRGB->data[0],
            pFrameRGB->linesize[0],
            pCodecCtx->width,
            pCodecCtx->height
            );
        if (quit == 1) {
          break;
        }
      }
    }

    // Free the packet that was allocated by av_read_frame
    av_free_packet(&packet);
  }
  // Free the RGB image
  av_free(buffer);
  av_free(pFrameRGB);

  // Free the YUV frame
  av_free(pFrame);

  // Close the codec
  avcodec_close(pCodecCtx);

  // Close the video file
  avformat_close_input(&pFormatCtx);
  return 7;
}
**/

