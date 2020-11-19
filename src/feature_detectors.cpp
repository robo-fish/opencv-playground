//
//  feature_detectors.cpp
//  opencv-playground
//
//  Created by Kai Oezer on 6/30/18.
//

#include "feature_detectors.hpp"

#define DISABLE_OPENCV_24_COMPATIBILITY

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdocumentation"
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#pragma GCC diagnostic pop

namespace robofish {


static void findGFTTKeypoints(Mat input, std::vector<KeyPoint> & output)
{
  assert(input.type() == CV_8UC1);
  int const maxNumResults = 200;
  double const quality = 0.05;
  double const minDistance = 10;
  int const blockSize = 7;
  bool const useHarrisDetector = false;
  std::vector<cv::Point2f> points;
  goodFeaturesToTrack(input, points, maxNumResults, quality, minDistance, cv::noArray(), blockSize, useHarrisDetector, 0.0);
  output.reserve(points.size());
  for (auto p : points)
  {
    output.push_back(KeyPoint(p, -1));
  }
}

static void findSIFTKeypoints(Mat input, std::vector<KeyPoint> & keyPoints)
{
  int nFeatures = 0;
  int nOctaveLayers = 3;
  double contrastThreshold = 0.10;
  double edgeThreshold = 12;
  double sigma = 1.6;
  auto detector = cv::SiftFeatureDetector::create(nFeatures, nOctaveLayers, contrastThreshold, edgeThreshold, sigma);
  detector->detect(input, keyPoints);
}

static void findSURFKeypoints(Mat input, std::vector<KeyPoint> & keyPoints)
{
  double hessianThreshold = 100;
  int nOctaves = 4;
  int nOctaveLayers = 3;
  bool extended = false;
  bool upright = false;
  auto detector = xfeatures2d::SurfFeatureDetector::create(hessianThreshold, nOctaves, nOctaveLayers, extended, upright);
  detector->detect(input, keyPoints);
}

static void findFASTKeypoints(Mat input, std::vector<KeyPoint> & keyPoints)
{
  int const threshold = 100;
  bool nonmaxSuppression = true;
  auto const type = FastFeatureDetector::TYPE_9_16;
  auto detector = FastFeatureDetector::create(threshold, nonmaxSuppression, type);
  detector->detect(input, keyPoints);
}

static void findAGASTKeypoints(Mat input, std::vector<KeyPoint> & keyPoints)
{
  int threshold = 100;
  bool nonmaxSuppression = true;
  auto const type = AgastFeatureDetector::OAST_9_16;
  auto detector = AgastFeatureDetector::create(threshold, nonmaxSuppression, type);
  detector->detect(input, keyPoints);
}

static void findBRISKKeypoints(Mat input, std::vector<KeyPoint> & keyPoints)
{
  int threshold = 140;
  int octaves = 4;
  float patternScale = 1.0f;
  auto detector = cv::BRISK::create(threshold, octaves, patternScale);
  detector->detect(input, keyPoints);
}

static void findORBKeypoints(Mat input, std::vector<KeyPoint> & keyPoints)
{
  int nFeatures = 200;
  float scaleFactor = 1.2f;
  int nLevels = 3;
  int edgeThreshold = 45;
  int firstLevel = 0;
  int WTA_K = 2;
  auto const scoreType = ORB::HARRIS_SCORE;
  int patchSize = 31;
  int fastThreshold = 100;
  auto detector = ORB::create(nFeatures, scaleFactor, nLevels, edgeThreshold, firstLevel, WTA_K, scoreType, patchSize, fastThreshold);
  detector->detect(input, keyPoints);
}

static void findHarrisLaplaceKeypoints(Mat input, std::vector<KeyPoint> & keyPoints)
{
  int numOctaves = 6;
  float threshold_corner = 0.03f;
  float threshold_DOG = 0.025f;
  int maxNumCorners = 300;
  int numLayers = 4;
  auto detector = xfeatures2d::HarrisLaplaceFeatureDetector::create(numOctaves, threshold_corner, threshold_DOG, maxNumCorners, numLayers);
  detector->detect(input, keyPoints);
}

static void findStarKeypoints(Mat input, std::vector<KeyPoint> & keyPoints)
{
  int maxSize = 45; // one of 4, 6, 8, 11, 12, 16, 22, 23, 32, 45, 46, 64, 90, 128
  int responseThreshold = 30; // threshold to convolution with CenSurE (very cheaply approximated DoG) kernel
  int lineThresholdProjected = 10;
  int lineThresholdBinarized = 8;
  int suppressNonmaxSize = 5;
  auto detector = xfeatures2d::StarDetector::create(maxSize, responseThreshold, lineThresholdProjected, lineThresholdBinarized, suppressNonmaxSize);
  detector->detect(input, keyPoints);
}

static void findKAZEKeypoints(Mat input, std::vector<KeyPoint> & keyPoints)
{
  auto detector = KAZE::create();
  detector->setExtended(true);
  detector->setUpright(false);
  detector->setThreshold(0.002);
  detector->setDiffusivity(KAZE::DIFF_PM_G2);
  detector->detect(input, keyPoints);
}

static void findAKAZEKeypoints(Mat input, std::vector<KeyPoint> & keyPoints)
{
  auto detector = AKAZE::create();
  detector->setThreshold(0.002);
  detector->setDiffusivity(KAZE::DIFF_PM_G2);
  detector->detect(input, keyPoints);
}

const char* detectorName(DetectorType type)
{
  switch (type)
  {
    case DetectorType::GFTT:  return "Corners (Shi-Thomasi)";
    case DetectorType::SIFT:  return "SIFT";
    case DetectorType::SURF:  return "SURF";
    case DetectorType::FAST:  return "FAST";
    case DetectorType::AGAST: return "AGAST";
    case DetectorType::BRISK: return "BRISK";
    case DetectorType::ORB:   return "ORB";
    case DetectorType::HarrisLaplace: return "Harris-Laplace";
    case DetectorType::Star:  return "Star / CenSurE";
    case DetectorType::KAZE:  return "KAZE";
    case DetectorType::AKAZE: return "Accelerated KAZE";
  }
}

void findKeypoints(uint8_t type, Mat input, std::vector<KeyPoint> & output)
{
  switch (type)
  {
    case static_cast<uint8_t>(DetectorType::GFTT) :
      findGFTTKeypoints(input, output);
      break;
    case static_cast<uint8_t>(DetectorType::SIFT) :
      findSIFTKeypoints(input, output);
      break;
    case static_cast<uint8_t>(DetectorType::SURF) :
      findSURFKeypoints(input, output);
      break;
    case static_cast<uint8_t>(DetectorType::FAST) :
      findFASTKeypoints(input, output);
      break;
    case static_cast<uint8_t>(DetectorType::AGAST) :
      findAGASTKeypoints(input, output);
      break;
    case static_cast<uint8_t>(DetectorType::BRISK) :
      findBRISKKeypoints(input, output);
      break;
    case static_cast<uint8_t>(DetectorType::ORB) :
      findORBKeypoints(input, output);
      break;
    case static_cast<uint8_t>(DetectorType::HarrisLaplace) :
      findHarrisLaplaceKeypoints(input, output);
      break;
    case static_cast<uint8_t>(DetectorType::Star) :
      findStarKeypoints(input, output);
      break;
    case static_cast<uint8_t>(DetectorType::KAZE) :
      findKAZEKeypoints(input, output);
      break;
    case static_cast<uint8_t>(DetectorType::AKAZE) :
      findAKAZEKeypoints(input, output);
      break;
    default:
      break;
  }
}

} // namespace robofish
