#include <vector>
#include <iostream>
#include "auxiliarFunctions.hpp"
#include <opencv2/core/core.hpp> 
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>



int main(int argc, char **argv){
   if(argc<3){
         std::cerr<<"Error: more arguments are needed"<<std::endl; return 0;}

   std::vector<int> histogram;
   cv::Mat image=cv::imread(argv[1]);//opening the image
     if( image.rows==0) {std::cerr<<"Error reading image"<<std::endl;return 0;}
   
   //Transform BGR image to gray scale image
   cv::cvtColor(image,image,cv::COLOR_BGR2GRAY);

   //Getting the histogram from the image
   histogram=getHistogram(image);
     
   equalizate(image,getCumulativeNormalizedHistogram(histogram));
  
   cv::imwrite(argv[2],image);

}



