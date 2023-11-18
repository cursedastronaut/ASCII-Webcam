#include "ascii.hpp"

using namespace std;

char grayToChar(int gray, string &charList) {
	int numChars = charList.size();

    return charList[std::min(static_cast<int>(std::round(gray * numChars / 255.0)), numChars - 1)];
}

void decodeMJPEGBuffer(char* mjpegBuffer, size_t bufferSize, uint8_t* rgbBuffer, uint32_t width, uint32_t height) {
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    jpeg_mem_src(&cinfo, reinterpret_cast<unsigned char*>(mjpegBuffer), bufferSize);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    JSAMPROW row_pointer[1];
    row_pointer[0] = new uint8_t[cinfo.output_width * cinfo.output_components];

    while (cinfo.output_scanline < cinfo.output_height) {
        jpeg_read_scanlines(&cinfo, row_pointer, 1);
        memcpy(rgbBuffer + (cinfo.output_scanline - 1) * cinfo.output_width * cinfo.output_components,
               row_pointer[0], cinfo.output_width * cinfo.output_components);
    }

    delete[] row_pointer[0];

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
}

int toInt(const char* index) {
	std::string temp = index;
	if (temp.size() == 0)
		return 0;
	int result = 0;	

	for (size_t i = temp.size()-1; i < temp.size(); --i)
	{
		if (temp[i] < '0' || temp[i] > '9') {
			return 0;
		}
		result += (temp[i] - '0') * std::pow(10, temp.size()-1-i);
	}
	return result;
}

void help() {
	cout	<< "ASCII Webcam" << endl
			<< "\t--x [width]" << endl
			<< "\t\tChange output's width (under or equal to 320)" << endl
			<< "\t--y [height]" << endl
			<< "\t\tChange output's height (under or equal to 180)"
			<< "\t--video [device]" << endl
			<< "\t\tUse another camera (/dev/video0)" << endl
			<< "\t--string [string]" << endl
			<< "\t\tUse other characters. PUT IT BETWEEN QUOTES!" << endl
			<< "\t\tExample: \" .',;:clodxkO0KXNWM%@\"" << endl
			<< "\t--reverse" << endl
			<< "\t\tReverses horizontally the stream."
			<< "\t--help" << endl
			<< "\t\tShows this." << endl
			<< "Made by @cursedastronaut on GitHub." << endl;
}