#include "auxiliarFunctions.hpp"

#include <vector>
#include <iostream>
#include <opencv2/core/core.hpp> 
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui/highgui.hpp>

void readYML(std::string intrinsics,struct aux &l){
        cv::FileStorage fs(intrinsics, cv::FileStorage::READ);
        if( !fs.isOpened() )
            return;
    //l.time = fs["calibration_time"];
    l.calibrationImageWidth = fs["image_width"];
    l.calibrationImageHeight = fs["image_height"];
    l.numberOfCrossPointsInWidth = fs["board_width"];
    l.numberOfCrossPointsInHeight = fs["board_height"];
    l.squareSize = fs["square_size"];
    l.numberOfCalibratedImages = fs["nr_of_frames"];
    fs["camera_matrix"]>>l.cameraMatrix1;
    fs["distortion_coefficients"]>>l.distortionCoefficient1;
    //std::cout/*<<l.time*/<<l.calibrationImageWidth <<l.calibrationImageHeight;



       }
