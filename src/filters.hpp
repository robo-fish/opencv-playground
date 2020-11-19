//
//  filters.hpp
//  OpenCV Playground
//
//  Created by Kai Oezer on 29.03.19.
//

#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdocumentation"
#include <opencv2/core.hpp>
#pragma GCC diagnostic pop

namespace robofish {

enum class FilterType : uint8_t
{
  Sobel =   0,
  Median =  1
};

void filter(uint8_t type, cv::InputArray, cv::OutputArray);

} // namespace robofish
