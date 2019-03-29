#include "preprocessing.hpp"

#define DISABLE_OPENCV_24_COMPATIBILITY

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdocumentation"
#include <opencv2/imgproc.hpp>
#pragma GCC diagnostic pop

#include <algorithm>

namespace robofish {

Mat convertRAWToRGB(Mat input)
{
  Mat result;
  assert(!input.empty());
  cvtColor(input, result, COLOR_BayerGB2BGR);
  return result;
}

Mat extractChannel(Mat input, Channel channel)
{
  int const inputType = input.type();
  if (inputType == CV_8UC1)
  {
    return input;
  }
  else if (channel == Channel::gray)
  {
    if ((inputType == CV_8UC3) || (inputType == CV_8UC4))
    {
      Mat grayscaleImage;
      int const conversionCode = (inputType == CV_8UC3) ? COLOR_RGB2GRAY : COLOR_RGBA2GRAY;
      cvtColor(input, grayscaleImage, conversionCode);
      return grayscaleImage;
    }
  }
  else
  {
    std::vector<Mat> subMatrices(4);
    cv::split(input, subMatrices);
    int const maxChannels = static_cast<int>(subMatrices.size()) - 1;
    assert(maxChannels >= 0);
    int effectiveChannelIndex = std::min(maxChannels, static_cast<int>(channel));
    return subMatrices[effectiveChannelIndex];
  }
  return Mat();
}

void normalizeIntensity(Mat image)
{
  assert(image.type() == CV_8UC1);
  double minValue, maxValue;
  minMaxIdx(image, &minValue, &maxValue);
  double const range = maxValue - minValue;
  if (range > 0)
  {
    image = (image - minValue) * 255.0 / range;
  }
}

void reduceIntensity(Mat image, float factor)
{
  assert(image.type() == CV_8UC1);
  image = image * std::max(0.1f, std::min(1.0f, factor));
}


} // namespace robofish
