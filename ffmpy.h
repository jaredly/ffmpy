
typedef int callback(int id, int time, char* lines, int size, int width, int height);
int newpen(int id, char *fname, callback frameCallback);
int other(int one);
