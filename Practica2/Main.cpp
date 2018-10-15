#include <vector>
#include <iostream>
#include "auxiliarFunctions.hpp"
#include <opencv2/core/core.hpp> 
#include<opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>


const cv::String keys =
    "{r              |1     | radius               }"
    "{g              |1     | gain value           }"
    "{f              |0     | bipartition          }"
    "{c              |      | circular flag        }"
    "{@image1        |<none>| imput image          }"
    "{@image2        |<none>| output image         }"
    ;


int main(int argc,char **argv){ 
  cv::CommandLineParser parser(argc, argv, keys);
 
  //////////////////////////////////////////
  //command lines values                  //
  //////////////////////////////////////////
   float rValue = parser.get<float>("r");
   float gValue = parser.get<float>("g");
   int fValue = parser.get<int>("f");
   std::string image1 = parser.get<std::string>(0);
   std::string image2 = parser.get<std::string>(1);
   bool c = parser.has("c");
  
  cv::Mat image=cv::imread(image1);
   bool isBRG=false;
   
   if(image.channels()==3){
      isBRG=true;
     image.convertTo(image, CV_32FC3);
     
      cv::cvtColor(image,image,cv::COLOR_BGR2HSV,3);}
   
   cv::Mat hsv[3];   //destination array
   if(isBRG){
     split(image,hsv);//split source 
     image=hsv[2];
   
    }

    if(!isBRG) image.convertTo(image, CV_32FC1);
   if(image.channels()!=1){
               std::cout<<"Error, no grayscale image"<<std::endl;
               return 0;}
   
   if(rValue<1){
               std::cout<<"Error, radius value is less than 1"<<std::endl;
               return 0;}
   
   cv::Mat filtered(image.rows,image.cols,CV_32FC1);
   cv::Mat filter;
  if(fValue==0)
      filter=createBoxFilter(rValue*2+1); 

  else
      filter=createGaussianFilter(rValue*2+1);  

   filt(image,filter,filtered,c);
  
   multiplyMat(image,gValue+1);
   multiplyMat(filtered,gValue);
 
   cv::Mat output(image.rows,image.cols,CV_32FC1);
   convolve(image,filtered,output);

   
   if(isBRG){
         hsv[2]=image;
         cv::merge(hsv,3,image);
          image.convertTo(image, CV_8UC3);
          cv::cvtColor(image,image,cv::COLOR_HSV2BGR);
     }     

   cv::imwrite(image2,output);
 }
