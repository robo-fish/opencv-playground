// Robofish OpenCV Playground

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdocumentation"
#include <opencv2/highgui.hpp>
#pragma GCC diagnostic pop

#include <iostream>

#include "main_options.hpp"
#include "line_detector.hpp"
#include "other_detectors.hpp"
#include "preprocessing.hpp"
#include "utils.hpp"

using namespace std;
using namespace robofish;

void detectLine(cv::Mat image, vector<cv::KeyPoint> & result)
{
  image = convertRAWToRGB(image);
  image = extractChannel(image, robofish::Channel::blue);
  normalizeIntensity(image);

  seavision::LineDetector detector;
  detector.detect(image,result);
}

static cv::Mat loadImageFromPath(boost::filesystem::path path)
{
  cv::Mat image;
  if (!path.empty())
  {
    image = cv::imread(path.string(), cv::IMREAD_UNCHANGED);
    if(!image.empty())
    {
      debugOut() << "loaded " << image.size() << " type " << image.type() << endl;
    }
  }
  return image;
}

void runFeatureDetection(uint8_t detectionType, boost::filesystem::path imagePath)
{
  cv::Mat image = loadImageFromPath(imagePath);
  if ( image.empty() )
  {
    debugOut() << "Failed to load image from file at " << imagePath << endl;
    return;
  }
  vector<cv::KeyPoint> result;
  robofish::findKeypoints(detectionType, image, result);

  cv::Scalar const red(0,0,255); //BGR
  cv::drawKeypoints(image, result, image, red, cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

  cv::imshow("image",image);
  cv::waitKey(-10);
}

int main(int argc, char**argv)
{
  auto input = processInput(argc, argv);

  switch (get<0>(input))
  {
    case Task::FeatureDetection:
      runFeatureDetection(get<1>(input), get<2>(input));
      break;
    default:
      break;
  }
  cv::Mat image = loadImageFromPath(get<2>(input));



  return 0;
}
