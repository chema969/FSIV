#ifndef AUXILIAR_FUNCTIONS_HPP
#define AUXILIAR_FUNCTIONS_HPP
#include <vector>
#include <iostream>
#include <opencv2/core/core.hpp> 
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui/highgui.hpp>

struct aux{
    std::string time;
    int calibrationImageWidth;
    int calibrationImageHeight;
    int numberOfCrossPointsInWidth;
    int numberOfCrossPointsInHeight;
    double squareSize;
    int numberOfCalibratedImages;
    cv::Mat cameraMatrix1;
    cv::Mat distortionCoefficient1;
    std::vector<cv::Mat> rotationMatrix1;
    std::vector<cv::Mat> translationMatrix1;
};

void readYML(std::string intrinsics,struct aux &l);







#endif
