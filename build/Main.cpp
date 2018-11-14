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
    "{i              |      | imput reflected img  }"
    "{w              |      | write                }"
    ;
//bool condicion(cv::Point2f x,cv::Point2f y);
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
   std::string ref=parser.get<std::string>("i");
   std::string vidOut=parser.get<std::string>("w"); 

   cv::Mat frame;
   cv::VideoCapture video(input);
  cv::VideoWriter vidWritt;

   cv::VideoCapture prueba;
   if(ref!=""){
        prueba.open(ref);
       }

   if(vidOut!=""){
     int ex = static_cast<int>(video.get(CV_CAP_PROP_FOURCC));     // Get Codec Type- Int form
    //char EXT[] = {(char)(ex & 0XFF) , (char)((ex & 0XFF00) >> 8),(char)((ex & 0XFF0000) >> 16),(char)((ex & 0XFF000000) >> 24), 0};
     cv::Size S = cv::Size((int) video.get(CV_CAP_PROP_FRAME_WIDTH),    //Acquire input size
              (int) video.get(CV_CAP_PROP_FRAME_HEIGHT));
//outputVideo.open(NAME , ex, video.get(CV_CAP_PROP_FPS),S, true);
   vidWritt.open(vidOut,ex,video.get(CV_CAP_PROP_FPS),S);
   }
  if(vidWritt.isOpened()) std::cout<<"Video output is open"<<std::endl;



   while(video.read(frame)){

       aux l;
       std::vector<cv::Point2f>  corners;
       cv::Size xy(rows,cols);
        cv::Mat frameButInGrey;
        cv::cvtColor(frame, frameButInGrey, cv::COLOR_BGR2GRAY);
        std::vector<cv::Point3f> objectPoints;
        for(int y=0; y<cols; ++y) {
           for(int x=0; x<rows; ++x)
              objectPoints.push_back(cv::Point3f(x*size,y*size,0));
        }               
        bool patternfound=cv::findChessboardCorners(frame,xy,corners);
       if(patternfound){
        cv::cornerSubPix(frameButInGrey,corners,cv::Size(5,5), cv::Size(-1,-1), cv::TermCriteria());
        readYML(intrinsics,l);
        cv::Mat rvec,tvec;
    

       cv::solvePnP(cv::Mat(objectPoints),cv::Mat(corners),l.cameraMatrix1,l.distortionCoefficient1,rvec,tvec);
   if(ref==""){    
     std::vector<cv::Point3f> pointsProjected;
          pointsProjected.push_back(cv::Point3f((rows/2)*size,(cols/2)*size,0));
          pointsProjected.push_back(cv::Point3f(((rows/2)+1)*size,(cols/2)*size,0));
          pointsProjected.push_back(cv::Point3f((rows/2)*size,((cols/2)+1)*size,0));
         pointsProjected.push_back(cv::Point3f((rows/2)*size,(cols/2)*size,-size));

       std::vector<cv::Point2f> output;  
     cv::projectPoints(cv::Mat(pointsProjected),cv::Mat(rvec), cv::Mat(tvec), l.cameraMatrix1,l.distortionCoefficient1, output);
        
        cv::Scalar color(255,0,0);
        cv::Scalar color1(0,255,0);
        cv::Scalar color2(0,0,255);


       cv::line(frame,output[0],output[1], color,5); 
       cv::line(frame,output[0],output[2], color1,5); 
       cv::line(frame,output[0],output[3], color2,5);
}
      

else{
      cv::Mat source;
      if(prueba.read(source)==false){
         prueba.release();
         prueba.open(ref);
         prueba.read(source); }
        std::vector<cv::Point2f> sourcePoints;
        std::vector<cv::Point3f> pointsProjected;
          pointsProjected.push_back(cv::Point3f((rows-1)*size,(cols-1)*size,0));
          pointsProjected.push_back(cv::Point3f(0,0,0));
          pointsProjected.push_back(cv::Point3f((rows-1)*size,0,0));
         pointsProjected.push_back(cv::Point3f(0,(cols-1)*size,0));
       std::vector<cv::Point2f> output;  
            cv::projectPoints(cv::Mat(pointsProjected),cv::Mat(rvec), cv::Mat(tvec), l.cameraMatrix1,l.distortionCoefficient1, output);

 
 

       sourcePoints.push_back(cv::Point2f(0,0));   
       sourcePoints.push_back(cv::Point2f(source.cols,source.rows));
       sourcePoints.push_back(cv::Point2f(source.cols,0)); 
       sourcePoints.push_back(cv::Point2f(0,source.rows));             


 
      cv::Size x=cv::Size((int) video.get(CV_CAP_PROP_FRAME_WIDTH),    //Acquire input size
              (int) video.get(CV_CAP_PROP_FRAME_HEIGHT));
      cv::Mat H = findHomography(sourcePoints, output);
      cv::Mat M=getPerspectiveTransform(sourcePoints, output);

      cv::warpPerspective(source, frame, H, x,cv::INTER_LINEAR,cv::BORDER_TRANSPARENT);


       }
  }   
       cv::imshow("Live", frame);
        if (cv::waitKey(5) >= 0)
            break;
 
        if(vidOut!=""){

       vidWritt.write(frame);
       }


 }




  }


/*
bool condicion(cv::Point2f a,cv::Point2f b){
      if((a.x+a.y)>*/
