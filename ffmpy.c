
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#include <stdio.h>

#include "ffmpy.h"

/*
int cb(int id, int i, int num, int den, int start, int dur, int pts, int dts, char* lines, int size, int width, int height) {
  printf("Hi %d\n", time);
  return 0;
}
*/

void processFrames(state *s, callback cb) {
  AVPacket   packet;
  int        quit = 0;
  int        i=0;
  int        frameFinished;
  status     st;

  st.numerator = (int)s->pFormatCtx->streams[s->videoStream]->time_base.num;
  st.denominator = (int)s->pFormatCtx->streams[s->videoStream]->time_base.den;
  st.start_time = (int)s->pFormatCtx->streams[s->videoStream]->start_time;
  st.duration = (int)s->pFormatCtx->streams[s->videoStream]->duration;
  st.width = s->pCodecCtx->width;
  st.height = s->pCodecCtx->height;

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

        st.play_time = (int)s->pFrame->pkt_pts;
        st.decode_time = (int)s->pFrame->pkt_dts;
        st.line_size = s->pFrameRGB->linesize[0];
        st.data = (char*)s->pFrameRGB->data[0];
        st.index = i++;

        quit = cb(s->id, st);

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

