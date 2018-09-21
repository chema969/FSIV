#include <vector>
#include <iostream>
#include "auxiliarFunctions.hpp"
#include <opencv2/core/core.hpp> 
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>


int main(int argc, char **argv){

 if(argc<3){
         std::cerr<<"Error: more arguments are needed"<<std::endl; return 0;}

  CmdLineParser cml(argc,argv);
  int rValue=0;
  int auxVal=1;
  //check if a command is present
  if (cml["-r"]){
     rValue=std::stoi(cml("-r","0"));//if -p is not, then, return 1
     if(std::string(argv[1])=="-r")
        auxVal=3;
   }
  
  
//used with default values
 
  
   std::vector<int> histogram;
   cv::Mat image=cv::imread(argv[auxVal]);//opening the image
     if( image.rows==0) {std::cerr<<"Error reading image"<<std::endl;return 0;}
   
   //Transform BGR image to gray scale image
   cv::cvtColor(image,image,cv::COLOR_BGR2GRAY);
  
   if(rValue==0){
   //Getting the histogram from the image
     histogram=getHistogram(image);
     
     equalizate(image,getCumulativeNormalizedHistogram(histogram));}
   
   else{
      image=equalizedRadiusImage(image,rValue);
     
      }
   cv::imwrite(argv[auxVal+1],image);

}



