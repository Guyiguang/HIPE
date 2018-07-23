//READ LICENSE BEFORE ANY USAGE
/* Copyright (C) 2018  Damien DUBUC ddubuc@aneo.fr (ANEO S.A.S)
 *  Team Contact : hipe@aneo.fr
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  In addition, we kindly ask you to acknowledge ANEO and its authors in any program 
 *  or publication in which you use HIPE. You are not required to do so; it is up to your 
 *  common sense to decide whether you want to comply with this request or not.
 *  
 *  Non-free versions of HIPE are available under terms different from those of the General 
 *  Public License. e.g. they do not require you to accompany any object code using HIPE 
 *  with the corresponding source code. Following the new licensing any change request from 
 *  contributors to ANEO must accept terms of re-license by a general announcement. 
 *  For these alternative terms you must request a license from ANEO S.A.S Company 
 *  Licensing Office. Users and or developers interested in such a license should 
 *  contact us (hipe@aneo.fr) for more information.
 */

#include <algos/IDPlate/RotationTextReader.h>
#include "data/DirPatternData.h"
#include "data/PatternData.h"
#include "algos/IDPlate/IDPlateTools.h"


bool filter::algos::RotationTextReader::isDrawableSource(const data::Data& data)
{
	switch (data.getType())
	{
	case data::IMGF:
	case data::PATTERN:
	case data::DIRPATTERN:
		return true;
	default:
		return false;
	}
}

bool filter::algos::RotationTextReader::isShapeData(const data::Data& data)
{
	switch (data.getType())
	{
	case data::SHAPE:
		return true;
	default:
		return false;
	}
}

data::ImageData filter::algos::RotationTextReader::extractSourceImageData(data::Data& data)
{
	const data::IODataType& type = data.getType();
	if (type == data::IMGF)
	{
		return static_cast<data::ImageData &>(data);
	}
	else if (type == data::PATTERN)
	{
		data::PatternData pattern = static_cast<data::PatternData &>(data);
		return pattern.imageRequest();
	}
	// Now we can extract source image directly from dirpatterndata
	else if (type == data::DIRPATTERN)
	{
		data::DirPatternData& dirPattern = static_cast<data::DirPatternData &>(data);
		return dirPattern.imageSource();
	}
	else
	{
		return data::ImageData();
	}
}

cv::Mat filter::algos::RotationTextReader::getDiscFromCirlce(const cv::Mat& image, cv::Vec<float, 3> circle)
{
   // Draw the mask: white circle on black background
	cv::Mat1b mask(image.size(), uchar(0));
	cv::circle(mask, cv::Point(circle[0], circle[1]), circle[2], cv::Scalar(255), CV_FILLED);

	// Compute the bounding box
	cv::Rect bbox(circle[0] - circle[2], circle[1] - circle[2], 2 * circle[2], 2 * circle[2]);

	// Create a black image
	cv::Mat3b res(image.size(), cv::Vec3b(0, 0, 0));

	// Copy only the image under the white circle to black image
	image.copyTo(res, mask);

	// Crop according to the roi
	res = res(bbox);

	return res;
}

cv::Mat filter::algos::RotationTextReader::transformInBlackAndWhite(const cv::Mat& image)
{
	// Bilateral filtering to smooth images and reduce noise
	const int bDiameter = 31;
	cv::Mat output = filter::algos::IDPlate::applyBilateralFiltering(image, 2, bDiameter, bDiameter * 2, bDiameter * 0.5, _debug, false);
	algos::IDPlate::showImage(output);

	// Convert image to grayscale
	output = filter::algos::IDPlate::convertColor2Gray(output);
	algos::IDPlate::showImage(output);
	// Convert image to binary (black/white)
	// Paint everything but biggest blob in black
	cv::Mat res = cv::Mat::zeros(output.size(), output.type());

	for (int y = 0; y < image.size().height; y++)
	{
		const uchar* row = image.ptr(y);
		 uchar* rowBW = res.ptr(y);
		for (int x = 0; x < image.size().width; x++)
		{
			cv::Point currentPixel(x, y);
			if (row[x] >= 64 && row[x] < 148)
			{
				rowBW[x] = 255;
			}
		}
	}

	//cv::adaptiveThreshold (output, output, 70, CV_ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 3, 0);
	algos::IDPlate::showImage(res);
	return output;
}

std::vector<cv::Mat> filter::algos::RotationTextReader::generateRotationText(const cv::Mat& image, int offset_rotation)
{
	int angle = 0;
	std::vector<cv::Mat> result;

	for (int i = 0; i < 360/offset_rotation; i++)
	{
		cv::Point2f center((image.cols - 1) / 2.0, (image.rows - 1) / 2.0);
		cv::Mat rot = cv::getRotationMatrix2D(center, angle, 1.0);
		cv::Mat dst;
		cv::warpAffine(image, dst, rot, image.size());

		result.push_back(dst);

		angle += offset_rotation;
	}

	return result;
}

std::vector<std::string> filter::algos::RotationTextReader::readText(const std::vector<cv::Mat>& mats)
{
	std::vector<std::string> result;

	return result;
}

HipeStatus filter::algos::RotationTextReader::process()
{
	// Assert data is present in connector
	if (_connexData.empty())
	{
		throw HipeException("Error in OverlayFilter: No data in input.");
	}

	// Separate shapes from source image
	std::vector<data::ShapeData> shapeDataArray;
	data::ImageData image;
	bool isSourceFound = false;

	while (!_connexData.empty())
	{
		data::Data data = _connexData.pop();
		if (isShapeData(data))
		{
			shapeDataArray.push_back(static_cast<data::ShapeData &>(data));
		}
		else if (isDrawableSource(data))
		{
			if (isSourceFound)
				throw HipeException(
					"Error in OverlayFilter: Overlay filter only accepts one input source to draw on. Other input data should only be ShapeData objects.");
			isSourceFound = true;

			image = extractSourceImageData(data);
		}
		else
		{
			std::stringstream errorMessage;
			errorMessage << "Error in OverlayFilter: Input type is not handled: ";
			errorMessage << data::DataTypeMapper::getStringFromType(data.getType());
			errorMessage << std::endl;
			throw HipeException(errorMessage.str());
		}
	}

	if (image.empty() || !image.getMat().data)
		throw HipeException("Error in OverlayFilter: No input image to draw on found.");

	//Ok let test work with only one circle
	cv::Vec3f circle = shapeDataArray[0].CirclesArray()[0];
	
	cv::Mat res = getDiscFromCirlce(image.getMat(), circle);
	

	//Get B/W image to read text
	res = transformInBlackAndWhite(res);
	algos::IDPlate::showImage(res);

	std::vector<cv::Mat> allRotations;

	//  10/360 degrees shift to generate new image
	//Result of 36 images
	allRotations = generateRotationText(res , 10);

	algos::IDPlate::showImage(allRotations[0]);

	//get the list of recognition text by angle
	std::vector<std::string> allText = readText(allRotations);

	cv::Mat outputImage;
	return OK;
}
