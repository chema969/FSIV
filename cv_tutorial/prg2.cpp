/***********************************
 * Autor: Rafael Muñoz Salinas
 * Universidad de Córdoba (Spain)
 * Master en Sistemas Inteligentes
 * Asignatura: Realidad Virtual y Aumentada
 * 
 * Info:
 * Shows how to randomly access to the elements in an image and writes to file
 * 
 * 
 * ******************************************/

#include <opencv2/core/core.hpp> //core routines
#include <opencv2/highgui/highgui.hpp>//imread,imshow,namedWindow,waitKey
#include <iostream>
using namespace std;
int main(int argc,char **argv){
try{
//Creates an image of size 320x240 3 channels (BGR)
cv::Mat image(240,320,CV_8UC3);

//move through the image  setting values
for(int y=0;y<image.rows;y++){
  uchar *ptr=image.ptr<uchar>(y); //pointer to the y-th image row
  for(int x=0;x<image.cols;x++,ptr+=3){
    ptr[0]=ptr[1]=ptr[2]=y;
  }
 }
 //prints a single pixel
  int x=10,y=120;
  cout<<"val="<<(int)image.at<uchar>(y,x)<<endl;
  cout<<"B="<<(int)image.at<cv::Vec3b>(y,x)[0]<<endl;
  cout<<"G="<<(int)image.at<cv::Vec3b>(y,x)[1]<<endl;
  cout<<"R="<<(int)image.at<cv::Vec3b>(y,x)[2]<<endl;
  //saves to file
  cv::imwrite("out.jpg",image);
}catch(std::exception &ex)
{
  cout<<ex.what()<<endl;
}
}
 
