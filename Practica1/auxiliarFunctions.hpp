#ifndef AUXILIARFUNCTIONS_HPP
#define AUXILIARFUNCTIONS_HPP

#include <vector>
#include <opencv2/core/core.hpp> 

std::vector<int> getHistogram(cv::Mat &image);

std::vector<int> getCumulativeNormalizedHistogram(std::vector <int> histogram);

void equalizate(cv::Mat &image, std::vector<int> cumulativeHistogram);










#endif
