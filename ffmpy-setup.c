
#include "ffmpy.h"

int initState(char* fname, state *s) {

  s->buffer = NULL;
  s->pCodec = NULL;

  s->pFrame = NULL; 
  s->sws_ctx = NULL;
  s->pCodecCtx = NULL;
  s->pFrameRGB = NULL;
  s->pFormatCtx = NULL;
  s->optionsDict = NULL;

  int numBytes;
  int i;

  // Register all formats and codecs
  av_register_all();

  // Open video file
  if(avformat_open_input(&(s->pFormatCtx), fname, NULL, NULL)!=0) {
    fprintf(stderr, "File not found\n");
    return -1; // Couldn't open file
  }

  // Retrieve stream information
  if(avformat_find_stream_info(s->pFormatCtx, NULL)<0) {
    fprintf(stderr, "No stream information\n");
    return -1; // Couldn't find stream information
  }

  // Dump information about file onto standard error
  av_dump_format(s->pFormatCtx, 0, fname, 0);

  // Find the first video stream
  s->videoStream=-1;
  for(i=0; i<s->pFormatCtx->nb_streams; i++) {
    if(s->pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
      s->videoStream=i;
      break;
    }
  }
  if(s->videoStream==-1) {
    fprintf(stderr, "No video stream found\n");
    return -1; // Didn't find a video stream
  }
  // Get a pointer to the codec context for the video stream
  s->pCodecCtx=s->pFormatCtx->streams[s->videoStream]->codec;

  // Find the decoder for the video stream
  s->pCodec=avcodec_find_decoder(s->pCodecCtx->codec_id);
  if(s->pCodec==NULL) {
    fprintf(stderr, "Unsupported codec!\n");
    return -1; // Codec not found
  }
  // Open codec
  if(avcodec_open2(s->pCodecCtx, s->pCodec, &(s->optionsDict))<0) {
    fprintf(stderr, "Coundn't open codec\n");
    return -1; // Could not open codec
  }

  // Allocate video frame
  s->pFrame=avcodec_alloc_frame();

  // Allocate an AVFrame structure
  s->pFrameRGB=avcodec_alloc_frame();
  if(s->pFrameRGB==NULL) {
    fprintf(stderr, "Allocation error (rgb buffer)\n");
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

