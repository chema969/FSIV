#include <vector>
#include <iostream>
#include "auxiliarFunctions.hpp"
#include <opencv2/core/core.hpp> 
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>


const cv::String keys =
    "{r              |1     | radius               }"
    "{g              |1     | gain value           }"
    "{f              |0     | bipartition          }"
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
   
   cv::Mat image=cv::imread(image1,cv::IMREAD_GRAYSCALE);
   if(image.channels()!=1){
               std::cout<<"Error, no grayscale image"<<std::endl;
               return 0;}
   
   if(rValue<1){
               std::cout<<"Error, radius value is less than 1"<<std::endl;
               return 0;}
   
   cv::Mat output(image.rows,image.cols,CV_32FC1);
   cv::Mat box=createBoxFilter(rValue*2+1); 
   std::cout<<"Voy a aplicar el filtro"<<std::endl;
   filt(image,box,output);
   std::cout<<"salgo del filtro"<<std::endl;
   cv::imwrite(image2,output);
 }
