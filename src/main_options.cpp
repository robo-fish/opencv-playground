//
//  main_options.cpp
//  Robofish OpenCV Playground
//
//  Created by Kai Oezer on 28.03.19.
//
#include <iostream>
#include <boost/algorithm/string/predicate.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdocumentation"
#pragma GCC diagnostic ignored "-Wcomma"
#include <boost/program_options.hpp>
#pragma GCC diagnostic pop

#include "main_options.hpp"


using namespace std;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace robofish {

PlaygroundInput processInput(int argc, char** argv)
{
  auto result = make_tuple(Task::None, 0, fs::path());

  po::options_description desc("Allowed options");
  desc.add_options()
    (
      "help,h",
      "produce help message"
    )
    (
      "task,t",
      po::value<string>(),
      "The task to execute. You can specify one of the following:\n\n"
      "Feature detection tasks:\n"
      "   fd-shi-tomasi (Shi-Thomasi)\n"
      "   fd-sift (SIFT)\n"
      "   fd-surf (SURF)\n"
      "   fd-fast (FAST)\n"
      "   fd-agast (AGAST)\n"
      "   fd-brisk (BRISK)\n"
      "   fd-orb  (ORB)\n"
      "   fd-harris (Harris-Laplace)\n"
      "   fd-star (Star)\n"
      "   fd-kaze (KAZE)\n"
      "   fd-akaze (Accelerated KAZE)\n"
    )
    (
      "option,o",
      po::value<int>(),
      "The option for the selected task. The argument must be an integer number."
    )
    (
      "media,m",
      po::value<string>(),
      "The path to the media file used by the task, if applicable."
    )
  ;

  po::variables_map vm;
  try
  {
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
  }
  catch (po::invalid_option_value & e)
  {
    cout << e.what() << endl;
    return result;
  }

  if ( vm.count("help") || vm.empty() )
  {
    cout << desc << "\n";
  }
  else
  {
    static const string key_Task{"task"};
    const string val_Task = ( (vm.count(key_Task) > 0) && !vm[key_Task].empty() && (vm[key_Task].value().type() == typeid(string))) ? vm[key_Task].as<string>() : "";
    static const string key_Media{"media"};
    const string val_Media = ( (vm.count(key_Media) > 0) && !vm[key_Media].empty() && (vm[key_Media].value().type() == typeid(string)) ) ? vm[key_Media].as<string>() : "";
    static const string key_Option{"option"};
    int val_Option = ( (vm.count(key_Option) > 0) && !vm[key_Option].empty() && (vm[key_Option].value().type() == typeid(int)) ) ? vm[key_Option].as<int>() : -1;

    if ( boost::algorithm::starts_with(val_Task,"fd-") && !val_Media.empty())
    {
      const int detector = [val_Task] ()->int {
        string const strDetector = val_Task.substr(strlen("fd-"));
        if (strDetector == "shi-thomasi") return 0;
        if (strDetector == "sift") return 1;
        if (strDetector == "surf") return 2;
        if (strDetector == "fast") return 3;
        if (strDetector == "agast") return 4;
        if (strDetector == "brisk") return 5;
        if (strDetector == "orb") return 6;
        if (strDetector == "harris") return 7;
        if (strDetector == "star") return 8;
        if (strDetector == "kaze") return 9;
        if (strDetector == "akaze") return 10;
        return -1;
      }();

      result = make_tuple(Task::FeatureDetection, detector, fs::path(val_Media));
    }

  }

  return result;
}


} // namespace robofish
