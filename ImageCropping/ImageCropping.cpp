#include "ImageCropping.h"

int ImageCropping::cropWhiteBorder(WindowFileChk& filePath, std::string& outputName,
	float& thresholdLimit)
{
	cv::Mat unedited = cv::imread(filePath.name_path);

	if (unedited.empty())
		return -1;

	cv::Mat process1;
	cv::cvtColor(unedited, process1, cv::COLOR_BGR2GRAY);

	cv::Mat process2;
	cv::threshold(process1, process2, 251, 255, cv::THRESH_BINARY_INV);

	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(process2, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	if (!contours.empty())
	{
		cv::Point pointTL;
		cv::Point pointBR;

		bool firstTime = true;
		for (int i = 0; i < contours.size(); i++)
		{
			for (int j = 0; j < contours[i].size(); j++)
			{
				if (firstTime)
				{
					pointTL.x = contours[i][j].x;
					pointBR.x = contours[i][j].x;
					pointTL.y = contours[i][j].y;
					pointBR.y = contours[i][j].y;
					firstTime = false;
					continue;
				}

				if (contours[i][j].x < pointTL.x)
					pointTL.x = contours[i][j].x;
				else if (contours[i][j].x > pointBR.x)
					pointBR.x = contours[i][j].x;

				if (contours[i][j].y < pointTL.y)
					pointTL.y = contours[i][j].y;
				else if (contours[i][j].y > pointBR.y)
					pointBR.y = contours[i][j].y;
			}
		}

		cv::Rect rectBeforeCluster(pointTL, pointBR + cv::Point(1, 1));

		cv::Mat row_sum;
		cv::Mat smallerROI = process2(rectBeforeCluster);
		cv::Mat smallerROIINV;
		cv::threshold(smallerROI, smallerROIINV, 125, 1, cv::THRESH_BINARY);
		cv::reduce(smallerROIINV, row_sum, 1, cv::REDUCE_SUM, CV_32S);

#if _DEBUG
		std::ofstream file("OpenCV_row.txt");
		if (file.is_open())
		{
			for (int i = 0; i < pointBR.y - pointTL.y + 1; i++)
				file << i << " , " << row_sum.at<int>(i, 0) << std::endl;
			file.close();
		}
		std::cout << "Further vertical cropping\n";
#endif

		int pointMin, pointMax;
		int rtv = autoIncreaseClustering(row_sum, pointMin, pointMax, thresholdLimit,
			5, 20, 0.01, 5);

		if (rtv == 0)
		{
			pointBR.y = pointTL.y + pointMax;
			pointTL.y += pointMin;
		}
		else
			printErrorClustering(rtv, true);

		cv::Mat col_sum;
		cv::Point pointTL_Inter(0, pointMin);
		cv::Point pointBR_Inter(smallerROIINV.cols - 1, pointMax);
		cv::Rect rect_Inter(pointTL_Inter, pointBR_Inter + cv::Point(1, 1));
		cv::Mat smallerROIINVupdate = smallerROIINV(rect_Inter);
		cv::reduce(smallerROIINVupdate, col_sum, 0, cv::REDUCE_SUM, CV_32S);

#if _DEBUG
		file.open("OpenCV_col.txt");
		if (file.is_open())
		{
			for (int i = 0; i < pointBR.x - pointTL.x + 1; i++)
				file << i << " , " << col_sum.at<int>(0, i) << std::endl;
			file.close();
		}
		std::cout << "---------------------------------------\n\n";
		std::cout << "Further horizontal cropping\n";
#endif

		pointMin = 0;
		pointMax = 0;
		rtv = autoIncreaseClustering(col_sum, pointMin, pointMax, thresholdLimit,
			5, 20, 0.01, 5);

		if (rtv == 0)
		{
			pointBR.x = pointTL.x + pointMax;
			pointTL.x += pointMin;
		}
		else
			printErrorClustering(rtv, false);

		cv::Rect rectAfterCluster(pointTL, pointBR + cv::Point(1, 1));

#if _DEBUG
		/*For testing results*/
		cv::Mat row_test_ori, col_test_ori;
		cv::Mat ori;
		cv::threshold(process2, ori, 125, 1, cv::THRESH_BINARY);
		cv::reduce(ori, row_test_ori, 1, cv::REDUCE_SUM, CV_32S);
		cv::reduce(ori, col_test_ori, 0, cv::REDUCE_SUM, CV_32S);

		file.open("OpenCV_row_original.txt");
		if (file.is_open())
		{
			for (int i = 0; i < row_test_ori.rows; i++)
				file << i << " , " << row_test_ori.at<int>(i) << std::endl;
			file.close();
		}

		file.open("OpenCV_col_original.txt");
		if (file.is_open())
		{
			for (int i = 0; i < col_test_ori.cols; i++)
				file << i << " , " << col_test_ori.at<int>(i) << std::endl;
			file.close();
		}

		cv::Mat row_test_after, col_test_after;
		cv::Mat after = ori(rectAfterCluster);
		cv::reduce(after, row_test_after, 1, cv::REDUCE_SUM, CV_32S);
		cv::reduce(after, col_test_after, 0, cv::REDUCE_SUM, CV_32S);

		file.open("OpenCV_row_after.txt");
		if (file.is_open())
		{
			for (int i = 0; i < row_test_after.rows; i++)
				file << i << " , " << row_test_after.at<int>(i) << std::endl;
			file.close();
		}

		file.open("OpenCV_col_after.txt");
		if (file.is_open())
		{
			for (int i = 0; i < col_test_after.cols; i++)
				file << i << " , " << col_test_after.at<int>(i) << std::endl;
			file.close();
		}
#endif

		if (rectAfterCluster.width == 0 || rectAfterCluster.height == 0)
			return -2;
		else if (rectAfterCluster.width != unedited.cols || rectAfterCluster.height != unedited.rows)
		{
			std::cout << green << "Status" << def
				<< "\t: Cropping picture inside the rectangle of \n\t  (top left = "
				<< pointTL.x << " , " << pointTL.y << " ; bottom right = "
				<< pointBR.x << " , " << pointBR.y << ")\n\t  Image size = "
				<< pointBR.x - pointTL.x + 1 << " x " << pointBR.y - pointTL.y + 1 << " pixels\n";

			cv::Mat croppedImage = unedited(rectAfterCluster);
			std::vector<int> compression_params;

			if (!filePath.extension.compare("jpg") || !filePath.extension.compare("jpeg")
				|| forceJPG)
			{
				outputName = filePath.renameWithDuplicateCheck(filePath.pathName + filePath.fileName + "_c.jpg");
				compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
				compression_params.push_back(97);
				cv::imwrite(outputName, croppedImage, compression_params);
			}
			else if (!filePath.extension.compare("png"))
			{
				outputName = filePath.renameWithDuplicateCheck(filePath.pathName + filePath.fileName + "_c.png");
				compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
				compression_params.push_back(9);
				cv::imwrite(outputName, croppedImage, compression_params);
			}
		}
		else
			return 1;	
	}
	else
		return 2;

	return 0;
}

/*Return value = 0; Success with valid pointMin and pointMax outputs
* 1; All the pixels have the same value
* 
* -1; clustering number is less than 2, needs to have minimum of 2 clusters
* -2; wrong vect's dimension, needs either 1 row or 1 col
* -3; although the vect is 2 dimensions, one of the dimension < 1, so no data to be processed in vect
* -4; this vect only has 1 element, so no further clustering is required
* -5; one of the vect's dimension is unknown as less than -1
* 
* Below two return values are from clustering() function.
* -6; wrong parameters input (numberCluster > 2, maximumRange > 0, maxIteration > 0, noPass > 0)
* -7; failed to determine point max and min, so no cropping is recommended
*/
int ImageCropping::autoIncreaseClustering(cv::Mat& vect, int& pointMin, int& pointMax,
	float imagePercentage, unsigned int maxNoCluster,
	unsigned int maxIteration, float threshold, int noPass)
{
	if (maxNoCluster < 2)
		return -1;

	unsigned int maximumRange;
	if (vect.cols == 1)
		maximumRange = vect.rows;
	else if (vect.rows == 1)
		maximumRange = vect.cols;
	else
		return -2;

	if (maximumRange == 0)
		return -3;
	else if (maximumRange == 1)
		return -4;
	else if (maximumRange < 0)
		return -5;

	int maxValue = vect.at<int>(0);
	int minValue = vect.at<int>(0);
	for (unsigned int i = 1; i < maximumRange; i++)
	{
		if (vect.at<int>(i) > maxValue)
			maxValue = vect.at<int>(i);
		
		if (vect.at<int>(i) < maxValue)
			minValue = vect.at<int>(i);
	}

	if (maxValue - minValue == 0)
		return 1;
	
	int rtv = 0;
	float percentageDiff = 0;

	/*Clustering algo here*/
	for (int i = 2; i <= maxNoCluster; i++)
	{
#if _DEBUG 
		std::cout << "---------------------------------------\n";
		std::cout << "Cluster Number = " << i << "\n\n";
#endif
		rtv = clustering(vect, pointMin, pointMax,
			maxValue, i, maximumRange,
			maxIteration, threshold, noPass);
		if (rtv == 0)
		{
			percentageDiff = (float)(pointMax - pointMin + 1) / (float)maximumRange * 100.00;
#if _DEBUG 
			std::cout << "Percentage of cropped image for cluster number " << i 
				<< " = " << percentageDiff << " %\n";
#endif
			if (percentageDiff > imagePercentage)
				break;
		}
		else
			break;
	}

	if (rtv == -1)
		return -6;
	else if (rtv == -2)
		return -7;

	if (percentageDiff < imagePercentage)
		return -8;

	return 0;
}

/*direction = true (y, vertical direction); false (x, horizontal direction)
*/
void ImageCropping::printErrorClustering(int rtv, bool direction)
{
	std::cerr << yellow << "Warning" << def << "\t: ";
	if (direction)
		std::cerr << "Failed to cluster image in vertical or y direction.\n";
	else
		std::cerr << "Failed to cluster image in horizontal or x direction.\n";

	std::cerr << "Details\t: ";
	switch (rtv)
	{
	case 1:
		std::cerr << "All the pixels have the same value.\n";
		break;
	case -1:
		std::cerr << "Clustering number is less than 2, needs to have minimum of 2 clusters.\n";
		break;
	case -2:
		std::cerr << "wrong vectors's dimension, needs either 1 row or 1 col.\n";
		break;
	case -3:
		std::cerr << "One of the vector's dimension less than 1.\n";
		break;
	case -4:
		std::cerr << "This vector only has 1 element.\n";
		break;
	case -5:
		std::cerr << "One of the vector's dimension is unknown as less than -1.\n";
		break;
	case -6:
		std::cerr << "Wrong parameters input for clustering() function.\n";
		break;
	case -7:
		std::cerr << "Failed to determine point max and min.\n";
		break;
	default:
		std::cerr << "Unknown message.\n";
		break;
	}

	std::cout << green << "Status" << def << ":\t No further cropping will be done in ";

	if (direction)
		std::cout << "vertical or y direction.\n";
	else
		std::cout << "horizontal or x direction.\n";
}

/*Return = 0; success clustering and determine point min and max (min < max)(always true)
* -1 ; wrong parameters input (numberCluster > 2, maximumRange > 0, maxIteration > 0, noPass > 0)
* -2 ; failed to determine point max and min (min <= max), so no resizing is recommended.
*/
int ImageCropping::clustering(cv::Mat& vect, int& pointMin, int& pointMax,
	int maxValue,
	unsigned int numberCluster, unsigned int maximumRange, 
	unsigned int maxIteration, float threshold, unsigned int noPass)
{
	pointMin = -1;
	pointMax = -1;

	if (numberCluster < 2 || maximumRange == 0 ||
		maxIteration == 0 || noPass == 0)
		return -1;

	float* clusterMean = new float[numberCluster];
	float* clusterVar = new float[numberCluster];
	float* distance = new float[numberCluster];
	int* clusterIndex = new int[maximumRange];
	int* noElementCluster = new int[numberCluster];
	float* temp = new float[numberCluster];

	float* clusterMeanPrevious = new float[numberCluster];
	float* clusterVarPrevious = new float[numberCluster];

	std::deque<std::vector<bool>> lessThresholdDeque;

	/*Initialize mean*/
	maxValue /= numberCluster;
	for (unsigned int i = 0; i < numberCluster; i++)
	{
		clusterMean[i] = 0.50 * (i + 1) * maxValue;
		clusterVar[i] = 0;
	}

	for (unsigned int iteration = 0; iteration < maxIteration; iteration++)
	{
		for (unsigned int i = 0; i < numberCluster; i++)
		{
			clusterMeanPrevious[i] = clusterMean[i];
			clusterVarPrevious[i] = clusterVar[i];
		}

		for (unsigned int i = 0; i < numberCluster; i++)
			noElementCluster[i] = 0;

		/*Calculate distance and categorize based on shortest distance*/
		for (unsigned int i = 0; i < maximumRange; i++)
		{
			for (unsigned int j = 0; j < numberCluster; j++)
				distance[j] = abs((float)vect.at<int>(i) - clusterMean[j]);

			clusterIndex[i] = 0;
			float minimumDistance = distance[0];
			for (unsigned int j = 1; j < numberCluster; j++)
			{
				if (distance[j] < minimumDistance)
				{
					minimumDistance = distance[j];
					clusterIndex[i] = j;
				}
			}

			++noElementCluster[clusterIndex[i]];
		}

		/*Compute mean*/
		for (unsigned int i = 0; i < numberCluster; i++)
		{
			clusterMean[i] = 0;
			clusterVar[i] = 0;
			temp[i] = 0;
			temp[i] = 0;
		}

		for (unsigned int i = 0; i < maximumRange; i++)
		{
			if (FLT_MAX - temp[clusterIndex[i]] > vect.at<int>(i))
				temp[clusterIndex[i]] += vect.at<int>(i);
			else
			{
				clusterMean[clusterIndex[i]] += 1.000 * temp[clusterIndex[i]] / noElementCluster[clusterIndex[i]];
				temp[clusterIndex[i]] = vect.at<int>(i);
			}
		}

		for (unsigned int i = 0; i < numberCluster; i++)
		{
			if (noElementCluster[i] != 0)
			{
				clusterMean[i] += 1.000 * temp[i] / noElementCluster[i];
				temp[i] = 0;
			}
			else
				clusterMean[i] = clusterMeanPrevious[i];
		}

		/*Compute Variance*/
		for (unsigned int i = 0; i < maximumRange; i++)
		{
			float squared = vect.at<int>(i) - clusterMean[clusterIndex[i]];
			squared *= squared;
			if (FLT_MAX - temp[clusterIndex[i]] > squared)
				temp[clusterIndex[i]] += squared;
			else
			{
				clusterVar[clusterIndex[i]] += 1.000 * temp[clusterIndex[i]] / noElementCluster[clusterIndex[i]];
				temp[clusterIndex[i]] = squared;
			}
		}

		for (unsigned int i = 0; i < numberCluster; i++)
		{
			if (noElementCluster[i] != 0)
			{
				clusterVar[i] += 1.000 * temp[i] / noElementCluster[i];
				temp[i] = 0;
			}
			else
				clusterVar[i] = clusterVarPrevious[i];
		}

#if _DEBUG
		std::cout << "Iteration no : " << iteration << "\n";
		for (int i = 0; i < numberCluster; i++)
		{
			std::cout << "Cluster " << i << " : Mean = " << clusterMean[i]
				<< " , Var = " << clusterVar[i] << " , element = "
				<< noElementCluster[i] << std::endl;
		}
		std::cout << "\n";
#endif

		if (iteration == 0)
			continue;

		std::vector<bool> tempArray;
		for (unsigned int i = 0; i < numberCluster; i++)
		{
			if (abs(clusterMean[i] - clusterMeanPrevious[i]) < threshold
				&& abs(clusterVar[i] - clusterVarPrevious[i]) < threshold)
				tempArray.push_back(true);
			else
				tempArray.push_back(false);
		}

		if (lessThresholdDeque.size() == noPass)
			lessThresholdDeque.pop_front();

		lessThresholdDeque.push_back(tempArray);

#if _DEBUG
		for (size_t i = 0; i < lessThresholdDeque.size(); i++)
		{
			std::cout << "Deque's position : " << i << " ;\t";
			for (int j = 0; j < numberCluster; j++)
			{
				if ((lessThresholdDeque[i])[j])
					std::cout << "TRUE\t";
				else
					std::cout << "FALSE\t";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
#endif

		if (iteration < noPass)
			continue;

		int countSuccess = 0;
		bool stopChecking = false;
		for (size_t i = 0; i < lessThresholdDeque.size(); i++)
		{
			for (unsigned int j = 0; j < numberCluster; j++)
			{
				if ((lessThresholdDeque[i])[j])
					++countSuccess;
				else
				{
					stopChecking = true;
					break;
				}
			}
			if (stopChecking)
				break;
		}

		if (countSuccess == numberCluster * noPass)
			break;
	}

	/*Searching for minimum and maximum point*/
	int minMean = clusterMean[0];
	int minIndex = 0;
	pointMin = 0;
	pointMax = maximumRange - 1;
	for (unsigned int i = 1; i < numberCluster; i++)
	{
		if (clusterMean[i] < minMean)
		{
			minMean = clusterMean[i];
			minIndex = i;
		}
	}

	for (unsigned int i = 0; i < maximumRange; i++)
	{
		if (clusterIndex[i] != minIndex)
		{
			pointMin = i;
			break;
		}
	}

	for (unsigned int i = 0; i < maximumRange; i++)
	{
		if (clusterIndex[maximumRange - i - 1] != minIndex)
		{
			pointMax = maximumRange - i - 1;
			break;
		}
	}

#if _DEBUG
	std::cout << "Min mean = " << minMean
		<< " , Index = " << minIndex << std::endl;
	std::cout << "Point min = " << pointMin
		<< " , Point max = " << pointMax << std::endl;
#endif

	delete[] clusterMean;
	delete[] clusterVar;
	delete[] distance;
	delete[] clusterIndex;
	delete[] noElementCluster;
	delete[] temp;
	delete[] clusterMeanPrevious;
	delete[] clusterVarPrevious;

	/*retain same size*/
	if (pointMin >= pointMax)
	{
		pointMin = -1;
		pointMax = -1;
		return -2;
	}

	return 0;
}

