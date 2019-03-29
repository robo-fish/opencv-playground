#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdocumentation"
#include <opencv2/core.hpp>
#pragma GCC diagnostic pop

#include "playground_types.hpp"

using namespace cv;

namespace robofish {

Mat convertRAWToRGB(Mat input);

// Extracts the given color channel from the given array.
// Returns grayscale image for channel == Channel::gray
Mat extractChannel(Mat input, Channel channel);

// The input must be a single-channel image (input.type() == CV_8UC1)
void normalizeIntensity(Mat input);

/// @param[in] image must be single-channel.
/// @param[in] factor must be between 0 and 1.0
void reduceIntensity(Mat image, float factor);

} // nsmespace robofish
