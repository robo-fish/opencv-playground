//
//  line_detector.cpp
//  opencv-playground
//
//  Created by Kai Oezer on 6/14/18.
//  Copyright © 2018 Kai Oezer. All rights reserved.
//

#include "line_detector.hpp"

#define DISABLE_OPENCV_24_COMPATIBILITY

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdocumentation"
#include <opencv2/imgproc.hpp>
#pragma GCC diagnostic pop

#include "preprocessing.hpp"

#include <iostream>

#ifdef __APPLE__
  #define USE_METAL (0)
#elif defined __LINUX__
  #define USE_CUDA (1)
#endif

#if USE_METAL
#include "line_detector_metal.h"
#endif

#if USE_CUDA
#include "line_detector_cuda.hpp"
#endif

using namespace std;

namespace robofish {

LineDetector::Parameters LineDetector::defaultParameters = {4, WindowSize::px5, 240, 0.9f, 0.3f, 150.0f, 2, 10.0f, 2.0f};

struct LineDetector::Impl
{
  LineDetector::Parameters parameters;

  struct CubicCurve
  {
    double A, B, C, D; // the coefficients of the curve
    double eval(double x) { return A * x*x*x + B * x*x + C * x + D; }
  };

  struct Cluster
  {
    CubicCurve curve;
    vector<KeyPoint> keypoints;
  };

  struct ScaleData
  {
    float scaleFactor;
    Mat image;
    vector<KeyPoint> keypoints;
    vector<Cluster> clusters;
  };
  vector<ScaleData> scales;

  void detect(Mat image, vector<KeyPoint>& keypoints, InputArray mask)
  {
    robofish::normalizeIntensity(image);
    createScales(image);
    for (ScaleData & scale : scales)
    {
      detectInitialKeypoints(scale);
      createKeypointClusters(scale);
      for (Cluster & cluster : scale.clusters)
      {
        copy(cluster.keypoints.begin(), cluster.keypoints.end(), keypoints.end());
      }
    }
  }

  void createScales(InputArray image)
  {
    // TBD
  }

  void detectInitialKeypoints(ScaleData & scale)
  {
    int const regionSize = static_cast<int>(parameters.windowSize);

#if USE_METAL
    macos::detectInitialKeypointsUsingMetalAcceleration(scale.image, regionSize, scale.keypoints);
#else
  #if USE_CUDA
    if (isCUDAAvailable())
    {
      detectInitialKeypointsUsingCUDAAcceleration(scale.image, regionSize, scale.keypoints);
      return;
    }
    else
    {
      out << "CUDA not available. Falling back to CPU implementation." << endl;
    }
  #endif
    detectInitialKeypointsUsingCPU(scale.image, regionSize, scale.keypoints);
#endif
  }

  void detectInitialKeypointsUsingCPU(cv::Mat image, int windowSize, std::vector<cv::KeyPoint> & keypoints)
  {
    int const margin = (windowSize - 1)/2;

    for (int i = margin; i < (image.cols - margin); i++)
    {
      for (int j = margin; i < (image.rows - margin); j++)
      {
        Mat subregion = Mat(image, Rect(i - margin, j - margin, windowSize, windowSize));
        if (isCandidateKeypointRegion(subregion))
        {
          KeyPoint keypoint(Point2f(i, j), -1.0f);
          keypoint.angle = determineMainAxis(subregion);
          keypoints.push_back(keypoint);
        }
      }
    }
  }

  /// Implements step #4 of the keypoint detection algorithm described in readme.md
  bool isCandidateKeypointRegion(Mat region)
  {
    bool const is5x5 = (region.cols == 5);
    static Point2i const peripherals_5x5[] = { {2,4}, {1,4}, {0,3}, {0,2}, {0,1}, {1,0}, {2,0}, {3,0}, {4,1}, {4,2}, {4,3}, {3,4} };
    static int values_5x5[] = { 0,0,0,0,  0,0,0,0,  0,0,0,0 };
    static Point2i const peripherals_7x7[] = { {3,6}, {2,6}, {1,5}, {0,4}, {0,3}, {0,2}, {1,1}, {2,0}, {3,0}, {4,0}, {5,1}, {6,2}, {6,3}, {6,4}, {5,5}, {4,6} };
    static int values_7x7[] = { 0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0 };
    Point2i const * const peripheralPoints = is5x5 ? peripherals_5x5 : peripherals_7x7;
    size_t const numPoints = sizeof(peripheralPoints) / sizeof(Point2i);
    //int const minimumZoneLength = is5x5 ? 2 : 3;
    int* values = is5x5 ? values_5x5 : values_7x7;
    Point2i const centerPoint = is5x5 ? Point2i(2,2) : Point2i(3,3);

    int const centerPointBrightness = region.at<int>(centerPoint.x);
    if (centerPointBrightness <= parameters.centerMinBrightness)
    {
      return false;
    }
    int const brightPointThreshold = static_cast<int>(parameters.peripheralBrightPixelRelativeMinBrightness * centerPointBrightness);
    int const darkPointThreshold = static_cast<int>(parameters.peripheralDarkPixelRelativeMaxBrightness * centerPointBrightness);

    for (int i = 0; i < numPoints; ++i)
    {
      Point2i const currentPoint = peripheralPoints[i];
      int const pointBrightness = region.at<int>(currentPoint);
      if (pointBrightness >= brightPointThreshold)
        values[i] = 1;
      else if (pointBrightness <= darkPointThreshold)
        values[i] = -1;
      else
        values[i] = 0;
    }

    int currentZoneLength = 0;
    int numZoneTransitions = 0;
    int previousBrightness = -1;
    for (int i = 0; i < numPoints; ++i)
    {
      int value = values[i];
      if (value > 0)
      {
        if (previousBrightness < brightPointThreshold)
        {
          currentZoneLength = 1;
        }
        else
        {
          currentZoneLength++;
        }
        if ((previousBrightness >= 0) && (previousBrightness <= darkPointThreshold))
        {
          numZoneTransitions++;
        }
      }
      else if (value < 0)
      {
        if (previousBrightness < 0 || previousBrightness > darkPointThreshold)
        {
          currentZoneLength = 1;
        }
        else
        {
          currentZoneLength++;
        }
        if ((previousBrightness >= 0) && (previousBrightness >= brightPointThreshold))
        {
          numZoneTransitions++;
        }
      }
      else
      {
        break;
      }
      previousBrightness = value;
    }

    return false; // TBD
  }

  float determineMainAxis(Mat region)
  {
    // Create histogram of gradients.
    // Check that histogram has two peaks.
    // Check that the value difference for the two peaks is greater than the given minimum angle.
    // Calculate the main axis. It is orthogonal to the average angle determined from the peaks in the histogram.
    return 0.0f;
  }

  void createKeypointClusters(ScaleData scale)
  {
    for (auto cluster : scale.clusters)
    {
      fitCubicCurve(cluster);
      removeOutliers(cluster);
    }
  }

  void fitCubicCurve(Cluster & cluster)
  {
    // TBD
  }

  void removeOutliers(Cluster cluster)
  {
    vector<KeyPoint> outliers;
    for (auto keypoint : cluster.keypoints)
    {
      // TBD
    }
  }

};

LineDetector::LineDetector() : _impl(std::make_unique<Impl>()) {}

LineDetector::~LineDetector() = default;

cv::Ptr<LineDetector> LineDetector::create(Parameters parameters)
{
  cv::Ptr<LineDetector> ptr(new LineDetector);
  ptr->_impl->parameters = parameters;
  return ptr;
}

void LineDetector::detect(InputArray image, CV_OUT vector<KeyPoint>& keypoints, InputArray mask)
{
  assert(image.type() == CV_8UC1);
  _impl->detect(image.getMat(), keypoints, mask);
}


} // namespace seavision
