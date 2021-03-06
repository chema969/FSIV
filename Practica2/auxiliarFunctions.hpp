#ifndef AUXILIARFUNCTIONS_HPP
#define AUXILIARFUNCTIONS_HPP
#include <opencv2/core/core.hpp> 
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui/highgui.hpp>

cv::Mat createBoxFilter(int r);
void filt(cv::Mat& in,cv::Mat& filter, cv::Mat& out,bool circular);
void convolve(const cv::Mat& in, const cv::Mat& filter, cv::Mat& out); 
void multiplyMat(cv::Mat& in, int g);
cv::Mat createGaussianFilter(int r);
double gauss(int x,int y);
void resize(cv::Mat& in);
void unresize(cv::Mat& in);






#endif
