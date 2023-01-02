#include "ImageCropping.h"

#include <iostream>
#include <conio.h>
#include <string>
#include <windows.h>

#include "WindowFileChkImg.h"

const std::string red = "\x1B[91m";
const std::string green = "\x1B[92m";
const std::string def = "\x1B[39m";

inline void printHelp();
inline bool is_number(const std::string& s);
template<bool finish>
inline void responseAndContinue();

int main(int argc, char** argv)
{
	/*enable ANSI sequences for windows 10 (for color text display):*/
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD consoleMode;
	GetConsoleMode(console, &consoleMode);
	consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(console, consoleMode);

	WindowFileChkImg filePath;
	ImageCropping imageCropping;

	int number_of_setting = 0, value;
	float thresholdLimit;

	if (argc == 7)
	{
		std::cout << "<<< ---- \tWhite Border Picture Cropping\t ---- >>> \n\n";
		for (int i = 1; i < 7; i++)
		{
			std::string options(argv[i]);
			if (!options.compare("--input") || !options.compare("-I"))
			{
				filePath.name_path = std::string(argv[++i]);
				int rtv = filePath.fileNameExtract();
				if (rtv == -1)
				{
					std::cerr << red << "Error" << def << "\t: " << filePath.name_path << " is not found\n";
					responseAndContinue<false>();
					return -3;
				}
				else
				{
					if (filePath.checkFmtValidity(filePath.extension))
					{
						std::cerr << red << "Error" << def << "\t: " << filePath.extension << " cannot be processed.\n";
						responseAndContinue<false>();
						return -4;
					}
				}
				++number_of_setting;
			}
			else if (!options.compare("--percentage") || !options.compare("-P"))
			{
				options = std::string(argv[++i]);
				if (is_number(options))
				{
					value = std::stoi(options);
					if (value >= 0 && value <= 100)
					{
						thresholdLimit = value;
						++number_of_setting;
					}
				}
			}
			else if (!options.compare("--force") || !options.compare("-F"))
			{
				options = std::string(argv[++i]);
				if (!options.compare("y") || !options.compare("Y"))
				{
					imageCropping.forceJPG = true;
					++number_of_setting;
				}
				else if (!options.compare("n") || !options.compare("N"))
				{
					imageCropping.forceJPG = false;
					++number_of_setting;
				}
			}
		}

		if (number_of_setting == 3)
		{
			std::cout << "FileName\t\t: " <<
				filePath.name_path << std::endl;
			std::cout << "Image cropped (%)\t: " <<
				value << " %\n";
			if (imageCropping.forceJPG)
				std::cout << "Forcing Jpeg output\t: yes\n";
			else
				std::cout << "Forcing Jpeg output\t: no\n";
			std::cout << "\n---------------------------------------";
			std::cout << "-----------------------------------------\n\n";
		}
		else
		{
			printHelp();
			responseAndContinue<false>();
			return -2;
		}
	}
	else
	{
		printHelp();
		return -1;
	}

	std::string outFile;
	int rtnVal = imageCropping.cropWhiteBorder(filePath, outFile, thresholdLimit);

	switch (rtnVal)
	{
	case -2:
		std::cerr << red << "Error" << def << "\t: Failed to crop picture (ROI = 0x0 pixel).\n";
		responseAndContinue<false>();
		break;
	case -1:
		std::cerr << red << "Error" << def << "\t: Failed to read input file for process.\n";
		responseAndContinue<false>();
		break;
	case 0:
		std::cout << green << "\nOutput" << def << "\t: " << outFile << std::endl;
		responseAndContinue<true>();
		break;
	case 1:
		std::cout << green << "Status" << def << "\t: Picture is not cropped (Same size as original picture).\n";
		responseAndContinue<true>();
		break;
	case 2:
		std::cout << green << "Status" << def << "\t: Picture is not cropped (No ROI found).\n";
		responseAndContinue<true>();
		break;
	default:
		break;
	}

	return rtnVal;
}

inline void printHelp()
{
	std::cout << "<<< ---- \tWhite Border Picture Cropping\t ---- >>> \n\n";
	std::cerr << "Error : Insufficient parameter. \n\n";
	std::cout << "GUIDE: 3 options below are required.\n";
	std::cout << "--input (-I) fileName\n";
	std::cout << "--percentage (-P) The minimum percentage (0-100 in integer) of image to be retained in both x and y directions after binary threshold filtering\n";
	std::cout << "--force (-F) Forcing to compress to jpeg (y/n)\n\n";
}

inline bool is_number(const std::string& s)
{
	return !s.empty() && std::find_if(s.begin(),
		s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}

template<bool finish>
inline void responseAndContinue()
{
	if (finish)
		std::cout << "\n-------------------------------------"
		<< green << "(Fin)" << def 
		<< "-------------------------------------\n";
	else
		std::cout << "\n----------------------------"
		<< red << "(Incomplete Operations)" << def 
		<< "-------------------------------\n";

	std::cout << "Exiting.\n";
	std::cout << "Press any key to continue  .  .  .\n";
	_getch();
}
