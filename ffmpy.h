
typedef int callback(int id, int time, int* lines, int size, int width, int height);
int open(int id, char *fname, callback frameCallback);
int other(int one);
