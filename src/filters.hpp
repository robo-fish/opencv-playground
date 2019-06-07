//
//  filters.hpp
//  OpenCV Playground
//
//  Created by Kai Oezer on 29.03.19.
//

#pragma once

#include <opencv2/core.hpp>

namespace robofish {

enum class FilterType : uint8_t
{
  Sobel =   0,
  Median =  1
};

void filter(uint8_t type, cv::InputArray, cv::OutputArray);

} // namespace robofish
