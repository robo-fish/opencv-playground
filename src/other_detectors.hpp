//
//  other_detectors.hpp
//  opencv-playground
//
//  Created by Kai Oezer on 6/30/18.
//

#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdocumentation"
#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#pragma GCC diagnostic pop

#include <vector>

using namespace cv;

namespace robofish {

enum class DetectorType : uint8_t
{
  GFTT = 0,
  SIFT = 1,
  SURF = 2,
  FAST = 3,
  AGAST = 4,
  BRISK = 5,
  ORB = 6,
  HarrisLaplace = 7,
  Star = 8,
  KAZE = 9,
  AKAZE = 10
};

const char* detectorName(DetectorType type);

void findKeypoints(uint8_t type, Mat input, std::vector<KeyPoint> & output);

} // namespace
