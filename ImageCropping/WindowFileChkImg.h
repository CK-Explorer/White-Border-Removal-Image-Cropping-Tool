#pragma once
#include "WindowFileChk.h"

class WindowFileChkImg : public WindowFileChk
{
private:
	const std::string imgSupport[19] =
	{
		"jpg", "jpeg", "jpe", 
		"jp2",
		"png",
		"webp",
		"bmp", "dib",
		"pbm", "pgm", "ppm", "pxm",
		"sr", "ras",
		"tiff", "tif",
		"exr",
		"hdr", "pic"
	};

public:
	int checkFmtValidity(std::string& fmt);
};

