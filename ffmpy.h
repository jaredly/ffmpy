
typedef int callback(int64_t time, uint8_t* lines, int size, int width, int height, int i);
int open(char *fname, callback frameCallback);

