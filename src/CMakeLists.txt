find_package(OpenCV 4.0 COMPONENTS core features2d xfeatures2d)

set(Boost_USE_STATIC_LIBS OFF) 
find_package(Boost 1.68 COMPONENTS program_options)

include_directories(SYSTEM ${OpenCV_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
link_directories(${OpenCV_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS})
add_definitions(${OpenCV_CFLAGS_OTHER} ${Boost_CFLAGS_OTHER})

add_executable(opencv-playground main.cpp main_options.cpp preprocessing.cpp line_detector.cpp feature_detectors.cpp utils.cpp filters.cpp)
target_link_libraries(opencv-playground ${OpenCV_LIBRARIES} ${Boost_LIBRARIES})

