#include "WindowFileChk.h"

/*Return value = 0; Success in dividing the filePath to its respective categories.
* 1; File is not found.
*/
int WindowFileChk::fileNameExtract()
{
	DWORD found = GetFileAttributes(utf8_decode(name_path).c_str());
	if (found == INVALID_FILE_ATTRIBUTES)
		return -1;

	size_t posOfPath = name_path.find_last_of("\\");
	if (posOfPath != std::string::npos)
	{
		pathName = name_path.substr(0, posOfPath + 1);
		fileName = name_path.substr(posOfPath + 1);
	}
	else
	{
		pathName = "";
		fileName = name_path;
	}

	size_t posOfExt = fileName.find_last_of(".");
	if (posOfExt != std::wstring::npos)
	{
		extension = fileName.substr(posOfExt + 1);
		fileName = fileName.substr(0, posOfExt);
	}
	else
	{
		extension = "";
		fileName = fileName;
	}

	return 0;
}

std::string WindowFileChk::renameWithDuplicateCheck(const std::string& fullname)
{
	bool state = true;
	std::string pathname;
	std::string temp;
	std::string baseName;
	std::string ext;

	size_t posOfPath = fullname.find_last_of("\\");
	if (posOfPath != std::wstring::npos)
	{
		pathname = fullname.substr(0, posOfPath + 1);
		baseName = fullname.substr(posOfPath + 1);
	}
	else
	{
		pathname = "";
		baseName = fullname;
	}


	size_t posOfExt = baseName.find_last_of(".");
	if (posOfExt != std::wstring::npos)
	{
		ext = baseName.substr(posOfExt);
		baseName = baseName.substr(0, posOfExt);
	}
	else
	{
		ext = "";
		baseName = baseName;
	}

	int i = 0;
	while (state)
	{
		temp = "";
		if (i == 0)
			temp = pathname + baseName + ext;
		else
			temp = pathname + baseName + "(" + std::to_string(i) + ")" + ext;

		DWORD found = GetFileAttributes(utf8_decode(temp).c_str());
		if (found == INVALID_FILE_ATTRIBUTES)
			state = false;
		++i;
	}

	return temp;
}

std::wstring WindowFileChk::utf8_decode(const std::string& str)
{
	if (str.empty()) return std::wstring();
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}

std::string WindowFileChk::utf8_encode(const std::wstring& wstr)
{
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}
