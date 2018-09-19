#include <vector>
#include <iostream>
#include <opencv2/core/core.hpp> 
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

int main(int argc, char **argv){
   if(argc<3){
         std::cerr<<"Error: more arguments are needed"<<std::endl; return 0;}

   std::vector<int> hystogram(256,0);
   cv::Mat image=cv::imread(argv[1]);//opening the image
    if( image.rows==0) {std::cerr<<"Error reading image"<<std::endl;return 0;}
   cv::Mat image2;
   cv::cvtColor(image,image,cv::COLOR_BGR2GRAY);

   
   cv::namedWindow("myimage");
   //displays the image in the window
   cv::imshow("myimage",image);
   cv::waitKey(0);

}
