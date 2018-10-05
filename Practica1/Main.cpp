#include <vector>
#include <iostream>
#include "auxiliarFunctions.hpp"
#include <opencv2/core/core.hpp> 
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>

const cv::String keys =
    "{r              |0     | radius equalitation  }"
    "{b              |      | bipartition          }"
    "{@image1        |<none>| imput image          }"
    "{@image2        |<none>| output image         }"
    "{@mask          |<none>| mask image           }"
    ;

int main(int argc, char **argv){
try{
   cv::CommandLineParser parser(argc, argv, keys);
 

  //////////////////////////////////////////
  //command lines values                  //
  //////////////////////////////////////////
   int rValue = parser.get<int>("r");
   std::string images = parser.get<std::string>(0);
   std::string image2 = parser.get<std::string>(1);
   std::string mask = parser.get<std::string>(2);
   bool b = parser.has("b");
   
   
   if((images=="")||(image2=="")){std::cout<<"Imput and output image needed"<<std::endl; return 0;}
 
   
   std::vector<int> histogram;
   cv::Mat image=cv::imread(images);//opening the image
     if( image.rows==0) {std::cerr<<"Error reading image"<<std::endl;return 0;}
  
   
   bool isBRG=false;
   
   if(image.channels()==3){
      isBRG=true;
      cv::cvtColor(image,image,cv::COLOR_BGR2HSV,3);}
   
   cv::Mat hsv[3];   //destination array
   if(isBRG){
     split(image,hsv);//split source  
     image=hsv[2].clone();
    }




   if(rValue==0){
   //Getting the histogram from the image-
     histogram=getHistogram(image);
       if(mask!=""){
   
          cv::Mat maskIm=cv::imread(mask);
          cv::cvtColor(maskIm,maskIm,cv::COLOR_BGR2GRAY);
          if( maskIm.rows==0) {std::cerr<<"Error reading image"<<std::endl;return 0;}

          if((maskIm.rows!=image.rows)||(maskIm.cols!=image.cols)){
              std::cout<<"Error, size differences between mask and image"<<std::endl;return 0;}
          masked(image,maskIm,getCumulativeNormalizedHistogram(histogram,b));
          }
     else{
        //equalizing the image
        equalizate(image,getCumulativeNormalizedHistogram(histogram,b));
        }
     }

   
   else{
      if(mask=="")
         image=equalizedRadiusImage(image,rValue,b);
      else{
         cv::Mat maskIm=cv::imread(mask);
         cv::cvtColor(maskIm,maskIm,cv::COLOR_BGR2GRAY);
         image=equalizedRadiusImage(image,maskIm,rValue,b);
         }
      }

   if(isBRG){
         hsv[2]=image.clone();
         cv::merge(hsv,3,image);
          cv::cvtColor(image,image,cv::COLOR_HSV2BGR);
     }     
  
   cv::imwrite(image2,image);

}

catch(std::exception &ex){
  std::cout<<ex.what()<<std::endl;
}



}
