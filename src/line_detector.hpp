#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdocumentation"
#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#pragma GCC diagnostic pop

#include <memory>
#include <functional>

namespace robofish
{
  class CV_EXPORTS_W LineDetector : public cv::FeatureDetector
  {
    public:

      enum class WindowSize : int
      {
        px5 = 5, // 5 by 5 pixel window
        px7 = 7  // 7 by 7 pixel window
      };

      struct Parameters
      {
        /// the number of scale levels where the image is scaled down by a factor of 2 at each level
        int numberOfScales;
        /// the side length of the quadratic window in which we test whether the center point is a keypoint candidate
        LineDetector::WindowSize windowSize;
        /// The minimum brightness of the center point to qualify as a keypoint candidate
        int centerMinBrightness;
        /// the minimum brightness of a peripheral point, relative to the window center point, for the peripherial point to be considered bright
        float peripheralBrightPixelRelativeMinBrightness;
        /// the maximum brightness of a peripheral point, relative to the window center point, for the peripherial point to be considered dark
        float peripheralDarkPixelRelativeMaxBrightness;
        /// the minimum angle by which the two peaks in the histogram of gradient angles must be separated
        float histogramPeaksMinSeparationAngle;
        /// the number of clusters into which the keypoints will be grouped for curve fitting
        int numberOfClusters;
        /// the penalty term applied to the cubic term of the curve that will be fitted to the keypoints
        float curveCubicTermMultiplier;
        /// the penalty term applied to the quadratic term of the curve that will be fitted to the keypoints
        float curveQuadraticTermMultiplier;
      };

      static Parameters defaultParameters;

      CV_WRAP LineDetector();
      virtual ~LineDetector();

      /**
      * \brief Detects laser line keypoints according to the algorithm described in readme.md
      */
      static cv::Ptr<LineDetector> create(Parameters parameters = LineDetector::defaultParameters);

      /**
      * \brief Detect vertical laser lines in a given image
      *
      * \param[in] image Input image
      * \param[in] keypoints The detected laser line as keypoints
      * \param[in] mask Optional mask for masking valid image regions (must have the same size like image and type CV_8UC1, 0 = invalid, otherwise valid)
      *
      */
      CV_WRAP void detect(cv::InputArray image, CV_OUT std::vector<cv::KeyPoint>& keypoints, cv::InputArray mask = cv::noArray());

    private:
      struct Impl;
      std::unique_ptr<Impl> _impl;
  };
}
