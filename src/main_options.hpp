//
//  main_options.hpp
//  Robofish OpenCV Playground
//
//  Created by Kai Oezer on 28.03.19.
//

#pragma once

#include <stdio.h>
#include <tuple>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdocumentation"
#include <boost/filesystem.hpp>
#pragma GCC diagnostic pop

namespace robofish {

enum class Task
{
  None,
  FeatureDetection,
};

using TaskOption = int;

using PlaygroundInput = std::tuple<Task, TaskOption, boost::filesystem::path>;

PlaygroundInput processInput(int numProgramArgs, char** programArgs);

} // namespace robofish
