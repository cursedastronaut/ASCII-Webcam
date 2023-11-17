#include "ascii.hpp"
using namespace std;

int main(int argc, char *argv[]) {
	uint16_t wished_size_x= WISHED_SIZE_X;
	uint16_t wished_size_y= WISHED_SIZE_Y;
	int userVideo = false;
	int userX = false;
	int userY = false;
	for (uint8_t i = 1; i < argc; ++i) {
		if (strcmp(argv[i], "-video") == 0) {
			userVideo = i;
		} else if (strcmp(argv[i], "-x") == 0) {
			userX = i;
		} else if (strcmp(argv[i], "-y") == 0) {
			userY = i;
		}
	}
	

    // 1.  Open the device
    int fd; // A file descriptor to the video device
	{
		string videoDevice = "/dev/video0";
		if (userVideo > 0) {
			ASSERT(userVideo+1 >= argc, "ERROR: Argument -video empty. Cannot proceed.");
			videoDevice = argv[userVideo+1];
		}
    	fd = open(videoDevice.c_str(), O_RDWR);
	}

    if(fd < 0){
        perror("Failed to open device, OPEN");
		cout << "error";
        return 1;
    }


    // 2. Ask the device if it can capture frames
    v4l2_capability capability;
    if(ioctl(fd, VIDIOC_QUERYCAP, &capability) < 0){
        // something went wrong... exit
        perror("Failed to get device capabilities, VIDIOC_QUERYCAP");
        return 1;
    }


    // 3. Set Image format
    v4l2_format imageFormat;
    imageFormat.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    imageFormat.fmt.pix.width = 320;
    imageFormat.fmt.pix.height = 180;
    imageFormat.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
    imageFormat.fmt.pix.field = V4L2_FIELD_NONE;
    // tell the device you are using this format
    if(ioctl(fd, VIDIOC_S_FMT, &imageFormat) < 0){
        perror("Device could not set format, VIDIOC_S_FMT");
        return 1;
    }


    // 4. Request Buffers from the device
    v4l2_requestbuffers requestBuffer = {0};
    requestBuffer.count = 1; // one request buffer
    requestBuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; // request a buffer wich we an use for capturing frames
    requestBuffer.memory = V4L2_MEMORY_MMAP;

    if(ioctl(fd, VIDIOC_REQBUFS, &requestBuffer) < 0){
        perror("Could not request buffer from device, VIDIOC_REQBUFS");
        return 1;
    }


    // 5. Quety the buffer to get raw data ie. ask for the you requested buffer
    // and allocate memory for it
    v4l2_buffer queryBuffer = {0};
    queryBuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    queryBuffer.memory = V4L2_MEMORY_MMAP;
    queryBuffer.index = 0;
    if(ioctl(fd, VIDIOC_QUERYBUF, &queryBuffer) < 0){
        perror("Device did not return the buffer information, VIDIOC_QUERYBUF");
        return 1;
    }
    // use a pointer to point to the newly created buffer
    // mmap() will map the memory address of the device to
    // an address in memory
    char* buffer = (char*)mmap(NULL, queryBuffer.length, PROT_READ | PROT_WRITE, MAP_SHARED,
                        fd, queryBuffer.m.offset);
    memset(buffer, 0, queryBuffer.length);


    // 6. Get a frame
    // Create a new buffer type so the device knows whichbuffer we are talking about
    v4l2_buffer bufferinfo;
    memset(&bufferinfo, 0, sizeof(bufferinfo));
    bufferinfo.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    bufferinfo.memory = V4L2_MEMORY_MMAP;
    bufferinfo.index = 0;

    // Activate streaming
    int type = bufferinfo.type;
    if(ioctl(fd, VIDIOC_STREAMON, &type) < 0){
        perror("Could not start streaming, VIDIOC_STREAMON");
        return 1;
    }


	{
		if (userX > 0) {
			ASSERT(userX+1 >= argc, "ERROR: Argument -x is empty.");
			wished_size_x = toInt(argv[userX+1])-1;
			cout << "bonol" << wished_size_x << endl;
			ASSERT(wished_size_x == -1, "ERROR: Argument -x has an invalid value.");
		}
		if (userY > 0) {
			ASSERT(userY+1 >= argc, "ERROR: Argument -y is empty.");
			wished_size_y = toInt(argv[userY+1])-1;
			ASSERT(wished_size_y == -1, "ERROR: Argument -y has an invalid value.");
		}
	}

	int xJump = imageFormat.fmt.pix.width/wished_size_x;
	int yJump = imageFormat.fmt.pix.height/wished_size_y;

/***************************** Main loop *****************************/
	while (true) {
		// Queue the buffer
		if(ioctl(fd, VIDIOC_QBUF, &bufferinfo) < 0){
			perror("Could not queue buffer, VIDIOC_QBUF");
			return 1;
		}

		// Dequeue the buffer
		if(ioctl(fd, VIDIOC_DQBUF, &bufferinfo) < 0){
			perror("Could not dequeue the buffer, VIDIOC_DQBUF");
			return 1;
		}
		// Frames get written after dequeuing the buffer

		cout << "Buffer has: " << (double)bufferinfo.bytesused / 1024
				<< " KBytes of data and " << bufferinfo.length << "elements" << endl;
		cout << "x:" << imageFormat.fmt.pix.height << " " << V4L2_PIX_FMT_MJPEG <<
		endl;

		// Call decodeMJPEGBuffer to transform MJPEG buffer into RGB buffer
		uint8_t* rgbBuffer = new uint8_t[imageFormat.fmt.pix.width * imageFormat.fmt.pix.height * 3];
		decodeMJPEGBuffer(buffer, queryBuffer.length, rgbBuffer, imageFormat.fmt.pix.width, imageFormat.fmt.pix.height);

		// Loop to display decoded pixels
		for (uint32_t y = 0; y < imageFormat.fmt.pix.height; y+=yJump) {
			for (uint32_t x = 0; x < imageFormat.fmt.pix.width; x+=xJump) {
				// Assuming rgbBuffer is a buffer to store the RGB values after decoding
				uint8_t red = rgbBuffer[(y * imageFormat.fmt.pix.width + x) * 3];
				uint8_t green = rgbBuffer[(y * imageFormat.fmt.pix.width + x) * 3 + 1];
				uint8_t blue = rgbBuffer[(y * imageFormat.fmt.pix.width + x) * 3 + 2];

				char asciiChar = grayToChar(0.3 * red + 0.59 * green + 0.11 * blue);
				cout << asciiChar;
			}
			cout << endl;
		}
	}

/******************************** end looping here **********************/

    // end streaming
    if(ioctl(fd, VIDIOC_STREAMOFF, &type) < 0){
        perror("Could not end streaming, VIDIOC_STREAMOFF");
        return 1;
    }

    close(fd);
    return 0;
}