//
//  filters.cpp
//  OpenCV Playground
//
//  Created by Kai Oezer on 29.03.19.
//

#include "filters.hpp"

namespace robofish {

static void filterSobel(cv::InputArray in, cv::OutputArray out)
{

}

static void filterMedian(cv::InputArray in, cv::OutputArray out)
{

}

void filter(uint8_t type, cv::InputArray in, cv::OutputArray out)
{
  switch (type)
  {
    case static_cast<uint8_t>(FilterType::Sobel) : filterSobel(in, out); break;
    case static_cast<uint8_t>(FilterType::Median) : filterMedian(in, out); break;
  }
}

} // namespace robofish
