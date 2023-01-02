#pragma once
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <vector>
#include <iostream>
#include <deque>
#include <vector>

#include "WindowFileChk.h"

#if _DEBUG
#include <fstream>
#endif

class ImageCropping
{
public:
	bool forceJPG = false;

public:
	int cropWhiteBorder(WindowFileChk& filePath, std::string& outputName,
		float& thresholdLimit);

private:
	const std::string red = "\x1B[91m";
	const std::string green = "\x1B[92m";
	const std::string yellow = "\x1B[93m";
	const std::string def = "\x1B[39m";

private:
	int autoIncreaseClustering(cv::Mat& vect, int& pointMin, int& pointMax,
		float imagePercentage, unsigned int maxNoCluster,
		unsigned int maxIteration, float threshold, int noPass);
	void printErrorClustering(int rtv, bool direction);
	int clustering(cv::Mat& vect, int& pointMin, int& pointMax,
		int maxValue,
		unsigned int numberCluster, unsigned int maximumRange,
		unsigned int maxIteration, float threshold, unsigned int noPass);
};

