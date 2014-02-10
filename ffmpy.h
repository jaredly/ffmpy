
typedef int callback(int id, int pts, int dts, char* lines, int size, int width, int height);
int newpen(int id, char *fname, callback frameCallback);
int other(int one);
