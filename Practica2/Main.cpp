#include <vector>
#include <iostream>
#include "auxiliarFunctions.hpp"
#include <opencv2/core/core.hpp> 
#include<opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>
int rValueSlider=1;
int gValueSlider=1;
cv::Mat image;
int fValue;
bool c;
void slider(int, void*);
 bool isBRG;
cv::Mat slideroutput;
 cv::Mat hsv[3];

const cv::String keys =
    "{r              |1     | radius               }"
    "{g              |1     | gain value           }"
    "{f              |0     | bipartition          }"
    "{c              |      | circular flag        }"
    "{i              |      | interactive          }"
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
   fValue = parser.get<int>("f");
   std::string image1 = parser.get<std::string>(0);
   std::string image2 = parser.get<std::string>(1);
   c = parser.has("c");
   bool i = parser.has("i");
  
   image=cv::imread(image1,cv::IMREAD_GRAYSCALE);
   if(image.rows==0){
           std::cout<<"Error opening image"<<std::endl; return 0;}

  

   
   if(image.channels()!=1){
               std::cout<<"Error, no grayscale image"<<std::endl;
               return 0;}
   
   if(rValue<1){
               std::cout<<"Error, radius value is less than 1"<<std::endl;
               return 0;}
   

  if(!i){
   
   
   image.convertTo(image, CV_32FC1);
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



   cv::imwrite(image2,output);
  }





  else{
      cv::cvtColor(image,image,cv::COLOR_BGR2GRAY);
      image.convertTo(image, CV_32FC1); 
      cv::namedWindow("Values",0);
      cv::createTrackbar("r value","Values",&rValueSlider,50,slider);
      cv::createTrackbar("g value","Values",&gValueSlider,10,slider);
       cv::namedWindow("original");
   //displays the image in the window
   cv::Mat copy=image.clone();
   resize(copy);
   cv::imshow("original",copy);
   char c=0;
   while(c!=27){  //waits until ESC pressed
	c=cv::waitKey(0);
        if(c==13){std::cout<<"saving image"<<std::endl;
         cv::imwrite(image2,slideroutput);}
   } 
 }
}






void slider(int, void*){
 if((gValueSlider!=0)&&(rValueSlider!=0)){
   std::cout<<rValueSlider<<","<<gValueSlider<<std::endl;
     cv::Mat something=image.clone();
   cv::Mat filtered(image.rows,image.cols,CV_32FC1);
    cv::Mat filter;
    if(fValue==0)
       filter=createBoxFilter(rValueSlider*2+1); 

  else
      filter=createGaussianFilter(rValueSlider*2+1);  

   filt(something,filter,filtered,c);
  
   multiplyMat(something,gValueSlider+1);
   multiplyMat(filtered,gValueSlider);
 
   cv::Mat output(image.rows,image.cols,CV_32FC1);
   convolve(something,filtered,output);
   slideroutput=output.clone();
   
 
   resize(output);
   cv::namedWindow("myimage");
   //displays the image in the window
   
   cv::imshow("myimage",output);
   
   }







}
