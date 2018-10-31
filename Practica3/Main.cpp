#include <opencv2/videoio.hpp>
#include <vector>
#include <iostream>
#include "auxiliarFunctions.hpp"
#include <opencv2/core/core.hpp> 
#include <opencv2/opencv.hpp>
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
       aux l;
       cv::Mat corners;
       cv::Size xy(rows,cols);
        cv::Mat frameButInGrey;
        cv::cvtColor(frame, frameButInGrey, cv::COLOR_BGR2GRAY);
        std::vector<cv::Point3f> objectPoints;
        for(int y=0; y<cols; ++y) {
           for(int x=0; x<rows; ++x)
              objectPoints.push_back(cv::Point3f(x*size,y*size,0));
        }               
        bool patternfound=cv::findChessboardCorners(frame,xy,corners);
        cv::cornerSubPix(frameButInGrey,corners,cv::Size(5,5), cv::Size(-1,-1), cv::TermCriteria());
        readYML(intrinsics,l);
        cv::Mat rvec,tvec;
    //    cv::drawChessboardCorners(frame, xy, cv::Mat(corners), patternfound);
    
      std::vector<cv::Point2f> output;
       cv::solvePnP(cv::Mat(objectPoints),cv::Mat(corners),l.cameraMatrix1,l.distortionCoefficient1,rvec,tvec);
     cv::projectPoints(cv::Mat(objectPoints),cv::Mat(rvec), cv::Mat(tvec), l.cameraMatrix1,l.distortionCoefficient1, output);
     

      for(int y=0;y<output.size()-1;y++){
      cv::line(frame,output[y], output[y+1], const Scalar& color)
      }
    
       cv::imshow("Live", output);
        if (cv::waitKey(5) >= 0)
            break;
  }






  }
