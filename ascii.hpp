#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/v4l2-common.h>
#include <linux/v4l2-controls.h>
#include <linux/videodev2.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <jpeglib.h>
#include <string.h>
#include <fstream>
#include <string>
#include <cmath>
#include <cassert>
#define ASSERT(a,b) if (a) {std::cout << (b) << std::endl; throw std::runtime_error(b);}


#define DEF_X 320
#define DEF_Y 180
#define WISHED_SIZE_X 64
#define WISHED_SIZE_Y 45
#define DEFAULT_STRING " .:-=+*#%@"

char grayToChar(int gray);
void decodeMJPEGBuffer(char* mjpegBuffer, size_t bufferSize, uint8_t* rgbBuffer, uint32_t width, uint32_t height);
int toInt(const char* index);