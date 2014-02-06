
typedef int callback(int id, int64_t time, uint8_t* lines, int size, int width, int height, int i);
int open(int id, char *fname, callback frameCallback);

