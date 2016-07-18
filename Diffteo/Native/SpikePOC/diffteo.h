#pragma once
#include <opencv2/core/types.hpp>
#include "IPDesc.h"

#define ShowImage(name, img)\
	imshow(name, img);\
	waitKey(0);\

#define ShowImage(img)\
	imshow(#img, img);\
	waitKey(0);\

std::vector<IPDesc> drawShape(cv::Mat img, cv::Mat & mask, std::vector<std::vector<cv::Point> > & contours);

void convertGray(cv::Mat src, cv::Mat & src_gray);

cv::Mat extractBackground(cv::Mat img, cv::Mat & back);

void detectObject(cv::Mat & refImg, std::vector<IPDesc> interestObjects, cv::Mat & img);