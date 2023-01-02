#pragma once
#include <string>
#include <iostream>
#include <windows.h>

class WindowFileChk
{
public:
	std::string name_path;
	std::string pathName;
	std::string fileName;
	std::string extension;

public:
	int fileNameExtract();
	std::string renameWithDuplicateCheck(const std::string& fullname);

protected:
	std::wstring utf8_decode(const std::string& str);
	std::string utf8_encode(const std::wstring& wstr);
};

