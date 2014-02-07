
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#include <stdio.h>

#include "ffmpy.h"

void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame) {
  FILE *pFile;
  char szFilename[32];
  int  y;
  
  // Open file
  sprintf(szFilename, "frame%d.ppm", iFrame);
  pFile=fopen(szFilename, "wb");
  if(pFile==NULL)
    return;
  
  // Write header
  fprintf(pFile, "P6\n%d %d\n255\n", width, height);
  
  // Write pixel data
  for(y=0; y<height; y++)
    fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, pFile);
  
  // Close file
  fclose(pFile);
}

int cb(int id, int time, char* lines, int size, int width, int height) {
    printf("Hi %d\n", time);
    return 0;
}

int other(int one) {
    return one * 2;
}

int newpen(int id, char* fname, callback cb) {
    AVFormatContext *pFormatCtx = NULL;
    int             i, videoStream;
    AVCodecContext  *pCodecCtx = NULL;
    AVCodec         *pCodec = NULL;
    AVFrame         *pFrame = NULL; 
    AVFrame         *pFrameRGB = NULL;
    AVPacket        packet;
    int             quit = 0;
    int             frameFinished;
    int             numBytes;
    uint8_t         *buffer = NULL;
    printf("1\n");
    AVDictionary    *optionsDict = NULL;
    struct SwsContext      *sws_ctx = NULL;
    // Register all formats and codecs
    av_register_all();

    // Open video file
    if(avformat_open_input(&pFormatCtx, fname, NULL, NULL)!=0)
        return -1; // Couldn't open file

    // Retrieve stream information
    if(avformat_find_stream_info(pFormatCtx, NULL)<0)
        return -1; // Couldn't find stream information

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
        return -1; // Didn't find a video stream
    }
    return 7;
}

int open(int id, char *fname, callback frameCallback) {
    return 7;
    /*

  
  // Get a pointer to the codec context for the video stream
  pCodecCtx=pFormatCtx->streams[videoStream]->codec;
  
  printf("2\n");
  // Find the decoder for the video stream
  pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
  if(pCodec==NULL) {
    fprintf(stderr, "Unsupported codec!\n");
    return -1; // Codec not found
  }
  // Open codec
  if(avcodec_open2(pCodecCtx, pCodec, &optionsDict)<0)
    return -1; // Could not open codec
  
  // Allocate video frame
  pFrame=avcodec_alloc_frame();
  
  // Allocate an AVFrame structure
  pFrameRGB=avcodec_alloc_frame();
  if(pFrameRGB==NULL)
    return -1;
  
  // Determine required buffer size and allocate buffer
  numBytes=avpicture_get_size(PIX_FMT_RGB24, pCodecCtx->width,
			      pCodecCtx->height);
  buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

  printf("3\n");
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
  
  printf("4\n");
  // Read frames and save first five frames to disk
  i=0;
  while(av_read_frame(pFormatCtx, &packet)>=0) {
    // Is this a packet from the video stream?
    if(packet.stream_index==videoStream) {
      // Decode video frame
      avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, 
			   &packet);

      printf("5\n");
      // Did we get a video frame?
      if(frameFinished) {
        printf("Time %" PRId64 ", %" PRId64 "\n", pFrame->pkt_pts, pFrame->pkt_dts);
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
        quit = frameCallback(
            id,
            (int)pFrame->pkt_pts,
            (int*)pFrameRGB->data[0],
            pFrameRGB->linesize[0],
            pCodecCtx->width,
            pCodecCtx->height
        );
        if (quit == 1) {
            break;
        }
        if (++i > 50) break;

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

  return 0;
    */
    return 5;
}

int main(int argc, char *argv[]) {
  if(argc < 2) {
    printf("Please provide a movie file\n");
    return -1;
  }
  open(0, argv[1], cb);
  return 0;
}

