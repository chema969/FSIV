#include <opencv2/videoio.hpp>
#include <vector>
#include <iostream>
#include "auxiliarFunctions.hpp"
#include <opencv2/core/core.hpp> 
#include<opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui/highgui.hpp>

const cv::String keys =
    "{@rows          |<none>| number of rows       }"
    "{@cols          |<none>| number of cols       }"
    "{@size          |<none>| size                 }"
    "{@intrinsics    |<none>| intrinsics .yum file }"
    "{@input         |<none>| imput video          }"
    ;

int main(int argc,char **argv){
   
  cv::CommandLineParser parser(argc, argv, keys);
 
  //////////////////////////////////////////
  //command lines values                  //
  //////////////////////////////////////////
   int rows = parser.get<int>(0);
   int cols = parser.get<int>(1);
   int size = parser.get<int>(2);
   std::string intrinsics = parser.get<std::string>(3);
   std::string input = parser.get<std::string>(4);

   cv::Mat frame;
   cv::VideoCapture video(input);
   
   while(video.read(frame)){
       cv::Mat corners;
       cv::Size xy(size,size);
        cv::Mat frameButInGrey;
        cvtColor(frame, frameButInGrey, COLOR_BGR2GRAY);
        
        cv::findChessboardCorners(frame,xy,corners);
        cv::cornerSubPix(frameButInGrey,corners,xy
        cv::imshow("Live", frame);
        if (cv::waitKey(5) >= 0)
            break;
  }






  }
