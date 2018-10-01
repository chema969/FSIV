#ifndef AUXILIARFUNCTIONS_HPP
#define AUXILIARFUNCTIONS_HPP
#include <string>
#include <vector>
#include <opencv2/core/core.hpp> 




std::vector<int> getHistogram(cv::Mat &image);

std::vector<int> getCumulativeNormalizedHistogram(std::vector <int> histogram);

void equalizate(cv::Mat &image, std::vector<int> cumulativeHistogram);

cv::Mat equalizedRadiusImage(cv::Mat &image,int rValue);

int pixelVal(cv::Mat &image,int radius,int y,int x,int pixel);

std::vector<int> getHistogram(std::vector< std::vector<int> > i);

void masked(cv::Mat &image,cv::Mat &mask,std::vector<int> cumulativeHistogram);

cv::Mat equalizedRadiusImage(cv::Mat &image,cv::Mat &mask,int rValue);

#endif
