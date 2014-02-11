
typedef int callback(int id, int i, int num, int den, int start, int dur, int pts, int dts, char* lines, int size, int width, int height);
int getFrames(int id, char *fname, callback frameCallback);
int other(int one);
