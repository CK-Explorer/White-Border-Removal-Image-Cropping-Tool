#include "WindowFileChkImg.h"

int WindowFileChkImg::checkFmtValidity(std::string& fmt)
{
	size_t sz = sizeof(imgSupport) / sizeof(imgSupport[0]);
	for (int i = 0; i < sz; i++)
	{
		if (!fmt.compare(imgSupport[i]))
			return 0;
	}
	return -1;
}


